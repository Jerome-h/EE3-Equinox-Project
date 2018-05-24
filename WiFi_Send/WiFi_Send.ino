// Uses ESP8266 WiFi module to send data to a server database.
// Data is being taken from a PIR sensor for motion detection as an example
// Author: Jerome Hallett

#include <SoftwareSerial.h>

#define ESP_rxPin 6 //Connect to tx of ESP
#define ESP_txPin 7 //Connect to rx of ESP
#define pirPin 8 //PIR sensor pin

//Enter your SSID and password below
String ssid = "";
String password = "";

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

// Reset the ESP8266 module
void reset() {
  esp.println("AT+RST");
  delay(1000);
  if (esp.find("OK") ) Serial.println("Module Reset");
}

// Connect to your wifi network
void connectWifi() {
  String cmd = "AT+CWJAP=\"" + ssid + "\",\"" + password + "\"";
  esp.println(cmd);
  delay(5000);
  if (esp.find("OK")) {
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
    httppost();
  }
  delay(1000);
}

void httppost () {
  String data = "Motion Detected!";
  esp.println("AT+CIPSTART=\"TCP\",\"" + server + "\",80");//start a TCP connection.
  if ( esp.find("OK")) {
    Serial.println("TCP connection ready");
  } delay(1000);
  String postRequest =
    "POST " + uri + " HTTP/1.0\r\n" +
    "Host: " + server + "\r\n" +
    "Accept: *" + "/" + "*\r\n" +
    "Content-Length: " + data.length() + "\r\n" +
    "Content-Type: application/x-www-form-urlencoded\r\n" +
    "\r\n" + data;
  String sendCmd = "AT+CIPSEND=";//determine the number of characters to be sent.
  esp.print(sendCmd);
  esp.println(postRequest.length() );
  delay(500);
  if (esp.find(">")) {
    Serial.println("Sending.."); esp.print(postRequest);
    if ( esp.find("SEND OK")) {
      Serial.println("Packet sent");
      while (esp.available()) {
        String tmpResp = esp.readString();
        Serial.println(tmpResp);
      }
      // close the connection
      esp.println("AT+CIPCLOSE");
    }
  }
}
