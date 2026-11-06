#include "screens.h"
#include "demo_questions.h"
#include "ui.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static bool EnterPressed(void)
{
    return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER);
}

static bool ShiftDown(void)
{
    return IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
}

static bool MouseMoved(void)
{
    Vector2 delta = GetMouseDelta();
    return fabsf(delta.x) > 0.2f || fabsf(delta.y) > 0.2f;
}

static void DrawCornerBrackets(Rectangle bounds, Color color, float length, float thickness)
{
    DrawLineEx((Vector2){ bounds.x, bounds.y },
               (Vector2){ bounds.x + length, bounds.y }, thickness, color);
    DrawLineEx((Vector2){ bounds.x, bounds.y },
               (Vector2){ bounds.x, bounds.y + length }, thickness, color);

    DrawLineEx((Vector2){ bounds.x + bounds.width, bounds.y },
               (Vector2){ bounds.x + bounds.width - length, bounds.y }, thickness, color);
    DrawLineEx((Vector2){ bounds.x + bounds.width, bounds.y },
               (Vector2){ bounds.x + bounds.width, bounds.y + length }, thickness, color);

    DrawLineEx((Vector2){ bounds.x, bounds.y + bounds.height },
               (Vector2){ bounds.x + length, bounds.y + bounds.height }, thickness, color);
    DrawLineEx((Vector2){ bounds.x, bounds.y + bounds.height },
               (Vector2){ bounds.x, bounds.y + bounds.height - length }, thickness, color);

    DrawLineEx((Vector2){ bounds.x + bounds.width, bounds.y + bounds.height },
               (Vector2){ bounds.x + bounds.width - length, bounds.y + bounds.height },
               thickness, color);
    DrawLineEx((Vector2){ bounds.x + bounds.width, bounds.y + bounds.height },
               (Vector2){ bounds.x + bounds.width, bounds.y + bounds.height - length },
               thickness, color);
}

/* ------------------------------------------------------------------------- */
/* Main menu                                                                 */
/* ------------------------------------------------------------------------- */

static Rectangle MenuPanelRect(void)
{
    return (Rectangle){ 250.0f, 44.0f, 780.0f, 632.0f };
}

static Rectangle MenuNameRect(void)
{
    return (Rectangle){ 380.0f, 286.0f, 520.0f, 56.0f };
}

static Rectangle MenuButtonRect(int index)
{
    return (Rectangle){ 380.0f, 388.0f + (float)index * 66.0f, 520.0f, 52.0f };
}

static void DrawNameInput(const Game *game, Rectangle rect, bool focused)
{
    float shake = 0.0f;
    if (game->nameErrorTimer > 0.0f) {
        shake = sinf(game->totalTime * 55.0f) * 5.0f *
                (game->nameErrorTimer / 2.4f);
    }
    rect.x += shake;

    if (focused) {
        Rectangle glow = { rect.x - 4.0f, rect.y - 4.0f,
                           rect.width + 8.0f, rect.height + 8.0f };
        DrawRectangleRounded(glow, 0.18f, 10, Fade(UI_ACCENT, 0.09f));
    }

    DrawRectangleRounded(rect, 0.18f, 10, (Color){ 13, 18, 30, 255 });
    DrawRectangleRoundedLinesEx(rect, 0.18f, 10, focused ? 2.0f : 1.2f,
                                focused ? UI_ACCENT : UI_BORDER);

    const float textSize = 24.0f;
    const float spacing = 0.5f;
    Vector2 measured = MeasureTextEx(game->fonts.ui, game->nameBuffer, textSize, spacing);
    float textX = rect.x + 20.0f;
    float textY = rect.y + (rect.height - measured.y) * 0.5f - 1.0f;

    if (game->nameLength == 0) {
        if (!focused) {
            UI_Text(game->fonts.ui, "Ketik namamu di sini...", textX, textY,
                    textSize, Fade(UI_MUTED, 0.75f));
        }
    } else {
        UI_Text(game->fonts.ui, game->nameBuffer, textX, textY, textSize, UI_TEXT);
    }

    if (focused && fmodf(game->totalTime, 1.0f) < 0.55f) {
        float caretX = textX + (game->nameLength > 0 ? measured.x + 2.0f : 0.0f);
        DrawRectangle((int)caretX, (int)(rect.y + 13.0f), 2,
                      (int)(rect.height - 26.0f), UI_ACCENT);
    }

    char counter[16];
    snprintf(counter, sizeof(counter), "%d/%d", game->nameLength, MAX_PLAYER_NAME);
    Vector2 counterSize = MeasureTextEx(game->fonts.small, counter, 13.0f, 0.3f);
    UI_Text(game->fonts.small, counter,
            rect.x + rect.width - counterSize.x - 14.0f,
            rect.y + rect.height - 21.0f, 13.0f, UI_MUTED);

    if (game->nameErrorTimer > 0.0f) {
        const char *error = "Nama tidak boleh kosong. Ketik nama dulu, ya.";
        Vector2 errorSize = MeasureTextEx(game->fonts.small, error, 14.0f, 0.3f);
        UI_Text(game->fonts.small, error,
                rect.x + (rect.width - errorSize.x) * 0.5f,
                rect.y + rect.height + 12.0f, 14.0f,
                (Color){ 255, 102, 118, 255 });
    }
}

