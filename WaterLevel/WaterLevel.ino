const int volume = 0;

void setup() {
  Serial.begin(9600); // Serial begin texting 
}

void loop() {
  // put your main code here, to run repeatedly:

}

void waterLevel() {
  
}
long calcDistance(const int trigPin, const int echoPin) {

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
                                     // returns the duration of the pulse in microseconds
  return duration*0.034/2; // returns the distance
}

