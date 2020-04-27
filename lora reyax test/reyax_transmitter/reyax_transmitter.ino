#define ledPin 2

unsigned long lastTransmission;
const int interval = 1000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis()-lastTransmission > interval){
    //Transmission due!
    Serial.println("AT+SEND=1,6,Hello!");
    digitalWrite(ledPin,HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    lastTransmission = millis();
  }
}
