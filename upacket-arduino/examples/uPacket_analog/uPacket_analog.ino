/*
 uPacket demo program that demonstrates sending and
 recieving binary data. You will need a LED and 
 resistor attached to pwm_out_pin. You will also need
 a potentiometer, photocell or some other analog
 sensor on adc_in_pin. Must run the python program
 uPacket_analog.py to communicate with this program.
 
 NOTE: Some hardware(ESP32) does not support the 
 analogWrite() function.
 
*/

#include <uPacket.h>
uPacket upack;
//Initialize uPacket

int adc_in_pin = A0;//pin used for ADC read
int pwm_out_pin = 16;//pin used for PWM out
char recv_data[40] = " ";
//Char array should be large enough to hold data 
//plus protocol overhead
uint32_t baud_rate = 19200;

void read_adc(){
  //Reads value from ADC, converts value to bytes
  //and sends packet to python
  uint16_t adc_value = analogRead(adc_in_pin);
  char adc_data[2] = " ";
  adc_data[0] = (adc_value >> 8) & 0xFF;
  adc_data[1] = adc_value & 0xFF;
  upack.send_packet(adc_data, 2);   
}

void pwm_out(char packet_data[]){
  //Takes passed packet and converts char at index[8]
  //into a unsigned value. Writes value to analog
  //pin and sends a packet to python
  uint8_t pwm_val = packet_data[8];
  analogWrite(pwm_out_pin, pwm_val);
  upack.send_packet("PWM set", 7);
}

void setup() {
  Serial.begin(baud_rate);
  //Init serial port
  upack.begin(Serial);
  //Start uPacket by passing Stream object
}

void loop() {
  byte packet_check = upack.recv_packet(recv_data, sizeof(recv_data));
  //get status code
  if (packet_check == MSG_GOOD){
    //Packet is valid
    if (strcmp(recv_data, "read_adc") == 0){
      read_adc();
    }
    if (strcmp(recv_data, "pwm_out") == 0){
      pwm_out(recv_data);
    }
  }
}
