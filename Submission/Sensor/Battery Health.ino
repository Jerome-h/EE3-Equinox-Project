/*  Calculation of state of health of a lead acid battery based on internal resistance with Arduino 
 *  Created by Liew Guo Liang for E.quinox
 *  6/18/2018
 *   
 *  Components needed:
 *  - Temperature sensor: LM35DZ/NOPB (Texas Instrument) 
 *    -- quantity: at least 2 to get an average battery temperature
 *  - Voltage module: resistances used are 300k and 75k ohms;
 *    -- potential divider ratio, R is 1/5 
 *    -- max voltage that can be measured is 25V
 *  - Current sensor: ACS712ELCTR-20A-T Hall Effect Sensor IC (Allegro)
 *  - Load resistance = 20 ohm
 *  - N-channel MOSFET: BS170 (ON Semiconductor)
 *    -- on-time resistance = 5 ohm
 *  
 *  The battery temperature is measured to compensate the battery voltage measured to obtain a more actual voltage measurement as it changes with the temperature.
 *  The compensation of the voltage follows the relationship: actual sensor value = value + (T-25)(9/50)(0.024)(1024/5R)
 *  Internal resistance = (Open circuit voltage - Closed circuit voltage) / current through load resistance
 *  State of health (%) = 100*(current - threshold)/(fresh - threshold)
 */

//Analogue input pins
int tempPin1 = A0;   //temperature sensor pin 1
int tempPin2 = A5;   //temperature sensor pin 2
int voltagePin = A1; //voltage module pin
int currentPin = A4; //current sensor pin

//Digital output pin
int gatePin = 13;    //digital output pin for MOSFET gate control

void setup() {
  Serial.begin(9600);
  pinMode(gatePin, OUTPUT); //set the mode of the digital pin as output pin
}

void loop() {
  // put your main code here, to run repeatedly:
  float stateHealth = batteryHealth(tempPin1, tempPin2, voltagePin, currentPin, gatePin);
  Serial.print("state of health: "); 
  Serial.print(stateHealth); 
  Serial.println(" %");
  delay(500); //delay 0.5s
}

//function of battery health calculation
float batteryHealth(int tempPin1, int tempPin2, int voltagePin, int currentPin, int gatePin){
  //declaration of variables needed
  float State_Health = 0.0, temperatureC = 0.0, OCvoltage = 0.0, CCvoltage = 0.0, current = 0.0, R_internal = 0.0;
  int OCVreading = 0, CCVreading = 0;      

  temperatureC = temp(tempPin1, tempPin2);  // read battery temperature for voltage compensation

  while(OCVreading <= CCVreading){  //make sure the open circuit voltage reading is always bigger than the closed circuit voltage
    
    //turn off transistor to measure Open Circuit Voltage (OCV)
    digitalWrite(gatePin, LOW);
    delay(1); //delay 1us to account for the MOSFET off-time

    //read and correct open circuit voltage
    //take 3 readings of the open circuit voltage and take their average
    OCvoltage = volt_compensated(temperatureC, voltagePin, OCVreading);  
    OCvoltage += volt_compensated(temperatureC, voltagePin, OCVreading);
    OCvoltage += volt_compensated(temperatureC, voltagePin, OCVreading);
    OCvoltage /=3;
    
    //turn on transistor to measure CCV and current
    digitalWrite(gatePin, HIGH);
    delay(1); //delay 1us to account for the MOSFET on-time

    //read and correct closed circuit voltage
    //take 3 readings of the closed circuit voltage and take their average
    CCvoltage = volt_compensated(temperatureC, voltagePin, CCVreading);  
    CCvoltage += volt_compensated(temperatureC, voltagePin, CCVreading);
    CCvoltage += volt_compensated(temperatureC, voltagePin, CCVreading);
    CCvoltage /=3;
    
    //Serial.print(OCvoltage);Serial.print(", ");  Serial.print(CCvoltage); Serial.print(", ");
  }
  //read average load current
  current = current_reading(currentPin);                
  current += current_reading(currentPin);
  current += current_reading(currentPin);
  current += current_reading(currentPin);
  current += current_reading(currentPin);
  current /= 5;
  //Serial.print(current); Serial.print(", ");
   
  R_internal = (OCvoltage - CCvoltage)/current;         //calculate internal resistance
  // Serial.println(R_internal);
 
  State_Health = 100* (R_internal - 1.0)/(0.084 - 1.0); // calculate state of health
  
  /*  in case the internal resistance calculated is smaller than that of the fresh battery (from datasheet of the lead-acid battery)
  *   cap the state of health at 100%
  *   this shouldn't happen
  */
  if(State_Health > 100){
    State_Health = 100;
  }
  return State_Health;
}

/* measure battery temperature using LM35
*  no sensor output offset */
float temp(int sensorPin, int sensorPin2){  
  int reading = (analogRead(sensorPin)+analogRead(sensorPin2))/2; // get the voltage reading from the sensors
  float voltage = (reading * 5)/1024.0;                           // convert that reading to voltage 
  return voltage * 100 ;                                          // convert voltage to temperature
}

//measure voltage using potential divider for voltage > 5V
float volt_compensated(float temperatureC, int sensorPin, int& value){
  value = analogRead(sensorPin)+(temperatureC - 25)*(0.1769472);  //compensate sensor value with temperature using the relationship above
  float voltage = (value * 25.0) / 1024.0;                        //convert the sensor value to voltage
  return voltage;
}

/*  Measure current using ACS712
 *  mVperAmp = 100
 *  ACSoffset = 2500
 */
float current_reading(int sensorPin){
  int sensorValue = analogRead(sensorPin);        // get the voltage reading from the sensors
  float voltage = (sensorValue / 1024.0) * 5000;  //voltage measured in mV
  return (voltage - 2500) / 100;                  //convert voltage to Ampere
}
