//Analogue in pins
int tempPin1 = A0;   //temperature pin 1
int tempPin2 = A5;   //temperature pin 2
int voltagePin = A1; //voltage pin
int currentPin = A4; //current pin

//Digital out pin
int gatePin = 13;    //digital pin for MOSFET gate

void setup() {
  Serial.begin(9600);
  pinMode(gatePin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  float stateHealth = batteryHealth(tempPin1, tempPin2, voltagePin, currentPin, gatePin);
  Serial.print("state of health: "); 
  Serial.print(stateHealth); 
  Serial.println(" %");
  delay(5000);
}

float batteryHealth(int tempPin1, int tempPin2, int voltagePin, int currentPin, int gatePin){
  
  float State_Health = 0.0, temperatureC = 0.0, OCvoltage = 0.0, CCvoltage = 0.0, current = 0, R_internal = 0.0;
  int OCVreading = 0, CCVreading = 0;      

  temperatureC = temp(tempPin1, tempPin2);  // read battery temperature

  while(OCVreading <= CCVreading){
    
    //turn off transistor to measure OCV
    digitalWrite(gatePin, LOW);
    delay(1);
  
    volt_compensated(temperatureC, voltagePin, OCVreading, OCvoltage);  //read and correct open circuit voltage

    //turn on transistor to measure CCV and current
    digitalWrite(gatePin, HIGH);
    delay(1);
    
    volt_compensated(temperatureC, voltagePin, CCVreading, CCvoltage);  //read and correct closed circuit voltage
  }
  //Serial.println(OCvoltage);Serial.println(CCvoltage);
  current = current_reading(currentPin);                //read load current
  //Serial.println(current);
  R_internal = (OCvoltage - CCvoltage)/current;         //calculate internal resistance
  //Serial.println(R_internal);
  State_Health = 100* (R_internal - 1.0)/(0.084 - 1.0); // calculate state health
  
  return State_Health;
}

// calculates battery temperature 
float temp(int sensorPin, int sensorPin2){  
  int reading = (analogRead(sensorPin)+analogRead(sensorPin2))/2; // getting the voltage reading from the temperature sensor
  float voltage = (reading * 5)/1024.0;                           // converting that reading to voltage 
  return voltage * 100 ;                                          // converting voltage to temperature
}

void volt_compensated(float temperatureC, int sensorPin, int& value, float& voltage){
  value = analogRead(sensorPin)+(temperatureC - 25)*(0.1769472);
  voltage = (value * 25.0) / 1024.0;
}

float current_reading(int sensorPin){
  int sensorValue = analogRead(sensorPin);
  float voltage = (sensorValue / 1024.0) * 5000;
  return (voltage - 2500) / 100;
}
