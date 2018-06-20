int analogInPin = A15; // analog pin for pH sensor
void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.print("sensor = ");
  Serial.println(calcPH(analogInPin));
  delay(1000);
}

float calcPH(int inPin){  
  offset = -3; // offset for calibration
  int readingSum=0;
  float avgValue=0.0, pHVol=0.0;

  // obtain 10 readings 
  for(int i=0; i<10; i++){
    readingSum += analogRead(inPin);
    delay(10);
  }

  // get average value 
  avgValue = readingSum/10.0;

  // convert to voltage
  pHVol = avgValue*5.0/1024;

  // add constant and offset 
  return (3.5 * pHVol + offset);
}
