"""
uPacket example demonstrates sending binary data over the protocol.
The uPacket_analog.ino must be loaded on the controller 
to run this demo. 

"""

from uPacket import up
import struct
ser = up()

port = "/dev/ttyUSB1" #Enter your port here
baud = 19200
timeout = 0.5

print("Starting uPacket_analog demo")
check_connect = ser.connect_port(port, baud, timeout)
if check_connect == ser.CONNECT_FALSE:
    print("Could not find port...")
    exit()

while(True):
    key_input = input("Enter a PWM value(0-255):")
    pwm_value = struct.pack('>B', int(key_input))
    #Convert input value to byte
    ser.send_packet(b"pwm_out" + b'\0' + pwm_value)
    reply = ser.recv_packet()
    if reply[0] == 3:
        print("DISCONNECTED")
        exit()
    else:
        print(reply) 

    ser.send_packet(b"read_adc" + b'\0')
    reply = ser.recv_packet()
    if reply[0] == 3:
        print("DISCONNECTED")
        exit()
    else:
        raw_bytes = reply[1]
        adc_value = struct.unpack('>H', raw_bytes[0:2])
        #Take two bytes and convert to 16 bit number
        print("ADC Value = " + str(adc_value[0]))     
