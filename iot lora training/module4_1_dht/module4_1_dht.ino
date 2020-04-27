#include "DHT.h"
#define DHTPIN 3     // what digital pin we're connected to

#define DHTTYPE DHT22   // DHT 22
DHT dht(DHTPIN, DHTTYPE);

int myval;
byte payload[3];
String deviceID = "S2";

void setup()
{
  Serial.begin(9600);
  Serial.println("DHT22 test!");
  dht.begin();
}

void loop()
{
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  myval = t * 100;
  payload[0] = highByte(myval);
  payload[1] = lowByte(myval);
  payload[2]= deviceID;
  myval = (payload[0] << 8) + payload[1];
  float temp = myval / 100.0;

  String d = "{\"Temp\":" + String (t) + ",";
  d += "\"Hum\":" + String (h);
  d += "} ";

  Serial.println(d);
  Serial.println("DeviceID : " + deviceID + "  highByte : " + String (payload[0], 16) + "  lowByte : " + String(payload[1], 16));
  Serial.println(temp);
  delay(1000);

}
