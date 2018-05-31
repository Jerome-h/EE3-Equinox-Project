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

bool sendATcommand(String& ATcommand, String& expected_answer, unsigned int& timeout) {
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
    httppost();
  }
  delay(1000);
}

void configure_FTP() {

  sendATcommand("AT+SAPBR=3,1,"Contype","GPRS"", "OK", 2000);
  sendATcommand("AT+SAPBR=3,1,"APN","APN"", "OK", 2000);
  sendATcommand("AT+SAPBR=3,1,"USER","user_name"", "OK", 2000);
  sendATcommand("AT+SAPBR=3,1,"PWD","password"", "OK", 2000);

  while (sendATcommand("AT+SAPBR=1,1", "OK", 20000) != 1);
  sendATcommand("AT+FTPCID=1", "OK", 2000);
  sendATcommand("AT+FTPSERV="ftp.yourserver.com"", "OK", 2000);
  sendATcommand("AT+FTPPORT=21", "OK", 2000);
  sendATcommand("AT+FTPUN="user_name"", "OK", 2000);
  sendATcommand("AT+FTPPW="password"", "OK", 2000);

}


void uploadFTP() {

  sendATcommand("AT+FTPPUTNAME="file_name"", "OK", 2000); // AT+FTPPUTNAME command sets up the FTP upload file name
  sendATcommand("AT+FTPPUTPATH=" / path"", "OK", 2000);   // T+FTPPUTPATH sets up the path name of the upload file

  // AT+FTPPUT sets up the FTP upload
  // mode '1' is to open FTP put session
  if (sendATcommand("AT+FTPPUT=1", "+FTPPUT:1,1,", 30000) == 1) {
    data_size = 0;
    while (Serial.available() == 0);
    aux = Serial.read();

    // loops while aux is not carriage Return
    do {
      data_size *= 10;
      data_size += (aux - 0x30);
      while (Serial.available() == 0);
      aux = Serial.read();
    } while (aux != 0x0D);

    if (data_size >= 100) {

      // mode '2' is for write FTP upload data
      if (sendATcommand("AT+FTPPUT=2,100", "+FTPPUT:2,100", 30000) == 1) {
        Serial.println(sendATcommand(testString, "+FTPPUT:1,1", 30000), DEC);
        Serial.println(sendATcommand("AT+FTPPUT=2,0", "+FTPPUT:1,0", 30000), DEC);
        Serial.println("Upload done!!");
      }
      else {
        sendATcommand("AT+FTPPUT=2,0", "OK", 30000);
      }
    }
    else {
      sendATcommand("AT+FTPPUT=2,0", "OK", 30000);
    }
  }
  else {
    Serial.println("Error openning the FTP session");
  }
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
