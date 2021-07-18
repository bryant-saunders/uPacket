import serial
import socket
import serial.tools.list_ports
from time import sleep
import struct

class up():
    """
    uPacket object used to communicate with a controller implementing
    the uPacket protocol. Connections can be made via serial port or by
    TCP network on supported controllers(ESP8266, ESP32...)

    Class Variables
    ---------------
    CONNECT_FALSE : int
        Connect to controller failed
    CONNECT_TRUE : int
        Connect to controller was successful
    ERR_TIMEOUT : int
        Failed to send/receive a valid packet before timeout
        value was exceeded
    ERR_CRC : int
        Received CRC and calculated CRC did not match
    ERR_SER_PORT : int
        Serial port was disconnected
    ERR_INIT : int
        Communication object was not initialized
    MSG_GOOD : int
        Packet was sent/received successfully
    SER_DATA_F : int
        No serial data in buffer
    SER_DATA_T : int
        Serial data available to read
    PACK_TOO_BIG : int
        Sent/received packet was too large(More than 32 bytes)

    Instance Variables
    ----------------
    self.wait_time : number
        Contains time is seconds to wait before allowing transmission
        of packets after connecting to port
    self.coms : int/serial/socket object
        Network or Serial object
    self.coms_type: int
        Used to denote if connection type is network or serial.
        1 = Network communications. 0 = Serial communications.
        This variable will be automatically selected based on a
        successful call to any of the connect() functions       
    """
    #Status code constants
    CONNECT_FALSE = 0
    CONNECT_TRUE  = 1
    ERR_TIMEOUT   = 1
    ERR_CRC       = 2
    ERR_SER_PORT  = 3
    ERR_INIT      = 4
    MSG_GOOD      = 5
    SER_DATA_F    = 6
    SER_DATA_T    = 7
    PACK_TOO_BIG  = 8
    #Constants
    DELIM_CHAR    = b'|'
    REPLACE_CHAR  = b'+'

    
    def __init__(self):
        """
        Initializes the instance variables.
        """
        self.wait_time = 3
        self.coms = 0
        self.coms_type = 0
          
    def connect_port(self, port, baud_rate, timeout):               
        """
        Connects to controller over serial interface by port name. 

        Parameters
        -----------
        port : str 
            Port address of controller(eg "/dev/ttyUSB1" for
            Linux or "COM3" for Windows)
        baud_rate : int 
            Baud rate used to communicate with 
            controller(eg 19200 or 115200)
        timeout : number
            The time in seconds before a serial timeout(eg 1 or 0.5)

        Returns
        -------
        CONSTANT : int
            0 = Connect failed
            1 = Connect was succesfull
        """

        try:
            self.coms = serial.Serial(port, baudrate=baud_rate,
                                      timeout=timeout)
            sleep(self.wait_time) #Give time for controller to reset 
            self.coms.read() #Flush out garbage data  
                  
            return self.CONNECT_TRUE
        except serial.SerialException:
            return self.CONNECT_FALSE 
            pass    

    def connect_pid(self, pid, baud_rate, timeout):
        """
        Connects to controller over serial interface by PID
        (USB Product ID).  

        Parameters
        -----------
        pid : hex 
            PID of the controller(eg 0x6001)
        baud_rate : int 
            Baud rate used to communicate with 
            controller(eg 19200 or 115200)
        timeout : number
            The time in seconds before a serial timeout(eg 1 or 0.5)

        Returns
        -------
        CONSTANT : int
            0 = Connect failed
            1 = Connect was successful
        """
        
        connect_port = 0
        ports = serial.tools.list_ports.comports()
        for check in ports:
            if check.pid != None:
                if hex(check.pid) == hex(pid):
                    connect_port = check.device
                    break
        if connect_port == 0:
            return 0
        try:
            self.coms = serial.Serial(connect_port, baudrate=baud_rate,
                                      timeout=timeout)
            sleep(self.wait_time) #Give time for controller to reset
            self.coms.read() #Flush out garbage data
            return self.CONNECT_TRUE
        except serial.SerialException:
            return self.CONNECT_FALSE

    def connect_net(self, address, port, timeout, connect_timeout=5):
        """
        Connects to controller over TCP network.
        
        Parameters
        ----------
        address : str
            IP address of the controller to connect to(eg "10.0.0.1")
        port : int
            Port number of controller to connect to(eg 1336)
        timeout : number
            Time in seconds network functions will wait before timing
            out.
        connect_timeout(Optional) : number
            Time in seconds a connection will wait for before timing
            out. Default time is 5 seconds

        Returns
        -------
        CONSTANT : int
            0 = Connect failed
            1 = Connect was successful
        """

        self.coms = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.coms.settimeout(connect_timeout)
            self.coms.connect((address, port))
            self.coms.settimeout(timeout)
            self.coms_type = 1
        except:
            return self.CONNECT_FALSE
        return self.CONNECT_TRUE
            
    def send_packet(self, packet):
        """
        Sends a packet to a connected controller.

        Parameters
        ----------
        packet : bytes
            Bytes to be wrapped in packet and sent to controller
        
        Returns
        -------
        CONSTANT : int
            1 = Timeout 
            3 = Serial port disconnected
            4 = Communication object not initialized
            5 = Packet sent successfully
            8 = Packet data was too large(more than 32 bytes)
        """

        if self.coms == 0:
            return self.ERR_INIT 
        if len(packet) > 32:
            return self.PACK_TOO_BIG
        calc_crc = self.get_crc(packet)         
        crc_bytes = struct.pack('>H', calc_crc)
        reply = self.bit_encode(packet)
        bitmap = struct.pack('>I', reply[0])
        packet = reply[1]
        if self.coms_type == 0:
            try:                    
                self.coms.write(self.DELIM_CHAR + crc_bytes + bitmap
                                + packet + self.DELIM_CHAR)
            except serial.SerialException:
                return self.ERR_SER_PORT
        if self.coms_type == 1:
            try:
                self.coms.sendall(self.DELIM_CHAR + crc_bytes + bitmap 
                                  + packet + self.DELIM_CHAR)
            except:
                return self.ERR_TIMEOUT
        return self.MSG_GOOD    

    def recv_packet(self):
        """
        Receives a packet and decodes it. Will block until timeout
        occurs if no data is available.
        
        Returns
        -------
        NO_NAME : list
        A list type with two values
        index[0] : int
            1 = Timeout trying to get packet
            2 = CRC mismatch
            3 = Serial port disconnected
            4 = Communication object not initialized
            5 = Valid packet received successfully 
            8 = Received packet too large to parse
        index[1] : bytes
            data contains the decoded packet if successful or a
            space ' ' if not successful
        """

        if self.coms == 0:
             return [self.ERR_INIT, b' ']
        packet = bytearray()
        if self.coms_type == 0:
            try:
                if len(self.coms.read_until(self.DELIM_CHAR)) == 0:
                    return [self.ERR_TIMEOUT, b' ']          
            except serial.SerialException:
                return [self.ERR_SER_PORT, b' ']   
            try:
                packet = self.coms.read_until(self.DELIM_CHAR)
                if packet == self.DELIM_CHAR:
                    packet = self.coms.read_until(self.DELIM_CHAR)
            except serial.SerialException:
                return [self.ERR_SER_PORT, b' ']

        if self.coms_type == 1:
            bytes_read = 0
            try:
                while self.coms.recv(1) != self.DELIM_CHAR:
                    pass
            except:
                return [self.ERR_TIMEOUT, b' ']
            while (True):
                try:
                    current_byte = self.coms.recv(1)
                    bytes_read += 1
                except:
                    return [self.ERR_TIMEOUT, b' ']
                if current_byte == self.DELIM_CHAR and bytes_read > 1:
                    packet.append(ord(current_byte))
                    break
                else:
                    packet.append(ord(current_byte))
        if len(packet) < 7:
            return [self.ERR_TIMEOUT, b' ']
        packet = packet[0:len(packet) - 1]#discard delim char
        if len(packet) > 32:
            return [self.PACK_TOO_BIG, b' ']        
        check_raw = packet[0:2]
        recv_check = struct.unpack('>H', check_raw)
        bitmap_raw = packet[2:6]
        bitmap = struct.unpack('>I', bitmap_raw)
        decoded_packet = bytes(self.bit_decode(bitmap[0], packet[6:len(packet)]))       
        calc_check = self.get_crc(decoded_packet)
        if calc_check != recv_check[0]:
            return [self.ERR_CRC, b' ']    
        return [self.MSG_GOOD, decoded_packet]

    def close(self):
        """
        Closes a serial or network connection.

        """
        if self.coms != 0:
            self.coms.close()
        self.coms = 0

    def serial_available(self):
        """
        Checks to see if there are any packets waiting on a SERIAL
        CONNECTION. Can NOT be used on network based communications. 
        This function should be run before calling recv_packet() if 
        blocking the main loop is a concern.

        Returns
        -------
        CONSTANT : int
            3 = Serial port disconnected
            4 = Communication object not initialized
            6 = No serial data waiting
            7 = Serial data waiting           
        """
        if self.coms == 0:
            return self.ERR_INIT
        try:
            buff_size = self.coms.in_waiting
        except OSError:
            return self.ERR_SER_PORT
        if buff_size == 0:
            return self.SER_DATA_F
        else:
            return self.SER_DATA_T

    def bit_decode(self, bitmap, buff):
        """
        Decodes a packet using a 32 bit bitmap to allow binary data
        to be received.

        Parameters
        ----------
        bitmap : int
            32 bit int that contains the received bitmap
        buff : bytes
            Buffer containing the unencoded packet
        
        Returns
        -------
        buff : bytearray
            Contains the decoded packet buffer       
        """

        buff_idx = 0
        buff = bytearray(buff)
        for data in buff:
            if (bitmap >> buff_idx & 0x01) == 1:
                buff[buff_idx] = ord(self.DELIM_CHAR)
            buff_idx += 1
        return buff

    def bit_encode(self, buff):
        """
        Encodes a packet to allow binary data to be sent over
        connection.

        Parameters
        ----------
        buff: bytes
            Buffer containing unencoded data
        
        Returns
        -------
        encoded : list
            encoded[0] : int
                Calculated bitmap for transmission
            encoded[1] : bytearray
                Encoded packet buffer
        """

        buff_idx = 0
        bitmap = 0
        encoded = []
        buff = bytearray(buff)
        for data in buff:
            if buff[buff_idx] == ord(self.DELIM_CHAR):
                buff[buff_idx] = ord(self.REPLACE_CHAR)
                mask = 1 << buff_idx
                bitmap = bitmap | mask
            buff_idx += 1
        encoded.append(bitmap)
        encoded.append(buff)
        return encoded

    def get_crc(self, packet):
        """
        Calculates a 16 bit Xmodem CRC value.

        Parameters
        ----------
        packet : bytes/bytearray
            Contains the raw packet used to calculate CRC values

        Returns
        -------
        crc : int
            16 bit int containing the calculated CRC value       
        """

        crc = 0 
        for cur_byte in packet:
            crc = crc ^ (cur_byte << 8)
            for bits in range(0, 8):
                if (crc & 0x8000):
                    crc = (crc << 1) ^ 0x1021
                else:
                    crc <<= 1
        crc = crc & 0xFFFF
        return crc
