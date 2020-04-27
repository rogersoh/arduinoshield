#include <RH_RF95.h>

int sensorPin = A0; 
int sensorValue = 0; 


#define RFM95_CS 10
#define RFM95_RST 7
#define RFM95_INT 2

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
 
void setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  Serial.begin(9600);

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
}
 
void loop()
{
  delay(500); // Wait 1 second between transmits
  Serial.println("Transmitting..."); 
 
  sensorValue = analogRead(sensorPin); 
  Serial.println(sensorValue); 
  
  String d = "{\"LDR\":"+ String (sensorValue);
  d += "} "; 

  char data[d.length()];
  d.toCharArray(data,d.length());
  Serial.println(d);
  rf95.send((uint8_t*)data, sizeof(data));
 
  Serial.println("Waiting for packet to complete..."); 
  delay(1000);
  rf95.waitPacketSent();
  Serial.println(" complete...");
   
}
