#include "exam.h"
#include "ui.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define EXAM_HEADER_HEIGHT 72.0f
#define EXAM_MARGIN 24.0f
#define EXAM_CONTENT_Y 88.0f
#define EXAM_CONTENT_HEIGHT 596.0f
#define EXAM_LEFT_WIDTH 650.0f
#define EXAM_RIGHT_WIDTH 566.0f
#define EXAM_GAP 16.0f

#define PASSAGE_FONT_SIZE 16.0f
#define PASSAGE_LINE_HEIGHT 24.0f
#define PASSAGE_SPACING 0.35f

static bool EnterPressed(void)
{
    return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER);
}

static void FormatClock(int seconds, char *out, size_t outSize)
{
    if (seconds < 0) {
        seconds = 0;
    }

    int minutes = seconds / 60;
    int secs = seconds % 60;
    snprintf(out, outSize, "%02d:%02d", minutes, secs);
}

static Rectangle ExamHeaderRect(void)
{
    return (Rectangle){ 0.0f, 0.0f, SCREEN_WIDTH, EXAM_HEADER_HEIGHT };
}

static Rectangle ExamLeftPanelRect(void)
{
    return (Rectangle){ EXAM_MARGIN, EXAM_CONTENT_Y, EXAM_LEFT_WIDTH, EXAM_CONTENT_HEIGHT };
}

static Rectangle ExamRightPanelRect(void)
{
    return (Rectangle){ EXAM_MARGIN + EXAM_LEFT_WIDTH + EXAM_GAP,
                        EXAM_CONTENT_Y, EXAM_RIGHT_WIDTH, EXAM_CONTENT_HEIGHT };
}

static Rectangle PassageViewRect(void)
{
    Rectangle panel = ExamLeftPanelRect();
    return (Rectangle){ panel.x + 24.0f, panel.y + 44.0f,
                        panel.width - 48.0f, panel.height - 44.0f - 112.0f };
}

static Rectangle PassageTextRect(void)
{
    Rectangle view = PassageViewRect();
    return (Rectangle){ view.x, view.y, view.width - 16.0f, view.height };
}

static Rectangle PassageScrollTrackRect(void)
{
    Rectangle view = PassageViewRect();
    return (Rectangle){ view.x + view.width - 5.0f, view.y + 2.0f,
                        4.0f, view.height - 4.0f };
}

static Rectangle NavigatorBoxRect(const Game *game, int index)
{
    (void)game;
    Rectangle panel = ExamRightPanelRect();
    int columns = 10;
    int row = index / columns;
    int column = index % columns;
    return (Rectangle){ panel.x + 24.0f + (float)column * 30.0f,
                        panel.y + 42.0f + (float)row * 30.0f,
                        24.0f, 24.0f };
}

static float RightContentTop(const Game *game)
{
    Rectangle panel = ExamRightPanelRect();
    int rows = (game->session.questionCount + 9) / 10;
    if (rows < 1) {
        rows = 1;
    }
    return panel.y + 42.0f + (float)rows * 30.0f + 12.0f;
}

static Rectangle PromptRect(const Game *game)
{
    Rectangle panel = ExamRightPanelRect();
    return (Rectangle){ panel.x + 24.0f, RightContentTop(game),
                        panel.width - 48.0f, 74.0f };
}

static Rectangle OptionRect(const Game *game, int index)
{
    Rectangle prompt = PromptRect(game);
    return (Rectangle){ prompt.x, prompt.y + prompt.height + 10.0f + (float)index * 56.0f,
                        prompt.width, 48.0f };
}

static Rectangle ExamPrevRect(void)
{
    Rectangle panel = ExamRightPanelRect();
    return (Rectangle){ panel.x + 24.0f, panel.y + panel.height - 64.0f, 160.0f, 44.0f };
}

static Rectangle ExamNextRect(void)
{
    Rectangle prev = ExamPrevRect();
    return (Rectangle){ prev.x + prev.width + 12.0f, prev.y, 160.0f, 44.0f };
}

