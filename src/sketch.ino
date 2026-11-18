#include "secrets.h"
#define BLYNK_PRINT Serial
#define BLYNK_HEARTBEAT 15
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// ======== PIN ========
const int PIN_POT[3]       = {32, 33, 34};      // Outlet 1, 2, 3
const int PIN_LDR          = 35;
const int PIN_DHT_TAMU     = 4;
const int PIN_DHT_KAMAR    = 27;
const int PIN_LED_AC1      = 13;
const int PIN_LED_AC2      = 14;
const int PIN_LED_LAMPU[4] = {16, 17, 18, 19};
const int PIN_LED_OUT[3]   = {21, 22, 23};

// ======== PARAMETER (ubah setelah dikalibrasi) ========
const float AC_ON_TEMP  = 28.0;                 // AC nyala jika suhu lebih dari ini
const float AC_OFF_TEMP = 26.0;                 // AC padam jika suhu kurang dari atau sama dengan ini
const int   AC_WATT     = 350;                  // asumsi daya AC saat menyala
const int   LIMIT_W     = 900;                  // batas daya tetap (hardcoded)
const int   MARGIN_W    = 50;                   // margin sebelum beban disambung lagi
const int   POT_MAX_W[3] = {150, 400, 500};     // rentang daya tiap outlet
const int   LDR_GELAP_ON  = 2500;               // di atas ini dianggap gelap
const int   LDR_GELAP_OFF = 2200;               // di bawah ini dianggap terang

// ======== OBJEK ========
DHT dhtTamu(PIN_DHT_TAMU, DHT22);
DHT dhtKamar(PIN_DHT_KAMAR, DHT22);
BlynkTimer timer;

// ======== STATUS DARI DASHBOARD ========
int acMode[2]      = {0, 0};        // 0 = otomatis, 1 = manual (indeks 0 = AC1 tamu, 1 = AC2 kamar)
int acSwitch[2]    = {0, 0};
int lampuMode      = 0;             // 0 = otomatis (lampu 1 dan 2), 1 = manual
int lampuSwitch[4] = {0, 0, 0, 0};

// ======== STATUS INTERNAL ========
float tempTamu = NAN, tempKamar = NAN;
bool  acWant[2]  = {false, false};
bool  lampuGelap = false;

// Beban diurutkan berdasarkan prioritas (indeks kecil = prioritas tinggi):
// 0 = Outlet 1, 1 = AC kamar, 2 = Outlet 2, 3 = AC tamu, 4 = Outlet 3
int  demand[5] = {0, 0, 0, 0, 0};
bool shed[5]   = {false, false, false, false, false};
const char* NAMA_BEBAN[5] = {"Outlet 1", "AC kamar", "Outlet 2", "AC tamu", "Outlet 3"};

bool ledState[40];

// cache pengiriman agar hemat pesan
float  lastT[2];
int    lastTotal;
int    lastP[3];
int    lastInd[4];
String lastStatus;

void resetCache() {
  lastT[0] = lastT[1] = -100;
  lastTotal = -1;
  lastP[0] = lastP[1] = lastP[2] = -1;
  lastInd[0] = lastInd[1] = lastInd[2] = lastInd[3] = -1;
  lastStatus = "";
}

void logRx(const char* nama, int v) {
  Serial.printf("[%lu ms] terima %s = %d\n", millis(), nama, v);
}

void setLed(const char* nama, int pin, bool on) {
  if (ledState[pin] != on) {
    ledState[pin] = on;
    digitalWrite(pin, on ? HIGH : LOW);
    Serial.printf("[%lu ms] %s -> %s\n", millis(), nama, on ? "ON" : "OFF");
  }
}

