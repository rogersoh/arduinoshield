#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define ledPin 4

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     9 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String incomingString;
String myString;
String garbage;
char data;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.display();
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

    display.clearDisplay();
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0, 0);            // Start at top-left corner

    //    display.println("data: ");
    //    display.println(n);
    display.println("RSSI: ");
    display.println(o);
    display.println("SNR ");
    display.println(p);
    display.display();
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
