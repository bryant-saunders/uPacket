/* 
 uPacket demo showing a simplex stream to a connected
 python program. Example will read the IO pin state
 of button_pin and send the value every 0.5 seconds.
 Attach a button to 3.3v and resistor to ground
 on button_pin to change state.
*/

#include <uPacket.h>
uPacket upack;
//Initialize uPacket

byte button_pin = 14;
uint32_t baud_rate = 19200;

void setup(){
  Serial.begin(baud_rate);
  //Init serial port
  upack.begin(Serial);
  //Start uPacket by passing Stream object
  pinMode(button_pin, INPUT);
}

void loop() {
  byte button_state = digitalRead(button_pin);
  if (button_state == 0){
    upack.send_packet("Button = 0", 10);
  }
  if (button_state == 1){
    upack.send_packet("Button = 1", 10); 
  }
  delay(500);
}
