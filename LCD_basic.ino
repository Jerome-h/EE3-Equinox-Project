/*
 The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int delayPrint = 5000;
bool sent = false;
float level = 10.0;
float flow = 24.0;
float turb = 5.0;
float pH = 7.0;
float temp = 24.0;
float EC = 0.05;
float Temp = 24.0;
float Health = 80.5;
float dataWater[6] = {level, flow, turb, pH, temp, EC};
float dataBattery[2] = {Temp, Health};

void dataUpload(bool sent){
  //while sending data
  if(sent == false){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("uploading data...");
  }
  
  //when data sent
  else{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("data uploaded");
  }
}

void dataPrintWater(float data[]){
  lcd.clear();
    lcd.setCursor(0,0); lcd.print("level:"); lcd.setCursor(8,0);lcd.print("flow:");
    lcd.setCursor(0,1); lcd.print(data[0]); 
    lcd.setCursor(8,1); lcd.print(data[1]); 
  delay (delayPrint);
  
  lcd.clear();
    lcd.setCursor(0,0); lcd.print("turb:"); lcd.setCursor(8,0);lcd.print("pH:");
    lcd.setCursor(0,1); lcd.print(data[2]);
    lcd.setCursor(8,1);lcd.print(data[3]);
  delay (delayPrint);

  lcd.clear();
    lcd.setCursor(0,0); lcd.print("temp:");lcd.setCursor(8,0);lcd.print("EC:");
    lcd.setCursor(0,1); lcd.print(data[4]);
    lcd.setCursor(8,1);lcd.print(data[5]); 
  delay (delayPrint);
}

void dataPrintBattery(float data[]){
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Temp: "); lcd.setCursor(8,0); lcd.print("Health: ");
  lcd.setCursor(0,1); lcd.print(data[0]);
  lcd.setCursor(8,1); lcd.print(data[1]); lcd.print("%");
  delay (delayPrint);
}

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  //lcd.setCursor(0,0);
  //lcd.print("hello, world!");
  Serial.begin(9600);
}

void loop() {
  // Turn off the display:
  //lcd.noDisplay();
  
  // Turn on the display:
  //lcd.display();
  
  dataPrintWater(dataWater);
  dataPrintBattery(dataBattery);
  sent = false;
  dataUpload(sent);
  delay(delayPrint);
  sent = true;
  dataUpload(sent);
  delay(delayPrint);
}
