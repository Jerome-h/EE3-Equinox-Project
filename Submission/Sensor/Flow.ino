volatile int pulseCount;  // counts the rising and falling edges of the signal                              
int hallsensor = 2;       // pin location of the sensor.
float callibrationFactor = 7.5; // callibration factor

void pulseCounter ()      // function that the interupt calls 
{ 
 pulseCount++;  // pulse count increases based on flow of water through the turbine in the flow sensor
} 

void setup()
{ 
  Serial.begin(9600);
  pinMode(hallsensor, INPUT);
} 

void loop ()    
{
  Serial.println(calcFlowRate(hallsensor));
}

int calcFlowRate(int hallsensor) {
  int flowRate; 
  attachInterrupt(digitalPinToInterrupt(hallsensor), pulseCounter, RISING); // attach interrupt
  pulseCount = 0;      
  delay (1000);                         
  flowRate = (pulseCount * 60 / callibrationFactor);   // calculating flow rate in L/hour  
  detachInterrupt(digitalPinToInterrupt(hallsensor)); // deattach interrupt
  return flowRate;
}
