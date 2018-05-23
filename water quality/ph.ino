int analogInPin = A0; 
int sensorValue = 0; 
float avgValue; 
float phValue;
int buf[10];  // buffer of size 10
int temp;     // temporary parameter

void setup() {
  Serial.begin(9600);
}

void phData(int pin, float &ph, float &avgValue);
void loop() {
  phData(analogInPin, phValue, avgValue);
  Serial.print("sensor = ");
  Serial.println(phValue);
  delay(20);
}

void phData(int pin, float &ph, float &avgValue){
 for(int i=0;i<10;i++){ 
  buf[i]=analogRead(pin);
  delay(10);
 }
 for(int i=0; i<9; i++){
  for(int j=i+1; j<10; j++){
    if(buf[i] > buf[j]){
      temp = buf[i];
      buf[i] = buf[j];
      buf[j] = temp;
    }
  }
 }
 avgValue = 0;
 for(int i=2; i<8; i++){
  avgValue += buf[i];
  ph = -5.70 * (avgValue * 5.0 / 1024 / 6) + 21.34;
 }
}
