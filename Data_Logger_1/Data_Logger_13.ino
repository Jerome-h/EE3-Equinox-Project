// maybe have a few sensors measuring different things that are related (reduce chance that data is corrupted by one broken sensor)
// maybe have a way to run diagnostics (related to point above)

#include<SD.h>      // standard SD libary
#include<SPI.h>     // standard SPI libary
#include <SoftwareSerial.h>
#include <String.h> // standard libary for string manipulation 
#include <Wire.h>   // needed for clock module DS3231
#include "DS3231.h" // needed for clock module DS3231
#include <OneWire.h>            // needed for measuring water temperature and electrical conductivity
#include <DallasTemperature.h>  // needed for measuring water temperature and electrical conductivity
#include "SIM900.h" // needed for SIM900 module
#include "inetGSM.h"// needed for SIM900 module 


File dataFile;            // defines a file object
const int CSpin = 53;     // defines chip select pin for arduino (uno pin 10, mega pin 53)
RTClib RTC;               // needed for clock
volatile int pulseCount;  // counts the rising edges of a signal (for flow rate)
float batteryTMean = 0.0; // mean battery temperature
int ONE_WIRE_BUS = 14;     // Digital pin for water temperature
OneWire oneWire(ONE_WIRE_BUS); // oneWire instance to communicate with any OneWire devices 
DallasTemperature sensors(&oneWire); // Pass oneWire instance by reference to Dallas Temperature
InetGSM inet;             // needed for data transmission

struct sensor {
  const int sensorType;                            
  const int PinA;
  const int PinB;
  const int PinC;
};

struct waterLevel {
  float distance;
  float volume;
  int percentFull;
};




/* _________________________________________________________________________
 * 
 * User input
 * _________________________________________________________________________
 */

/*  Pre-set sensors:
 *   (1)  Water level (ultrasonic distance sensor) 
 *   (2)  Flow rate (flow sensor)
 *   (3)  turbidity sensor (measure of relative water clarity)
 *   (4)  Battery temperature
 *   (5)  Battery health  *battery temperature must be measured previously to claculate battery health
 *   (6)  PH sensor
 *   (7)  Water temperature
 *   (8)  Electrical conductivitey
 */

/*  User input:
 *  Enter each sensor type as a number (see list above) with
 *  their corresponding input pin and output pin (output pin may not be required)
 *  Enter these number into an array (each element corresponds to one sensor)
 *  User must also enter the dimensions of the water tank for waer level calculations
 */
const int waterStatusFrequency = 5;       // Enter how often measurments for water status should be taken (in seconds)
const int batteryStatusFrequency = 15;       // Enter how often measurments for water status should be taken (in seconds)
const int loopDelay = 1000;

const int transmitFrequency = 60; // Enter how often measurments should transmitted (in seconds) 
const int sensorNum = 5;          // Enter the number of sensors

struct sensor sensorArray[sensorNum] = {{1, 5, 6, -1},{3, A10, -1, -1},{6, A12, -1, -1},{7, 14, -1, -1},{8, A11, -1, -1}};       
                                                          // Enter the sensor type, input pin and output pin
                                                          // sensor type found from list of pre-set sensors
                                                          // if no output pin is required enter -1 
                                                          // if adding a new sensor (not in pre-set list) put type as 0.
                                                          // i.e. if want ultrasonic distance sensor (with echo pin 2 and
                                                          // trigger pin 3) and a custom sensor (with input pin 4 and no 
                                                          // output pin) enter: {{1,2,3, -1},{0,4,-1, -1}};
                                                          
const float tankHeight = 1.0;   // Enter the height of the tank in meters
const float tankDiameter = 1.0; // Enter the Diameter of the tank in meters

// inputs required for data transmission
int dataByteCount = 1000;  // number of bytes transmitted (should be equal or larger than number of bytes stored in the file)
char filePath[]="/";                  // set the folder destination for the file to be uploaded
char APN[] = "everywhere"; 
char APNuserName[]="eesecure";
char APNpassWord[]="secure";
char FTPserverName[] = "ftp.drivehq.com";
char serverUserName[] = "omm15";
char serverPassWord[] = "Infinity25";

