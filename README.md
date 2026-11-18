# Simulasi Smart Home Cloud IoT dengan Pembatasan Beban Listrik

Proyek Mini Telematika (Topik 9: Simulasi Cloud IoT untuk Smart Home).
Terakhir diperbarui: 13 November 2026.

## Anggota Kelompok
1. Vincentius Raynard Thedy (`140810240028`) - Pembatasan beban, pengelolaan GitHub, integrasi akhir
2. Bagas Anung Bayu Nugraha (`140810240046`) - Logika AC otomatis dan desain dashboard Blynk
3. Hamdan Arif (`140810240055`) - Lampu otomatis, pengujian latensi dan kestabilan, dokumentasi

## Tujuan & Arsitektur Sistem
Proyek ini mensimulasikan sistem Smart Home untuk hunian/apartemen berdaya awal **900 W** (representasi disederhanakan dari golongan 900 VA) menggunakan **ESP32 di Wokwi** yang terhubung ke **Blynk Cloud**.

Ringkasan perilaku sistem:
* **AC Otomatis (2 unit, masing-masing diasumsikan 350 W):** Menyala jika suhu `> 28,0°C` dan padam jika `<= 26,0°C` (histeresis `2°C`). Mendukung mode manual dari dashboard Blynk.
* **Lampu Otomatis & Manual (4 unit, daya diabaikan):** Lampu 1 (tamu) dan Lampu 2 (dapur) mengikuti sensor LDR (`> 2500` gelap/ON, `< 2200` terang/OFF) dengan override mode manual. Lampu 3 (kamar) dan Lampu 4 (kamar mandi) dikendalikan manual via dashboard.
* **Pembatasan Beban (Load Shedding) Berprioritas:** Urutan prioritas dari tertinggi ke terendah:
  1. **Outlet 1 / Kulkas** (`0-150 W`, tidak pernah diputus)
  2. **AC Kamar Tidur / AC 2** (`350 W`)
  3. **Outlet 2 / Rice Cooker** (`0-400 W`)
  4. **AC Ruang Tamu / AC 1** (`350 W`)
  5. **Outlet 3 / Dispenser** (`0-500 W`, diputus paling awal)
* Jika total daya melampaui batas daya tetap (`LIMIT_W = 900 W`), beban prioritas terendah diputus hingga total di bawah batas (load shedding selalu mengalahkan mode otomatis maupun manual). Beban disambungkan kembali setelah total turun di bawah `LIMIT_W - 50 W` (margin `50 W`).

## Struktur Repositori

```text
smart-home/
  README.md
  .gitignore
  src/
    sketch.ino          # Program utama ESP32 + Blynk + Wokwi
    diagram.json        # Skema rangkaian virtual Wokwi
    libraries.txt       # Daftar library Wokwi
    secrets.example.h   # Contoh kredensial (salin menjadi secrets.h lokal)
  docs/
    datastreams.md      # Konfigurasi 21 datastream & widget dashboard Blynk
    pin-mapping.md      # Daftar komponen dan pemetaan pin GPIO ESP32
    diagram/            # Gambar diagram arsitektur & aliran data
```

## Cara Menjalankan di Wokwi
1. Siapkan Template `SmartHome` dan Device di Blynk Cloud sesuai panduan pada [docs/datastreams.md](docs/datastreams.md).
2. Buka [wokwi.com](https://wokwi.com/) -> **New Project** -> **ESP32**.
3. Isi tab `libraries.txt`, `diagram.json`, dan `sketch.ino` sesuai file di folder `src/`.
4. Buat tab baru bernama **`secrets.h`** di Wokwi dengan format dari [src/secrets.example.h](src/secrets.example.h), lalu isi `BLYNK_TEMPLATE_ID`, `BLYNK_TEMPLATE_NAME`, dan `BLYNK_AUTH_TOKEN` milik device masing-masing (satu device per anggota agar koneksi tidak saling menendang).
5. Jalankan simulasi (tombol **Start**). Hentikan simulasi saat tidak menguji agar kuota 100.000 pesan/bulan Blynk tetap awet.

## Keamanan Token
* File `secrets.h` **dilarang** di-commit ke GitHub dan sudah didaftarkan di `.gitignore`.
* Hanya `src/secrets.example.h` (nilai placeholder) yang masuk ke repositori.
* Jangan membagikan tautan proyek Wokwi publik yang masih berisi `BLYNK_AUTH_TOKEN` asli di `secrets.h`.

## Alur Kerja Git & Sinkronisasi
* Wokwi tidak tersinkron otomatis dengan GitHub; salin perubahan `sketch.ino` atau `diagram.json` secara manual lalu catat pembaruannya di commit.
* Branch yang disarankan: `main` (stabil), `feature/ac`, `feature/lampu`, `feature/load-shedding`.
