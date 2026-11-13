# Pemetaan Pin & Komponen Wokwi (ESP32 DevKit C V4)

## 1. Daftar Komponen

| Komponen | Jumlah | Tipe Wokwi (`diagram.json`) | Keterangan |
|---|---|---|---|
| ESP32 | 1 | `board-esp32-devkit-c-v4` | Pengendali utama |
| DHT22 | 2 | `wokwi-dht22` | Sensor suhu ruang tamu dan kamar |
| LDR | 1 | `wokwi-photoresistor-sensor` | Sensor cahaya modul (pin AO) |
| Potensiometer | 3 | `wokwi-potentiometer` | Simulasi beban daya Outlet 1-3 |
| LED | 9 | `wokwi-led` | Indikator AC (biru), lampu (kuning), outlet (hijau) |

## 2. Pemetaan Pin GPIO ESP32

| GPIO ESP32 | Terhubung ke | Fungsi |
|---|---|---|
| 32 | Potensiometer 1, pin `SIG` | Daya Outlet 1 / Kulkas (0-150 W) |
| 33 | Potensiometer 2, pin `SIG` | Daya Outlet 2 / Rice Cooker (0-400 W) |
| 34 | Potensiometer 3, pin `SIG` | Daya Outlet 3 / Dispenser (0-500 W) |
| 35 | Modul LDR, pin `AO` | Terang/gelap ruang tamu & dapur |
| 4 | DHT22 #1 (`dht_tamu`), pin `SDA` | Suhu ruang tamu dan dapur |
| 27 | DHT22 #2 (`dht_kamar`), pin `SDA` | Suhu kamar tidur |
| 13 | LED AC 1 (`led_ac1`), anode `A` | Status AC 1 (ruang tamu) |
| 14 | LED AC 2 (`led_ac2`), anode `A` | Status AC 2 (kamar tidur) |
| 16 | LED Lampu 1 (`led_l1`), anode `A` | Lampu 1 (ruang tamu) |
| 17 | LED Lampu 2 (`led_l2`), anode `A` | Lampu 2 (dapur) |
| 18 | LED Lampu 3 (`led_l3`), anode `A` | Lampu 3 (kamar tidur) |
| 19 | LED Lampu 4 (`led_l4`), anode `A` | Lampu 4 (kamar mandi) |
| 21 | LED Outlet 1 (`led_o1`), anode `A` | Outlet 1 (menyala = tersambung) |
| 22 | LED Outlet 2 (`led_o2`), anode `A` | Outlet 2 (menyala = tersambung) |
| 23 | LED Outlet 3 (`led_o3`), anode `A` | Outlet 3 (menyala = tersambung) |

Catatan:
* Pin 6-11 tidak dipakai (terhubung ke flash internal ESP32). GPIO 5 tidak dipakai karena merupakan pin strapping. GPIO 34-35 hanya dipakai sebagai input.
* Semua pin `VCC` sensor & potensiometer disambungkan ke `3V3`, semua `GND` sensor/potensiometer serta katode (`C`) LED disambungkan ke pin `GND` ESP32.
* Pin `NC` pada DHT22 dibiarkan tidak terhubung.
* Pada simulasi Wokwi, resistor 220 ohm untuk LED dilewati agar rangkaian ringkas; pada implementasi perangkat keras nyata, setiap LED wajib memakai resistor pembatas arus (220 ohm).