// ======== TERIMA DATA DARI BLYNK ========
BLYNK_WRITE(V0) { acMode[0]   = param.asInt(); logRx("ac1_mode", acMode[0]); }
BLYNK_WRITE(V1) { acSwitch[0] = param.asInt(); logRx("ac1_switch", acSwitch[0]); }
BLYNK_WRITE(V2) { acMode[1]   = param.asInt(); logRx("ac2_mode", acMode[1]); }
BLYNK_WRITE(V3) { acSwitch[1] = param.asInt(); logRx("ac2_switch", acSwitch[1]); }
BLYNK_WRITE(V4) { lampuMode   = param.asInt(); logRx("lampu_mode", lampuMode); }
BLYNK_WRITE(V5) { lampuSwitch[0] = param.asInt(); logRx("lampu1_switch", lampuSwitch[0]); }
BLYNK_WRITE(V6) { lampuSwitch[1] = param.asInt(); logRx("lampu2_switch", lampuSwitch[1]); }
BLYNK_WRITE(V7) { lampuSwitch[2] = param.asInt(); logRx("lampu3_switch", lampuSwitch[2]); }
BLYNK_WRITE(V8) { lampuSwitch[3] = param.asInt(); logRx("lampu4_switch", lampuSwitch[3]); }

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0, V1, V2, V3, V4, V5, V6, V7, V8);
  Blynk.virtualWrite(V9, LIMIT_W);
  resetCache();
}

// ======== SENSOR (tiap 2 detik) ========
void readSensors() {
  float t1 = dhtTamu.readTemperature();
  float t2 = dhtKamar.readTemperature();
  if (!isnan(t1)) tempTamu = t1;
  if (!isnan(t2)) tempKamar = t2;

  int ldr = analogRead(PIN_LDR);
  if (ldr > LDR_GELAP_ON) lampuGelap = true;
  else if (ldr < LDR_GELAP_OFF) lampuGelap = false;
}

int readOutletWatt(int i) {
  return map(analogRead(PIN_POT[i]), 0, 4095, 0, POT_MAX_W[i]);
}

// ======== KEPUTUSAN AC ========
void decideAc() {
  float temp[2] = {tempTamu, tempKamar};
  for (int i = 0; i < 2; i++) {
    if (acMode[i] == 1) {
      acWant[i] = (acSwitch[i] == 1);
    } else if (!isnan(temp[i])) {
      if (temp[i] > AC_ON_TEMP) acWant[i] = true;
      else if (temp[i] <= AC_OFF_TEMP) acWant[i] = false;
    }
  }
}

// ======== PEMBATASAN BEBAN ========
int totalActive() {
  int sum = 0;
  for (int i = 0; i < 5; i++) if (!shed[i]) sum += demand[i];
  return sum;
}

void updateShedding() {
  demand[0] = readOutletWatt(0);
  demand[1] = acWant[1] ? AC_WATT : 0;
  demand[2] = readOutletWatt(1);
  demand[3] = acWant[0] ? AC_WATT : 0;
  demand[4] = readOutletWatt(2);

  // beban yang tidak lagi meminta daya tidak perlu berstatus diputus
  for (int i = 0; i < 5; i++) if (demand[i] == 0) shed[i] = false;

  // 1) putus dari prioritas terendah sampai total di bawah batas.
  //    Outlet 1 (indeks 0) tidak pernah diputus.
  bool adaPemutusan = false;
  while (totalActive() > LIMIT_W) {
    int korban = -1;
    for (int i = 4; i >= 1; i--) {
      if (!shed[i] && demand[i] > 0) { korban = i; break; }
    }
    if (korban < 0) break;
    int sebelum = totalActive();
    shed[korban] = true;
    adaPemutusan = true;
    Serial.printf("[%lu ms] SHED %s (%d W -> %d W, batas %d W)\n",
                  millis(), NAMA_BEBAN[korban], sebelum, totalActive(), LIMIT_W);
  }

  // 2) sambung kembali dari prioritas tertinggi, hanya jika muat dengan margin
  if (!adaPemutusan) {
    for (int i = 1; i <= 4; i++) {
      if (!shed[i]) continue;
      if (totalActive() + demand[i] <= LIMIT_W - MARGIN_W) {
        shed[i] = false;
        Serial.printf("[%lu ms] SAMBUNG %s (total %d W)\n", millis(), NAMA_BEBAN[i], totalActive());
      } else {
        break;
      }
    }
  }
}

