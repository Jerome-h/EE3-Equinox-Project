// maybe have a few sensors measuring different things that are related (reduce chance that data is corrupted by one broken sensor)
// maybe have a way to run diagnostics (related to point above)

#include<SD.h>      // standard SD libary
#include<SPI.h>     // standard SPI libary
#include <Wire.h>   // needed for clock module DS3231
#include "DS3231.h" // needed for clock module DS3231

File dataFile;            // defines a file object
const int CSpin = 53;     // defines chip select pin for arduino (uno pin 10, mega pin 53)
RTClib RTC;               // needed for clock
volatile int pulseCount;  // counts the rising edges of a signal (for flow rate)

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
 *   (3)  Water quality (turbidity sensor: measure of relative water clarity)
 *   (4)  Battery temperature
 *   (5)  Battery health  *battery temperature must be measured previously to claculate battery health
 */

/*  User input:
 *  Enter each sensor type as a number (see list above) with
 *  their corresponding input pin and output pin (output pin may not be required)
 *  Enter these number into an array (each element corresponds to one sensor)
 *  User must also enter the dimensions of the water tank for waer level calculations
 */
const int sensorNum = 2;  // Enter the number of sensors

struct sensor sensorArray[sensorNum] = {{4, A0, A5, -1},{5, A1, A4, 13}};       
                                                          // Enter the sensor type, input pin and output pin
                                                          // sensor type found from list of pre-set sensors
                                                          // if no output pin is required enter -1 
                                                          // if adding a new sensor (not in pre-set list) put type as 0.
                                                          // i.e. if want ultrasonic distance sensor (with echo pin 2 and
                                                          // trigger pin 3) and a custom sensor (with input pin 4 and no 
                                                          // output pin) enter: {{1,2,3},{0,4,-1}};
                                                          
const float tankHeight = 1.0;   // Enter the height of the tank in meters
const float tankDiameter = 1.0; // Enter the Diameter of the tank in meters






/* Program that user will not have to change unless using a different sensor
 * 
 */


/* _________________________________________________________________________
 * 
 * Setup: initaialise sensor pins
 * _________________________________________________________________________
 */
  
void setup() {

  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  Wire.begin();       // needed for clock
  pinMode(CSpin, OUTPUT); // needed for SD card

  // SD card check
  if(SD.begin()){
    Serial.println("SD card okay");
  }
  else{
    Serial.println("SD card error");  // at the moment if there is an SD
    return;                           // card error then program wont work
  }

  // initialises sensor pins 
  for(int i = 0; i < sensorNum ; i++){
      switch (sensorArray[i].sensorType) {
        case 0:
          // return some kind of error which should be replaced   
          // by custom code for input and output pins 
          break;
        case 1:
          // sets up input and output pins for ultra sonic sensor
          pinMode(sensorArray[i].PinA, INPUT);
          pinMode(sensorArray[i].PinB, OUTPUT);
          break;
        case 2:
          // sets up input pin for flow sensor
          pinMode(sensorArray[i].PinA, INPUT);
          break;
        case 3: 
          break;
        case 4:
          break;    
        case 5:
          pinMode(sensorArray[i].PinC, OUTPUT);
          break;     
        default:
          // return some kind of error
          break; 
  }
  
  }

}



/* _________________________________________________________________________
 * 
 * Main loop: Takes sensor measurments at specified time 
 * _________________________________________________________________________
 */

float batteryTMean = 0.0;

