/* 
 uPacket example program to flash onboard LED. Must
 run the python program uPacket_basic.py to 
 communicate with this program.
 NOTE: On some platforms, the builtin LED is
 switched so that a HIGH write will result in
 the LED going off and a LOW will turn it on.
*/

#include <uPacket.h>
uPacket upack;
//Initialize uPacket

char recv_data[40] = " ";
//Char array should be large enough to hold data 
//plus protocol overhead
uint32_t baud_rate = 19200;

void led_on(){
  digitalWrite(LED_BUILTIN, HIGH);
  upack.send_packet("LED=ON", 7);
 //Send packet to python. Must specify number of
 //chars to send
}
void led_off(){
  digitalWrite(LED_BUILTIN, LOW);
  upack.send_packet("LED=OFF", 8);
  //Send packet to python. Must specify number of
  //chars to send
}

void setup(){
  Serial.begin(baud_rate);
  //Init Serial port with baud rate 
  upack.begin(Serial);
  //Start uPacket by passing Serial object
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  byte packet_check = upack.recv_packet(recv_data, sizeof(recv_data));
  //Receive a packet. Fills passed char array with
  //data. Returns MSG_GOOD if char array is valid 
  if (packet_check == MSG_GOOD){   
    //Packet is valid
    if (strcmp(recv_data, "led_on") == 0){
      //recv_data matched. Turn LED on
      led_on();
    }
    if (strcmp(recv_data, "led_off") == 0){
      //recv_data matched. Turn LED off
      led_off();           
    }
  }
}
