byte statusLed    = 13;
byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;

struct flow {
  float flowRate;
  unsigned int flowMilliLitres;
  unsigned long totalMilliLitres;
  unsigned int frac;
}waterFlow;

volatile byte pulseCount;  
unsigned long oldTime;

void pulseCounter();
void flowInfo(volatile byte &pulseCount, struct flow &info, unsigned long &oldTime);
void printFlow(struct flow &info);

void setup(){
  // Initialize a serial connection for reporting values to the host
  Serial.begin(9600);
   
  // Set up the status LED line as an output
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);  // We have an active-low LED attached
  
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount                  = 0;
  waterFlow.flowRate          = 0.0;
  waterFlow.flowMilliLitres   = 0;
  waterFlow.totalMilliLitres  = 0;
  oldTime                     = 0;

  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}

/**
 * Main program loop
 */
void loop(){
  if((millis() - oldTime) > 1000){    // Only process counters once per second
    flowInfo(pulseCount, waterFlow, frac, oldTime);  
    printFlow(waterFlow, frac); 
  }
}

/*
Insterrupt Service Routine
 */
void pulseCounter(){
  // Increment the pulse counter
  pulseCount++;
}

// calculates flow rate, current flow rate and total flow output 
void flowInfo(volatile byte &pulseCount, struct flow &info, unsigned long &oldTime){
  // Disable the interrupt while calculating flow rate and sending the value to
  // the host
  detachInterrupt(sensorInterrupt);
     
  // Because this loop may not complete in exactly 1 second intervals we calculate
  // the number of milliseconds that have passed since the last execution and use
  // that to scale the output. We also apply the calibrationFactor to scale the output
  // based on the number of pulses per second per units of measure (litres/minute in
  // this case) coming from the sensor.
  info.flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
  
  // Note the time this processing pass was executed. Note that because we've
  // disabled interrupts the millis() function won't actually be incrementing right
  // at this point, but it will still return the value it was set to just before
  // interrupts went away.
  oldTime = millis();
    
  // Divide the flow rate in litres/minute by 60 to determine how many litres have
  // passed through the sensor in this 1 second interval, then multiply by 1000 to
  // convert to millilitres.
  info.flowMilliLitres = (info.flowRate / 60) * 1000;
    
  // Add the millilitres passed in this second to the cumulative total
  info.totalMilliLitres += info.flowMilliLitres;
  
  // Determine the fractional part. The 10 multiplier gives us 1 decimal place.     
  info.frac = (info.flowRate - int(info.flowRate)) * 10;
       
  // Reset the pulse counter so we can start incrementing again
  pulseCount = 0;
    
  // Enable the interrupt again now that we've finished sending output
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}

// print flow rate, current flow rate and total flow output 
void printFlow(struct flow &info){
  // Print the flow rate for this second in litres / minute
  Serial.print("Flow rate: ");
  Serial.print(int(info.flowRate));  // Print the integer part of the variable
  Serial.print(".");             // Print the decimal point
  Serial.print(info.frac, DEC) ;      // Print the fractional part of the variable
  Serial.print("L/min");
  
  // Print the number of litres flowed in this second
  Serial.print("  Current Liquid Flowing: ");             // Output separator
  Serial.print(info.flowMilliLitres);
  Serial.print("mL/Sec");
  
  // Print the cumulative total of litres flowed since starting
  Serial.print("  Output Liquid Quantity: ");             // Output separator
  Serial.print(info.totalMilliLitres);
  Serial.println("mL");   
}
