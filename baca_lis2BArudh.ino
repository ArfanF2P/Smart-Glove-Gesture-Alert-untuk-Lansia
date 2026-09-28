/**
 * Baca akselerasi X, Y, Z dari LIS2DH via ESP32-S3
 * Library: DFRobot_LIS2DH12 (github.com/DFRobot/DFRobot_LIS2DH12)
 *          -> install via Sketch > Include Library > Add .ZIP Library
 *
 * Buka Serial Monitor (115200 baud) atau Serial Plotter untuk melihat data.
 *
 * Wiring:
 *   LIS2DH VCC -> ESP32-S3 3V3
 *   LIS2DH GND -> ESP32-S3 GND
 *   LIS2DH SDA -> ESP32-S3 GPIO8
 *   LIS2DH SCL -> ESP32-S3 GPIO9
 */

#include <Wire.h>
#include <DFRobot_LIS2DH12.h>

#define SDA_PIN  8
#define SCL_PIN  9
#define LIS_ADDR 0x18   // ganti ke 0x19 jika I2C scanner menemukan alamat itu

DFRobot_LIS2DH12 LIS;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Library ini TIDAK memanggil Wire.begin(), jadi wajib dipanggil di sini
  Wire.begin(SDA_PIN, SCL_PIN);

  // Range +-4g dulu (lebih akurat untuk gerakan tangan; hindari 16g karena
  // konversi mgScale() di library ini kurang tepat pada range tersebut)
  while (LIS.init(LIS2DH12_RANGE_4GA, LIS_ADDR) == -1) {
    Serial.println("LIS2DH tidak terdeteksi. Cek wiring & alamat I2C (0x18/0x19).");
    delay(1000);
  }

  Serial.print("WHO_AM_I: 0x");
  Serial.println(LIS.readReg(0x0F), HEX);   // LIS2DH12 seharusnya 0x33

  // Bawaan library: ODR 10 Hz + mode low-power 8-bit -> terlalu lambat untuk gestur.
  // 0x57 = ODR 100 Hz, mode normal, sumbu X/Y/Z aktif (register CTRL_REG1).
  LIS.writeReg(0x20, 0x57);

  Serial.println("Siap. Gerakkan sensor untuk melihat perubahan (satuan: mg).");
}

void loop() {
  int16_t x, y, z;
  LIS.readXYZ(x, y, z);
  LIS.mgScale(x, y, z);   // konversi ke milli-g

  Serial.print("X:"); Serial.print(x);
  Serial.print(",Y:"); Serial.print(y);
  Serial.print(",Z:"); Serial.println(z);

  delay(10);   // ~100 Hz
}
