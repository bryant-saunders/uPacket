"""
uPacket demo that will read the IO pin state of the controller.
The uPacket_stream_wifi.ino must be loaded on the controller 
to run this demo. 

"""

from uPacket import up
from time import sleep
net = up()

address = "10.0.0.0" #Enter controller IP here
port = 1336 
timeout = 1

print("Starting uPacket stream wifi demo")
check_connect = net.connect_net(address, port, timeout)
if check_connect == net.CONNECT_FALSE:
    print("Could not find server...")
    exit()

while(True):
    reply = net.recv_packet() 
    sleep(0.1)
    print(reply)
