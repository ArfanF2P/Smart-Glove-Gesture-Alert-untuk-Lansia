/**
 * I2C Scanner untuk ESP32-S3 + LIS2DH
 * Jalankan sketch ini PERTAMA, sebelum pakai library LIS2DH,
 * untuk memastikan wiring I2C benar dan menemukan alamat sensor.
 *
 * Wiring:
 *   LIS2DH VCC -> ESP32-S3 3V3
 *   LIS2DH GND -> ESP32-S3 GND
 *   LIS2DH SDA -> ESP32-S3 GPIO8
 *   LIS2DH SCL -> ESP32-S3 GPIO9
 */

#include <Wire.h>

#define SDA_PIN 8
#define SCL_PIN 9

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("\nI2C Scanner dimulai...");
}

void loop() {
  byte error, address;
  int devicesFound = 0;

  Serial.println("Memindai bus I2C...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Perangkat I2C ditemukan di alamat 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      devicesFound++;
    }
  }

  if (devicesFound == 0) {
    Serial.println("Tidak ada perangkat I2C ditemukan.");
    Serial.println("Cek kembali: VCC, GND, SDA, SCL, dan pastikan LIS2DH mendapat daya.");
  } else {
    Serial.print(devicesFound);
    Serial.println(" perangkat ditemukan. LIS2DH biasanya di 0x18 atau 0x19.");
  }

  delay(3000);
}
