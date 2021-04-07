/* 
 uPacket server program demonstrates two controllers
 communicating over serial connection. The server
 demo receives commands from the client and sends
 back a response.

 uPacket_client.ino must be run on other controller.
 This demo uses the SoftwareSerial library.
 
 TX pin should be attached to the RX pin of the other
 controller and the RX pin should be attached
 to TX pin of other controller.
 Controllers NEED to have a common ground connection.
 
 NOTE: On some platforms, the builtin LED is
 switched so that a HIGH write will result in
 the LED going off and a LOW will turn it on.
*/

#include <uPacket.h>
#include <SoftwareSerial.h>
uPacket upack;
//Initialize uPacket
SoftwareSerial soft_ser(4, 5);
//Init software serial. Choose TX/RX pins that suit
//your hardware

char recv_data[40] = " ";
//Char array should be large enough to hold data 
//plus protocol overhead
uint32_t baud_rate = 19200;

void led_on(){
  digitalWrite(LED_BUILTIN, HIGH);
  upack.send_packet("LED=ON", 7);
  //Send packet to client controller.
  //Must specify number of chars to send
}

void led_off(){
  digitalWrite(LED_BUILTIN, LOW);
  upack.send_packet("LED=OFF", 8);
  //Send packet to client controller.
  //Must specify number of chars to send
}

void setup(){
  soft_ser.begin(baud_rate);
  upack.begin(soft_ser);
  //Start uPacket by passing Software Serial object
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  byte packet_check = upack.recv_packet(recv_data, sizeof(recv_data));
  //get status code
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