static Rectangle ExamFinishRect(void)
{
    Rectangle next = ExamNextRect();
    return (Rectangle){ next.x + next.width + 12.0f, next.y, 160.0f, 44.0f };
}

static Rectangle TimerPanelRect(void)
{
    return (Rectangle){ 900.0f, 8.0f, 220.0f, 56.0f };
}

static Rectangle ExamExitRect(void)
{
    return (Rectangle){ 1140.0f, 16.0f, 116.0f, 40.0f };
}

static Rectangle ExamOverlayPanelRect(void)
{
    return (Rectangle){ 390.0f, 230.0f, 500.0f, 250.0f };
}

static Rectangle OverlayCancelRect(void)
{
    return (Rectangle){ 430.0f, 400.0f, 190.0f, 44.0f };
}

static Rectangle OverlayConfirmRect(void)
{
    return (Rectangle){ 660.0f, 400.0f, 190.0f, 44.0f };
}

static Rectangle OverlayTimeUpRect(void)
{
    return (Rectangle){ 545.0f, 400.0f, 190.0f, 44.0f };
}

static float ExamMaxScroll(const Game *game)
{
    const Question *question = GameCurrentQuestion(game);
    if (question == NULL || question->passage == NULL) {
        return 0.0f;
    }

    Rectangle text = PassageTextRect();
    float contentHeight = UI_MeasureWrappedTextHeight(game->fonts.small, question->passage,
                                                      text.width, PASSAGE_FONT_SIZE,
                                                      PASSAGE_SPACING, PASSAGE_LINE_HEIGHT);
    float maxScroll = contentHeight - text.height;
    return maxScroll > 0.0f ? maxScroll : 0.0f;
}

static const char *ExamSaeTip(const Game *game)
{
    static const char *tips[] = {
        "Sae: Baca stimulus dengan teliti sebelum menjawab.",
        "Sae: Jangan terjebak terlalu lama pada satu soal.",
        "Sae: Eliminasi opsi yang jelas salah, lalu pilih yang paling tepat.",
        "Sae: Periksa kembali jawabanmu sebelum menekan SELESAI."
    };

    int count = (int)(sizeof(tips) / sizeof(tips[0]));
    int index = game->session.currentQuestion % count;
    return tips[index];
}

