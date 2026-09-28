// Pin LED bawaan pada sebagian besar board ESP32-S3
#define RGB_BUILTIN 48

void setup() {
  // Inisialisasi komunikasi serial untuk monitor
  Serial.begin(115200);
}

void loop() {
  // Nyalakan LED warna MERAH (Format: Pin, R, G, B | nilai 0 - 255)
  neopixelWrite(RGB_BUILTIN, 64, 0, 0); 
  Serial.println("LED RGB: Nyala (Merah)");
  delay(1000); // Jeda 1 detik

  // Matikan LED RGB (Semua warna 0)
  neopixelWrite(RGB_BUILTIN, 0, 0, 0); 
  Serial.println("LED RGB: Mati");
  delay(1000); // Jeda 1 detik
}