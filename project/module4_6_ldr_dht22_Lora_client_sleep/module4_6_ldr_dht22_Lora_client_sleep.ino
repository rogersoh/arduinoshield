#include <RH_RF95.h>
#include <Adafruit_SleepyDog.h>
#include "DHT.h"

//DHT temperature n humidity sensor
#define DHTPIN 10     // Digital pin connected to the DHT sensor 
#define DHTTYPE  DHT22     // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

//LDR and battery level
int sensorPin = A0; //LDR
int battPin = A7;  //Battery
int sensorValue = 0;
float battValue = 0.0;
int battRaw = 0;

String deviceID = "S1";  // deviceID identification

//initiation of Lora data in byte
int myval;
byte payload[8];
int payloadsize = 8;

//lora radio setting
/* for arduino lora shield
  #define RFM95_CS 10
  #define RFM95_RST 7
  #define RFM95_INT 2
*/

//for adafruit mo feather lora radio
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3


// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  dht.begin();
  delay(1000);
  int countdownMS = Watchdog.enable();

  Serial.begin(9600);

  //Lora radio setup
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  rf95.setTxPower(23, false);

  Serial.print("Watchdog maximum time is ");
  Serial.println(countdownMS, DEC);
  Watchdog.reset();
}

void loop() {
  // Get temperature event and print its value.
  Serial.println("Transmitting...");

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  sensorValue = analogRead(sensorPin);
  battRaw = 0;
  for (int i = 0; i < 10; i++) {
    battRaw += analogRead(battPin);
  }
  battValue = battRaw / (1024.00 * 10) * 3.3 * 2;

  //lora data in byte
  myval = t * 100;
  payload[0] = highByte(myval);
  payload[1] = lowByte(myval);
  myval = h * 100;
  payload[2] = highByte(myval);
  payload[3] = lowByte(myval);
  myval = sensorValue;
  payload[4] = highByte(myval);
  payload[5] = lowByte(myval);
  myval = battValue * 100;
  payload[6] = highByte(myval);
  payload[7] = lowByte(myval);

  String d = "{\"ID\": \"" + String (deviceID) + "\"" + ",";
  d += "\"Temp\":" + String (t) + ",";
  d += "\"Hum\":" + String (h) + ",";
  d += "\"LDR\":" + String (sensorValue) + ",";
  d += "\"Battery\":" + String (battValue);
  d += "} ";  // Add a trailing space is necessary

  // convert dec to byte
  String e = deviceID;
  for (int i = 0; i < payloadsize; i++) {
    e += "," + String (payload[i], 16);
  }
  e += " ";

  Serial.println(d);

  Watchdog.reset();

  char data[e.length()];
  e.toCharArray(data, e.length());
  Serial.println(e);

  rf95.send((uint8_t*)data, sizeof(data));

  Serial.println("Waiting for packet to complete...");
  delay(1000);
  rf95.waitPacketSent();
  Serial.println(" complete...");

  //set sleep duration each sleep
  for (int i = 0; i < 60; i++) {
    int sleepMS = Watchdog.sleep(4000);
    Watchdog.reset();
  }
  // Try to reattach USB connection on "native USB" boards (connection is
  // lost on sleep). Host will also need to reattach to the Serial monitor.
  // Seems not entirely reliable, hence the LED indicator fallback.
#if defined(USBCON) && !defined(USE_TINYUSB)
  USBDevice.attach();
#endif

}
