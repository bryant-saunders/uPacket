#ifndef uPacket_h
#define uPacket_h

#include "Arduino.h"

//return codes
#define ERR_NODATA  0 //No packets waiting in receive buffer
#define ERR_TIMEOUT 1 //Read timed out
#define ERR_CRC     2 //CRC does not match received value
#define ERR_INVALID 3 //Packet was an invalid format
#define MSG_GOOD    4 //Message read with no errors

class uPacket
{
  public:
    //Configuration
    bool CHK_FOR_PREV = 1; 
    //1 = Drain RX if more bytes after parsing. 0 = Don't drain RX buffer
    char DELIM_CHAR = '|'; 
    //This is the delimiting character.
    char REPLACE_CHAR = '+';
    //This is the dummy char used to replace the delimiter in packets
    unsigned long RESET_TIME = 1000;
    //How long controller will wait after reset
    unsigned long TIMEOUT = 50; 
    //How long(in mS) recv_packet() will wait for packet data

    //Functions
    uPacket(void);
    void begin(Stream &serial_port);
    uint8_t recv_packet(char recv_buff[], size_t recv_buff_size);
    void send_packet(const char send_buff[], byte buff_size);
    void bit_decode(uint32_t bitmap, char buff[], byte buff_size);
    uint32_t bit_encode(const char buff[], byte buff_size);
    uint16_t get_crc(byte buff_size, const char buff[]);
  private:
    Stream * _port;  
};

#endif