char waterStatusFile[] = "water.txt";     // Name of file containing water status measurements 
char batteryStatusFile[] = "battery.txt"; // Name of file containing battery status measurements 




/* Program that user will not have to change unless using a non-preset sensor:
 * 
 */


/* _________________________________________________________________________
 * 
 * Setup: initaialise sensor pins
 * _________________________________________________________________________
 */

  
void setup() {

  Serial.begin(9600);       // opens serial port, sets data rate to 9600 bps
  Wire.begin();             // needed for clock
  pinMode(CSpin, OUTPUT);   // needed for SD card
  sensors.begin();          // needed for water temperature
  // SD card check
  if(SD.begin()){
    Serial.println("SD card okay");
  }
  else{
    Serial.println("SD card error");  // at the moment if there is an SD
    Serial.println("Terminating program");
    return;                           // card error then program wont work
  }

  printHeader(waterStatusFile, "Time");
  printHeader(batteryStatusFile, "Time");

  // initialises sensor pins 
  for(int i = 0; i < sensorNum ; i++){
    switch (sensorArray[i].sensorType) {
      case 0:
        Serial.print("Replace by custom code");
        // Should be replaced   
        // by custom code for input and output pins 
        break;
      case 1: // sets up input and output pins for ultra sonic sensor
        pinMode(sensorArray[i].PinA, INPUT);
        pinMode(sensorArray[i].PinB, OUTPUT);
        printHeader(waterStatusFile, "Level");
        break;
      case 2: // sets up input pin for flow sensor
        pinMode(sensorArray[i].PinA, INPUT);
        printHeader(waterStatusFile, "Flow");
        break;
      case 3: // turbidity
        printHeader(waterStatusFile, "Turb");
        break;
      case 4: // Battery temperature
        printHeader(batteryStatusFile, "Temp");
        break;    
      case 5: // Battery health 
        pinMode(sensorArray[i].PinC, OUTPUT);
        printHeader(batteryStatusFile, "Health");
        break;    
      case 6: // PH
        printHeader(waterStatusFile, "PH");
        break; 
      case 7: // Temperature
        printHeader(waterStatusFile, "Temp");
        break;
      case 8:
        dataFile = SD.open(waterStatusFile, FILE_WRITE);
        printHeader(waterStatusFile, "EC");
        break;
      default:
        // return some kind of error
        Serial.println("No a valid sensor type");
        Serial.println("Terminating program");
        return;
        break; 
    }
  }
  printHeader(waterStatusFile, "\n");
  printHeader(batteryStatusFile, "\n");
}



/* _________________________________________________________________________
 * 
 * Main loop: Takes sensor measurments at specified time 
 * _________________________________________________________________________
 */

void loop() {

  DateTime now = RTC.now();
  //int secondNow = now.second();
  
  if(now.second() % batteryStatusFrequency == 0){
    logSensorMeasurements(now, batteryStatusFile, true);
  }

  if(now.second() % waterStatusFrequency == 0){
    logSensorMeasurements(now, waterStatusFile, false);
  }

  if(now.second() % 60 == 0){
    //sendDataFTP(batteryStatusFile);
    //sendData(waterStatusFile);
  }
  
  delay(loopDelay);
}

/*  _________________________________________________________________________
 *  _________________________________________________________________________
 * 
 *  DATA Transmission
 * __________________________________________________________________________
 * __________________________________________________________________________
 */

void sendData(char fileName[]){

  Serial.println("starting send data process");
  
  // opens a text file in the SD card with the file name
  dataFile = SD.open(fileName, FILE_READ);

  if(dataFile){
    Serial.println("Reading data file for transmission");
  }
  else{
    Serial.println("Error rerading data file for transmission");
  }

  char loggedData[dataByteCount];

  int count = 0;
  Serial.println(dataByteCount);

  char character;
  while(((character = dataFile.read()) != -1) && count < dataByteCount){
    Serial.print(character);
    loggedData[count]=character;
    count++;
    if(count == dataByteCount){
      Serial.println("");
      Serial.println("Increase dataByteCount");
    }
  }
  
  char dataArray[count];
  for(int i = 0; i < count; i++){
    dataArray[i]=loggedData[i];
  }

  Serial.println("connecting GSM Shield");
    
  boolean started = false; //Start configuration of shield with baudrate
  if (gsm.begin(4800)) {
    Serial.println("\nstatus=READY");
    started=true;
  } 
  else {
    Serial.println("\nstatus=IDLE");
  }
  
  if(started) {

//    char myFileName[]="owter.txt";      // name the file that will be recieved 
//    char myFileContent[] = "abcdefghij";       // initialise the content you want to send
//    int myCharNum = 10;
//    FTPfileTransfer(myFileName, myFileContent, myCharNum);

    Serial.println("starting FTP transfer");
    gsm.WhileSimpleRead(); // read until serial buffer is empty.
    FTPfileTransfer(fileName, dataArray, count);
  }

  dataFile.close();
}