void UpdateMenu(Game *game)
{
    Rectangle inputRect = MenuNameRect();
    Vector2 mouse = GetMousePosition();
    bool mouseMoved = MouseMoved();

    if (CheckCollisionPointRec(mouse, inputRect)) {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
    }

    if (mouseMoved) {
        if (CheckCollisionPointRec(mouse, inputRect)) {
            game->menuFocus = 0;
        }
        for (int i = 0; i < 3; ++i) {
            if (CheckCollisionPointRec(mouse, MenuButtonRect(i))) {
                game->menuFocus = i + 1;
            }
        }
    }

    if (IsKeyPressed(KEY_TAB)) {
        int direction = ShiftDown() ? -1 : 1;
        game->menuFocus = (game->menuFocus + direction + 4) % 4;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        game->menuFocus = (game->menuFocus + 1) % 4;
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        game->menuFocus = (game->menuFocus + 3) % 4;
    }

    int codepoint = 0;
    while ((codepoint = GetCharPressed()) > 0) {
        if (game->menuFocus == 0 && codepoint >= 32 && codepoint <= 126 &&
            game->nameLength < MAX_PLAYER_NAME) {
            game->nameBuffer[game->nameLength++] = (char)codepoint;
            game->nameBuffer[game->nameLength] = '\0';
            game->nameErrorTimer = 0.0f;
        }
    }

    if (game->menuFocus == 0) {
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
            if (game->nameLength > 0) {
                --game->nameLength;
                game->nameBuffer[game->nameLength] = '\0';
            }
        }
        if (EnterPressed()) {
            GameBeginFromMenu(game);
            return;
        }
    }

    for (int i = 0; i < 3; ++i) {
        if (UI_IsClicked(MenuButtonRect(i))) {
            game->menuFocus = i + 1;
            if (i == 0) {
                GameBeginFromMenu(game);
            } else if (i == 1) {
                GameChangeState(game, STATE_LEADERBOARD);
            } else {
                game->exitRequested = true;
            }
            return;
        }
    }

    if (EnterPressed()) {
        if (game->menuFocus == 1) {
            GameBeginFromMenu(game);
        } else if (game->menuFocus == 2) {
            GameChangeState(game, STATE_LEADERBOARD);
        } else if (game->menuFocus == 3) {
            game->exitRequested = true;
        }
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        game->exitRequested = true;
    }
}

