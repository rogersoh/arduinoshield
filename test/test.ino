int myVal = 255;
byte payload[2];
String txtMsg = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    char inChar = Serial.read();
    txtMsg += inChar;
    myVal = txtMsg.toInt();
    payload[0] = highByte(myVal);
    payload[1] = lowByte(myVal);
    Serial.print("myVal ");
    Serial.println(myVal);
    Serial.print("payload[0] ");
    Serial.println(payload[0], HEX);
    Serial.print("payload[1] ");
    Serial.println(payload[1], HEX);

  }


}
