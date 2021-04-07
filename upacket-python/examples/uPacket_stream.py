"""
uPacket demo that will read the IO pin state of the controller.
The uPacket_stream.ino must be loaded on the controller 
to run this demo. 

"""

from uPacket import up
from time import sleep
ser = up()

port = "/dev/ttyUSB1" #Enter your port here
baud = 19200
timeout = 0.5

print("Starting uPacket stream demo")
check_connect = ser.connect_port(port, baud, timeout)
if check_connect == ser.CONNECT_FALSE:
    print("Could not find port...")
    exit()

while(True):
    reply = ser.recv_packet()
    if reply[0] == ser.ERR_SER_PORT:
        print("Disconnected")
        ser.close()
        exit()
    else:
        print(reply)
