#include <RH_RF95.h>
#include "DHT.h"

#define DHTPIN 3     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22
DHT dht(DHTPIN, DHTTYPE);

int sensorPin = A0;
int battPin = A3;
int sensorValue = 0;
float battValue = 0.0;
int battRaw = 0;
String deviceID = "S2";  //device identification

int myval;
byte payload[8];
int payloadsize = 8;


//for arduino uno lora shield
#define RFM95_CS 10
#define RFM95_RST 7
#define RFM95_INT 2

/*
  //for adafruit mo feather
  #define RFM95_CS 8
  #define RFM95_RST 4
  #define RFM95_INT 3
*/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup()
{
  dht.begin();
  delay(1000);

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
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

}

void loop()
{
  // Get temperature event and print its value.
  delay(500);
  Serial.println("Transmitting...");

  float  t = dht.readTemperature();
  float  h = dht.readHumidity();
  sensorValue = analogRead(sensorPin);
  battRaw = 0;
  for (int i = 0; i < 10; i++) {
    battRaw += analogRead(battPin);
  }
  battValue = battRaw / (1024.00 * 10) * 5 * 2;


  myval = t * 100;
  payload[0] = highByte(myval);
  payload[1] = lowByte(myval);
  myval = h * 100;
  payload[2] = highByte(myval);
  payload[3] = lowByte(myval);
  /*decode payload
    myval = (payload[0] << 8) + payload[1];
    float temp = myval / 100.0;
  */
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
  //Serial.println(e);
  delay(1000);

  char data[e.length()];
  e.toCharArray(data, e.length());
  Serial.println(e);

  rf95.send((uint8_t*)data, sizeof(data));

  Serial.println("Waiting for packet to complete...");
  delay(1000);
  rf95.waitPacketSent();
  Serial.println(" complete...");

  delay(50000);
}
