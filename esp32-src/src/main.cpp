/*
  ESP32 WiFi Configuration via Web (Captive Portal)
  --------------------------------------------------
  - Saat pertama kali nyala (atau gagal connect ke WiFi tersimpan),
    ESP32 akan buat Access Point sendiri bernama "ESP32-Setup".
  - Konek ke AP itu dari HP/laptop, browser otomatis kebuka
    halaman setup (captive portal), atau buka manual ke 192.168.4.1
  - Pilih SSID + masukin password, ESP32 akan restart dan connect
    ke WiFi tersebut, lalu menyimpannya di flash untuk nyala berikutnya.
  - Tahan tombol BOOT (GPIO0) saat nyala kalau mau reset WiFi tersimpan.

  Library: WiFiManager by tzapu (didefinisikan di platformio.ini)
*/

#include <WiFi.h>
#include <WiFiManager.h>

#define TRIGGER_PIN 0      // tombol BOOT bawaan ESP32, aktif LOW
#define AP_NAME     "ESP32-Setup"
#define AP_PASSWORD ""     // kosongkan = AP tanpa password. Isi min 8 char kalau mau proteksi

WiFiManager wm;

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("Booting ESP32 WiFi Config...");

  pinMode(TRIGGER_PIN, INPUT_PULLUP);

  // Kalau tombol BOOT ditekan saat startup -> hapus setting WiFi lama
  if (digitalRead(TRIGGER_PIN) == LOW) {
    Serial.println("Tombol BOOT ditekan, reset WiFi settings...");
    wm.resetSettings();
  }

  // Timeout config portal (detik). Kalau habis, ESP32 restart.
  wm.setConfigPortalTimeout(180);

  // Callback saat masuk mode AP config
  wm.setAPCallback([](WiFiManager *myWM) {
    Serial.println("Masuk mode Access Point untuk konfigurasi.");
    Serial.print("Konek ke WiFi: ");
    Serial.println(AP_NAME);
    Serial.println("Lalu buka browser ke 192.168.4.1 (atau tunggu captive portal muncul otomatis)");
  });

  bool connected;
  if (strlen(AP_PASSWORD) > 0) {
    connected = wm.autoConnect(AP_NAME, AP_PASSWORD);
  } else {
    connected = wm.autoConnect(AP_NAME);
  }

  if (!connected) {
    Serial.println("Gagal connect & timeout. Restarting...");
    ESP.restart();
    delay(1000);
  }

  Serial.println("Berhasil connect ke WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
}

void loop() {
  // Cek koneksi tiap 10 detik, kalau putus coba reconnect
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 10000) {
    lastCheck = millis();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi terputus, mencoba reconnect...");
      WiFi.reconnect();
    } else {
      Serial.print("Status: connected, IP = ");
      Serial.println(WiFi.localIP());
    }
  }

  // Taruh kode utama kamu di sini
}