static void DrawSaePortrait(const Game *game, Rectangle dest)
{
    Vector2 center = { dest.x + dest.width * 0.5f, dest.y + dest.height * 0.5f };
    DrawCircleV(center, dest.width * 0.56f, Fade(UI_CYAN, 0.06f));
    DrawCircleLines((int)center.x, (int)center.y, dest.width * 0.52f,
                    Fade(UI_CYAN, 0.18f));

    if (game->saeTexture.id != 0) {
        float frameWidth = (float)game->saeTexture.width / 3.0f;
        Rectangle source = { (float)game->saeFrame * frameWidth, 0.0f,
                             frameWidth, (float)game->saeTexture.height };
        Rectangle target = { dest.x, dest.y + sinf(game->totalTime * 2.2f) * 1.5f,
                             dest.width, dest.height };
        DrawTexturePro(game->saeTexture, source, target,
                       (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
    } else {
        DrawRectangleRounded(dest, 0.2f, 8, Fade(UI_CYAN, 0.12f));
        DrawRectangleRoundedLinesEx(dest, 0.2f, 8, 1.0f, Fade(UI_CYAN, 0.45f));
        UI_TextCentered(game->fonts.small, "SAE", dest, 14.0f, 1.0f, UI_CYAN);
    }
}

static void DrawExamHeader(const Game *game)
{
    const DifficultyInfo *difficulty = GameCurrentDifficulty(game);
    int subtest = game->session.currentSubtest;
    if (subtest < 0 || subtest >= SUBTEST_COUNT) {
        subtest = 0;
    }

    Rectangle header = ExamHeaderRect();
    DrawRectangleRec(header, UI_PANEL);
    DrawRectangle(0, (int)header.height - 1, SCREEN_WIDTH, 1, Fade(UI_BORDER, 0.85f));
    DrawRectangle(0, 0, SCREEN_WIDTH, 2, Fade(difficulty->accent, 0.85f));

    Rectangle badge = { 24.0f, 18.0f, 52.0f, 36.0f };
    DrawRectangleRounded(badge, 0.25f, 8, Fade(UI_CYAN, 0.12f));
    DrawRectangleRoundedLinesEx(badge, 0.25f, 8, 1.0f, Fade(UI_CYAN, 0.55f));
    UI_TextCentered(game->fonts.small, SUBTEST_CODES[subtest], badge,
                    14.0f, 1.0f, UI_CYAN);

    UI_Text(game->fonts.ui, SUBTEST_NAMES[subtest], 92.0f, 11.0f, 20.0f, UI_TEXT);

    char mode[128];
    snprintf(mode, sizeof(mode), "MODE %s  //  %s", difficulty->name,
             game->session.playerName);
    UI_Text(game->fonts.small, mode, 92.0f, 38.0f, 13.0f, difficulty->accent);

    Rectangle timer = TimerPanelRect();
    UI_DrawPanel(timer, (Color){ 13, 18, 30, 255 }, Fade(UI_BORDER, 0.85f));
    UI_TextSpaced(game->fonts.small, "SISA WAKTU", timer.x + 14.0f, timer.y + 7.0f,
                  10.0f, 1.8f, UI_MUTED);

    int remaining = (int)ceilf(game->session.timeRemaining);
    if (remaining < 0) {
        remaining = 0;
    }

    char clock[16];
    FormatClock(remaining, clock, sizeof(clock));

    Color clockColor = UI_TEXT;
    if (remaining <= 300) {
        clockColor = UI_ACCENT;
    } else if (remaining <= 600) {
        clockColor = UI_WARNING;
    }

    Vector2 clockSize = MeasureTextEx(game->fonts.ui, clock, 26.0f, 1.0f);
    UI_Text(game->fonts.ui, clock, timer.x + timer.width - clockSize.x - 14.0f,
            timer.y + 13.0f, 26.0f, clockColor);

    float fraction = 0.0f;
    if (game->examTotalSeconds > 0) {
        fraction = game->session.timeRemaining / (float)game->examTotalSeconds;
        if (fraction < 0.0f) {
            fraction = 0.0f;
        } else if (fraction > 1.0f) {
            fraction = 1.0f;
        }
    }

    Rectangle barBackground = { timer.x + 14.0f, timer.y + 45.0f,
                                timer.width - 28.0f, 4.0f };
    DrawRectangleRounded(barBackground, 0.5f, 4, Fade(UI_BORDER, 0.55f));
    Rectangle bar = { barBackground.x, barBackground.y,
                      barBackground.width * fraction, barBackground.height };
    if (bar.width > 0.5f) {
        DrawRectangleRounded(bar, 0.5f, 4, clockColor);
    }

    UI_Button(game->fonts.small, ExamExitRect(), "KELUAR", false, true,
              UI_BUTTON_GHOST, UI_ACCENT);
}

static void DrawPassagePanel(const Game *game)
{
    Rectangle panel = ExamLeftPanelRect();
    UI_DrawPanel(panel, UI_PANEL, UI_BORDER);
    UI_DrawSectionLabel(game->fonts.small, "STIMULUS / TEKS BACAAN",
                        panel.x + 24.0f, panel.y + 16.0f, UI_CYAN);

    const Question *question = GameCurrentQuestion(game);
    const char *passage = (question != NULL && question->passage != NULL)
                              ? question->passage
                              : "Soal ini tidak memiliki teks stimulus. Baca pertanyaan di panel kanan.";

    Rectangle textRect = PassageTextRect();
    float contentHeight = UI_MeasureWrappedTextHeight(game->fonts.small, passage,
                                                      textRect.width, PASSAGE_FONT_SIZE,
                                                      PASSAGE_SPACING, PASSAGE_LINE_HEIGHT);
    float maxScroll = contentHeight - textRect.height;
    if (maxScroll < 0.0f) {
        maxScroll = 0.0f;
    }

    UI_DrawTextWrappedScrolled(game->fonts.small, passage, textRect,
                               PASSAGE_FONT_SIZE, PASSAGE_SPACING, PASSAGE_LINE_HEIGHT,
                               game->examScroll, (Color){ 205, 212, 227, 255 });

    if (maxScroll > 0.5f) {
        Rectangle track = PassageScrollTrackRect();
        DrawRectangleRounded(track, 0.5f, 4, Fade(UI_BORDER, 0.45f));

        float visibleRatio = textRect.height / contentHeight;
        if (visibleRatio > 1.0f) {
            visibleRatio = 1.0f;
        }
        float thumbHeight = track.height * visibleRatio;
        if (thumbHeight < 24.0f) {
            thumbHeight = 24.0f;
        }

        float progress = game->examScroll / maxScroll;
        if (progress < 0.0f) {
            progress = 0.0f;
        } else if (progress > 1.0f) {
            progress = 1.0f;
        }

        Rectangle thumb = { track.x, track.y + (track.height - thumbHeight) * progress,
                            track.width, thumbHeight };
        DrawRectangleRounded(thumb, 0.5f, 4, Fade(UI_CYAN, 0.55f));
    }

    float separatorY = panel.y + panel.height - 100.0f;
    DrawRectangle((int)(panel.x + 20.0f), (int)separatorY,
                  (int)(panel.width - 40.0f), 1, Fade(UI_BORDER, 0.65f));

    Rectangle sae = { panel.x + 24.0f, panel.y + panel.height - 84.0f, 64.0f, 64.0f };
    DrawSaePortrait(game, sae);

    UI_TextSpaced(game->fonts.small, "SAE CHABASHIRA // PENGAWAS",
                  sae.x + 78.0f, sae.y + 4.0f, 10.0f, 1.5f, Fade(UI_CYAN, 0.85f));
    UI_DrawTextWrapped(game->fonts.small, ExamSaeTip(game),
                       (Rectangle){ sae.x + 78.0f, sae.y + 22.0f,
                                    panel.width - 24.0f - 78.0f - 24.0f, 40.0f },
                       14.0f, 0.25f, 19.0f, UI_MUTED);
}

static void DrawQuestionPanel(const Game *game)
{
    Rectangle panel = ExamRightPanelRect();
    UI_DrawPanel(panel, UI_PANEL, UI_BORDER);

    const Question *question = GameCurrentQuestion(game);
    if (question == NULL) {
        UI_TextCentered(game->fonts.ui, "Tidak ada soal.", panel, 18.0f, 0.5f, UI_MUTED);
        return;
    }

    char questionLabel[64];
    snprintf(questionLabel, sizeof(questionLabel), "SOAL %02d / %02d",
             game->session.currentQuestion + 1, game->session.questionCount);
    UI_Text(game->fonts.ui, questionLabel, panel.x + 24.0f, panel.y + 14.0f,
            18.0f, UI_TEXT);

    int answered = 0;
    for (int i = 0; i < game->session.questionCount; ++i) {
        if (game->session.userAnswers[game->session.answerOffset + i] >= 0) {
            ++answered;
        }
    }

    char answeredLabel[48];
    snprintf(answeredLabel, sizeof(answeredLabel), "%d TERJAWAB", answered);
    Vector2 answeredSize = MeasureTextEx(game->fonts.small, answeredLabel, 11.0f, 1.2f);
    UI_TextSpaced(game->fonts.small, answeredLabel,
                  panel.x + panel.width - answeredSize.x - 24.0f, panel.y + 19.0f,
                  11.0f, 1.2f, UI_MUTED);

    for (int i = 0; i < game->session.questionCount; ++i) {
        Rectangle box = NavigatorBoxRect(game, i);
        int answer = game->session.userAnswers[game->session.answerOffset + i];
        bool current = (i == game->session.currentQuestion);

        Color fill = (Color){ 22, 28, 44, 255 };
        Color border = Fade(UI_BORDER, 0.8f);
        Color textColor = UI_MUTED;

        if (answer >= 0) {
            fill = Fade(UI_SUCCESS, 0.16f);
            border = Fade(UI_SUCCESS, 0.6f);
            textColor = UI_SUCCESS;
        }
        if (current) {
            fill = Fade(UI_CYAN, 0.2f);
            border = UI_CYAN;
            textColor = UI_TEXT;
        }

        DrawRectangleRounded(box, 0.2f, 6, fill);
        DrawRectangleRoundedLinesEx(box, 0.2f, 6, current ? 1.6f : 1.0f, border);

        char number[16];
        snprintf(number, sizeof(number), "%d", i + 1);
        UI_TextCentered(game->fonts.small, number, box, 12.0f, 0.3f, textColor);
    }

    Rectangle prompt = PromptRect(game);
    UI_DrawTextWrapped(game->fonts.ui, question->prompt, prompt,
                       16.0f, 0.3f, 22.0f, UI_TEXT);

    int selected = GameCurrentAnswer(game);
    for (int i = 0; i < MAX_OPTIONS; ++i) {
        Rectangle option = OptionRect(game, i);
        bool hovered = CheckCollisionPointRec(GetMousePosition(), option);
        bool isSelected = (selected == i);
        bool isFocused = (game->examFocusedOption == i);

        Color fill = UI_PANEL_LIGHT;
        Color border = UI_BORDER;
        Color letterFill = Fade(UI_CYAN, 0.08f);
        Color letterText = UI_CYAN;

        if (isSelected) {
            fill = Fade(UI_CYAN, 0.14f);
            border = Fade(UI_CYAN, 0.85f);
            letterFill = UI_CYAN;
            letterText = UI_BG_TOP;
        } else if (isFocused) {
            fill = (Color){ 30, 38, 58, 255 };
            border = Fade(UI_CYAN, 0.45f);
        } else if (hovered) {
            fill = (Color){ 26, 33, 50, 255 };
            border = Fade(UI_CYAN, 0.25f);
        }

        DrawRectangleRounded(option, 0.18f, 8, fill);
        DrawRectangleRoundedLinesEx(option, 0.18f, 8,
                                    isSelected ? 1.6f : 1.0f, border);

        Rectangle letter = { option.x + 10.0f, option.y + 10.0f, 28.0f, 28.0f };
        DrawRectangleRounded(letter, 0.25f, 8, letterFill);

        char letterLabel[2] = { (char)('A' + i), '\0' };
        UI_TextCentered(game->fonts.small, letterLabel, letter,
                        14.0f, 0.5f, letterText);

        UI_DrawTextWrapped(game->fonts.small, question->options[i],
                           (Rectangle){ option.x + 50.0f, option.y + 6.0f,
                                        option.width - 62.0f, option.height - 12.0f },
                           15.0f, 0.25f, 19.0f,
                           isSelected ? UI_TEXT : (Color){ 205, 212, 227, 255 });
    }

    const DifficultyInfo *difficulty = GameCurrentDifficulty(game);
    bool canPrev = game->session.currentQuestion > 0;
    bool canNext = game->session.currentQuestion + 1 < game->session.questionCount;

    UI_Button(game->fonts.ui, ExamPrevRect(), "SEBELUMNYA", false, canPrev,
              UI_BUTTON_SECONDARY, UI_CYAN);
    UI_Button(game->fonts.ui, ExamNextRect(), "BERIKUTNYA", false, canNext,
              UI_BUTTON_SECONDARY, UI_CYAN);
    UI_Button(game->fonts.ui, ExamFinishRect(), "SELESAI", false, true,
              UI_BUTTON_PRIMARY, difficulty->accent);
}

static const char *ExamOverlayTitle(const Game *game)
{
    switch (game->examOverlay) {
        case EXAM_OVERLAY_FINISH:
            return "SELESAI UJIAN?";
        case EXAM_OVERLAY_EXIT:
            return "KELUAR DARI UJIAN?";
        case EXAM_OVERLAY_TIMEUP:
            return "WAKTU HABIS";
        default:
            return "";
    }
}

static void DrawExamOverlay(const Game *game)
{
    if (game->examOverlay == EXAM_OVERLAY_NONE) {
        return;
    }

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.68f));

    Rectangle panel = ExamOverlayPanelRect();
    UI_DrawPanel(panel, (Color){ 16, 22, 36, 255 }, Fade(UI_CYAN, 0.6f));

    UI_TextCentered(game->fonts.title, ExamOverlayTitle(game),
                    (Rectangle){ panel.x, panel.y + 22.0f, panel.width, 44.0f },
                    30.0f, 1.0f, UI_TEXT);

    int unanswered = 0;
    for (int i = 0; i < game->session.questionCount; ++i) {
        if (game->session.userAnswers[game->session.answerOffset + i] < 0) {
            ++unanswered;
        }
    }

    char message[256];
    if (game->examOverlay == EXAM_OVERLAY_FINISH) {
        snprintf(message, sizeof(message),
                 "Masih ada %d soal yang belum dijawab. Tetap selesaikan ujian?",
                 unanswered);
    } else if (game->examOverlay == EXAM_OVERLAY_EXIT) {
        snprintf(message, sizeof(message),
                 "Progres ujian akan hilang jika kamu keluar sekarang.");
    } else {
        snprintf(message, sizeof(message),
                 "Waktu pengerjaan subtes ini sudah habis.");
    }

    UI_DrawTextWrapped(game->fonts.small, message,
                       (Rectangle){ panel.x + 40.0f, panel.y + 82.0f,
                                    panel.width - 80.0f, 54.0f },
                       15.0f, 0.3f, 21.0f, UI_MUTED);

    if (game->examOverlay == EXAM_OVERLAY_TIMEUP) {
        UI_Button(game->fonts.ui, OverlayTimeUpRect(), "LIHAT HASIL", false, true,
                  UI_BUTTON_PRIMARY, UI_ACCENT);
    } else {
        const char *confirmLabel = game->examOverlay == EXAM_OVERLAY_FINISH
                                       ? "YA, SELESAI"
                                       : "YA, KELUAR";
        UI_Button(game->fonts.ui, OverlayCancelRect(), "BATAL", false, true,
                  UI_BUTTON_SECONDARY, UI_CYAN);
        UI_Button(game->fonts.ui, OverlayConfirmRect(), confirmLabel, false, true,
                  UI_BUTTON_PRIMARY, UI_ACCENT);
    }
}

