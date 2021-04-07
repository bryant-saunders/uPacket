"""
uPacket demo that will flash the built in LED on the controller.
The uPacket_wifi.ino must be loaded on the controller 
to run this demo. 

"""

from uPacket import up
from time import sleep
net = up()

address = "10.0.0.0" #Enter controller IP here
port = 1336 
timeout = 1

print("Starting uPacket_wifi demo")
check_connect = net.connect_net(address, port, timeout)
if check_connect == net.CONNECT_FALSE:
    print("Could not find server...")
    exit()

while(True):
    net.send_packet(b"led_on" + b'\0')
    reply = net.recv_packet()
    print(reply)
    sleep(0.5)

    net.send_packet(b"led_off" + b'\0')
    reply = net.recv_packet()
    print(reply)
    sleep(0.5)


