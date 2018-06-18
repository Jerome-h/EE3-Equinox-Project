/*
High quality deionized water has a conductivity of about 5.5 μS/m at 25 °C, typical drinking water in the range of 5–50 mS/m, 
while sea water about 5 S/m (or 50 mS/cm) (i.e., sea water's conductivity is one million times higher than that of deionized water:).
*/

#include <OneWire.h>
#include <DallasTemperature.h>

// data wire plugged into digital pin for temperature sensor 
int ONE_WIRE_BUS = 4;

// Set up a oneWire instance to communcate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS); 

// Pass our oneWire reference to Dallas Temperature
DallasTemperature sensors(&oneWire);

float analogValue = 0;
float analogAverage;
float temperature;
float averageVoltage;
float ECcurrent;
int ECpin = A13;

void setup() {
  Serial.begin(9600);
}

void loop() {
  ECcurrent = getEC(analogValue, analogAverage, temperature, averageVoltage, ECcurrent, ECpin);
  Serial.print(ECcurrent);  
  Serial.println(" mS/m");
  delay(500);
}

float getEC(float &analogValue, float &analogAverage, float &temperature, float &averageVoltage, float &ECcurrent, int ECpin){
  for(int i = 0; i < 100; i++){
    analogValue = analogValue + analogRead(ECpin);
  }
  analogAverage = analogValue / 100; //analog average,from 0 to 1023
  analogValue = 0;
  averageVoltage = analogAverage * 5000 / 1024; //millivolt average,from 0mv to 4995mV
  sensors.requestTemperatures(); // get temperature readings
  temperature = sensors.getTempCByIndex(0); // current temperature

  float tempCoefficient =  1.0 + 0.0185 * (temperature - 25.0);   // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.0185*(fTP-25.0));
  float coefficientVolatge = averageVoltage / tempCoefficient; 
  if(coefficientVolatge<=448) ECcurrent=6.84 * coefficientVolatge - 64.32;   // 1mS/cm < EC <= 3mS/cm
  else if(coefficientVolatge<=1457) ECcurrent= 6.98 * coefficientVolatge - 127;  //3mS/cm < EC < = 10mS/cm
  else ECcurrent = 5.3 * coefficientVolatge + 2278;                           //10mS/cm < EC < 20mS/cm
  ECcurrent/=10;    // convert uS/cm to mS/m
  return ECcurrent;    
}
