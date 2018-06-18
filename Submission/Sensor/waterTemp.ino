// First we include the libraries
#include <OneWire.h> 
#include <DallasTemperature.h>

// Data wire is plugged into any digital pin on the Arduino 
int ONE_WIRE_BUS = 4;

// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

void setup(void) 
{ 
 // start serial port 
 Serial.begin(9600); 
 // Start up the library 
 sensors.begin(); 
} 
void loop(void) 
{ 
 // call sensors.requestTemperatures() to issue a global temperature 
 // request to all devices on the bus 
 sensors.requestTemperatures(); // Send the command to get temperature readings 
 Serial.print("Temperature is: "); 
 Serial.println(sensors.getTempCByIndex(0)); // Why "byIndex"?  
   // You can have more than one DS18B20 on the same bus.  
   // 0 refers to the first IC on the wire 
   delay(1000); 
} 