void DrawMenu(const Game *game)
{
    UI_DrawBackground(game->totalTime);

    Rectangle panel = MenuPanelRect();
    UI_DrawPanel(panel, UI_PANEL, (Color){ 70, 82, 110, 255 });
    DrawCornerBrackets(panel, Fade(UI_ACCENT, 0.65f), 30.0f, 2.0f);

    const char *topLabel = "CLASSROOM OF THE ELITE - FAN PROJECT";
    Vector2 topSize = MeasureTextEx(game->fonts.small, topLabel, 11.0f, 3.0f);
    DrawTextEx(game->fonts.small, topLabel,
               (Vector2){ panel.x + (panel.width - topSize.x) * 0.5f, 64.0f },
               11.0f, 3.0f, Fade(UI_MUTED, 0.85f));

    const char *brand = "UTBK";
    const char *brandRest = " TACTICS";
    const float titleSize = 58.0f;
    const float titleSpacing = 2.0f;
    Vector2 brandMeasure = MeasureTextEx(game->fonts.title, brand, titleSize, titleSpacing);
    Vector2 restMeasure = MeasureTextEx(game->fonts.title, brandRest, titleSize, titleSpacing);
    float titleX = panel.x + (panel.width - brandMeasure.x - restMeasure.x) * 0.5f;
    float titleY = 92.0f;

    DrawTextEx(game->fonts.title, brand, (Vector2){ titleX + 2.0f, titleY + 4.0f },
               titleSize, titleSpacing, Fade(BLACK, 0.35f));
    DrawTextEx(game->fonts.title, brandRest,
               (Vector2){ titleX + brandMeasure.x + 2.0f, titleY + 4.0f },
               titleSize, titleSpacing, Fade(BLACK, 0.35f));
    DrawTextEx(game->fonts.title, brand, (Vector2){ titleX, titleY },
               titleSize, titleSpacing, UI_TEXT);
    DrawTextEx(game->fonts.title, brandRest,
               (Vector2){ titleX + brandMeasure.x, titleY },
               titleSize, titleSpacing, UI_ACCENT);

    const char *subtitle = "SNBT EXAM ARENA";
    Vector2 subtitleSize = MeasureTextEx(game->fonts.ui, subtitle, 19.0f, 7.0f);
    DrawTextEx(game->fonts.ui, subtitle,
               (Vector2){ panel.x + (panel.width - subtitleSize.x) * 0.5f, 162.0f },
               19.0f, 7.0f, Fade(UI_CYAN, 0.9f));

    DrawRectangle((int)(panel.x + 180.0f), 196, (int)(panel.width - 360.0f), 1,
                  Fade(UI_BORDER, 0.9f));

    const char *tagline = "Simulasi ujian UTBK SNBT bergaya visual novel.";
    UI_TextCentered(game->fonts.small, tagline,
                    (Rectangle){ panel.x, 208.0f, panel.width, 24.0f },
                    15.0f, 0.4f, UI_MUTED);

    Rectangle inputRect = MenuNameRect();
    UI_DrawSectionLabel(game->fonts.ui, "NAMA PESERTA", inputRect.x, 254.0f, UI_ACCENT);

    const char *limitLabel = "MAKS. 31 KARAKTER";
    Vector2 limitSize = MeasureTextEx(game->fonts.small, limitLabel, 11.0f, 1.2f);
    UI_TextSpaced(game->fonts.small, limitLabel,
                  inputRect.x + inputRect.width - limitSize.x, 256.0f,
                  11.0f, 1.2f, Fade(UI_MUTED, 0.7f));

    DrawNameInput(game, inputRect, game->menuFocus == 0);

    UI_Button(game->fonts.ui, MenuButtonRect(0), "MULAI UJIAN",
              game->menuFocus == 1, true, UI_BUTTON_PRIMARY, UI_ACCENT);
    UI_Button(game->fonts.ui, MenuButtonRect(1), "PAPAN PERINGKAT",
              game->menuFocus == 2, true, UI_BUTTON_SECONDARY, UI_CYAN);
    UI_Button(game->fonts.ui, MenuButtonRect(2), "KELUAR",
              game->menuFocus == 3, true, UI_BUTTON_GHOST, UI_ACCENT);

    const char *hint = "TAB / PANAH: NAVIGASI   |   ENTER: PILIH   |   ESC: KELUAR";
    UI_TextCentered(game->fonts.small, hint,
                    (Rectangle){ panel.x, 602.0f, panel.width, 22.0f },
                    13.0f, 0.5f, Fade(UI_MUTED, 0.8f));

    UI_TextSpaced(game->fonts.small, "v0.2 // PHASE 2",
                  panel.x + 34.0f, 638.0f, 11.0f, 1.5f, Fade(UI_MUTED, 0.55f));

    const char *engine = "POWERED BY RAYLIB";
    Vector2 engineSize = MeasureTextEx(game->fonts.small, engine, 11.0f, 1.5f);
    UI_TextSpaced(game->fonts.small, engine,
                  panel.x + panel.width - engineSize.x - 34.0f, 638.0f,
                  11.0f, 1.5f, Fade(UI_MUTED, 0.55f));
}

/* ------------------------------------------------------------------------- */
/* Difficulty selection                                                      */
/* ------------------------------------------------------------------------- */

static Rectangle DifficultyCardRect(int index)
{
    const float cardWidth = 204.0f;
    const float cardHeight = 292.0f;
    const float gap = 16.0f;
    float total = DIFFICULTY_COUNT * cardWidth + (DIFFICULTY_COUNT - 1) * gap;
    float startX = (SCREEN_WIDTH - total) * 0.5f;

    return (Rectangle){
        startX + (float)index * (cardWidth + gap),
        188.0f,
        cardWidth,
        cardHeight
    };
}

static Rectangle DifficultyDetailRect(void)
{
    return (Rectangle){ 90.0f, 508.0f, 1100.0f, 118.0f };
}

static Rectangle DifficultyBackRect(void)
{
    return (Rectangle){ 872.0f, 542.0f, 140.0f, 48.0f };
}

