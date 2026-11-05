# UTBK Tactics: SNBT Exam Arena

Game edukasi berbasis **C99** dan **Raylib** yang mensimulasikan UTBK SNBT dengan nuansa visual novel. Pemain memasukkan nama, memilih tingkat kesulitan, lalu menjalani sesi ujian yang dipandu oleh karakter bergaya *Classroom of the Elite*.

> **Status:** Phase 1 selesai. Sesi ujian, scoring IRT, pembahasan, TTS, dan leaderboard masih berupa placeholder dan akan dibangun pada fase berikutnya.

![Menu utama](docs/screenshots/menu.png)

## Tangkapan Layar

| Pemilihan Difficulty | Pemilihan Subtes |
| :---: | :---: |
| ![Difficulty](docs/screenshots/difficulty.png) | ![Subtes](docs/screenshots/subtest.png) |

## Fitur Phase 1

- State machine dengan 7 state sesuai SDD: menu, difficulty, subtes, ujian, ringkasan skor, pembahasan, dan leaderboard.
- Input nama pemain interaktif dengan validasi, batas 31 karakter, caret, dan backspace.
- Navigasi menu menggunakan keyboard dan mouse.
- Lima tingkat kesulitan dengan konfigurasi time multiplier, score multiplier, dan status TTS.
- Data 7 subtes resmi UTBK SNBT beserta jumlah soal dan durasi acuan mode Hard.
- Tampilan custom immediate-mode dengan animasi background, panel, tombol, dan transisi fade.
- Font sistem otomatis (Segoe UI / DejaVu Sans) dengan fallback ke font bawaan Raylib.
- Placeholder terstruktur untuk state yang akan dikerjakan pada Phase 2–5.

## Teknologi

- **Bahasa:** C99
- **Graphics & Multimedia:** Raylib 5.5+
- **Build:** GNU Make / GCC (MinGW-w64)
- **Target:** Windows (MSYS2 UCRT64) dan Linux

## Struktur Proyek

```text
.
├── src/
│   ├── main.c          # Entry point dan game loop
│   ├── game.c/.h       # State machine, session, difficulty, font
│   ├── screens.c/.h    # Menu, difficulty, subtes, placeholder
│   └── ui.c/.h         # Komponen UI custom
├── docs/
│   └── screenshots/    # Dokumentasi visual
├── Makefile
├── Plan.md
├── SDD.md
└── README.md
```

## Persyaratan

- GCC dengan dukungan C99
- Raylib **5.5 atau lebih baru**
- GNU Make (opsional, untuk build via Makefile)

## Cara Build

### Windows — MSYS2 UCRT64 (direkomendasikan)

Jalankan dari **MSYS2 UCRT64** shell:

```bash
pacman -Syu
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-raylib make
```

Lalu dari folder proyek:

```bash
make
./utbk_tactics.exe
```

### Windows — PowerShell (build manual)

Pastikan GCC dan Raylib sudah terpasang dan tersedia di `PATH`:

```powershell
gcc -std=c99 -Wall -Wextra -O2 -Isrc src/main.c src/game.c src/ui.c src/screens.c `
    -o utbk_tactics.exe -lraylib -lopengl32 -lgdi32 -lwinmm -lm

.\utbk_tactics.exe
```

### Linux

Pastikan Raylib 5.5+ tersedia. Jika paket distro masih lebih lama, build Raylib dari source terlebih dahulu.

```bash
make
./utbk_tactics
```

## Kontrol

### Menu Utama

| Tombol | Aksi |
| --- | --- |
| Ketik | Mengisi nama pemain |
| `Tab` / `↑` / `↓` / `W` / `S` | Berpindah fokus |
| `Enter` | Memilih / melanjutkan |
| `Backspace` | Menghapus karakter |
| `Esc` | Keluar |

### Pemilihan Difficulty

| Tombol | Aksi |
| --- | --- |
| `A` / `D` / `←` / `→` | Memilih difficulty |
| `1`–`5` | Memilih difficulty secara langsung |
| `Enter` / `Space` | Lanjut ke pemilihan subtes |
| `Esc` | Kembali ke menu |

## Roadmap

- [x] **Phase 1** — Boilerplate, state machine, input nama, pemilihan difficulty
- [ ] **Phase 2** — Layout ujian, scrolling passage, sprite Sae Chabashira
- [ ] **Phase 3** — Parsing JSON modular, 1600+ bank soal, Fisher–Yates, timer per subtes
- [ ] **Phase 4** — Scoring IRT, review phase, sprite Arisu Sakayanagi, TTS
- [ ] **Phase 5** — Leaderboard Top 10, file I/O, memory management

## Catatan Pengembangan

- Phase 1 belum memuat bank soal, timer ujian, scoring, TTS, atau leaderboard.
- Tombol **Papan Peringkat** dan state lanjutan saat ini menampilkan placeholder informatif.
- Data soal akan dipecah menjadi 7 file JSON modular pada Phase 3.
- Aset karakter akan ditempatkan di `assets/characters/` pada Phase 2.

## Kredit & Disclaimer

Proyek penggemar yang tidak berafiliasi dengan penyelenggara resmi UTBK/SNBT maupun pemegang hak *Classroom of the Elite*. Karakter dan aset pihak ketiga tetap menjadi milik pemegang haknya masing-masing.

Dibuat dengan [Raylib](https://www.raylib.com/).

## Lisensi

Kode sumber dilisensikan di bawah [MIT License](LICENSE).
