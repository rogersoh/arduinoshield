#include <RH_RF95.h>
#include <Adafruit_SleepyDog.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"

#define DHTPIN 10     // Digital pin connected to the DHT sensor 
#define DHTTYPE  DHT22     // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

int sensorPin = A0;
int battPin = A7;
int sensorValue = 0;
float battValue = 0.0;
int battRaw = 0;
String deviceID = "S1";  // deviceID identification

/*
  #define RFM95_CS 10
  #define RFM95_RST 7
  #define RFM95_INT 2
*/

//for adafruit mo feather
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3


// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  Serial.println("Adafruit Watchdog Library Sleep Demo!");
  Serial.println();

  Serial.println("Feather LoRa TX Test!");

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

  dht.begin();
  Serial.println("DHT11 test!");
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

  String d = "{\"ID\": \"" + String (deviceID)+"\"" + ",";
  d += "\"Temp\":" + String (t) + ",";
  d += "\"Hum\":" + String (h) + ",";
  d += "\"LDR\":" + String (sensorValue) + ",";
  d += "\"Battery\":" + String (battValue);
  d += "} ";  // Add a trailing space is necessary

  char data[d.length()];
  d.toCharArray(data, d.length());
  Serial.println(d);
  rf95.send((uint8_t*)data, sizeof(data));

  Serial.println("Waiting for packet to complete...");
  delay(1000);
  rf95.waitPacketSent();
  Serial.println(" complete...");

  int sleepMS = Watchdog.sleep();
  sleepMS = Watchdog.sleep();
}
