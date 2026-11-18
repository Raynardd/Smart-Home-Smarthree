# Konfigurasi Datastream & Web Dashboard Blynk

Template Name: `SmartHome`

## 1. Daftar 21 Datastream (Virtual Pin)

| Pin | Nama (Datastream name) | Tipe | Min | Max | Default | Unit | Arah |
|---|---|---|---|---|---|---|---|
| V0 | `ac1_mode` | Integer | 0 | 1 | 0 | - | Dashboard -> ESP32 (0 = otomatis, 1 = manual) |
| V1 | `ac1_switch` | Integer | 0 | 1 | 0 | - | Dashboard -> ESP32 |
| V2 | `ac2_mode` | Integer | 0 | 1 | 0 | - | Dashboard -> ESP32 |
| V3 | `ac2_switch` | Integer | 0 | 1 | 0 | - | Dashboard -> ESP32 |
| V4 | `lampu_mode` | Integer | 0 | 1 | 0 | - | Dashboard -> ESP32 (lampu 1 dan 2) |
| V5 | `lampu1_switch` | Integer | 0 | 1 | 0 | - | Dashboard -> ESP32 |
| V6 | `lampu2_switch` | Integer | 0 | 1 | 0 | - | Dashboard -> ESP32 |
| V7 | `lampu3_switch` | Integer | 0 | 1 | 0 | - | Dashboard -> ESP32 |
| V8 | `lampu4_switch` | Integer | 0 | 1 | 0 | - | Dashboard -> ESP32 |
| V9 | `batas_daya` | Integer | 0 | 1500 | 900 | W | ESP32 -> Dashboard (tetap 900 W / *hardcoded*) |
| V10 | `suhu_tamu` | Double | 0 | 50 | 0 | °C | ESP32 -> Dashboard |
| V11 | `suhu_kamar` | Double | 0 | 50 | 0 | °C | ESP32 -> Dashboard |
| V12 | `total_daya` | Integer | 0 | 2000 | 0 | W | ESP32 -> Dashboard |
| V13 | `status_beban` | String | - | - | - | - | ESP32 -> Dashboard |
| V14 | `daya_outlet1` | Integer | 0 | 150 | 0 | W | ESP32 -> Dashboard |
| V15 | `daya_outlet2` | Integer | 0 | 400 | 0 | W | ESP32 -> Dashboard |
| V16 | `daya_outlet3` | Integer | 0 | 500 | 0 | W | ESP32 -> Dashboard |
| V17 | `status_ac1` | Integer | 0 | 1 | 0 | - | ESP32 -> Dashboard (status aktual AC 1) |
| V18 | `status_ac2` | Integer | 0 | 1 | 0 | - | ESP32 -> Dashboard (status aktual AC 2) |
| V19 | `status_lampu1` | Integer | 0 | 1 | 0 | - | ESP32 -> Dashboard (status aktual Lampu 1) |
| V20 | `status_lampu2` | Integer | 0 | 1 | 0 | - | ESP32 -> Dashboard (status aktual Lampu 2) |

## 2. Susunan Widget Web Dashboard

| Widget | Datastream | Keterangan |
|---|---|---|
| Switch (2) | `V0`, `V2` | Label `AC 1 Manual`, `AC 2 Manual` (ON = mode manual) |
| Switch (2) | `V1`, `V3` | Label `AC 1 Nyala`, `AC 2 Nyala` (hanya berlaku di mode manual) |
| Switch | `V4` | Label `Lampu 1-2 Manual` |
| Switch (4) | `V5`, `V6`, `V7`, `V8` | Lampu 1 (Tamu), Lampu 2 (Dapur), Lampu 3 (Kamar), Lampu 4 (Mandi) |
| Value / Label | `V9` | Batas Daya (menampilkan nilai tetap `900 W`) |
| HTML (1) | `V17`, `V18`, `V19`, `V20` | Widget HTML kustom (`src/blynk-indicator-widget.html`) yang menampilkan 4 indikator status aktual (`Status AC 1`, `Status AC 2`, `Status Lampu 1`, `Status Lampu 2`) dalam satu kartu (berurutan index `0`–`3`) |
| Label | `V13` | Status Beban (teks: `Aman`, `Diputus: ...`, atau peringatan) |
| Value / Label | `V12` | Total Daya (W) |
| Gauge (2) | `V10`, `V11` | Suhu Tamu dan Suhu Kamar, rentang `15-40` °C |
| Value / Label (3) | `V14`, `V15`, `V16` | Daya Outlet 1 (Kulkas), Outlet 2 (Rice Cooker), Outlet 3 (Dispenser) |
| Chart (1) | `V12` | Riwayat Total Daya (W) |
| Chart (1) | `V10`, `V11` | Riwayat Suhu Tamu & Kamar (°C) |
