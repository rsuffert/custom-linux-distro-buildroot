import time
from http.server import BaseHTTPRequestHandler,HTTPServer
import re


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
            f"<p>Percentage of processor in use: {get_percentage_processor_in_use() :.2f}%</p>"
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
    with open("/proc/driver/rtc", 'r') as f:
        content = f.read()
    rtc_time = re.search(r'rtc_time\s*:\s*(\d+:\d+:\d+)', content).group(1)
    rtc_date = re.search(r'rtc_date\s*:\s*(\d+-\d+-\d+)', content).group(1)
    return f"{rtc_date}, {rtc_time}"

def get_system_uptime_seconds() -> str: # Ricardo
    with open("/proc/uptime", 'r') as f:
        content = f.read()
    uptime = re.split(r'\s+', content)[0]
    return f"{uptime}"

def get_processor_model_and_velocity() -> str: # Gustavo
    # /proc/cpuinfo
    return ""

def get_percentage_processor_in_use() -> float: # Ricardo
    # read two snapshots of CPU stats (1 second of difference)
    with open("/proc/stat") as f:
        content1 = f.read()
    time.sleep(1)
    with open("/proc/stat") as f:
        content2 = f.read()
    
    # process first snapshot
    lines1 = content1.split("\n")
    first_line_split1 = lines1[0].split()[1:]
    total1 = sum(map(float, first_line_split1))
    idle1 = float(first_line_split1[3])

    # process second snapshot
    lines2 = content2.split("\n")
    first_line_split2 = lines2[0].split()[1:]
    total2 = sum(map(float, first_line_split2))
    idle2 = float(first_line_split2[3])

    #calculate CPU usage percentage
    total_diff = total2 - total1
    idle_diff = idle2 - idle1
    return 100 * (1 - (idle_diff/total_diff))

def get_total_and_used_ram() -> str: # Gustavo
    # /proc/meminfo
    return ""

def get_system_version() -> str: # Ricardo
    # /proc/version
    with open("/proc/version") as f:
        content = f.read()
    return content

def get_processes_in_execution() -> list: # Balejos
    # ler os subdiretorios de /proc e capturar apenas aqueles com valores numricos (expressao regular ^[0-9]+$)
    return []

def get_disk_units_with_capacity() -> list: # Balejos
    # /sys/block para listar as unidades de disco
    # /sys/block/[device]/size para listar o tamanho da unidade de disco em blocos (1 bloco = 512 bytes)
    return []

def get_usb_devices_with_port() -> list: # Gustavo
    # /sys/bus/usb/devices
    return []

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