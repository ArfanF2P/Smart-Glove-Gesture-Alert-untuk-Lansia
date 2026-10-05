/**
 * Rekam data LIS2DH per sesi, dipicu lewat Serial Monitor.
 *
 * Cara pakai:
 *   1. Upload sketch ini, buka Serial Monitor (115200 baud).
 *   2. Ketik '1' lalu Enter untuk mulai satu sesi rekam.
 *   3. Sesi berjalan otomatis selama RECORD_DURATION_SECONDS, lalu berhenti sendiri.
 *   4. Ulangi kapan pun untuk sesi berikutnya (gestur yang sama atau beda).
 *
 * Output per sesi diapit marker START ... END, dengan header CSV
 * yang sudah sesuai format yang diminta Edge Impulse:
 *   timestamp,accX,accY,accZ
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
#define LIS_ADDR 0x18   // ganti ke 0x19 jika perlu (cek lewat i2c_scanner)

// ==== Ubah dua nilai ini sesuai kebutuhan sesi rekam ====
const unsigned long RECORD_DURATION_SECONDS = 5;    // lama tiap sesi (detik)
const unsigned int  SAMPLE_RATE_HZ          = 50;  // sampling rate (Hz)

const unsigned long SAMPLE_INTERVAL_MS = 1000UL / SAMPLE_RATE_HZ;
const unsigned long RECORD_DURATION_MS = RECORD_DURATION_SECONDS * 1000UL;

DFRobot_LIS2DH12 LIS;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);

  while (LIS.init(LIS2DH12_RANGE_4GA, LIS_ADDR) == -1) {
    Serial.println("LIS2DH tidak terdeteksi. Cek wiring & alamat I2C.");
    delay(1000);
  }
  LIS.writeReg(0x20, 0x57); // CTRL_REG1: ODR 100 Hz, mode normal, X/Y/Z aktif

  printMenu();
}

void printMenu() {
  Serial.println();
  Serial.print(F("Ketik '1' lalu Enter untuk mulai merekam selama "));
  Serial.print(RECORD_DURATION_SECONDS);
  Serial.println(F(" detik..."));
}

void loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '1') {
      recordSession();
      printMenu();
    }
  }
}

void recordSession() {
  Serial.println("START");
  Serial.println("timestamp,accX,accY,accZ");

  unsigned long sessionStart = millis();
  unsigned long nextSample   = sessionStart;
  unsigned long sessionEnd   = sessionStart + RECORD_DURATION_MS;

  while (millis() < sessionEnd) {
    if (millis() >= nextSample) {
      int16_t x, y, z;
      LIS.readXYZ(x, y, z);
      LIS.mgScale(x, y, z);

      Serial.print(nextSample - sessionStart); // timestamp relatif, ms
      Serial.print(",");
      Serial.print(x);
      Serial.print(",");
      Serial.print(y);
      Serial.print(",");
      Serial.println(z);

      nextSample += SAMPLE_INTERVAL_MS;
    }
  }

  Serial.println("END");
}