static Rectangle DifficultyContinueRect(void)
{
    return (Rectangle){ 1028.0f, 542.0f, 140.0f, 48.0f };
}

static void DrawStatRow(Font font, const char *label, const char *value,
                        float x, float y, float width, Color valueColor)
{
    UI_Text(font, label, x, y, 13.0f, UI_MUTED);
    Vector2 valueSize = MeasureTextEx(font, value, 13.0f, 0.2f);
    UI_Text(font, value, x + width - valueSize.x, y, 13.0f, valueColor);
}

static void DrawDifficultyCard(const Game *game, int index)
{
    const DifficultyInfo *info = &DIFFICULTIES[index];
    Rectangle base = DifficultyCardRect(index);
    bool selected = index == game->selectedDifficulty;
    bool hovered = CheckCollisionPointRec(GetMousePosition(), base);

    Rectangle rect = base;
    if (selected) {
        rect.y -= 10.0f;
        rect.height += 10.0f;
    }

    Color accent = info->accent;
    Color fill = selected ? (Color){ 27, 35, 53, 255 } : (Color){ 21, 28, 44, 245 };
    if (hovered && !selected) {
        fill = (Color){ 25, 33, 51, 255 };
    }
    Color border = selected ? accent : (hovered ? Fade(accent, 0.65f) : UI_BORDER);

    if (selected) {
        for (int i = 4; i >= 1; --i) {
            Rectangle glow = {
                rect.x - (float)i * 3.0f,
                rect.y - (float)i * 3.0f,
                rect.width + (float)i * 6.0f,
                rect.height + (float)i * 6.0f
            };
            DrawRectangleRounded(glow, 0.05f, 12, Fade(accent, 0.022f));
        }
    }

    DrawRectangleRounded(rect, 0.045f, 12, fill);
    DrawRectangleRoundedLinesEx(rect, 0.045f, 12, selected ? 2.0f : 1.2f, border);
    DrawRectangleRounded((Rectangle){ rect.x + 14.0f, rect.y + 10.0f,
                                      rect.width - 28.0f, 4.0f },
                         0.9f, 8, accent);

    char number[8];
    snprintf(number, sizeof(number), "%02d", index + 1);
    Vector2 numberSize = MeasureTextEx(game->fonts.small, number, 22.0f, 1.0f);
    UI_Text(game->fonts.small, number,
            rect.x + rect.width - numberSize.x - 16.0f, rect.y + 22.0f,
            22.0f, Fade(accent, 0.6f));

    float nameSize = 24.0f;
    Vector2 nameMeasure = MeasureTextEx(game->fonts.ui, info->name, nameSize, 1.0f);
    if (nameMeasure.x > rect.width - 30.0f) {
        nameSize = 20.0f;
    }
    UI_TextCentered(game->fonts.ui, info->name,
                    (Rectangle){ rect.x, rect.y + 48.0f, rect.width, 34.0f },
                    nameSize, 1.0f, selected ? UI_TEXT : (Color){ 208, 215, 229, 255 });

    DrawRectangle((int)(rect.x + 24.0f), (int)(rect.y + 92.0f),
                  (int)(rect.width - 48.0f), 1, Fade(UI_BORDER, 0.85f));

    UI_DrawTextWrapped(game->fonts.small, info->description,
                       (Rectangle){ rect.x + 18.0f, rect.y + 106.0f,
                                    rect.width - 36.0f, 66.0f },
                       13.0f, 0.2f, 18.0f, UI_MUTED);

    char value[32];
    float statX = rect.x + 18.0f;
    float statWidth = rect.width - 36.0f;

    snprintf(value, sizeof(value), "x%.2f", info->timeMultiplier);
    DrawStatRow(game->fonts.small, "WAKTU", value, statX, rect.y + 184.0f,
                statWidth, accent);

    snprintf(value, sizeof(value), "x%.2f", info->scoreMultiplier);
    DrawStatRow(game->fonts.small, "SKOR", value, statX, rect.y + 208.0f,
                statWidth, accent);

    DrawStatRow(game->fonts.small, "TTS UJIAN",
                info->ttsDuringExam ? "ON" : "OFF",
                statX, rect.y + 232.0f, statWidth,
                info->ttsDuringExam ? UI_SUCCESS : UI_MUTED);

    DrawStatRow(game->fonts.small, "TTS REVIEW", "ON", statX, rect.y + 256.0f,
                statWidth, UI_SUCCESS);
}

