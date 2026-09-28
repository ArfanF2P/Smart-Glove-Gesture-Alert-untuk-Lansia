/**
 * Baca akselerasi X, Y, Z dari LIS2DH via ESP32-S3
 * Library: DFRobot_LIS2DH12 (install lewat Library Manager)
 *
 * Setelah upload, buka Serial Monitor (115200 baud) atau
 * Serial Plotter untuk melihat grafik real-time.
 * Coba gerakkan sensor dengan tangan untuk melihat pola sinyalnya.
 *
 * Wiring:
 *   LIS2DH VCC -> ESP32-S3 3V3
 *   LIS2DH GND -> ESP32-S3 GND
 *   LIS2DH SDA -> ESP32-S3 GPIO8
 *   LIS2DH SCL -> ESP32-S3 GPIO9
 */

#include <Wire.h>
#include <DFRobot_LIS2DH12.h>

#define SDA_PIN 8
#define SCL_PIN 9

DFRobot_LIS2DH12 acce(&Wire, 0x18); // Jika gagal, coba ganti ke 0x19

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println("Menghubungkan ke LIS2DH...");
  while (!acce.begin()) {
    Serial.println("Gagal terhubung. Cek wiring & alamat I2C (0x18/0x19).");
    delay(1000);
  }

  Serial.print("Chip ID: 0x");
  Serial.println(acce.getID(), HEX);

  acce.setRange(DFRobot_LIS2DH12::eLIS2DH12_16g);
  acce.setAcquireRate(DFRobot_LIS2DH12::eLowPower_100Hz);

  Serial.println("Siap membaca data. Gerakkan sensor untuk melihat perubahan.");
  Serial.println("X\tY\tZ  (satuan: mg)");
}

void loop() {
  int16_t x = acce.readAccX();
  int16_t y = acce.readAccY();
  int16_t z = acce.readAccZ();

  Serial.print(x);
  Serial.print("\t");
  Serial.print(y);
  Serial.print("\t");
  Serial.println(z);

  delay(20); // ~50 Hz, cukup untuk observasi awal
}
