int pin = A14; // analog pin 
int turbidityValue; // analog value of sensor 
float voltage;  // sensor data in voltage (0 - 5V)
float NTU; // sensor data in nephelometric turbidity units (NTU)
void setup() {
  Serial.begin(9600); //Baud rate: 9600
}

void loop() {
  turbidity(pin, voltage, NTU);
  Serial.print("analog reading = ");
  Serial.print(analogRead(pin));
  Serial.print(", Voltage = ");
  Serial.print(voltage); // print out the value you read:
  Serial.print(", NTU = ");
  Serial.println(NTU);
  delay(300);
}
void turbidity(int pin, float &voltage, float &NTU){
  int sensorValue = analogRead(pin);// read the input on analog pin 0:
  voltage = sensorValue * (5 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  // the relationship is a quadratic equation where NTU will decrease when voltage is less than 2.5 which is not logical
  // voltages less than 2.5 will be fixed at 2.5
  if(voltage < 2.5){
    voltage = 2.5;
  }
  NTU = -1120.4*sq(voltage) + 5742.3*voltage - 4352.9; // relationship obtained from the documentation site
  // NTU = 0 is the lowest value 
  if(NTU < 0){
    NTU = 0;
  }
}