void UpdateDifficulty(Game *game)
{
    Vector2 mouse = GetMousePosition();

    for (int i = 0; i < DIFFICULTY_COUNT; ++i) {
        Rectangle card = DifficultyCardRect(i);
        if (CheckCollisionPointRec(mouse, card)) {
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                game->selectedDifficulty = i;
            }
        }
    }

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) ||
        IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        game->selectedDifficulty =
            (game->selectedDifficulty + DIFFICULTY_COUNT - 1) % DIFFICULTY_COUNT;
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) ||
        IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        game->selectedDifficulty =
            (game->selectedDifficulty + 1) % DIFFICULTY_COUNT;
    }

    if (IsKeyPressed(KEY_ONE)) {
        game->selectedDifficulty = DIFFICULTY_EASY;
    } else if (IsKeyPressed(KEY_TWO)) {
        game->selectedDifficulty = DIFFICULTY_NORMAL;
    } else if (IsKeyPressed(KEY_THREE)) {
        game->selectedDifficulty = DIFFICULTY_HARD;
    } else if (IsKeyPressed(KEY_FOUR)) {
        game->selectedDifficulty = DIFFICULTY_INSANE;
    } else if (IsKeyPressed(KEY_FIVE)) {
        game->selectedDifficulty = DIFFICULTY_IMPOSSIBLE;
    }

    if (UI_IsClicked(DifficultyBackRect())) {
        GameChangeState(game, STATE_MENU);
        return;
    }
    if (UI_IsClicked(DifficultyContinueRect())) {
        GameConfirmDifficulty(game);
        return;
    }

    if (EnterPressed() || IsKeyPressed(KEY_SPACE)) {
        GameConfirmDifficulty(game);
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        GameChangeState(game, STATE_MENU);
    }
}

void DrawDifficulty(const Game *game)
{
    UI_DrawBackground(game->totalTime);

    const char *eyebrow = "KONFIGURASI UJIAN // 5 TINGKAT KESULITAN";
    Vector2 eyebrowSize = MeasureTextEx(game->fonts.small, eyebrow, 12.0f, 2.6f);
    DrawTextEx(game->fonts.small, eyebrow,
               (Vector2){ (SCREEN_WIDTH - eyebrowSize.x) * 0.5f, 48.0f },
               12.0f, 2.6f, Fade(UI_ACCENT, 0.9f));

    UI_TextCentered(game->fonts.title, "PILIH TINGKAT KESULITAN",
                    (Rectangle){ 0.0f, 66.0f, SCREEN_WIDTH, 52.0f },
                    44.0f, 1.5f, UI_TEXT);

    char subtitle[160];
    snprintf(subtitle, sizeof(subtitle),
             "Halo, %s. Pilih mode yang ingin kamu taklukkan.",
             game->session.playerName);
    UI_TextCentered(game->fonts.ui, subtitle,
                    (Rectangle){ 0.0f, 120.0f, SCREEN_WIDTH, 30.0f },
                    17.0f, 0.4f, UI_MUTED);

    for (int i = 0; i < DIFFICULTY_COUNT; ++i) {
        DrawDifficultyCard(game, i);
    }

    Rectangle detail = DifficultyDetailRect();
    UI_DrawPanel(detail, UI_PANEL, UI_BORDER);

    const DifficultyInfo *selectedInfo = &DIFFICULTIES[game->selectedDifficulty];
    char detailTitle[64];
    snprintf(detailTitle, sizeof(detailTitle), "MODE %s", selectedInfo->name);
    UI_Text(game->fonts.ui, detailTitle, detail.x + 28.0f, detail.y + 18.0f,
            24.0f, selectedInfo->accent);

    UI_Text(game->fonts.small, selectedInfo->description,
            detail.x + 28.0f, detail.y + 50.0f, 14.0f, UI_MUTED);

    char totalDuration[32];
    GameFormatDuration(GameDifficultyTotalSeconds(game->selectedDifficulty),
                       totalDuration, sizeof(totalDuration));
    char stats[256];
    snprintf(stats, sizeof(stats),
             "Durasi total %s   |   Waktu x%.2f   |   Skor x%.2f   |   TTS ujian: %s",
             totalDuration, selectedInfo->timeMultiplier,
             selectedInfo->scoreMultiplier,
             selectedInfo->ttsDuringExam ? "ON" : "OFF");
    UI_Text(game->fonts.small, stats, detail.x + 28.0f, detail.y + 78.0f,
            13.0f, UI_TEXT);

    UI_Button(game->fonts.ui, DifficultyBackRect(), "KEMBALI",
              false, true, UI_BUTTON_SECONDARY, UI_ACCENT);
    UI_Button(game->fonts.ui, DifficultyContinueRect(), "LANJUT",
              false, true, UI_BUTTON_PRIMARY, selectedInfo->accent);

    const char *hint = "A/D ATAU PANAH: PILIH MODE   |   ENTER: LANJUT   |   ESC: KEMBALI";
    UI_TextCentered(game->fonts.small, hint,
                    (Rectangle){ 0.0f, 668.0f, SCREEN_WIDTH, 22.0f },
                    13.0f, 0.5f, Fade(UI_MUTED, 0.8f));
}

