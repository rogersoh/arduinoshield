#include <RH_RF95.h>
#include <OneWire.h>

OneWire  ds(8);  // on pin 8 (a 4.7K resistor is necessary)


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

void setup() {
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

void loop() {
  delay(500);

  byte i;
  byte present = 0;
  byte type_s;
  byte ds_data[12];
  byte addr[8];
  float celsius, fahrenheit;

  if ( !ds.search(addr)) {
    //  Serial.println("No more addresses.");
    //  Serial.println();
    ds.reset_search();
    delay(250);
    return;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return;
  }
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      /*  Serial.println("  Chip = DS18S20");  // or old DS1820
        Serial.print(" addr[0] ");
        Serial.println(addr[0]);*/
      type_s = 1;
      break;
    case 0x28:
      //  Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      // Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad

  /*  Serial.print("  Data = ");
    Serial.print(present, HEX);
    Serial.print(" ");*/
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    ds_data[i] = ds.read();
    //   Serial.print(ds_data[i], HEX);
    //  Serial.print(" ");
  }
  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (ds_data[1] << 8) | ds_data[0];
  /*Serial.println(data[1]);
    Serial.println(data[0]);
    Serial.println(raw);*/
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (ds_data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - ds_data[6];
    }
  } else {
    byte cfg = (ds_data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }

  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;



  Serial.println("Transmitting...");

  float t = celsius;
  float h = 75 * 30.0 / celsius;

  String d = "{\"Temp\":" + String (t) + ",";
  d += "\"Hum\":" + String (h);
  d += "} "; // Add a trailing space is necessary

  char data[d.length()];
  d.toCharArray(data, d.length());
  Serial.println(d);
  rf95.send((uint8_t*)data, sizeof(data));

  Serial.println("Waiting for packet to complete...");
  delay(1000);
  rf95.waitPacketSent();
  Serial.println(" complete...");

}
