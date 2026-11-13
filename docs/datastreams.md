# Konfigurasi Datastream & Web Dashboard Blynk

Template Name: `SmartHome`

## 1. Daftar 17 Datastream (Virtual Pin)

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
| V9 | `batas_daya` | Integer | 100 | 1500 | 900 | W | Dashboard -> ESP32 |
| V10 | `suhu_tamu` | Double | 0 | 50 | 0 | °C | ESP32 -> Dashboard |
| V11 | `suhu_kamar` | Double | 0 | 50 | 0 | °C | ESP32 -> Dashboard |
| V12 | `total_daya` | Integer | 0 | 2000 | 0 | W | ESP32 -> Dashboard |
| V13 | `status_beban` | String | - | - | - | - | ESP32 -> Dashboard |
| V14 | `daya_outlet1` | Integer | 0 | 150 | 0 | W | ESP32 -> Dashboard |
| V15 | `daya_outlet2` | Integer | 0 | 400 | 0 | W | ESP32 -> Dashboard |
| V16 | `daya_outlet3` | Integer | 0 | 500 | 0 | W | ESP32 -> Dashboard |

## 2. Susunan Widget Web Dashboard

| Widget | Datastream | Keterangan |
|---|---|---|
| Switch (2) | `V0`, `V2` | Label `AC 1 Manual`, `AC 2 Manual` (ON = mode manual) |
| Switch (2) | `V1`, `V3` | Label `AC 1 Nyala`, `AC 2 Nyala` (hanya berlaku di mode manual) |
| Switch | `V4` | Label `Lampu 1-2 Manual` |
| Switch (4) | `V5`, `V6`, `V7`, `V8` | Lampu 1 (Tamu), Lampu 2 (Dapur), Lampu 3 (Kamar), Lampu 4 (Mandi) |
| Slider | `V9` | Batas Daya, rentang `100-1500` W, langkah `50` |
| Gauge (2) | `V10`, `V11` | Suhu Tamu dan Suhu Kamar, rentang `15-40` °C |
| Label | `V13` | Status Beban (teks: `Aman`, `Diputus: ...`, atau peringatan) |
| Value / Label | `V12` | Total Daya (W) |
| Value / Label (3) | `V14`, `V15`, `V16` | Daya Outlet 1 (Kulkas), Outlet 2 (Rice Cooker), Outlet 3 (Dispenser) |
| Chart (opsional) | `V12` | Riwayat Total Daya |
