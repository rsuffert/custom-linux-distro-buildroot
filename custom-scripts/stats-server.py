import os
import time
from http.server import BaseHTTPRequestHandler,HTTPServer


HOST_NAME = '192.168.1.10' # !!!REMEMBER TO CHANGE THIS!!!
PORT_NUMBER = 8000


class MyHandler(BaseHTTPRequestHandler):
    def do_GET(s):
        """Respond to a GET request."""
        s.send_response(200)
        s.send_header("Content-type", "text/html")
        s.end_headers()
        response = (
            "<html><head><title>System stats</title></head><body>"
            f"<p>System date & time: {get_system_datetime()}</p>"
            f"<p>System uptime: {get_system_uptime_seconds()} seconds</p>"
            f"<p>Processor model & velocity: {get_processor_model_and_velocity()}</p>"
            f"<p>Percentage of processor in use: {get_percentage_processor_in_use() * 100:.2f}%</p>"
            f"<p>Total and used RAM: {get_total_and_used_ram()}</p>"
            f"<p>System version: {get_system_version()}</p>"
            f"<p>Processes in execution: {', '.join(get_processes_in_execution())}</p>"
            f"<p>Disk units with capacity: {', '.join(get_disk_units_with_capacity())}</p>"
            f"<p>USB devices with port: {', '.join(get_usb_devices_with_port())}</p>"
            f"<p>Network adapters with IP: {', '.join(get_network_adapters_with_ip())}</p>"
            "</body></html>"
        )
        s.wfile.write(response.encode())


def get_system_datetime() -> str: # Ricardo
    #/proc/driver/rtc
    return ""

def get_system_uptime_seconds() -> str: # Ricardo
    # /proc/uptime
    return ""

def get_processor_model_and_velocity() -> str: # Gustavo
    # /proc/cpuinfo
    cpu_info={}
    try:
        with open('/proc/cpuinfo', 'r') as f:
            for line in f:
                if line.startswith('model name'):
                    #pega o modelo separa depois do :
                    key, value = line.strip().split(':', 1)
                    cpu_info['model'] = value.strip()
                elif line.startswith('cpu MHz'):
                    #pega a velocidade separa depois do :
                    key, value = line.strip().split(':', 1)
                    cpu_info['speed'] = value.strip()
    except FileNotFoundError:
        print("/proc/cpuinfo not found.")
    except Exception as e:
        print(f"An error occurred: {e}")
    return f"{cpu_info['model']} || {cpu_info['speed']} MHz"

def get_percentage_processor_in_use() -> float: # Ricardo
    # /proc/stat
    return 0.0

def get_total_and_used_ram() -> str: # Gustavo
    # /proc/meminfo
    mem_info = {}
    
    try:
        with open('/proc/meminfo', 'r') as f:
            for line in f:
                if line.startswith('MemTotal'):
                    key, value = line.strip().split(':', 1)
                    mem_info['MemTotal'] = int(value.strip().split()[0])
                    #memAvailable pois indica a quantidade livre, levando em conta a memoria em caching e buffering que pode ser liberado 
                elif line.startswith('MemAvailable'):
                    key, value = line.strip().split(':', 1)
                    mem_info['MemAvailable'] = int(value.strip().split()[0])
    except FileNotFoundError:
        return "/proc/meminfo not found."
    except Exception as e:
        return f"An error occurred: {e}"

    if 'MemTotal' in mem_info and 'MemAvailable' in mem_info:
        used_memory = mem_info['MemTotal'] - mem_info['MemAvailable']
        used_memory= used_memory/1_048_576
        total_mem=mem_info['MemTotal']/1_048_576
        return f"{total_mem:.2f} gB || {used_memory:.2f} gB"
    else:
        return "Memory information is incomplete."

def get_system_version() -> str: # Ricardo
    # /proc/version
    return ""

def get_processes_in_execution() -> list: # Balejos
    # ler os subdiretorios de /proc e capturar apenas aqueles com valores numricos (expressao regular ^[0-9]+$)
    return []

def get_disk_units_with_capacity() -> list: # Balejos
    # /sys/block para listar as unidades de disco
    # /sys/block/[device]/size para listar o tamanho da unidade de disco em blocos (1 bloco = 512 bytes)
    return []

def get_usb_devices_with_port() -> list: # Gustavo
    # /sys/bus/usb/devices
    usb_path = '/sys/bus/usb/devices'
    
    try:

        usb_entries = os.listdir(usb_path)
        
        #filtra os diretorios
        dir = [entry for entry in usb_entries if os.path.isdir(os.path.join(usb_path, entry))]
        
        return dir
    except FileNotFoundError:
        return ["/sys/bus/usb/devices  not found."]
    except Exception as e:
        return [f"An error occurred: {e}"]
    

def get_network_adapters_with_ip() -> list: # Balejos
    #/proc/net/route
    return []

if __name__ == '__main__':
    httpd = HTTPServer((HOST_NAME, PORT_NUMBER), MyHandler)
    print("Server Starts - %s:%s" % (HOST_NAME, PORT_NUMBER))
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    print("Server Stops - %s:%s" % (HOST_NAME, PORT_NUMBER))