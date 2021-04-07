/* 
 uPacket client program demonstrates two controllers
 communicating over serial connection. The client
 demo sends commands to the server and receives a
 reply. The client then flashes it's built in LED
 depending on the response.
 
 uPacket_server.ino must be run on other controller.
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
}

void led_off(){
  digitalWrite(LED_BUILTIN, LOW);
}

void setup(){
  soft_ser.begin(baud_rate);
  upack.begin(soft_ser);
  //Start uPacket by passing Software Serial object
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  byte packet_check = 0;
  upack.send_packet("led_on", 7);
  //Send packet to server controller
  while(packet_check != MSG_GOOD){
    //loop until valid packet received
    packet_check = upack.recv_packet(recv_data, sizeof(recv_data));
  }
  if (strcmp(recv_data, "LED=ON") == 0){
    led_on();
  }
  delay(500);
  packet_check = 0;
  
  upack.send_packet("led_off", 8);
  //Send packet to server controller
  while(packet_check != MSG_GOOD){
    //loop until valid packet received
    packet_check = upack.recv_packet(recv_data, sizeof(recv_data));
  }
  if (strcmp(recv_data, "LED=OFF") == 0){
    led_off();
  }
  delay(500);
}