static void UpdateExamOverlay(Game *game)
{
    if (game->examOverlay == EXAM_OVERLAY_NONE) {
        return;
    }

    if (game->examOverlay == EXAM_OVERLAY_TIMEUP) {
        if (UI_IsClicked(OverlayTimeUpRect()) || EnterPressed()) {
            game->examOverlay = EXAM_OVERLAY_NONE;
            GameChangeState(game, STATE_SCORE_SUMMARY);
        }
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE) || UI_IsClicked(OverlayCancelRect())) {
        game->examOverlay = EXAM_OVERLAY_NONE;
        return;
    }

    if (UI_IsClicked(OverlayConfirmRect()) || EnterPressed()) {
        if (game->examOverlay == EXAM_OVERLAY_FINISH) {
            game->examOverlay = EXAM_OVERLAY_NONE;
            GameChangeState(game, STATE_SCORE_SUMMARY);
        } else if (game->examOverlay == EXAM_OVERLAY_EXIT) {
            game->examOverlay = EXAM_OVERLAY_NONE;
            GameChangeState(game, STATE_SUBTEST_SELECT);
        }
    }
}

void UpdateExam(Game *game)
{
    float dt = GetFrameTime();

    if (game->session.timeRemaining > 0.0f) {
        game->session.timeRemaining -= dt;
        if (game->session.timeRemaining <= 0.0f) {
            game->session.timeRemaining = 0.0f;
            game->examOverlay = EXAM_OVERLAY_TIMEUP;
        }
    }

    if (game->examOverlay != EXAM_OVERLAY_NONE) {
        UpdateExamOverlay(game);
        return;
    }

    const Question *question = GameCurrentQuestion(game);
    if (question == NULL) {
        return;
    }

    Rectangle passageView = PassageViewRect();
    float wheel = GetMouseWheelMove();
    if (CheckCollisionPointRec(GetMousePosition(), passageView) && wheel != 0.0f) {
        game->examScrollTarget -= wheel * 52.0f;
    }
    if (IsKeyPressed(KEY_PAGE_DOWN)) {
        game->examScrollTarget += 160.0f;
    }
    if (IsKeyPressed(KEY_PAGE_UP)) {
        game->examScrollTarget -= 160.0f;
    }

    float maxScroll = ExamMaxScroll(game);
    if (game->examScrollTarget < 0.0f) {
        game->examScrollTarget = 0.0f;
    } else if (game->examScrollTarget > maxScroll) {
        game->examScrollTarget = maxScroll;
    }
    if (game->examScroll > maxScroll) {
        game->examScroll = maxScroll;
    }

    float smoothing = 1.0f - expf(-dt * 14.0f);
    game->examScroll += (game->examScrollTarget - game->examScroll) * smoothing;
    if (fabsf(game->examScrollTarget - game->examScroll) < 0.2f) {
        game->examScroll = game->examScrollTarget;
    }

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        GameGoToQuestion(game, game->session.currentQuestion - 1);
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        GameGoToQuestion(game, game->session.currentQuestion + 1);
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        game->examFocusedOption = (game->examFocusedOption + MAX_OPTIONS - 1) % MAX_OPTIONS;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        game->examFocusedOption = (game->examFocusedOption + 1) % MAX_OPTIONS;
    }

    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
        game->examFocusedOption = 0;
        GameSetCurrentAnswer(game, 0);
    }
    if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
        game->examFocusedOption = 1;
        GameSetCurrentAnswer(game, 1);
    }
    if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) {
        game->examFocusedOption = 2;
        GameSetCurrentAnswer(game, 2);
    }
    if (IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_KP_4)) {
        game->examFocusedOption = 3;
        GameSetCurrentAnswer(game, 3);
    }
    if (IsKeyPressed(KEY_FIVE) || IsKeyPressed(KEY_KP_5)) {
        game->examFocusedOption = 4;
        GameSetCurrentAnswer(game, 4);
    }

    if (EnterPressed()) {
        GameSetCurrentAnswer(game, game->examFocusedOption);
    }

    for (int i = 0; i < MAX_OPTIONS; ++i) {
        if (UI_IsClicked(OptionRect(game, i))) {
            game->examFocusedOption = i;
            GameSetCurrentAnswer(game, i);
        }
    }

    for (int i = 0; i < game->session.questionCount; ++i) {
        if (UI_IsClicked(NavigatorBoxRect(game, i))) {
            GameGoToQuestion(game, i);
        }
    }

    if (UI_IsClicked(ExamPrevRect())) {
        GameGoToQuestion(game, game->session.currentQuestion - 1);
    }
    if (UI_IsClicked(ExamNextRect())) {
        GameGoToQuestion(game, game->session.currentQuestion + 1);
    }
    if (UI_IsClicked(ExamFinishRect()) || IsKeyPressed(KEY_F)) {
        game->examOverlay = EXAM_OVERLAY_FINISH;
    }
    if (UI_IsClicked(ExamExitRect()) || IsKeyPressed(KEY_ESCAPE)) {
        game->examOverlay = EXAM_OVERLAY_EXIT;
    }
}

void DrawExam(const Game *game)
{
    UI_DrawBackground(game->totalTime);
    DrawExamHeader(game);
    DrawPassagePanel(game);
    DrawQuestionPanel(game);

    UI_TextCentered(game->fonts.small,
                    "A/D: SOAL   W/S: OPSI   1-5: JAWAB   ENTER: PILIH   F: SELESAI   SCROLL: BACA   ESC: KELUAR",
                    (Rectangle){ 0.0f, 696.0f, SCREEN_WIDTH, 18.0f },
                    12.0f, 0.5f, Fade(UI_MUTED, 0.75f));

    DrawExamOverlay(game);
}
