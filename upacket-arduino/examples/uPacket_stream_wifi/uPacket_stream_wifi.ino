/* 
 uPacket demo showing a simplex stream to a connected
 python program. Example will read the IO pin state
 of button_pin and send the value every 0.5 seconds.
 Attach a button to VCC and resistor to ground
 on button_pin to change state.

 Sets up a basic TCP server.
 IP address of the server will be printed on the
 serial monitor. Must run the uPacket_stream_wifi.py
 example to communicate with the controller.
*/

//Uncomment WiFi library for your controller
//#include "WiFi.h"//ESP32
#include <ESP8266WiFi.h>//ESP8266
#include <uPacket.h>

const char* ssid = "SSID"; //Your SSID
const char* pass = "PASS"; //Your password
uint16_t port = 1336;
byte button_pin = 14;

WiFiServer server(port);
//Intialize WiFi server
uPacket upack;
//Initialize uPacket

void setup(){
  Serial.begin(19200);
  WiFi.begin(ssid, pass);
  //Connect to network
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println(WiFi.localIP());
  server.begin();
  pinMode(button_pin, INPUT);
}

void loop(){
  WiFiClient client = server.available();
  if (client){
    Serial.print("Connected");
    upack.begin(client);
    //Start uPacket if client connected
  }
  while(client){
    byte button_state = digitalRead(button_pin);
    //Read button I/O pin and send packet based on
    //value of the state
    if (button_state == 0){
      upack.send_packet("Button = 0", 10);
    }
    if (button_state == 1){
      upack.send_packet("Button = 1", 10); 
    }
    delay(500);
  } 
}
