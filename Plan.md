---

### 2. `Plan.md` (Project Development Plan)

```markdown
# Project Development Plan (Plan.md): UTBK Tactics: SNBT Exam Arena

Rencana implementasi pengembangan game C Raylib ini dibagi menjadi 5 fase terstruktur untuk memastikan kestabilan pengelolaan data bank soal besar dan mencegah error kompilasi.

## Phase 1: Boilerplate, State Machine & Input Nama
- [ ] Inisialisasi jendela Raylib (`InitWindow`, `SetTargetFPS(60)`).
- [ ] Membuat struktur `GameState` (`STATE_MENU`, `STATE_SELECT_DIFFICULTY`, `STATE_EXAM`, dll.).
- [ ] Implementasi sistem *text input box* interaktif di menu utama agar pemain dapat mengetik nama mereka (`playerName[32]`) sebelum memulai permainan.
- [ ] Setup *input handler* untuk navigasi menu utama dan pemilihan tingkat kesulitan (*Easy* hingga *Impossible*).

## Phase 2: UI Layout, Scrolling, & Sprite Animations
- [ ] Mendesain antarmuka ujian menyerupai UTBK asli (Header timer, panel kiri untuk teks bacaan/paragraf panjang, panel kanan untuk soal dan opsi A-E).
- [ ] Implementasi fungsi `BeginScissorMode()` dan `EndScissorMode()` agar teks bacaan panjang pada subtest literasi dapat di-scroll menggunakan *mouse wheel* tanpa merusak layout.
- [ ] Integrasi sprite sheet karakter **Sae Chabashira** (3 frame: Neutral, Asking, Warning) di sisi atas dialog ujian dengan pemotongan `Rectangle sourceRec` yang dinamis.

## Phase 3: Modular JSON Parsing (1600+ Soal Pool) & Shuffling Logic
- [ ] Menyiapkan struktur 7 file JSON eksternal terpisah (`data/pu_questions.json`, `data/ppu_questions.json`, dll.) untuk menampung total 1600 soal UTBK.
- [ ] Mengintegrasikan library `cJSON` untuk memparsing data soal dari file modular ke dalam memori dinamis C.
- [ ] Mengimplementasikan algoritma pengacakan soal (*Fisher-Yates Shuffle*) agar game mengambil subset soal secara acak (misal: 30 soal PU dari total pool) setiap kali sesi ujian dimulai.
- [ ] Mengimplementasikan sistem penghitungan waktu mundur (*countdown timer*) per subtes yang dikalikan secara otomatis berdasarkan *Time Multiplier* tingkat kesulitan.

## Phase 4: Scoring (IRT), Review Phase, & Text-to-Speech (TTS)
- [ ] Menerapkan rumus kalkulasi skor **Item Response Theory (IRT)** pada skala 200 - 1000 untuk Mode Hard, beserta pengalian *Score Multiplier* untuk level lainnya.
- [ ] Membangun *Scene Review & Pembahasan* yang menampilkan sprite sheet karakter **Arisu Sakayanagi** (3 frame: Reviewing, Smug, Critique) berdasarkan kebenaran jawaban user.
- [ ] Mengimplementasikan modul **Text-to-Speech (TTS)**:
  - Aktif di Mode Easy saat ujian (tombol toggle play/stop).
  - Aktif secara otomatis pada sesi pembahasan (*Review Phase*) untuk semua tingkat kesulitan.

## Phase 5: Leaderboard, File I/O, & Memory Management
- [ ] Membuat sistem penyimpanan skor lokal (`leaderboard.dat`) menggunakan *struct* C untuk menyimpan Top 10 pemain beserta nama, skor akhir, dan tingkat kesulitan yang digunakan.
- [ ] Menambahkan *sorting algorithm* (Insertion/Quicksort) untuk mengurutkan skor tertinggi secara *descending*.
- [ ] Pembersihan memori (*memory leak check*): Memastikan seluruh tekstur (`UnloadTexture`) dan alokasi soal (`free`) dibersihkan dengan benar saat aplikasi ditutup.
