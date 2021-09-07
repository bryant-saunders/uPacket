# uPacket
uPacket is a protocol used to exchange small messages between Python and a compatible Arduino enabled controller. The protocol consists of the following features,
* Send/receive packets over a serial or WiFi connection
* 32 bit bitmap array to allow binary data to be sent/received
* 16 bit xmodem CRC algorithm for error checking
* Delimiter character to ensure packet boundaries
* 32 byte maximum packet data length
* Small size. Arduino library uses minimal flash and RAM


## Arduino library tested on
* Arduino(ATmega328)
* ESP8266(D1 Mini, NodeMCU)
* ESP32(TTGO Feather)
* SAMD21(SEEED XIAO)


## Python library tested on
* Python 3.7+
* Windows 10, Ubuntu 18.04



# Installation

## Arduino
No external packages are required for install.
1. Download/extract repo if not already done
2. Copy the upacket-arduino folder and place in your arduino libraries folder
3. Restart the Arduino IDE if already running
4. Verify installation by compiling one of the arduino examples
5. If using the network functionality, you will need the respective ESP8266 or ESP32 cores installed


## Python
The python library supports python3 only. You will need to add the pyserial library. This can be done with,

`pip install pyserial`

1. Download/extract repo if not already done
2. Copy the uPacket.py file from the upacket-python folder and paste into the PythonXX/lib/site-packages directory.
3. Verify installation by opening up a terminal, start python and type `import uPacket`
4. Copy the examples folder of upacket-python to somewhere you can run them easily



# Usage
The uPacket protocol was designed to be simple but flexible. Below is a list of the common configurations for uPacket with examples.

## Client/server
The main usage scenario is having your python program act as the "client" and the Arduino act as the "server". You send commands from the python side, the Arduino acts upon those commands and sends a reply back to python. The general procedure for this is outlined in the following examples.
* uPacket_basic(Serial)
* uPacket_analog(Serial)
* uPacket_wifi(WiFi)

## Simplex streaming
Sometimes you just want to read a stream of data. The Arduino sends data at a interval and python reads it. The following examples illustrate this.
* uPacket_stream(Serial)
* uPacket_stream_wifi(WiFi)

## Controller to controller
You can have controllers send messages to one another without the need for the python library. You will need two controllers for this demo. They BOTH should be operating on the same voltage for serial communications. This demo also requires the SoftwareSerial library.
* uPacket_client(Serial)(Load this on sending controller)
* uPacket_server(Serial)(Load this on host controller)

# FAQs

## Why the 32 byte packet data maximum?
This is due to the way that uPacket handles delimiter characters. See the "How does the protocol allow binary data" FAQ

## How does the protocol allow binary data if it uses packet delimiters?
The protocol replaces delimiter characters with a dummy value for transmission. In the case of sending packets, it then shifts through a 32 bit "bitmap" and adds a 1 at the bit location that corresponds with a delimiter character. It will then replace the character at that location with the dummy value. Finally, the protocol sends the bitmap + encoded packet to a receiver who then performs the same process in reverse. The receiver shifts through the bitmap and if it finds a 1, it will replace the dummy character at that location with the original delimiter value. 

## How fast is it?
I used the built python module(perf_counter) for timing round trip packet time. I used a NodeMCU esp8266 for hardware. For a serial connection at 115200 baud with a 32 byte payload sent and received, it took a little under 2mS. For a WiFi connection with the same send/receive payloads(32 bytes), it took less than one mS.

## What does the packet structure look like?
Delimiter(1 byte) + CRC(2 bytes) + Bitmap(4 Bytes) + Payload(1-32 bytes) + Delimiter(1 byte)

# Documentation
The Python code is well documented in the form of docstrings. The Arduino code has useful information in the uPacket.cpp file. The examples are a good place to find general usage guidelines.

# Support
This was a side tinkering of mine that I use in my own projects. I have tested the code as extensively as I can and decided to upload it to the community. If you see an issue that affects core functionality, please raise it on Github. If you want to make drastic changes, consider making a fork.


