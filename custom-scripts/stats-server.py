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


def get_system_datetime() -> str:
    #/proc/driver/rtc
    return ""

def get_system_uptime_seconds() -> str:
    # /proc/uptime
    return ""

def get_processor_model_and_velocity() -> str:
    # /proc/cpuinfo
    return ""

def get_percentage_processor_in_use() -> float:
    # /proc/stat
    return 0.0

def get_total_and_used_ram() -> str:
    # /proc/meminfo
    return ""

def get_system_version() -> str:
    # /proc/version
    return ""

def get_processes_in_execution() -> list:
    # ler os subdiretorios de /proc e capturar apenas aqueles com valores numricos (expressao regular ^[0-9]+$)
    return ""

def get_disk_units_with_capacity() -> list:
    # /proc/partitions (para listar as unidades de disco)
    # sys/block/[device]/size (para o tamamnho em blocos - 1 bloco = 512 bytes)
    return ""

def get_usb_devices_with_port() -> list:
    # /sys/bus/usb/devices
    return ""

def get_network_adapters_with_ip() -> list:
    #/proc/net/route
    return ""

if __name__ == '__main__':
    httpd = HTTPServer((HOST_NAME, PORT_NUMBER), MyHandler)
    print("Server Starts - %s:%s" % (HOST_NAME, PORT_NUMBER))
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    print("Server Stops - %s:%s" % (HOST_NAME, PORT_NUMBER))