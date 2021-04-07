"""
uPacket demo that will flash the built in LED on the controller.
The uPacket_basic.ino must be loaded on the controller 
to run this demo. 

"""

from uPacket import up
from time import sleep
ser = up()

port = "/dev/ttyUSB0" #Enter your port here
baud = 19200
timeout = 0.5

print("Starting uPacket basic demo")
check_connect = ser.connect_port(port, baud, timeout)
if check_connect == ser.CONNECT_FALSE:
    print("Could not find port...")
    exit()

while(True):
    ser.send_packet(b"led_on" + b'\0')
    reply = ser.recv_packet()
    if reply[0] == ser.ERR_SER_PORT:
        print("Disconnected")
        ser.close()
        exit()
    print(reply)
    sleep(0.5)

    ser.send_packet(b"led_off" + b'\0')
    reply = ser.recv_packet()
    if reply[0] == ser.ERR_SER_PORT:
        print("Disconnected")
        ser.close()
        exit()
    print(reply)  
    sleep(0.5)