void FTPfileTransfer(char fileName[], char fileContent[], int contentLength){

  int swreadLoop = 100;

  // checking connection signal quality
  Serial.println("\nTest Connection Quality (AT+CSQ)"); 
  gsm.SimpleWriteln("AT+CSQ");
  delay(1000);
  for(int i=0; i<swreadLoop; i++){
    serialswread();
  }

  // set connection type to GPRS
  Serial.println("\nConfigure connection parameters...");
  Serial.println("\nAT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");  
  gsm.SimpleWriteln("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  delay(1000);
  for(int i=0; i<swreadLoop; i++){
    serialswread();
  }

  // set APN  
  char APNchar[50];
  String APNstr = "\nAT+SAPBR=3,1,\"APN\",\"";
  concatStr(APNstr, APN, APNchar);
  Serial.println(APNchar);  
  gsm.SimpleWriteln(APNchar);
  delay(1000);
  for(int i=0; i<swreadLoop; i++){
    serialswread();
  }

  // set APN username
  char APNuserChar[50];
  String APNuserStr = "\nAT+SAPBR=3,1,\"USER\",\"";
  concatStr(APNuserStr, APNuserName, APNuserChar);
  Serial.println(APNuserChar);  
  gsm.SimpleWriteln(APNuserChar);
  delay(1000);
  for(int i=0; i<swreadLoop; i++){
    serialswread();
  }

  // set APN password
  char APNpassChar[50];
  String APNpassStr = "\nAT+SAPBR=3,1,\"PASS\",\"";
  concatStr(APNpassStr, APNpassWord, APNpassChar);
  Serial.println(APNpassChar); 
  gsm.SimpleWriteln(APNpassChar);
  delay(1000);
  for(int i=0; i<swreadLoop; i++){
    serialswread();
  }

  // enable the GPRS
  Serial.println("\nAT+SAPBR=1,1"); 
  gsm.SimpleWriteln("AT+SAPBR=1,1");
  delay(2000);
  for(int i=0; i<swreadLoop; i++){
    serialswread();
  }

  // If connection is setup properly, will get back an IP address
  Serial.println("\nAT+SAPBR=2,1"); 
  gsm.SimpleWriteln("AT+SAPBR=2,1");
  delay(1000);
  for(int i=0; i<swreadLoop; i++){
    serialswread();
  }
  
  Serial.println("\nSetting up FTP parameters...");

  // set FTP CID
  Serial.println("\nAT+FTPCID=1"); 
  gsm.SimpleWriteln("AT+FTPCID=1");
  delay(1000);
  for(int i=0; i<swreadLoop; i++){
    serialswread();
  }

  // set up FTP server name  
  char FTPserverChar[50];
  String FTPserverStr = "\nAT+FTPSERV=\"";
  concatStr(FTPserverStr, FTPserverName, FTPserverChar);
  Serial.println(FTPserverChar); 
  gsm.SimpleWriteln(FTPserverChar);
  delay(1000);
  for(int i=0; i<swreadLoop; i++){
    serialswread();
  }

  // server username
  char serverUserChar[50];
  String serverUserStr = "\nAT+FTPUN=\"";
  concatStr(serverUserStr, serverUserName, serverUserChar);
  Serial.println(serverUserChar); 
  gsm.SimpleWriteln(serverUserChar);
  delay(1000); 
  for(int i=0; i<swreadLoop; i++){
    serialswread();
  }

  // server password 
  char serverPassChar[50];
  String serverPassStr = "\nAT+FTPPW=\"";
  concatStr(serverPassStr, serverPassWord, serverPassChar);
  Serial.println(serverPassChar); 
  gsm.SimpleWriteln(serverPassChar);
  delay(1000); 
  for(int i=0; i<swreadLoop; i++){
    serialswread();
  }

  // name of the file that is being recieved
  char fileNameChar[50];  // file name (with extensioin) cannot be over 30 characters long
  String fileNameStr="\nAT+FTPPUTNAME=\"";
  concatStr(fileNameStr, fileName, fileNameChar);
  Serial.println(fileNameChar);
  gsm.SimpleWriteln(fileNameChar);
  delay(1000);
  for(int i=0; i<swreadLoop; i++){
    serialswread();
  }
  
  // path to upload file
  char filePathChar[50];  // file path cannot be over 30 characters long
  String filePathStr="\nAT+FTPPUTPATH=\"";
  concatStr(filePathStr, filePath, filePathChar);
  Serial.println(filePathChar);
  gsm.SimpleWriteln(filePathChar);
  delay(1000);
  for(int i=0; i<swreadLoop; i++){
    serialswread();
  }

  // enable FTP operation
  Serial.println("\nAT+FTPPUT=1");  
  gsm.SimpleWriteln("AT+FTPPUT=1");
  delay(7000);
  for(int i=0; i<500; i++){
    serialswread();
  }
  delay(500);

  // write FTP upload data size
  char dataSizeChar[50];
  String dataSizeStr = "AT+FTPPUT=2,";         
  dataSizeStr.concat(contentLength);     
  dataSizeStr.toCharArray(dataSizeChar,50); 
  Serial.println(dataSizeChar);
  gsm.SimpleWriteln(dataSizeChar);
  delay(1000);
  for(int i=0; i<swreadLoop; i++){
    serialswread();
  }

  // upload file content 
  Serial.println("printing file content:");  
  Serial.println(fileContent);  
  Serial.println("printed file content.");  
  gsm.SimpleWriteln(fileContent);
  delay(1000);
  for(int i=0; i <= contentLength+50 ; i++){
    serialswread();
  }

  // turn off FTP operation, no more data to be uploaded
  Serial.println("\nAT+FTPPUT=2,0");  
  gsm.SimpleWriteln("AT+FTPPUT=2,0");
  delay(1000);
  for(int i=0; i<swreadLoop; i++){
    serialswread();
  }
}

void serialswread(){
     gsm.SimpleRead();
}

void concatStr(String str1, String str2, char charArray[]){
  char bufferArray[50];
  str1.concat(str2);
  str1.concat("\"");
  str1.toCharArray(charArray, 50);
}

/*  _________________________________________________________________________
 *  _________________________________________________________________________
 * 
 *  DATA LOGGING
 * __________________________________________________________________________
 * __________________________________________________________________________
 */

void logSensorMeasurements(DateTime now, char fileName[], bool isBatterStatus){

  Serial.println(fileName);

  // creates or opens a text file in the SD card 
  dataFile = SD.open(fileName, FILE_WRITE);
  
  // prints time and date stamp to data file
  if(dataFile){
    dataFile.print(now.year());
    dataFile.print('/');
    dataFile.print(now.month());
    dataFile.print('/');
    dataFile.print(now.day());
    dataFile.print(' ');
    dataFile.print(now.hour());
    dataFile.print(':');
    dataFile.print(now.minute());
    dataFile.print(':');
    dataFile.print(now.second());
    dataFile.print(",");
    Serial.println("time written to file");
  }
  else{
    Serial.println("Error opening file");
  }

  if(isBatterStatus == true){
    // prints each sensor reading to the data file 
    for(int i = 0; i < sensorNum ; i++){
      switch (sensorArray[i].sensorType) {
        case 0:
          // return some kind of error which should be replaced   
          // by custom code for input and output pins 
          break;         
        case 1:
          break;     
        case 2:   
          break;
        case 3:          
          break;  
        case 4:
          float batteryT1, batteryT2, batteryT3;
    
          batteryT1 = batteryTemp(sensorArray[i].PinA, sensorArray[i].PinB);
          batteryT2 = batteryTemp(sensorArray[i].PinA, sensorArray[i].PinB);
          batteryT3 = batteryTemp(sensorArray[i].PinA, sensorArray[i].PinB);
          batteryTMean = (batteryT1+batteryT2+batteryT3)/3.0;
    
          //dataFile.print("Battery temperature: ");
          dataFile.print(batteryTMean);
          dataFile.print(",");
          
          //Serial.print("Battery temperature: ");
          Serial.print(batteryTMean); 
          Serial.print(",");
          
          break;   
        case 5:
          float stateHealth1, stateHealth2, stateHealth3, stateHealthMean;
          
          stateHealth1 = batteryHealth(sensorArray[i].PinA, sensorArray[i].PinB, sensorArray[i].PinC);
          stateHealth2 = batteryHealth(sensorArray[i].PinA, sensorArray[i].PinB, sensorArray[i].PinC);
          stateHealth3 = batteryHealth(sensorArray[i].PinA, sensorArray[i].PinB, sensorArray[i].PinC);
          stateHealthMean = (stateHealth1+stateHealth2+stateHealth3)/3.0;
          
          //dataFile.print("State of health: ");
          dataFile.print(stateHealthMean);
          dataFile.print(",");
          
          //Serial.print("State of health: ");
          Serial.print(stateHealthMean);
          Serial.print(",");
        
          break;   
        case 6:
          break; 
        case 7:
          break;
        case 8:
          break;
        default:
          // return some kind of error
          break; 
      }
    }
  }
  else{
    // prints each sensor reading to the data file 
    for(int i = 0; i < sensorNum ; i++){
      switch (sensorArray[i].sensorType) {
        case 0:
          // return some kind of error which should be replaced   
          // by custom code for input and output pins 
          break;
          
        case 1:
          struct waterLevel level1, level2, level3, levelMean;
          
          level1 = calcLevel(sensorArray[i].PinB, sensorArray[i].PinA);
          level2 = calcLevel(sensorArray[i].PinB, sensorArray[i].PinA);
          level3 = calcLevel(sensorArray[i].PinB, sensorArray[i].PinA);
          
          levelMean.distance = (level1.distance+level2.distance+level3.distance)/3;
          levelMean.volume = (level1.volume+level2.volume+level3.volume)/3;
          levelMean.percentFull = (level1.percentFull+level2.percentFull+level3.percentFull)/3;
          
          //dataFile.print("Distance: ");
          dataFile.print(levelMean.distance);
          dataFile.print(",");
//          dataFile.print("Volume: ");
//          dataFile.print(levelMean.volume);
//          dataFile.print(" , ");
//          dataFile.print("Percentage full: ");
//          dataFile.print(levelMean.percentFull);
//          dataFile.print(",");
    
          // check what is written to SD card with serial monitor
          //Serial.print("Distance: ");
          Serial.print(levelMean.distance);
          Serial.print(",");
//          Serial.print("Volume: ");
//          Serial.print(levelMean.volume);
//          Serial.print(" , ");
//          Serial.print("Percentage full: ");
//          Serial.print(levelMean.percentFull);
//          Serial.print(",");
          break;
          
        case 2:
          int flowRate1, flowRate2, flowRate3, flowRateMean;
          
          flowRate1 = calcFlowRate(sensorArray[i].PinA);
          flowRate2 = calcFlowRate(sensorArray[i].PinA);
          flowRate3 = calcFlowRate(sensorArray[i].PinA);
          flowRateMean = (flowRate1 + flowRate2 + flowRate3)/3;
    
          //dataFile.print("Flow rate: ");
          dataFile.print(flowRateMean);
          dataFile.print(",");
    
          // check what is written to SD card with serial monitor
          //Serial.print("Flow rate: ");
          Serial.print(flowRateMean);
          Serial.print(",");        
          break;
          
        case 3:
          float turbVoltage1, turbVoltage2, turbVoltage3, turbVoltageMean;
    
          turbVoltage1 = turbidity(sensorArray[i].PinA);
          turbVoltage2 = turbidity(sensorArray[i].PinA);
          turbVoltage3 = turbidity(sensorArray[i].PinA);
          turbVoltageMean = (turbVoltage1+turbVoltage2+turbVoltage3)/3.0;
    
          //dataFile.print("Voltage: ");
          dataFile.print(turbVoltageMean);
          dataFile.print(",");
          
          //Serial.print("Voltage: ");
          Serial.print(turbVoltageMean); 
          Serial.print(",");
          
          break; 
        case 4:
          break;
        case 5:
          break;   
        case 6:
          float ph1, ph2, ph3, phMean;
    
          ph1 = calcPH(sensorArray[i].PinA);
          ph2 = calcPH(sensorArray[i].PinA);
          ph3 = calcPH(sensorArray[i].PinA);
          phMean = (ph1+ph2+ph3)/3.0;
    
          //dataFile.print("PH: ");
          dataFile.print(phMean);
          dataFile.print(",");
    
          //Serial.print("PH: ");
          Serial.print(phMean);
          Serial.print(",");
          
          break; 
        case 7:
          sensors.begin()
          sensors.requestTemperatures(); // Send the command to get temperature readings 
                    
          //dataFile.print("Water temperature (C): ");
          dataFile.print(sensors.getTempCByIndex(0));
          dataFile.print(",");
    
          //Serial.print("Water temperature (C): ");
          Serial.print(sensors.getTempCByIndex(0));
          Serial.print(",");
          break;
        case 8:
          float ECcurrent;
          ECcurrent = getEC(A11);

          //dataFile.print("Electrical conductivity (mS/m): ");
          dataFile.print(ECcurrent);
          dataFile.print(",");
          
          //Serial.print("Electrical conductivity (mS/m): ");
          Serial.print(ECcurrent);
          Serial.print(",");
          break;
        default:
          // return some kind of error
          break; 
      }
    }
  }

  dataFile.println();
  Serial.println("\n");
  dataFile.close();
}


void printHeader(char fileName[], char header[]){
  dataFile = SD.open(fileName, FILE_WRITE);

  if(dataFile){
    dataFile.print(header);
    dataFile.print(",");
  }
  else{
    Serial.println("Error opening data File");
  }
  dataFile.close();
}

/* _________________________________________________________________________
 * 
 * Battery health
 * _________________________________________________________________________
 */
 
float batteryTemp(int sensorPin, int sensorPin2){
    
  int reading = (analogRead(sensorPin)+analogRead(sensorPin2))/2; // getting the voltage reading from the temperature sensor
  float voltage = (reading * 5)/1024.0;                           // converting that reading to voltage 
  return voltage * 100 ;                                          // converting voltage to temperature
}

float batteryHealth(int voltagePin, int currentPin, int gatePin){
  
  float State_Health = 0.0, OCvoltage = 0.0, CCvoltage = 0.0, current = 0, R_internal = 0.0;
  int OCVreading = 0, CCVreading = 0;      

  while(OCVreading <= CCVreading){
    
    //turn off transistor to measure OCV
    digitalWrite(gatePin, LOW);
    delay(5);
  
    volt_compensated(voltagePin, OCVreading, OCvoltage);  //read and correct open circuit voltage

    //turn on transistor to measure CCV and current
    digitalWrite(gatePin, HIGH);
    delay(5);
    
    volt_compensated(voltagePin, CCVreading, CCvoltage);  //read and correct closed circuit voltage
  }
  
  current = current_reading(currentPin);                //read load current
  R_internal = (OCvoltage - CCvoltage)/current;         //calculate internal resistance
  State_Health = 100* (R_internal - 1.0)/(0.084 - 1.0); // calculate state health
  
  return State_Health;
}

void volt_compensated(int sensorPin, int& value, float& voltage){
  value = analogRead(sensorPin)+(batteryTMean - 25)*(0.1769472);
  voltage = (value * 25.0) / 1024.0;
}

float current_reading(int sensorPin){
  int sensorValue = analogRead(sensorPin);
  float voltage = (sensorValue / 1024.0) * 5000;
  return (voltage - 2500) / 100;
}


/* _________________________________________________________________________
 * 
 * Water quality
 * _________________________________________________________________________
 */

float calcPH(int inPin){  
  
  int readingSum=0;
  float avgValue=0.0, pHVol=0.0;
  
  for(int i=0; i<10; i++){
    readingSum += analogRead(inPin);
    delay(10);
  }

  avgValue = readingSum/10.0;
  pHVol = avgValue*5.0/1024;
  
  return (-5.70 * pHVol + 21.34);
}

float turbidity(int turbPin){
  int sensorValue = analogRead(turbPin);    // read the input on analog pin 0:
  float voltage = sensorValue * (5 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  if(voltage < 2.5){
    voltage = 2.5;
  }
  float NTU = -1120.4*sq(voltage) + 5742.3*voltage - 4352.9;
  if(NTU < 0){
    NTU = 0;
  }
  return NTU;
}


/*
High quality deionized water has a conductivity of about 5.5 μS/m at 25 °C, typical drinking water in the range of 5–50 mS/m, 
while sea water about 5 S/m (or 50 mS/cm) (i.e., sea water's conductivity is one million times higher than that of deionized water:).
*/
float getEC(int ECpin){

  float analogValue = 0;
  for(int i = 0; i < 100; i++){
    analogValue = analogValue + analogRead(ECpin);
  }
  
  float analogAverage = analogValue / 100; //analog average,from 0 to 1023
  float averageVoltage = analogAverage * 5000 / 1024; //millivolt average,from 0mv to 4995mV
  sensors.requestTemperatures(); 
  float temperature = sensors.getTempCByIndex(0); //current temperature

  float tempCoefficient =  1.0 + 0.0185 * (temperature - 25.0);   // temperature compensation formula:
                                                                  // FinalResult(25^C) = FinalResult(current)/(1.0+0.0185*(fTP-25.0))
  float coefficientVolatge = averageVoltage / tempCoefficient; 
  float ECcurrent;
  if(coefficientVolatge<=448) ECcurrent=6.84 * coefficientVolatge - 64.32;   // 1mS/cm < EC <= 3mS/cm
  else if(coefficientVolatge<=1457) ECcurrent= 6.98 * coefficientVolatge - 127;  //3mS/cm < EC < = 10mS/cm
  else ECcurrent = 5.3 * coefficientVolatge + 2278;                           //10mS/cm < EC < 20mS/cm
  ECcurrent/=10;    // convert uS/cm to mS/m
  return ECcurrent;    
}

/* _________________________________________________________________________
 * 
 * Flow rate
 * _________________________________________________________________________
 */
 
void pulseCounter ()      // function that the interupt for flow rate calls 
{ 
 pulseCount++;
}


int calcFlowRate(int hallsensor) {
  
  int flowRate; 
  
  attachInterrupt(digitalPinToInterrupt(hallsensor), pulseCounter, RISING); 
  pulseCount = 0;      
  delay (1000);                         
  flowRate = (pulseCount * 60 / 7.5);   // calculating flow rate in L/hour  
  detachInterrupt(digitalPinToInterrupt(hallsensor));

  return flowRate;
}




/* _________________________________________________________________________
 * 
 * Water level
 * _________________________________________________________________________
 */

// calculates and returns water distance from sensor, water volume and 
// percentage to which the tank is full (using an ulatra sonic sensor)
struct waterLevel calcLevel(const int trigPin, const int echoPin) {

  struct waterLevel tankLevel = {0, 0, 0};
  long duration = 0;
  
  // clears the trigPin for two microseconds
  digitalWrite(trigPin, OUTPUT); 
  delayMicroseconds(2);

  // sets trigPin for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // calculates distance using: D = S*T
  duration = pulseIn(echoPin, HIGH); // waits for echo pin to go high which starts the timing
                                     // when echo pin goes low the timing stops
                                     // returns the durration of the pule in microseconds
  if(duration <= 0){
    duration = -1000;
    // return with exit condition and error message?
  }
  
  tankLevel.distance = duration*0.00034/2; // calculates the distance in meters
  tankLevel.volume = (tankHeight - tankLevel.distance) * 3.14 * sq(tankDiameter / 2) * 1000;  // Volume = height*pi*(r^2)*(litre conversion)
  tankLevel.percentFull = (1 - (tankLevel.distance / tankHeight)) * 100; // calculates the percentage of water in the tank
  
  return tankLevel;
  
}
