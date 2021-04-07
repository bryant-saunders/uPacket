/* 
 uPacket example program that demonstrates protocol
 over a WiFi connection. Flash the built in LED
 Sets up a basic TCP server.
 IP address of the server will be printed on the
 serial monitor. Must run the uPacket_wifi.py example
 to communicate with controller.

 NOTE: On some platforms, the builtin LED is
 switched so that a HIGH write will result in
 the LED going off and a LOW will turn it on.
*/

//Uncomment WiFi library for your controller
//#include "WiFi.h"//ESP32
#include <ESP8266WiFi.h>//ESP8266
#include <uPacket.h>

//Enter your network details
const char* ssid = "SSID"; //Your SSID
const char* pass = "PASS"; //Your password
char recv_data[40] = " ";
//Char array should be large enough to hold data 
//plus protocol overhead
uint16_t port = 1336;

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
  //Start TCP server
  pinMode(LED_BUILTIN, OUTPUT);
}

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

void loop() {
  WiFiClient client = server.available();
  if (client){
    Serial.print("Connected");
    upack.begin(client);
    //Start uPacket if client connected
  }
  while(client){   
    byte packet_check = upack.recv_packet(recv_data, sizeof(recv_data));
    if (packet_check == MSG_GOOD){
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
}
