#include "Arduino.h"
#include "uPacket.h"

uPacket::uPacket(void){
  /*
   Object intializer
  */
}

void uPacket::begin(Stream &com_port) {
  /*
   Initialize communication port. Accepts any Stream type.
   Delays as long as RESET_TIME is set for to allow ports to settle.

   Parameters
   ----------
   com_port : Stream
       Passed Stream object used to communicate with a uPacket 
       enabled device 
  */
  _port = &com_port;
  _port->setTimeout(TIMEOUT);
  delay(RESET_TIME);//Give time to reset.
}

uint8_t uPacket::recv_packet(char recv_buff[], size_t recv_buff_size) {
  /* 
   Receives a packet. Fills the passed char array with the decoded
   data. Will return immediately if there is no data to read or return
   after TIMEOUT is exceeded if transmission is interrupted.

   Parameters
   ----------
   recv_buff : char array
       Contains the parsed packet. Array size must be large
       enough to contain the packet data you wish to receive.
       The protocol supports a maximum length of 32 bytes per packet
   recv_buff_size : size_t
       Size in bytes of the passer char array

   Returns
   -------
   CONSTANT : uint8_t
       Contains the status code of the transaction. Use this to 
       determine if passed char array is valid.
       0 = No packets available to read
       1 = Function timed out trying to receive packet
       2 = Received CRC did not match calculated CRC
       3 = Invalid packet
       4 = Packet was decoded successfully
  */

  uint16_t recv_crc = 0;
  unsigned long start_ms = 0;
  uint32_t bitmap = 0;
  int recv_char = 0;
  byte buff_size = 0;
  byte buff_idx = 0;
  
  if (_port->available() < 1){
    return ERR_NODATA;
  }
  start_ms = millis();
  while(_port->read() != DELIM_CHAR){
    if (millis() - start_ms > TIMEOUT){
      return ERR_TIMEOUT;     
    }   
  } 
  while(_port->available() < 7){
    if (millis() - start_ms > TIMEOUT){
      return ERR_INVALID;     
    }
  }
  recv_crc = _port->read() << 8;
  recv_crc |= _port->read();
  for(byte i=0; i<4; i++){
    bitmap = bitmap << 8;
    bitmap |= _port->read();     
  } 
  while(true){   
    if (_port->available() > 0){
      recv_char = _port->read();
      if (recv_char == DELIM_CHAR){
        buff_size = buff_idx;
        break;
      }
      recv_buff[buff_idx] = recv_char;
      buff_idx++;
    }
    if (millis() - start_ms > TIMEOUT){
      return ERR_INVALID;     
    }
    if (buff_idx >= recv_buff_size){
      return ERR_INVALID;
    }
  }
  bit_decode(bitmap, recv_buff, buff_size);
  uint16_t calc_crc = get_crc(buff_size, recv_buff);
  if (calc_crc != recv_crc){
    return ERR_CRC;
  }
  if (CHK_FOR_PREV == 1){
    byte drain_len = _port->available();
    for(byte i=0; i<drain_len; i++){
      byte drain_rx = _port->read();
    }
  }
  return MSG_GOOD;
}

void uPacket::send_packet(const char send_buff[], byte buff_size){
  /* 
   This function will packetize the passed char array 
   and send it to a connected device.
  
   Parameters
   ----------
   send_buff : const char array
       Buffer containing data to be sent
   buff_size : byte
       Value containing the amount of bytes to be sent 
  */
  
  uint16_t calc_crc = get_crc(buff_size, send_buff);
  uint32_t bitmap = bit_encode(send_buff, buff_size);
  _port->print(DELIM_CHAR);
  _port->write((calc_crc >> 8) & 0xFF);
  _port->write(calc_crc & 0xFF);
  _port->write((bitmap >> 24) & 0xFF);
  _port->write((bitmap >> 16) & 0xFF);
  _port->write((bitmap >> 8) & 0xFF);
  _port->write(bitmap & 0xFF);
  for (byte i = 0; i < buff_size; i++){
    if (send_buff[i] == DELIM_CHAR){
      _port->write(REPLACE_CHAR);
    }
    else{
      _port->write(send_buff[i]);
    }
  }
  _port->print(DELIM_CHAR);
}

void uPacket::bit_decode(uint32_t bitmap, char buff[], byte buff_size){
  /*
   Decodes an incoming packet. Uses a 32 bit bitmap to determine if the
   function needs to replace a dummy byte with the delimiter value.
   Fills the passed buff[] with the decoded data.
   
   Parameters
   ----------
   bitmap : uint32_t
       Contains the 32 bit bitmap used to decode buff[]
   buff : char array
       Data buffer to operate on
   buff_size : byte
       Value used to determine how much of the passed char array needs
       to be processed
  */

  for(uint8_t i=0; i<buff_size; i++){
    if(bitRead(bitmap, i) == 1){
      buff[i] = DELIM_CHAR;
    }
  }
}

uint32_t uPacket::bit_encode(const char buff[], byte buff_size){
  /*
   Encodes an outgoing packet. Creates a 32 bit bitmap and replaces any
   delimiter char with a dummy value if needed.
   Fills the passed char array with the encoded data.
   
   Parameters
   ----------
   buff : const char array
       Data buffer to operate on
   buff_size : byte
       Value used to determine how much of the passed char array needs
       to be processed
   
   Returns
   -------
   bitmap : uint32_t
       Calculated bitmap to be sent with packet
  */
  
  uint32_t bitmap = 0;
  for(uint8_t i = 0; i<buff_size; i++){
    if (buff[i] == DELIM_CHAR){
      bitWrite(bitmap, i, 1);
    }
  }
  return bitmap;
}

uint16_t uPacket::get_crc(byte buff_size, const char buff[]){
  /* 
   16bit CRC algorithm using the xmodem standard.
   
   Parameters
   ----------
   buff_size : byte
       Size of buffer to calculate CRC value for
   buff : const char array
       Buffer used to calculate value
   
   Returns
   -------
   crc : uint16_t
       16 bit CRC calculated using passed char array
  */

  uint16_t crc = 0;
  for(uint8_t char_index = 0; char_index < buff_size; char_index++){  
      crc = crc ^ ((uint16_t)buff[char_index] << 8);
      for (uint8_t i = 0; i < 8; i++) {
        if (crc & 0x8000)
          crc = (crc << 1) ^ 0x1021;
        else
          crc <<= 1;
      }   
  }  
  return crc; 
}
