// Uses ESP8266 WiFi module to send data to a server database.
// Data is being taken from a PIR sensor for motion detection as an example
// Author: Jerome Hallett

#include <SoftwareSerial.h>

#define ESP_rxPin 6 //Connect to tx of ESP
#define ESP_txPin 7 //Connect to rx of ESP
#define pirPin 8 //PIR sensor pin

//Enter your SSID and password below
String ssid = "TNCAP67FB25";
String password = "28F88D9429";

SoftwareSerial esp(ESP_rxPin, ESP_txPin);// RX, TX

String server = "frp.drivehq.com"; // www.example.com
String uri = "yourURI";// our example is /esppost.php

int pirState = LOW; // We start assuming no motion detected

void setup() {
  //Pin Modes
  pinMode(ESP_rxPin, INPUT);
  pinMode(ESP_txPin, OUTPUT);
  pinMode (pirPin, INPUT);

  esp.begin(115200);
  esp.listen();
  Serial.begin(9600);
  reset();
  connectWifi();
}

bool sendATcommand(const String& ATcommand, const char& expected_answer, unsigned int& timeout) {
  esp.println(ATcommand);
  delay(timeout);
  if (esp.find(expected_answer) ) return true;
  else return false;
}

// Reset the ESP8266 module
void reset() {
  if (sendATcommand("AT+RST","OK",3000)) Serial.println("Module Reset");
}

// Connect to your wifi network
void connectWifi() {
  String cmd = "AT+CWJAP=\"" + ssid + "\",\"" + password + "\"";
  if (sendATcommand(cmd,"OK",5000)) {
    Serial.println("Connected!");
  }
  else {
    Serial.println("Cannot connect to wifi");
    connectWifi();
  }
}

//Function to detect motion using PIR sensor. Returns true if motion detected
bool motionDetect() {
  int val = digitalRead(pirPin);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    if (pirState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      // We only want to print on the output change, not state
      pirState = HIGH;
      return true;
    }
  } else {
    if (pirState == HIGH) {
      // we have just turned off
      Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      pirState = LOW;
    }
  }
  return false;
}

void loop () {
  if (motionDetect ()) {
  }
  delay(1000);
}

