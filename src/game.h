#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <stdbool.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define MAX_PLAYER_NAME 31
#define MAX_SESSION_QUESTIONS 160
#define DIFFICULTY_COUNT 5
#define SUBTEST_COUNT 7
#define SNBT_TOTAL_SECONDS 11700

typedef enum {
    STATE_MENU = 0,
    STATE_SELECT_DIFFICULTY,
    STATE_SUBTEST_SELECT,
    STATE_EXAM,
    STATE_SCORE_SUMMARY,
    STATE_REVIEW_PHASE,
    STATE_LEADERBOARD,
    STATE_COUNT
} GameState;

typedef enum {
    DIFFICULTY_EASY = 0,
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD,
    DIFFICULTY_INSANE,
    DIFFICULTY_IMPOSSIBLE
} DifficultyLevel;

typedef struct {
    const char *name;
    const char *description;
    float timeMultiplier;
    float scoreMultiplier;
    bool ttsDuringExam;
    Color accent;
} DifficultyInfo;

typedef struct {
    char playerName[32];
    int currentDifficulty;
    int userAnswers[MAX_SESSION_QUESTIONS];
    float timeRemaining;
    int rawIRTScore;
    int finalScore;
} PlayerSession;

typedef struct {
    char name[32];
    int score;
    int difficultyUsed;
} LeaderboardEntry;

typedef struct {
    Font title;
    Font ui;
    Font small;
} GameFonts;

typedef struct {
    GameState state;
    PlayerSession session;
    GameFonts fonts;

    float stateTime;
    float totalTime;
    float fadeAlpha;

    int menuFocus;
    char nameBuffer[32];
    int nameLength;
    float nameErrorTimer;

    int selectedDifficulty;
    bool exitRequested;
} Game;

extern const DifficultyInfo DIFFICULTIES[DIFFICULTY_COUNT];
extern const char *SUBTEST_NAMES[SUBTEST_COUNT];
extern const char *SUBTEST_CODES[SUBTEST_COUNT];
extern const int SUBTEST_QUESTION_COUNT[SUBTEST_COUNT];
extern const int SUBTEST_MINUTES_HARD[SUBTEST_COUNT];

void GameInit(Game *game);
void GameUnload(Game *game);
void GameUpdate(Game *game);
void GameDraw(const Game *game);
void GameChangeState(Game *game, GameState state);

bool GamePlayerNameValid(const char *name);
void GameApplyPlayerName(Game *game);
void GameBeginFromMenu(Game *game);
void GameConfirmDifficulty(Game *game);

const DifficultyInfo *GameCurrentDifficulty(const Game *game);
int GameDifficultyTotalSeconds(int difficulty);
int GameSubtestDurationSeconds(int difficulty, int subtest);
void GameFormatDuration(int seconds, char *out, int outSize);

#endif
