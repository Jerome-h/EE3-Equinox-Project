int analogInput = A1;
 
float vout = 0.0;
float vin = 0.0;
float R1 = 30000.0; //30k
float R2 = 7500.0; //7500 ohm resistor
int value = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(analogInput, INPUT);
  Serial.begin(9600);
  Serial.println("BASIC DC VOLTMETER");
}

void loop() {
  // put your main code here, to run repeatedly:
  // read the value at analog input
  value = analogRead(analogInput);
  vout = (value * 5.0) / 1024.0;
  vin = vout / (R2/(R1+R2)); 
 
  Serial.print("Voltage = ");
  Serial.println(vin,2);
  delay(500);
}
