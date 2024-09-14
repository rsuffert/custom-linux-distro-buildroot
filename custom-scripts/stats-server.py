import os
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
            f"<p>1. System date & time: {get_system_datetime()}</p>"
            f"<p>2. System uptime: {get_system_uptime_seconds()} seconds</p>"
            f"<p>3. Processor model & velocity: {get_processor_model_and_velocity()}</p>"
            f"<p>4. Percentage of processor in use: {get_percentage_processor_in_use() :.2f}%</p>"
            f"<p>5. Total and used RAM: {get_total_and_used_ram()}</p>"
            f"<p>6. System version: {get_system_version()}</p>"
            f"<p>7. Processes in execution: {', '.join(get_processes_in_execution())}</p>"
            f"<p>8. Disk units with capacity: {', '.join(get_disk_units_with_capacity())}</p>"
            f"<p>9. USB devices with port: {', '.join(get_usb_devices_with_port())}</p>"
            f"<p>10. Network adapters with IP: <br/> {get_network_adapters_with_ip()}</p>"
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
    with open('/proc/cpuinfo', 'r') as f:
        model = re.search(r'model name\s*:\s*(.*)', f.read()).group(1) # assumes it's the same for all cores

    BASE_DIR = "/sys/devices/system/cpu"
    pattern = re.compile(r'^cpu.*\d$')
    max_freqs = []
    for dir in os.listdir(BASE_DIR):
        if not pattern.match(dir): continue
        with open(os.path.join(BASE_DIR, dir, "cpufreq/cpuinfo_max_freq")) as f:
            max_freqs.append(float(f.read())/(10**6)) # convert to GHz
    return f"{model} (highest core maximum frequency is {max(max_freqs)} GHz)"

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
        return f"{total_mem:.2f} GB || {used_memory:.2f} GB"
    else:
        return "Memory information is incomplete."

def get_system_version() -> str: # Ricardo
    # /proc/version
    with open("/proc/version") as f:
        content = f.read()
    return content

def get_processes_in_execution() -> list: # Balejos
    # ler os subdiretorios de /proc e capturar apenas aqueles com valores numricos (expressao regular ^[0-9]+$)
    sub_dir = []
    for pid in os.listdir('/proc'):
        if pid.isnumeric():
            try:
                with open(f'/proc/{pid}/comm') as f: name = f.read().strip()
                sub_dir.append(f"{pid} ({name})")
            except FileNotFoundError:
                pass # process is no longer running
    return sub_dir

def get_disk_units_with_capacity() -> list: # Balejos
    # /sys/block para listar as unidades de disco
    # /sys/block/[device]/size para listar o tamanho da unidade de disco em blocos (1 bloco = 512 bytes)
    disk_units = []
    if os.path.exists('/sys/block'): 
        for device in os.listdir('/sys/block'): 
            size_path = os.path.join ('/sys/block', device, 'size') 
            if os.path.exists(size_path): 
                with open (size_path, 'r') as f: 
                    size_in_blocks = int(f.read().strip()) 
                    size_in_bytes = size_in_blocks * 512 
                    disk_units.append (f"{device}: {size_in_bytes / (1024 ** 3):.2f} GB")
    return disk_units

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
    

def get_network_adapters_with_ip() -> str:
    with open('/proc/net/route', 'r') as file:
        content = file.read()
    return content.replace('\n', '<br/>')

if __name__ == '__main__':
    httpd = HTTPServer((HOST_NAME, PORT_NUMBER), MyHandler)
    print("Server Starts - %s:%s" % (HOST_NAME, PORT_NUMBER))
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    print("Server Stops - %s:%s" % (HOST_NAME, PORT_NUMBER))