/* ------------------------------------------------------------------------- */
/* Subtest selection                                                         */
/* ------------------------------------------------------------------------- */

static Rectangle SubtestPanelRect(void)
{
    return (Rectangle){ 220.0f, 150.0f, 840.0f, 476.0f };
}

static Rectangle SubtestRowRect(int index)
{
    return (Rectangle){ 250.0f, 176.0f + (float)index * 48.0f, 780.0f, 42.0f };
}

static Rectangle SubtestBackRect(void)
{
    return (Rectangle){ 515.0f, 570.0f, 190.0f, 44.0f };
}

static Rectangle SubtestStartRect(void)
{
    return (Rectangle){ 715.0f, 570.0f, 190.0f, 44.0f };
}

void UpdateSubtestSelect(Game *game)
{
    for (int i = 0; i < SUBTEST_COUNT; ++i) {
        if (UI_IsHovered(SubtestRowRect(i))) {
            game->selectedSubtest = i;
        }
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        game->selectedSubtest = (game->selectedSubtest + 1) % SUBTEST_COUNT;
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        game->selectedSubtest = (game->selectedSubtest + SUBTEST_COUNT - 1) % SUBTEST_COUNT;
    }

    if (UI_IsClicked(SubtestBackRect()) || IsKeyPressed(KEY_ESCAPE)) {
        GameChangeState(game, STATE_SELECT_DIFFICULTY);
        return;
    }

    if (UI_IsClicked(SubtestStartRect()) || EnterPressed()) {
        GameStartExam(game, game->selectedSubtest);
    }
}

