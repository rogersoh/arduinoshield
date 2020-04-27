#include "DHT.h"
#define DHTPIN 10     // what digital pin we're connected to

#define DHTTYPE DHT22   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

void setup() 
{
  Serial.begin(9600);
  Serial.println("DHT11 test!");
  dht.begin();
}
 
void loop()
{
 float t = dht.readTemperature();
 float h = dht.readHumidity();
  
  String d = "{\"Temp\":"+ String (t)+",";
  d += "\"Hum\":"+ String (h);
  d += "} ";
  
  Serial.println(d);
  delay(1000);
   
}
