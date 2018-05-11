/*  Pre-set sensors:
 *   (1)  ultrasonic distance sensor
 *   (2)  _
 *   (3)  _
 *   (4)  _
 */


struct sensor {
  const int sensorType;                            
  const int inPin;
  const int outPin;
};


/*  User input:
 *  Enter each sensor type as an array of numbers (see list above) with
 *  their corresponding input pin and output pin (output pin may not be required)
 */
const int sensorNum = 1;  // Enter the number of sensors

struct sensor sensorArray[sensorNum] = {{1, 3, 2}};               // Enter the sensor type, input pin and output pin
                                                                  // sensor type found from list of pre-set sensors
                                                                  // if no output pin is required enter -1 
                                                                  // if adding a new sensor (not in pre-set list) put type as 0.
                                                                  // i.e. if want ultrasonic distance sensor (with echo pin 2 and
                                                                  // trigger pin 3) and a custom sensor (with input pin 4 and no 
                                                                  // output pin) enter: {{1,2,3},{0,4,-1}};


/* Program that user will not have to change unless using a different sensor
 * 
 */
void setup() {

  for(int i = 0; i < sensorNum ; i++){
      switch (sensorArray[i].sensorType) {
        case 0:
          // return some kind of error which should be replaced   
          // by custom code for input and output pins 
          break;
        case 1:
          // sets up input and output pins for ultra sonic sensor
          pinMode(sensorArray[i].inPin, INPUT);
          pinMode(sensorArray[i].outPin, OUTPUT);
          Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
          break;
        case 2:
    
          break;
        case 3:
    
          break;
        case 4:
    
          break;         
        default:
          // return some kind of error
          break; 
  }
  
  }

}





void loop() {

  for(int i = 0; i < sensorNum ; i++){
      switch (sensorArray[i].sensorType) {
        case 0:
          // return some kind of error which should be replaced   
          // by custom code for input and output pins 
          break;
        case 1:
          int distance;
          distance = calcDistance(sensorArray[i].outPin, sensorArray[i].inPin);
            Serial.print("Distance: ");
            Serial.println(calcDistance(sensorArray[i].outPin, sensorArray[i].inPin));

          break;
        case 2:
    
          break;
        case 3:
    
          break;
        case 4:
    
          break;         
        default:
          // return some kind of error
          break; 
  }
  
  }

  delay(100);
  
}





// calculates and returns distance using an ulatra sonic sensor
int calcDistance(const int trigPin, const int echoPin) {

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
  return duration*0.034/2; // returns the distance
  
}

