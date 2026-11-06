#include "game.h"
#include "demo_questions.h"
#include "exam.h"
#include "screens.h"
#include "ui.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

const DifficultyInfo DIFFICULTIES[DIFFICULTY_COUNT] = {
    {
        "EASY",
        "Waktu paling longgar. TTS aktif saat ujian dan pembahasan.",
        1.50f, 0.60f, true,
        { 96, 211, 148, 255 }
    },
    {
        "NORMAL",
        "Waktu lebih longgar. TTS aktif pada fase pembahasan.",
        1.25f, 0.80f, false,
        { 86, 170, 255, 255 }
    },
    {
        "HARD",
        "Standar resmi SNBT. Acuan waktu dan skor pemerintah.",
        1.00f, 1.00f, false,
        { 224, 74, 96, 255 }
    },
    {
        "INSANE",
        "Waktu dipangkas 25 persen. Skor akhir lebih tinggi.",
        0.75f, 1.50f, false,
        { 255, 166, 77, 255 }
    },
    {
        "IMPOSSIBLE",
        "Separuh waktu resmi. Hanya untuk yang benar-benar nekat.",
        0.50f, 2.00f, false,
        { 186, 112, 255, 255 }
    }
};

const char *SUBTEST_NAMES[SUBTEST_COUNT] = {
    "Penalaran Umum",
    "Pengetahuan & Pemahaman Umum",
    "Pemahaman Bacaan & Menulis",
    "Pengetahuan Kuantitatif",
    "Literasi Bahasa Indonesia",
    "Literasi Bahasa Inggris",
    "Penalaran Matematika"
};

const char *SUBTEST_CODES[SUBTEST_COUNT] = {
    "PU", "PPU", "PBM", "PK", "LBI", "LBE", "PM"
};

const int SUBTEST_QUESTION_COUNT[SUBTEST_COUNT] = {
    30, 20, 20, 20, 30, 20, 20
};

const int SUBTEST_MINUTES_HARD[SUBTEST_COUNT] = {
    30, 15, 25, 20, 45, 30, 30
};

static Font LoadGameFont(int size)
{
    static const char *fontPaths[] = {
        "assets/fonts/NotoSans-Regular.ttf",
        "assets/fonts/Inter-Regular.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf"
    };

    int pathCount = (int)(sizeof(fontPaths) / sizeof(fontPaths[0]));
    for (int i = 0; i < pathCount; ++i) {
        if (!FileExists(fontPaths[i])) {
            continue;
        }

        Font font = LoadFontEx(fontPaths[i], size, NULL, 0);
        if (font.texture.id != 0) {
            SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
            return font;
        }
    }

    return GetFontDefault();
}

static void UnloadGameFont(Font *font)
{
    if (font->texture.id != 0 && font->texture.id != GetFontDefault().texture.id) {
        UnloadFont(*font);
    }
}

void GameInit(Game *game)
{
    memset(game, 0, sizeof(*game));

    game->state = STATE_MENU;
    game->stateTime = 0.0f;
    game->totalTime = 0.0f;
    game->fadeAlpha = 1.0f;
    game->menuFocus = 0;
    game->nameBuffer[0] = '\0';
    game->nameLength = 0;
    game->nameErrorTimer = 0.0f;
    game->selectedDifficulty = DIFFICULTY_HARD;
    game->selectedSubtest = 0;
    game->exitRequested = false;

    game->examScroll = 0.0f;
    game->examScrollTarget = 0.0f;
    game->examFocusedOption = 0;
    game->examTotalSeconds = 0;
    game->examOverlay = EXAM_OVERLAY_NONE;

    game->session.currentDifficulty = DIFFICULTY_HARD;
    game->session.currentSubtest = 0;
    game->session.currentQuestion = 0;
    game->session.questionCount = 0;
    game->session.questions = NULL;
    game->session.answerOffset = 0;
    game->session.timeRemaining = 0.0f;
    game->session.rawIRTScore = 0;
    game->session.finalScore = 0;
    snprintf(game->session.playerName, sizeof(game->session.playerName), "Peserta");
    for (int i = 0; i < MAX_SESSION_QUESTIONS; ++i) {
        game->session.userAnswers[i] = -1;
    }

    game->fonts.title = LoadGameFont(96);
    game->fonts.ui = LoadGameFont(32);
    game->fonts.small = LoadGameFont(20);

    game->saeTexture = LoadTexture("assets/characters/sae_chabashira.png");
    if (game->saeTexture.id != 0) {
        SetTextureFilter(game->saeTexture, TEXTURE_FILTER_BILINEAR);
    }
    game->saeFrame = 0;
    game->saeAnimTimer = 0.0f;
}