void loop() {

  DateTime now = RTC.now();
  int secondNow = now.second();
  
  if(secondNow%3 == 0){

    // creates or opens a text file in the SD card called data.txt
    dataFile = SD.open("data.txt", FILE_WRITE);

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
      dataFile.print("    ");
      Serial.println("time written to file");
    }
    else{
      Serial.println("Error opening file");
    }

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
          
          dataFile.print("Distance: ");
          dataFile.print(levelMean.distance);
          dataFile.print(" , ");
          dataFile.print("Volume: ");
          dataFile.print(levelMean.volume);
          dataFile.print(" , ");
          dataFile.print("Percentage full: ");
          dataFile.print(levelMean.percentFull);
          dataFile.print(" ; ");

          // check what is written to SD card with serial monitor
          Serial.print("Distance: ");
          Serial.print(levelMean.distance);
          Serial.print(" , ");
          Serial.print("Volume: ");
          Serial.print(levelMean.volume);
          Serial.print(" , ");
          Serial.print("Percentage full: ");
          Serial.print(levelMean.percentFull);
          Serial.print(" ; ");
          break;
          
        case 2:
          int flowRate1, flowRate2, flowRate3, flowRateMean;
          
          flowRate1 = calcFlowRate(sensorArray[i].PinA);
          flowRate2 = calcFlowRate(sensorArray[i].PinA);
          flowRate3 = calcFlowRate(sensorArray[i].PinA);
          flowRateMean = (flowRate1 + flowRate2 + flowRate3)/3;

          dataFile.print("Flow rate: ");
          dataFile.print(flowRateMean);
          dataFile.print(" ; ");

          // check what is written to SD card with serial monitor
          Serial.print("Flow rate: ");
          Serial.print(flowRateMean);
          Serial.print(" ; ");        
          break;
          
        case 3:
          float turbVoltage1, turbVoltage2, turbVoltage3, turbVoltageMean;

          turbVoltage1 = turbidity(sensorArray[i].PinA);
          turbVoltage2 = turbidity(sensorArray[i].PinA);
          turbVoltage3 = turbidity(sensorArray[i].PinA);
          turbVoltageMean = (turbVoltage1+turbVoltage2+turbVoltage3)/3.0;

          dataFile.print("Voltage: ");
          dataFile.print(turbVoltageMean);
          dataFile.print(" ; ");
          
          Serial.print("Voltage: ");
          Serial.print(turbVoltageMean); 
          Serial.print(" ; ");
          
          break;
          
        case 4:
          float batteryT1, batteryT2, batteryT3;

          batteryT1 = batteryTemp(sensorArray[i].PinA, sensorArray[i].PinB);
          batteryT2 = batteryTemp(sensorArray[i].PinA, sensorArray[i].PinB);
          batteryT3 = batteryTemp(sensorArray[i].PinA, sensorArray[i].PinB);
          batteryTMean = (batteryT1+batteryT2+batteryT3)/3.0;

          dataFile.print("Battery temperature: ");
          dataFile.print(batteryTMean);
          dataFile.print(" ; ");
          
          Serial.print("Battery temperature: ");
          Serial.print(batteryTMean); 
          Serial.print(" ; ");

        case 5:
          float stateHealth1, stateHealth2, stateHealth3, stateHealthMean;
          
          stateHealth1 = batteryHealth(sensorArray[i].PinA, sensorArray[i].PinB, sensorArray[i].PinC);
          stateHealth2 = batteryHealth(sensorArray[i].PinA, sensorArray[i].PinB, sensorArray[i].PinC);
          stateHealth3 = batteryHealth(sensorArray[i].PinA, sensorArray[i].PinB, sensorArray[i].PinC);
          stateHealthMean = (stateHealth1+stateHealth2+stateHealth3)/3;
          
          dataFile.print("State of health: ");
          dataFile.print(stateHealthMean);
          dataFile.print("%");
          dataFile.print(" ; ");
          
          Serial.print("State of health: ");
          Serial.print(stateHealthMean);
          Serial.print("%");
          Serial.print(" ; ");
        
          break;   
                
        default:
          // return some kind of error
          break; 
      }
    }
    dataFile.println();
    Serial.println("\n");
    dataFile.close();
  }
  delay(1000);
}


/* _________________________________________________________________________
 * 
 * Battery health: battery temperature, battery state of health
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
    delay(1);
  
    volt_compensated(voltagePin, OCVreading, OCvoltage);  //read and correct open circuit voltage

    //turn on transistor to measure CCV and current
    digitalWrite(gatePin, HIGH);
    delay(1);
    
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
 * Water quality: turbidity, ...
 * _________________________________________________________________________
 */

float turbidity(int turbPin){
  int sensorValue = analogRead(turbPin);    // read the input on analog pin 0:
  return sensorValue * (5 / 1024.0);     // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
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






