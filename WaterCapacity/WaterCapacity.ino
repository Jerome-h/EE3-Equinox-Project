// Ultrasonic Pins
const int tank2Echo = 6;
const int tank2Trig = 7;

struct tank {
  const float tankHeight;
  const float tankDiameter;
  float waterDistance;
  float waterHeight;
  float waterVolume;
};

//Initialise tanks being monitored. Input: {Height, Diameter} in metres
struct tank tank2 = {1, 1} ;

void tankCapacity(struct tank &Tank, const int trigPin, const int echoPin);

void setup() {
  Serial.begin(9600); // Serial begin texting
  pinMode (tank2Echo,  INPUT);
  pinMode (tank2Trig, OUTPUT);
}

void loop() {
  tankCapacity(tank2, tank2Trig, tank2Echo);

  Serial.print("Water distance: ");
  Serial.print(tank2.waterDistance);
  Serial.println("m");
  Serial.print("Water height: ");
  Serial.print(tank2.waterHeight);
  Serial.println("m");
  Serial.print("Water Volume: ");
  Serial.print(tank2.waterVolume);
  Serial.println("L");
  Serial.println();

  delay(500);
}

void tankCapacity(struct tank &Tank, const int trigPin, const int echoPin) {
  float duration , waterDistance, waterHeight, waterVolume ;

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
  waterDistance = duration * 0.000343 / 2 ;   // Calculates the distance in metres
  Tank.waterDistance = waterDistance;

  waterHeight = Tank.tankHeight - waterDistance;
  Tank.waterHeight = waterHeight;

  waterVolume = waterHeight * 3.14 * sq(Tank.tankDiameter / 2) * 1000; // V = height * pi * r^2 * litre conversion
  Tank.waterVolume = waterVolume;

  return Tank;
}

