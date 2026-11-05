# Software Design Document (SDD): UTBK Tactics: SNBT Exam Arena

## 1. System Overview
- **Project Name:** UTBK Tactics: SNBT Exam Arena
- **Target Platform:** Desktop (Windows via MinGW-w64 / Linux via WSL)
- **Programming Language:** C (C99 Standard)
- **Core Graphics & Multimedia Engine:** Raylib (Dilengkapi Raygui untuk komponen UI dasar)
- **Genre:** Educational RPG / Visual Novel Quiz Simulator
- **Core Theme & Art Style:** Modern 2D Anime / Visual Novel (*Classroom of the Elite style*)

## 2. Character Roles & Asset Specifications
- **Sae Chabashira (Pengawas & Guru Ujian):**
  - **Role:** Memandu sesi ujian, membacakan aturan, memberikan soal, dan menyampaikan peringatan waktu.
  - **Sprite Sheet Grid:** 3 Frame Horizontal (`Neutral`, `Asking`, `Warning`). Format: `.png`.
- **Arisu Sakayanagi (Evaluator & Pembahas):**
  - **Role:** Menampilkan skor akhir, menganalisis lembar jawaban, serta memandu sesi pembahasan detail.
  - **Sprite Sheet Grid:** 3 Frame Horizontal (`Reviewing`, `Smug`, `Critique`). Format: `.png`.

## 3. Game State Machine
Aplikasi menggunakan state machine berbasis `enum` berikut:
```c
typedef enum {
    STATE_MENU,              // Menu utama & input nama pemain
    STATE_SELECT_DIFFICULTY, // Pemilihan tingkat kesulitan (Easy -> Impossible)
    STATE_SUBTEST_SELECT,    // Navigasi 7 Subtes UTBK
    STATE_EXAM,              // Sesi pengerjaan soal (Sae Chabashira)
    STATE_SCORE_SUMMARY,     // Kalkulasi Skor IRT & Ringkasan (Sakayanagi Review)
    STATE_REVIEW_PHASE,      // Sesi pembahasan nomor per nomor (Sakayanagi Pembahasan)
    STATE_LEADERBOARD        // Papan peringkat global (Top 10)
} GameState;