// Volume of water tank 2. Enter dimensions in metres.
const int tankWidth = 1;
const int tankLength = 1; 
const int tankHeight = 1;

void setup() {
  Serial.begin(9600); // Serial begin texting 
}

void loop() {
  // put your main code here, to run repeatedly:

}

long WaterLevel(const int trigPin, const int echoPin) {
  long duration , distance, waterHeight, waterVolume ;
  
  // Clears the trigPin for two microseconds
  digitalWrite(trigPin, OUTPUT); 
  delayMicroseconds(2);
  
  // sets trigPin for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Calculates distance using: D = S*T
  duration = pulseIn(echoPin, HIGH); // Waits for echo pin to go high which starts the timing
                                     // When echo pin goes low the timing stops
                                     // Returns the duration of the pulse in microseconds
  distance = duration*0.34/2;       // Calculates the distance in metres
      
  waterHeight = tankHeight - distance;
  waterVolume = waterHeight * tankWidth * tankLength;
  return waterVolume;

}

