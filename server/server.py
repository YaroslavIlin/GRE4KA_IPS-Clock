import time
from socket_wrapper import Server
from datetime import datetime

server = Server(1337)
server.start()

try:
    while server.is_alive:
        if server.is_client_connected:
            server.send_data(datetime.now().strftime('%H:%M:%S'))
            time.sleep(1)
except KeyboardInterrupt:
    exit()