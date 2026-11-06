# UTBK Tactics: SNBT Exam Arena

Game edukasi berbasis **C99** dan **Raylib** yang mensimulasikan UTBK SNBT dengan nuansa visual novel. Pemain memasukkan nama, memilih tingkat kesulitan, lalu menjalani sesi ujian yang dipandu oleh karakter bergaya *Classroom of the Elite*.

> **Status:** Phase 2 selesai. Layar ujian interaktif sudah berjalan dengan bank soal demo. Scoring IRT, pembahasan, TTS, dan leaderboard masih berupa placeholder dan akan dibangun pada fase berikutnya.

![Menu utama](docs/screenshots/menu.png)

## Tangkapan Layar

| Pemilihan Difficulty | Pemilihan Subtes |
| :---: | :---: |
| ![Difficulty](docs/screenshots/difficulty.png) | ![Subtes](docs/screenshots/subtest.png) |

### Layar Ujian

![Layar ujian](docs/screenshots/exam.png)

## Fitur Phase 2

- State machine dengan 7 state sesuai SDD: menu, difficulty, subtes, ujian, ringkasan skor, pembahasan, dan leaderboard.
- Input nama pemain interaktif dengan validasi, batas 31 karakter, caret, dan backspace.
- Lima tingkat kesulitan dengan konfigurasi time multiplier, score multiplier, dan status TTS.
- Pemilihan subtes interaktif dengan 7 subtes resmi UTBK SNBT dan durasi per subtes.
- Layar ujian dengan header subtes, timer countdown, progress bar, dan tombol keluar.
- Panel stimulus dengan word-wrap, scrollbar, serta gulir via mouse wheel dan `PageUp`/`PageDown`.
- Panel soal dengan navigator 30 soal, status terjawab, opsi A–E, dan fokus keyboard/mouse.
- Sprite Sae Chabashira 3 frame dengan animasi idle dan tips kontekstual per soal.
- Overlay konfirmasi selesai ujian, keluar ujian, dan waktu habis.
- Bank soal demo 15 soal: PU 3 soal, PPU/PBM/PK/LBI/LBE/PM masing-masing 2 soal.
- Tampilan custom immediate-mode dengan animasi background, panel, tombol, dan transisi fade.
- Font sistem otomatis (Segoe UI / DejaVu Sans) dengan fallback ke font bawaan Raylib.
- Placeholder terstruktur untuk scoring IRT, pembahasan, TTS, dan leaderboard (Phase 3–5).

## Teknologi

- **Bahasa:** C99
- **Graphics & Multimedia:** Raylib 5.5+
- **Build:** GNU Make / GCC (MinGW-w64)
- **Target:** Windows (MSYS2 UCRT64) dan Linux

## Struktur Proyek

```text
.
├── src/
│   ├── main.c              # Entry point dan game loop
│   ├── game.c/.h           # State machine, session, difficulty, font, asset
│   ├── screens.c/.h        # Menu, difficulty, subtes, placeholder
│   ├── exam.c/.h           # Layar ujian, timer, scroll, overlay
│   ├── demo_questions.c/.h # Bank soal demo 7 subtes
│   └── ui.c/.h             # Komponen UI custom
├── assets/
│   └── characters/
│       └── sae_chabashira.png
├── docs/
│   └── screenshots/        # Dokumentasi visual
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
    src/exam.c src/demo_questions.c `
    -o utbk_tactics.exe -lraylib -lopengl32 -lgdi32 -lwinmm -luser32 -lm

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

### Pemilihan Subtes

| Tombol | Aksi |
| --- | --- |
| `W` / `S` / `↑` / `↓` | Memilih subtes |
| `Enter` | Memulai ujian |
| `Esc` | Kembali ke pemilihan difficulty |

### Layar Ujian

| Tombol | Aksi |
| --- | --- |
| `A` / `D` / `←` / `→` | Soal sebelumnya / berikutnya |
| `W` / `S` / `↑` / `↓` | Memindahkan fokus opsi |
| `1`–`5` | Memilih jawaban A–E |
| `Enter` | Mengunci jawaban pada opsi yang fokus |
| `F` | Membuka konfirmasi selesai ujian |
| `PageUp` / `PageDown` / scroll | Menggulir teks stimulus |
| `Esc` | Membuka konfirmasi keluar ujian |
| Mouse | Klik opsi, navigator soal, dan tombol |

## Roadmap

- [x] **Phase 1** — Boilerplate, state machine, input nama, pemilihan difficulty
- [x] **Phase 2** — Layout ujian, scrolling passage, sprite Sae Chabashira, bank soal demo
- [ ] **Phase 3** — Parsing JSON modular, bank soal lengkap 1600+, Fisher–Yates, seleksi soal per subtes
- [ ] **Phase 4** — Scoring IRT, review phase, sprite Arisu Sakayanagi, TTS
- [ ] **Phase 5** — Leaderboard Top 10, file I/O, memory management

## Catatan Pengembangan

- Phase 2 memuat bank soal demo 15 soal; bank soal lengkap 1600+ soal dan parsing JSON menyusul Phase 3.
- Timer per subtes sudah berjalan, tetapi scoring IRT, review, TTS, dan leaderboard masih placeholder.
- Tombol **Papan Peringkat** dan state lanjutan saat ini menampilkan placeholder informatif.
- Aset karakter berada di `assets/characters/sae_chabashira.png` dan dimuat saat runtime.

## Kredit & Disclaimer

Proyek penggemar yang tidak berafiliasi dengan penyelenggara resmi UTBK/SNBT maupun pemegang hak *Classroom of the Elite*. Karakter dan aset pihak ketiga tetap menjadi milik pemegang haknya masing-masing.

Dibuat dengan [Raylib](https://www.raylib.com/).

## Lisensi

Kode sumber dilisensikan di bawah [MIT License](LICENSE).
