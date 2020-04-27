#define ledPin 4

String incomingString;
String myString;
String garbage;
char data;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    garbage = Serial.readString();//consists of the '+ERR=2 ERROR.
    //incomingString = Serial.readString();
    myString = Serial.readString();
    Serial.print("myString: ");
    Serial.println(myString);
    Serial.print("Garbage: ");
    Serial.println(garbage);
    if (garbage.indexOf("Hello!") > 0) {
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
    }
    String l = getValue(myString, ',', 0); //address
    String m = getValue(myString, ',', 1); //data length
    String n = getValue(myString, ',', 2); //data
    String o = getValue(myString, ',', 3); //RSSI
    String p = getValue(myString, ',', 4); //SNR

    Serial.print("data: ");
    Serial.println(n);
    Serial.print("data length: ");
    Serial.println(o);
    Serial.print("SNR ");
    Serial.println(p);
  }
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length();

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