void GameUnload(Game *game)
{
    if (game->saeTexture.id != 0) {
        UnloadTexture(game->saeTexture);
        game->saeTexture.id = 0;
    }

    UnloadGameFont(&game->fonts.title);
    UnloadGameFont(&game->fonts.ui);
    UnloadGameFont(&game->fonts.small);
}

void GameChangeState(Game *game, GameState state)
{
    if (game->state == state) {
        return;
    }

    game->state = state;
    game->stateTime = 0.0f;
    game->fadeAlpha = 1.0f;
}

void GameUpdate(Game *game)
{
    float dt = GetFrameTime();

    game->totalTime += dt;
    game->stateTime += dt;

    if (game->fadeAlpha > 0.0f) {
        game->fadeAlpha -= dt * 2.8f;
        if (game->fadeAlpha < 0.0f) {
            game->fadeAlpha = 0.0f;
        }
    }

    if (game->nameErrorTimer > 0.0f) {
        game->nameErrorTimer -= dt;
        if (game->nameErrorTimer < 0.0f) {
            game->nameErrorTimer = 0.0f;
        }
    }

    game->saeAnimTimer += dt;
    if (game->saeAnimTimer >= 0.45f) {
        game->saeAnimTimer -= 0.45f;
        game->saeFrame = (game->saeFrame + 1) % 3;
    }

    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    switch (game->state) {
        case STATE_MENU:
            UpdateMenu(game);
            break;
        case STATE_SELECT_DIFFICULTY:
            UpdateDifficulty(game);
            break;
        case STATE_SUBTEST_SELECT:
            UpdateSubtestSelect(game);
            break;
        case STATE_EXAM:
            UpdateExam(game);
            break;
        case STATE_LEADERBOARD:
            UpdatePlaceholder(game);
            break;
        case STATE_SCORE_SUMMARY:
        case STATE_REVIEW_PHASE:
            UpdatePlaceholder(game);
            break;
        default:
            break;
    }
}

void GameDraw(const Game *game)
{
    ClearBackground(UI_BG_TOP);

    switch (game->state) {
        case STATE_MENU:
            DrawMenu(game);
            break;
        case STATE_SELECT_DIFFICULTY:
            DrawDifficulty(game);
            break;
        case STATE_SUBTEST_SELECT:
            DrawSubtestSelect(game);
            break;
        case STATE_EXAM:
            DrawExam(game);
            break;
        case STATE_LEADERBOARD:
            DrawPlaceholder(game);
            break;
        case STATE_SCORE_SUMMARY:
        case STATE_REVIEW_PHASE:
            DrawPlaceholder(game);
            break;
        default:
            break;
    }

    if (game->fadeAlpha > 0.0f) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, game->fadeAlpha));
    }
}

bool GamePlayerNameValid(const char *name)
{
    if (name == NULL) {
        return false;
    }

    while (*name != '\0') {
        if (!isspace((unsigned char)*name)) {
            return true;
        }
        ++name;
    }

    return false;
}

void GameApplyPlayerName(Game *game)
{
    const char *start = game->nameBuffer;
    while (*start != '\0' && isspace((unsigned char)*start)) {
        ++start;
    }

    const char *end = start + strlen(start);
    while (end > start && isspace((unsigned char)end[-1])) {
        --end;
    }

    size_t length = (size_t)(end - start);
    if (length > MAX_PLAYER_NAME) {
        length = MAX_PLAYER_NAME;
    }

    memcpy(game->session.playerName, start, length);
    game->session.playerName[length] = '\0';
}

void GameBeginFromMenu(Game *game)
{
    if (!GamePlayerNameValid(game->nameBuffer)) {
        game->nameErrorTimer = 2.4f;
        game->menuFocus = 0;
        return;
    }

    GameApplyPlayerName(game);
    GameChangeState(game, STATE_SELECT_DIFFICULTY);
}

void GameConfirmDifficulty(Game *game)
{
    game->session.currentDifficulty = game->selectedDifficulty;
    GameChangeState(game, STATE_SUBTEST_SELECT);
}

const DifficultyInfo *GameCurrentDifficulty(const Game *game)
{
    int index = game->session.currentDifficulty;
    if (index < 0 || index >= DIFFICULTY_COUNT) {
        index = DIFFICULTY_HARD;
    }
    return &DIFFICULTIES[index];
}