void DrawSubtestSelect(const Game *game)
{
    UI_DrawBackground(game->totalTime);

    const DifficultyInfo *difficulty = GameCurrentDifficulty(game);
    int selected = game->selectedSubtest;
    if (selected < 0 || selected >= SUBTEST_COUNT) {
        selected = 0;
    }

    const char *eyebrow = "PEMILIHAN SUBTES // SIAP DIMULAI";
    Vector2 eyebrowSize = MeasureTextEx(game->fonts.small, eyebrow, 12.0f, 2.6f);
    DrawTextEx(game->fonts.small, eyebrow,
               (Vector2){ (SCREEN_WIDTH - eyebrowSize.x) * 0.5f, 42.0f },
               12.0f, 2.6f, Fade(UI_CYAN, 0.9f));

    UI_TextCentered(game->fonts.title, "PILIH SUBTES",
                    (Rectangle){ 0.0f, 60.0f, SCREEN_WIDTH, 48.0f },
                    40.0f, 1.5f, UI_TEXT);

    char subtitle[192];
    snprintf(subtitle, sizeof(subtitle),
             "%s  //  MODE %s  //  DEMO BANK SOAL",
             game->session.playerName, difficulty->name);
    UI_TextCentered(game->fonts.ui, subtitle,
                    (Rectangle){ 0.0f, 112.0f, SCREEN_WIDTH, 28.0f },
                    15.0f, 0.6f, UI_MUTED);

    Rectangle panel = SubtestPanelRect();
    UI_DrawPanel(panel, UI_PANEL, UI_BORDER);

    for (int i = 0; i < SUBTEST_COUNT; ++i) {
        Rectangle row = SubtestRowRect(i);
        bool isSelected = (i == selected);
        bool hovered = CheckCollisionPointRec(GetMousePosition(), row);

        if (isSelected) {
            DrawRectangleRounded(row, 0.22f, 8, Fade(UI_CYAN, 0.12f));
            DrawRectangleRoundedLinesEx(row, 0.22f, 8, 1.5f, Fade(UI_CYAN, 0.75f));
            DrawRectangle((int)row.x, (int)row.y + 6, 3, (int)row.height - 12, UI_CYAN);
        } else if (hovered) {
            DrawRectangleRounded(row, 0.22f, 8, Fade(UI_CYAN, 0.05f));
        } else if (i % 2 == 0) {
            DrawRectangleRounded(row, 0.22f, 8, Fade(UI_PANEL_LIGHT, 0.35f));
        }

        Rectangle badge = { row.x + 12.0f, row.y + 7.0f, 48.0f, 28.0f };
        DrawRectangleRounded(badge, 0.25f, 8, Fade(UI_CYAN, isSelected ? 0.20f : 0.10f));
        DrawRectangleRoundedLinesEx(badge, 0.25f, 8, 1.0f,
                                    Fade(UI_CYAN, isSelected ? 0.80f : 0.45f));
        UI_TextCentered(game->fonts.small, SUBTEST_CODES[i], badge,
                        13.0f, 1.0f, UI_CYAN);

        UI_Text(game->fonts.ui, SUBTEST_NAMES[i],
                row.x + 76.0f, row.y + 10.0f, 17.0f,
                isSelected ? UI_TEXT : (Color){ 205, 212, 227, 255 });

        char questionLabel[32];
        snprintf(questionLabel, sizeof(questionLabel), "%d soal demo",
                 DEMO_QUESTION_COUNTS[i]);
        Vector2 questionSize = MeasureTextEx(game->fonts.small, questionLabel,
                                             13.0f, 0.3f);
        UI_Text(game->fonts.small, questionLabel,
                row.x + row.width - 228.0f - questionSize.x,
                row.y + 13.0f, 13.0f, UI_MUTED);

        char duration[32];
        GameFormatDuration(GameSubtestDurationSeconds(game->session.currentDifficulty, i),
                           duration, sizeof(duration));
        Vector2 durationSize = MeasureTextEx(game->fonts.small, duration, 13.0f, 0.3f);
        UI_Text(game->fonts.small, duration,
                row.x + row.width - 24.0f - durationSize.x,
                row.y + 13.0f, 13.0f, Fade(difficulty->accent, 0.95f));

        if (isSelected) {
            UI_Text(game->fonts.small, ">", row.x + row.width - 10.0f,
                    row.y + 10.0f, 16.0f, UI_CYAN);
        }
    }

    const char *note = "Phase 2 memakai bank soal demo. Bank soal lengkap 160 soal menyusul di Phase 3.";
    UI_TextCentered(game->fonts.small, note,
                    (Rectangle){ panel.x, 520.0f, panel.width, 22.0f },
                    13.0f, 0.3f, Fade(UI_MUTED, 0.85f));

    UI_Button(game->fonts.ui, SubtestBackRect(), "KEMBALI",
              false, true, UI_BUTTON_SECONDARY, UI_CYAN);
    UI_Button(game->fonts.ui, SubtestStartRect(), "MULAI UJIAN",
              false, true, UI_BUTTON_PRIMARY, difficulty->accent);

    UI_TextCentered(game->fonts.small,
                    "W/S ATAU PANAH: PILIH SUBTES   |   ENTER: MULAI UJIAN   |   ESC: KEMBALI",
                    (Rectangle){ 0.0f, 692.0f, SCREEN_WIDTH, 20.0f },
                    12.0f, 0.8f, Fade(UI_MUTED, 0.7f));
}

/* ------------------------------------------------------------------------- */
/* Generic placeholder screen                                                */
/* ------------------------------------------------------------------------- */

static const char *PlaceholderTitle(GameState state)
{
    switch (state) {
        case STATE_LEADERBOARD:
            return "PAPAN PERINGKAT";
        case STATE_EXAM:
            return "SESI UJIAN";
        case STATE_SCORE_SUMMARY:
            return "RINGKASAN SKOR";
        case STATE_REVIEW_PHASE:
            return "FASE PEMBAHASAN";
        default:
            return "SEGERA HADIR";
    }
}

static const char *PlaceholderPhase(GameState state)
{
    switch (state) {
        case STATE_LEADERBOARD:
            return "PHASE 5";
        case STATE_EXAM:
            return "PHASE 2";
        case STATE_SCORE_SUMMARY:
            return "PHASE 3";
        case STATE_REVIEW_PHASE:
            return "PHASE 4";
        default:
            return "PHASE 1";
    }
}

static const char *PlaceholderNote(GameState state)
{
    switch (state) {
        case STATE_LEADERBOARD:
            return "Penyimpanan Top 10 akan dibuat pada Phase 5: file leaderboard.dat, "
                   "sorting skor descending, dan tampilan papan peringkat.";
        case STATE_EXAM:
            return "UI ujian, timer per subtes, panel passage yang bisa di-scroll, "
                   "dan sprite Sae Chabashira tersedia pada Phase 2.";
        case STATE_SCORE_SUMMARY:
            return "Kalkulasi Item Response Theory (IRT) dan ringkasan skor akhir "
                   "dibuat pada Phase 3.";
        case STATE_REVIEW_PHASE:
            return "Pembahasan per nomor, sprite Arisu Sakayanagi, dan integrasi "
                   "Text-to-Speech dibuat pada Phase 4.";
        default:
            return "Modul ini belum tersedia.";
    }
}