// ======== TERAPKAN KE LED ========
void applyOutputs() {
  setLed("LED AC1 (tamu)",  PIN_LED_AC1, acWant[0] && !shed[3]);
  setLed("LED AC2 (kamar)", PIN_LED_AC2, acWant[1] && !shed[1]);

  setLed("LED Outlet 1", PIN_LED_OUT[0], !shed[0]);
  setLed("LED Outlet 2", PIN_LED_OUT[1], !shed[2]);
  setLed("LED Outlet 3", PIN_LED_OUT[2], !shed[4]);

  bool otomatis = (lampuMode == 0);
  setLed("LED Lampu 1", PIN_LED_LAMPU[0], otomatis ? lampuGelap : lampuSwitch[0] == 1);
  setLed("LED Lampu 2", PIN_LED_LAMPU[1], otomatis ? lampuGelap : lampuSwitch[1] == 1);
  setLed("LED Lampu 3", PIN_LED_LAMPU[2], lampuSwitch[2] == 1);
  setLed("LED Lampu 4", PIN_LED_LAMPU[3], lampuSwitch[3] == 1);
}

void controlLoop() {
  decideAc();
  updateShedding();
  applyOutputs();
}

// ======== KIRIM KE BLYNK (hanya jika berubah) ========
int effective(int i) { return shed[i] ? 0 : demand[i]; }

String statusText() {
  if (totalActive() > LIMIT_W) return "Peringatan: Outlet 1 melebihi batas";
  String s = "";
  for (int i = 0; i < 5; i++) {
    if (shed[i]) {
      if (s.length() > 0) s += ", ";
      s += NAMA_BEBAN[i];
    }
  }
  return s.length() == 0 ? String("Aman") : "Diputus: " + s;
}

void sendToBlynk() {
  if (!Blynk.connected()) return;

  float t[2] = {tempTamu, tempKamar};
  for (int i = 0; i < 2; i++) {
    if (!isnan(t[i]) && fabs(t[i] - lastT[i]) >= 0.5) {
      Blynk.virtualWrite(V10 + i, t[i]);
      lastT[i] = t[i];
    }
  }

  int total = totalActive();
  if (total != lastTotal) { Blynk.virtualWrite(V12, total); lastTotal = total; }

  int p[3] = {effective(0), effective(2), effective(4)};
  for (int i = 0; i < 3; i++) {
    if (p[i] != lastP[i]) { Blynk.virtualWrite(V14 + i, p[i]); lastP[i] = p[i]; }
  }

  int ind[4] = {
    ledState[PIN_LED_AC1] ? 1 : 0,
    ledState[PIN_LED_AC2] ? 1 : 0,
    ledState[PIN_LED_LAMPU[0]] ? 1 : 0,
    ledState[PIN_LED_LAMPU[1]] ? 1 : 0
  };
  for (int i = 0; i < 4; i++) {
    if (ind[i] != lastInd[i]) { Blynk.virtualWrite(V17 + i, ind[i]); lastInd[i] = ind[i]; }
  }

  String st = statusText();
  if (st != lastStatus) { Blynk.virtualWrite(V13, st.c_str()); lastStatus = st; }
}

// ======== SETUP DAN LOOP ========
void setup() {
  Serial.begin(115200);

  int semuaLed[] = {PIN_LED_AC1, PIN_LED_AC2,
                    PIN_LED_LAMPU[0], PIN_LED_LAMPU[1], PIN_LED_LAMPU[2], PIN_LED_LAMPU[3],
                    PIN_LED_OUT[0], PIN_LED_OUT[1], PIN_LED_OUT[2]};
  for (int pin : semuaLed) { pinMode(pin, OUTPUT); digitalWrite(pin, LOW); }

  dhtTamu.begin();
  dhtKamar.begin();
  resetCache();

  // Channel 6 mempercepat koneksi Wi-Fi virtual Wokwi (tanpa scan semua channel)
  Serial.println("Menghubungkan ke WiFi (channel 6)...");
  WiFi.begin("Wokwi-GUEST", "", 6);
  int coba = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
    if (++coba >= 60) {
      Serial.println("\nWiFi lambat, ulangi...");
      WiFi.disconnect(true);
      delay(500);
      WiFi.begin("Wokwi-GUEST", "", 6);
      coba = 0;
    }
  }
  Serial.printf("\nWiFi OK! IP: %s\n", WiFi.localIP().toString().c_str());

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect(15000);

  readSensors();
  timer.setInterval(2000L, readSensors);
  timer.setInterval(300L, controlLoop);
  timer.setInterval(2000L, sendToBlynk);
}

void loop() {
  Blynk.run();
  timer.run();
}