int GameDifficultyTotalSeconds(int difficulty)
{
    if (difficulty < 0 || difficulty >= DIFFICULTY_COUNT) {
        difficulty = DIFFICULTY_HARD;
    }
    return (int)(SNBT_TOTAL_SECONDS * DIFFICULTIES[difficulty].timeMultiplier + 0.5f);
}

int GameSubtestDurationSeconds(int difficulty, int subtest)
{
    if (difficulty < 0 || difficulty >= DIFFICULTY_COUNT) {
        difficulty = DIFFICULTY_HARD;
    }
    if (subtest < 0 || subtest >= SUBTEST_COUNT) {
        subtest = 0;
    }

    float seconds = SUBTEST_MINUTES_HARD[subtest] * 60.0f *
                    DIFFICULTIES[difficulty].timeMultiplier;
    return (int)(seconds + 0.5f);
}

void GameFormatDuration(int seconds, char *out, int outSize)
{
    if (out == NULL || outSize <= 0) {
        return;
    }

    if (seconds < 0) {
        seconds = 0;
    }

    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;

    if (hours > 0) {
        snprintf(out, (size_t)outSize, "%dj %02dm", hours, minutes);
    } else {
        snprintf(out, (size_t)outSize, "%dm %02ds", minutes, secs);
    }
}

int GameSubtestAnswerOffset(int subtest)
{
    if (subtest < 0 || subtest >= SUBTEST_COUNT) {
        return 0;
    }

    int offset = 0;
    for (int i = 0; i < subtest; ++i) {
        offset += SUBTEST_QUESTION_COUNT[i];
    }
    return offset;
}

void GameStartExam(Game *game, int subtest)
{
    if (subtest < 0 || subtest >= SUBTEST_COUNT) {
        subtest = 0;
    }

    game->session.currentSubtest = subtest;
    game->session.currentQuestion = 0;
    game->session.questionCount = DEMO_QUESTION_COUNTS[subtest];
    game->session.questions = DEMO_QUESTIONS[subtest];
    game->session.answerOffset = GameSubtestAnswerOffset(subtest);

    for (int i = 0; i < SUBTEST_QUESTION_COUNT[subtest]; ++i) {
        game->session.userAnswers[game->session.answerOffset + i] = -1;
    }

    int seconds = GameSubtestDurationSeconds(game->session.currentDifficulty, subtest);
    game->examTotalSeconds = seconds;
    game->session.timeRemaining = (float)seconds;
    game->examScroll = 0.0f;
    game->examScrollTarget = 0.0f;
    game->examFocusedOption = 0;
    game->examOverlay = EXAM_OVERLAY_NONE;

    GameChangeState(game, STATE_EXAM);
}

const Question *GameCurrentQuestion(const Game *game)
{
    if (game->session.questions == NULL || game->session.questionCount <= 0) {
        return NULL;
    }

    int index = game->session.currentQuestion;
    if (index < 0) {
        index = 0;
    } else if (index >= game->session.questionCount) {
        index = game->session.questionCount - 1;
    }
    return &game->session.questions[index];
}

int GameCurrentAnswer(const Game *game)
{
    if (game->session.questions == NULL) {
        return -1;
    }

    int index = game->session.answerOffset + game->session.currentQuestion;
    if (index < 0 || index >= MAX_SESSION_QUESTIONS) {
        return -1;
    }
    return game->session.userAnswers[index];
}

void GameSetCurrentAnswer(Game *game, int option)
{
    if (game->session.questions == NULL || option < 0 || option >= MAX_OPTIONS) {
        return;
    }

    int index = game->session.answerOffset + game->session.currentQuestion;
    if (index < 0 || index >= MAX_SESSION_QUESTIONS) {
        return;
    }
    game->session.userAnswers[index] = option;
}

void GameGoToQuestion(Game *game, int index)
{
    if (game->session.questions == NULL || game->session.questionCount <= 0) {
        return;
    }

    if (index < 0) {
        index = 0;
    } else if (index >= game->session.questionCount) {
        index = game->session.questionCount - 1;
    }

    if (index == game->session.currentQuestion) {
        return;
    }

    const Question *oldQuestion = GameCurrentQuestion(game);
    const char *oldPassage = oldQuestion != NULL ? oldQuestion->passage : NULL;

    game->session.currentQuestion = index;

    const Question *newQuestion = GameCurrentQuestion(game);
    const char *newPassage = newQuestion != NULL ? newQuestion->passage : NULL;
    if (oldPassage != newPassage) {
        game->examScroll = 0.0f;
        game->examScrollTarget = 0.0f;
    }

    int answer = GameCurrentAnswer(game);
    game->examFocusedOption = answer >= 0 ? answer : 0;
}
