/*
 * For now I'm still trying to find out the relationship between the turbidity and the voltage of the
 * analog pin, the relationship given in the website is not accurate. Right now I can only give 
 * ranges of voltage as estimates.
 * 4V - 5V ~= 0 NTU --> Clean water
 * 3.5V - 4V ~= 1000 NTU
 * 2V - 3V ~= 3000 NTU
*/

int pin = A0; // analog pin 
int turbidityValue;
float voltage;
void setup() {
  Serial.begin(9600); //Baud rate: 9600
}

void turbidity(int pin, float &voltage);

void loop() {
  turbidity(pin, voltage);
  Serial.print("Voltage = ");
  Serial.println(voltage); // print out the value you read:
  delay(300);
}

void turbidity(int pin, float &voltage){
  int sensorValue = analogRead(pin);// read the input on analog pin 0:
  voltage = sensorValue * (5 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
}