static Rectangle PlaceholderPanelRect(void)
{
    return (Rectangle){ 300.0f, 96.0f, 680.0f, 528.0f };
}

static Rectangle PlaceholderBackRect(void)
{
    return (Rectangle){ 545.0f, 492.0f, 190.0f, 48.0f };
}

void UpdatePlaceholder(Game *game)
{
    if (UI_IsClicked(PlaceholderBackRect()) || IsKeyPressed(KEY_ESCAPE)) {
        if (game->state == STATE_LEADERBOARD) {
            GameChangeState(game, STATE_MENU);
        } else {
            GameChangeState(game, STATE_SUBTEST_SELECT);
        }
    }
}

void DrawPlaceholder(const Game *game)
{
    UI_DrawBackground(game->totalTime);

    Rectangle panel = PlaceholderPanelRect();
    UI_DrawPanel(panel, UI_PANEL, UI_BORDER);
    DrawCornerBrackets(panel, Fade(UI_CYAN, 0.45f), 26.0f, 1.6f);

    const char *phase = PlaceholderPhase(game->state);
    Vector2 phaseSize = MeasureTextEx(game->fonts.small, phase, 12.0f, 2.4f);
    Rectangle pill = {
        panel.x + (panel.width - phaseSize.x - 36.0f) * 0.5f,
        panel.y + 28.0f,
        phaseSize.x + 36.0f,
        28.0f
    };
    DrawRectangleRounded(pill, 0.5f, 10, Fade(UI_ACCENT, 0.12f));
    DrawRectangleRoundedLinesEx(pill, 0.5f, 10, 1.0f, Fade(UI_ACCENT, 0.55f));
    UI_TextCentered(game->fonts.small, phase, pill, 12.0f, 2.4f, UI_ACCENT);

    UI_TextCentered(game->fonts.title, PlaceholderTitle(game->state),
                    (Rectangle){ panel.x, panel.y + 68.0f, panel.width, 52.0f },
                    38.0f, 1.2f, UI_TEXT);

    const char *subtitle = "Modul ini akan dibangun pada fase berikutnya.";
    UI_TextCentered(game->fonts.small, subtitle,
                    (Rectangle){ panel.x, panel.y + 122.0f, panel.width, 24.0f },
                    14.0f, 0.4f, UI_MUTED);

    DrawRectangle((int)(panel.x + 90.0f), (int)(panel.y + 162.0f),
                  (int)(panel.width - 180.0f), 1, Fade(UI_BORDER, 0.8f));

    UI_DrawTextWrapped(game->fonts.ui, PlaceholderNote(game->state),
                       (Rectangle){ panel.x + 70.0f, panel.y + 186.0f,
                                    panel.width - 140.0f, 90.0f },
                       16.0f, 0.3f, 24.0f, (Color){ 200, 208, 224, 255 });

    Rectangle info = { panel.x + 70.0f, panel.y + 296.0f,
                       panel.width - 140.0f, 74.0f };
    DrawRectangleRounded(info, 0.12f, 10, (Color){ 14, 20, 33, 255 });
    DrawRectangleRoundedLinesEx(info, 0.12f, 10, 1.0f, Fade(UI_BORDER, 0.8f));

    char playerLine[96];
    snprintf(playerLine, sizeof(playerLine), "PESERTA   %s", game->session.playerName);
    UI_Text(game->fonts.small, playerLine, info.x + 20.0f, info.y + 14.0f,
            13.0f, UI_MUTED);

    const DifficultyInfo *difficulty = GameCurrentDifficulty(game);
    char modeLine[96];
    snprintf(modeLine, sizeof(modeLine), "MODE      %s", difficulty->name);
    UI_Text(game->fonts.small, modeLine, info.x + 20.0f, info.y + 40.0f,
            13.0f, difficulty->accent);

    UI_Button(game->fonts.ui, PlaceholderBackRect(), "KEMBALI",
              false, true, UI_BUTTON_SECONDARY, UI_CYAN);

    UI_TextCentered(game->fonts.small, "ESC: KEMBALI",
                    (Rectangle){ 0.0f, 660.0f, SCREEN_WIDTH, 20.0f },
                    12.0f, 0.8f, Fade(UI_MUTED, 0.7f));
}
