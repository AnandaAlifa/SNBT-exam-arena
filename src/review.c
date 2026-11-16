#include "review.h"
#include "ui.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define REVIEW_MARGIN 24.0f
#define REVIEW_TOP 72.0f
#define REVIEW_PANEL_HEIGHT 576.0f
#define REVIEW_LEFT_WIDTH 650.0f
#define REVIEW_RIGHT_WIDTH 566.0f
#define REVIEW_GAP 16.0f

static Rectangle ReviewLeftPanelRect(void)
{
    return (Rectangle){ REVIEW_MARGIN, REVIEW_TOP,
                        REVIEW_LEFT_WIDTH, REVIEW_PANEL_HEIGHT };
}

static Rectangle ReviewRightPanelRect(void)
{
    return (Rectangle){ REVIEW_MARGIN + REVIEW_LEFT_WIDTH + REVIEW_GAP,
                        REVIEW_TOP, REVIEW_RIGHT_WIDTH, REVIEW_PANEL_HEIGHT };
}

static Rectangle ReviewPassageRect(void)
{
    Rectangle panel = ReviewLeftPanelRect();
    return (Rectangle){ panel.x + 18.0f, panel.y + 46.0f,
                        panel.width - 36.0f, 180.0f };
}

static Rectangle ReviewPromptRect(void)
{
    Rectangle panel = ReviewLeftPanelRect();
    return (Rectangle){ panel.x + 18.0f, panel.y + 234.0f,
                        panel.width - 36.0f, 54.0f };
}

static Rectangle ReviewOptionRect(int index)
{
    Rectangle panel = ReviewLeftPanelRect();
    return (Rectangle){ panel.x + 18.0f, panel.y + 296.0f + (float)index * 49.0f,
                        panel.width - 36.0f, 44.0f };
}

static Rectangle ReviewSpeechRect(void)
{
    Rectangle panel = ReviewRightPanelRect();
    return (Rectangle){ panel.x + 20.0f, panel.y + 356.0f,
                        panel.width - 40.0f, 206.0f };
}

static Rectangle ReviewPortraitRect(void)
{
    Rectangle panel = ReviewRightPanelRect();
    return (Rectangle){ panel.x + panel.width - 310.0f, panel.y + 28.0f,
                        290.0f, 360.0f };
}

static Rectangle ReviewPrevRect(void)
{
    return (Rectangle){ 24.0f, 660.0f, 150.0f, 44.0f };
}

static Rectangle ReviewNextRect(void)
{
    return (Rectangle){ 186.0f, 660.0f, 150.0f, 44.0f };
}

static Rectangle ReviewSummaryRect(void)
{
    return (Rectangle){ 690.0f, 660.0f, 190.0f, 44.0f };
}

static Rectangle ReviewFinishRect(void)
{
    return (Rectangle){ 1080.0f, 660.0f, 176.0f, 44.0f };
}

static Rectangle SummaryBackRect(void)
{
    return (Rectangle){ 24.0f, 608.0f, 220.0f, 44.0f };
}

static Rectangle SummaryReviewRect(void)
{
    return (Rectangle){ 1036.0f, 608.0f, 220.0f, 44.0f };
}

static void DrawRoomBackground(const Game *game)
{
    if (game->examBackground.id != 0) {
        float scale = fmaxf((float)SCREEN_WIDTH / (float)game->examBackground.width,
                            (float)SCREEN_HEIGHT / (float)game->examBackground.height);
        float width = (float)game->examBackground.width * scale;
        float height = (float)game->examBackground.height * scale;
        Rectangle source = { 0.0f, 0.0f,
                             (float)game->examBackground.width,
                             (float)game->examBackground.height };
        Rectangle dest = { (SCREEN_WIDTH - width) * 0.5f,
                           (SCREEN_HEIGHT - height) * 0.5f, width, height };
        DrawTexturePro(game->examBackground, source, dest,
                       (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
        DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                               Fade((Color){ 7, 10, 18, 255 }, 0.72f),
                               Fade((Color){ 7, 10, 18, 255 }, 0.94f));
    } else {
        UI_DrawBackground(game->totalTime);
    }
}

static void DrawPortrait(Texture2D texture, Rectangle area)
{
    if (texture.id != 0) {
        float scale = fminf(area.width / (float)texture.width,
                            area.height / (float)texture.height);
        float width = (float)texture.width * scale;
        float height = (float)texture.height * scale;
        Rectangle target = { area.x + (area.width - width) * 0.5f,
                             area.y + area.height - height, width, height };
        DrawTexturePro(texture, (Rectangle){ 0.0f, 0.0f,
                                            (float)texture.width,
                                            (float)texture.height },
                       target, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
    } else {
        DrawRectangleRounded(area, 0.12f, 10, Fade(UI_CYAN, 0.10f));
        DrawRectangleRoundedLinesEx(area, 0.12f, 10, 1.0f, Fade(UI_CYAN, 0.35f));
        UI_TextCentered(GetFontDefault(), "ARISU", area, 20.0f, 1.0f, UI_CYAN);
    }
}

static void DrawSpeechBox(const Game *game, Rectangle box, const char *speaker,
                          const char *status, const char *text, Color accent,
                          float scroll)
{
    DrawRectangleRounded(box, 0.08f, 12, (Color){ 10, 15, 26, 238 });
    DrawRectangleRoundedLinesEx(box, 0.08f, 12, 1.4f, Fade(accent, 0.75f));
    DrawRectangle((int)box.x + 18, (int)box.y + 1,
                  (int)box.width - 36, 2, Fade(accent, 0.55f));

    UI_TextSpaced(game->fonts.small, speaker, box.x + 18.0f, box.y + 12.0f,
                  10.0f, 1.8f, accent);
    if (status != NULL && status[0] != '\0') {
        Vector2 size = MeasureTextEx(game->fonts.small, status, 10.0f, 1.2f);
        UI_TextSpaced(game->fonts.small, status,
                      box.x + box.width - size.x - 18.0f, box.y + 12.0f,
                      10.0f, 1.2f, UI_MUTED);
    }

    Rectangle inner = { box.x + 18.0f, box.y + 40.0f,
                        box.width - 36.0f, box.height - 52.0f };
    UI_DrawTextWrappedScrolled(game->fonts.small, text, inner,
                               14.0f, 0.25f, 20.0f, scroll,
                               (Color){ 225, 231, 242, 255 });
}

static void DrawScrollBar(Rectangle track, float scroll, float maxScroll)
{
    DrawRectangleRounded(track, 0.5f, 4, Fade(UI_BORDER, 0.45f));
    if (maxScroll <= 0.5f) {
        return;
    }

    float visible = track.height * 0.28f;
    if (visible < 20.0f) {
        visible = 20.0f;
    }
    if (visible > track.height) {
        visible = track.height;
    }

    float progress = scroll / maxScroll;
    if (progress < 0.0f) {
        progress = 0.0f;
    } else if (progress > 1.0f) {
        progress = 1.0f;
    }

    Rectangle thumb = { track.x, track.y + (track.height - visible) * progress,
                        track.width, visible };
    DrawRectangleRounded(thumb, 0.5f, 4, Fade(UI_CYAN, 0.55f));
}

static float ReviewPassageMaxScroll(const Game *game)
{
    const Question *question = GameCurrentQuestion(game);
    if (question == NULL || question->passage == NULL) {
        return 0.0f;
    }

    Rectangle rect = ReviewPassageRect();
    float height = UI_MeasureWrappedTextHeight(game->fonts.small, question->passage,
                                               rect.width - 14.0f, 15.0f, 0.25f, 21.0f);
    float maxScroll = height - rect.height;
    return maxScroll > 0.0f ? maxScroll : 0.0f;
}

static float ReviewExplanationMaxScroll(const Game *game)
{
    const Question *question = GameCurrentQuestion(game);
    if (question == NULL || question->explanation == NULL) {
        return 0.0f;
    }

    Rectangle speech = ReviewSpeechRect();
    float height = UI_MeasureWrappedTextHeight(game->fonts.small, question->explanation,
                                               speech.width - 36.0f, 14.0f, 0.25f, 20.0f);
    float maxScroll = height - (speech.height - 52.0f);
    return maxScroll > 0.0f ? maxScroll : 0.0f;
}

static void ClampReviewScroll(Game *game)
{
    float passageMax = ReviewPassageMaxScroll(game);
    float explanationMax = ReviewExplanationMaxScroll(game);

    if (game->reviewScrollTarget < 0.0f) {
        game->reviewScrollTarget = 0.0f;
    } else if (game->reviewScrollTarget > passageMax) {
        game->reviewScrollTarget = passageMax;
    }
    if (game->explanationScrollTarget < 0.0f) {
        game->explanationScrollTarget = 0.0f;
    } else if (game->explanationScrollTarget > explanationMax) {
        game->explanationScrollTarget = explanationMax;
    }
    if (game->reviewScroll > passageMax) {
        game->reviewScroll = passageMax;
    }
    if (game->explanationScroll > explanationMax) {
        game->explanationScroll = explanationMax;
    }
}

static void SmoothReviewScroll(Game *game, float dt)
{
    float smoothing = 1.0f - expf(-dt * 14.0f);
    game->reviewScroll += (game->reviewScrollTarget - game->reviewScroll) * smoothing;
    game->explanationScroll += (game->explanationScrollTarget - game->explanationScroll) * smoothing;

    if (fabsf(game->reviewScrollTarget - game->reviewScroll) < 0.2f) {
        game->reviewScroll = game->reviewScrollTarget;
    }
    if (fabsf(game->explanationScrollTarget - game->explanationScroll) < 0.2f) {
        game->explanationScroll = game->explanationScrollTarget;
    }
}

static void ReviewGoToQuestion(Game *game, int index)
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

    game->session.currentQuestion = index;
    game->reviewScroll = 0.0f;
    game->reviewScrollTarget = 0.0f;
    game->explanationScroll = 0.0f;
    game->explanationScrollTarget = 0.0f;
}

static int SessionCorrectCount(const Game *game)
{
    int correct = 0;
    for (int i = 0; i < game->session.questionCount; ++i) {
        const Question *question = &game->session.questions[i];
        if (game->session.userAnswers[game->session.answerOffset + i] ==
            question->correctIndex) {
            ++correct;
        }
    }
    return correct;
}

static int SessionWrongCount(const Game *game)
{
    int wrong = 0;
    for (int i = 0; i < game->session.questionCount; ++i) {
        const Question *question = &game->session.questions[i];
        int answer = game->session.userAnswers[game->session.answerOffset + i];
        if (answer >= 0 && answer != question->correctIndex) {
            ++wrong;
        }
    }
    return wrong;
}

static int SessionEmptyCount(const Game *game)
{
    int empty = 0;
    for (int i = 0; i < game->session.questionCount; ++i) {
        if (game->session.userAnswers[game->session.answerOffset + i] < 0) {
            ++empty;
        }
    }
    return empty;
}

static Texture2D ArisuTextureFor(const Game *game, bool correct, bool answered)
{
    if (!answered) {
        return game->arisuExplain;
    }
    return correct ? game->arisuSmug : game->arisuCritical;
}

void UpdateScoreSummary(Game *game)
{
    if (UI_IsClicked(SummaryReviewRect()) || IsKeyPressed(KEY_ENTER)) {
        game->session.currentQuestion = 0;
        game->reviewScroll = 0.0f;
        game->reviewScrollTarget = 0.0f;
        game->explanationScroll = 0.0f;
        game->explanationScrollTarget = 0.0f;
        GameChangeState(game, STATE_REVIEW_PHASE);
        return;
    }

    if (UI_IsClicked(SummaryBackRect()) || IsKeyPressed(KEY_ESCAPE)) {
        GameChangeState(game, STATE_SUBTEST_SELECT);
    }
}

void DrawScoreSummary(const Game *game)
{
    DrawRoomBackground(game);

    int subtest = game->session.currentSubtest;
    if (subtest < 0 || subtest >= SUBTEST_COUNT) {
        subtest = 0;
    }

    const DifficultyInfo *difficulty = GameCurrentDifficulty(game);
    int correct = SessionCorrectCount(game);
    int wrong = SessionWrongCount(game);
    int empty = SessionEmptyCount(game);
    int total = game->session.questionCount;

    char eyebrow[128];
    snprintf(eyebrow, sizeof(eyebrow), "RINGKASAN SKOR  //  %s",
             SUBTEST_CODES[subtest]);
    UI_TextSpaced(game->fonts.small, eyebrow, 24.0f, 16.0f, 11.0f, 2.2f, UI_CYAN);
    UI_Text(game->fonts.ui, "HASIL SUBTES", 24.0f, 34.0f, 26.0f, UI_TEXT);

    char subtitle[192];
    snprintf(subtitle, sizeof(subtitle), "%s  //  MODE %s  //  %d SOAL",
             game->session.playerName, difficulty->name, total);
    UI_Text(game->fonts.small, subtitle, 24.0f, 74.0f, 13.0f, UI_MUTED);

    Rectangle left = { 24.0f, 104.0f, 610.0f, 480.0f };
    Rectangle right = { 650.0f, 104.0f, 606.0f, 480.0f };
    UI_DrawPanel(left, (Color){ 10, 15, 26, 255 }, UI_BORDER);
    UI_DrawPanel(right, (Color){ 10, 15, 26, 255 }, UI_BORDER);

    UI_TextSpaced(game->fonts.small, "SKOR AKHIR", left.x + 24.0f, left.y + 20.0f,
                  10.0f, 1.8f, UI_MUTED);

    char score[32];
    snprintf(score, sizeof(score), "%d", game->session.finalScore);
    UI_Text(game->fonts.title, score, left.x + 24.0f, left.y + 44.0f, 72.0f,
            difficulty->accent);

    Vector2 scoreSize = MeasureTextEx(game->fonts.title, score, 72.0f, 1.0f);
    UI_Text(game->fonts.small, "dari 1000",
            left.x + 36.0f + scoreSize.x, left.y + 92.0f, 13.0f, UI_MUTED);

    char accuracy[64];
    snprintf(accuracy, sizeof(accuracy), "AKURASI %d%%  //  %d BENAR DARI %d SOAL",
             game->session.rawIRTScore, correct, total);
    UI_Text(game->fonts.small, accuracy, left.x + 24.0f, left.y + 136.0f,
            13.0f, UI_TEXT);

    Rectangle barBg = { left.x + 24.0f, left.y + 160.0f, left.width - 48.0f, 8.0f };
    DrawRectangleRounded(barBg, 0.5f, 6, Fade(UI_BORDER, 0.55f));
    float fraction = total > 0 ? (float)correct / (float)total : 0.0f;
    Rectangle bar = { barBg.x, barBg.y, barBg.width * fraction, barBg.height };
    if (bar.width > 0.5f) {
        DrawRectangleRounded(bar, 0.5f, 6, difficulty->accent);
    }

    float statWidth = (left.width - 48.0f - 24.0f) / 3.0f;
    const char *labels[3] = { "BENAR", "SALAH", "KOSONG" };
    int values[3] = { correct, wrong, empty };
    Color colors[3] = { UI_SUCCESS, UI_ACCENT, UI_MUTED };
    for (int i = 0; i < 3; ++i) {
        Rectangle box = { left.x + 24.0f + (float)i * (statWidth + 12.0f),
                          left.y + 202.0f, statWidth, 92.0f };
        DrawRectangleRounded(box, 0.12f, 10, (Color){ 14, 20, 33, 255 });
        DrawRectangleRoundedLinesEx(box, 0.12f, 10, 1.0f, Fade(colors[i], 0.45f));
        UI_TextSpaced(game->fonts.small, labels[i], box.x + 14.0f, box.y + 14.0f,
                      10.0f, 1.6f, UI_MUTED);
        char value[16];
        snprintf(value, sizeof(value), "%d", values[i]);
        UI_Text(game->fonts.ui, value, box.x + 14.0f, box.y + 38.0f, 34.0f, colors[i]);
    }

    char modeLine[128];
    snprintf(modeLine, sizeof(modeLine), "MODE %s  //  PENGALI SKOR %.2fx",
             difficulty->name, difficulty->scoreMultiplier);
    UI_Text(game->fonts.small, modeLine, left.x + 24.0f, left.y + 330.0f,
            13.0f, difficulty->accent);

    UI_DrawTextWrapped(game->fonts.small,
                       "Skor akhir menggabungkan akurasi subtes dengan pengali tingkat kesulitan. "
                       "Buka pembahasan untuk melihat kunci dan strategi tiap soal.",
                       (Rectangle){ left.x + 24.0f, left.y + 362.0f,
                                    left.width - 48.0f, 80.0f },
                       13.0f, 0.25f, 19.0f, UI_MUTED);

    DrawPortrait(game->arisuExplain, (Rectangle){ right.x + right.width - 300.0f,
                                                  right.y + 18.0f, 280.0f, 430.0f });

    const char *speech = "Kerja bagus. Sekarang kita bedah satu per satu. "
                         "Fokus pada soal yang salah, bukan hanya skornya.";
    if (game->session.rawIRTScore >= 80) {
        speech = "Impresif. Kamu sudah menguasai sebagian besar subtes ini. "
                 "Pertahankan ketelitianmu pada soal jebakan.";
    } else if (game->session.rawIRTScore < 50) {
        speech = "Jangan berkecil hati. Kesalahan adalah data. "
                 "Mari kita telusuri pola jawabanmu dan perbaiki satu per satu.";
    }

    DrawSpeechBox(game, (Rectangle){ right.x + 20.0f, right.y + 300.0f,
                                     right.width - 40.0f, 170.0f },
                  "ARISU SAKAYANAGI", "PEMBAHASAN", speech,
                  difficulty->accent, 0.0f);

    UI_Button(game->fonts.ui, SummaryBackRect(), "KEMBALI",
              false, true, UI_BUTTON_SECONDARY, UI_CYAN);
    UI_Button(game->fonts.ui, SummaryReviewRect(), "LIHAT PEMBAHASAN",
              false, true, UI_BUTTON_PRIMARY, difficulty->accent);

    UI_TextCentered(game->fonts.small,
                    "ENTER: PEMBAHASAN   |   ESC: KEMBALI KE PILIHAN SUBTES",
                    (Rectangle){ 0.0f, 692.0f, SCREEN_WIDTH, 18.0f },
                    11.0f, 0.8f, Fade(UI_MUTED, 0.75f));
}

void UpdateReviewPhase(Game *game)
{
    if (game->session.questions == NULL || game->session.questionCount <= 0) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            GameChangeState(game, STATE_SCORE_SUMMARY);
        }
        return;
    }

    float dt = GetFrameTime();

    Rectangle passageView = ReviewPassageRect();
    Rectangle speechView = ReviewSpeechRect();
    Vector2 mouse = GetMousePosition();
    float wheel = GetMouseWheelMove();

    if (CheckCollisionPointRec(mouse, passageView) && wheel != 0.0f) {
        game->reviewScrollTarget -= wheel * 48.0f;
    }
    if (CheckCollisionPointRec(mouse, speechView) && wheel != 0.0f) {
        game->explanationScrollTarget -= wheel * 48.0f;
    }
    if (IsKeyPressed(KEY_PAGE_DOWN)) {
        game->explanationScrollTarget += 120.0f;
    }
    if (IsKeyPressed(KEY_PAGE_UP)) {
        game->explanationScrollTarget -= 120.0f;
    }

    ClampReviewScroll(game);
    SmoothReviewScroll(game, dt);

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        ReviewGoToQuestion(game, game->session.currentQuestion - 1);
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        ReviewGoToQuestion(game, game->session.currentQuestion + 1);
    }

    if (UI_IsClicked(ReviewPrevRect())) {
        ReviewGoToQuestion(game, game->session.currentQuestion - 1);
    }
    if (UI_IsClicked(ReviewNextRect())) {
        ReviewGoToQuestion(game, game->session.currentQuestion + 1);
    }
    if (UI_IsClicked(ReviewSummaryRect()) || IsKeyPressed(KEY_ESCAPE)) {
        GameChangeState(game, STATE_SCORE_SUMMARY);
        return;
    }
    if (UI_IsClicked(ReviewFinishRect())) {
        GameChangeState(game, STATE_SUBTEST_SELECT);
    }
}

void DrawReviewPhase(const Game *game)
{
    DrawRoomBackground(game);

    const Question *question = GameCurrentQuestion(game);
    if (question == NULL) {
        UI_TextCentered(game->fonts.ui, "Tidak ada pembahasan.",
                        (Rectangle){ 0.0f, 300.0f, SCREEN_WIDTH, 40.0f },
                        20.0f, 0.5f, UI_MUTED);
        return;
    }

    int subtest = game->session.currentSubtest;
    if (subtest < 0 || subtest >= SUBTEST_COUNT) {
        subtest = 0;
    }

    int answer = GameCurrentAnswer(game);
    bool answered = answer >= 0;
    bool correct = answered && answer == question->correctIndex;
    const DifficultyInfo *difficulty = GameCurrentDifficulty(game);

    char eyebrow[128];
    snprintf(eyebrow, sizeof(eyebrow), "PEMBAHASAN  //  %s", SUBTEST_CODES[subtest]);
    UI_TextSpaced(game->fonts.small, eyebrow, 24.0f, 14.0f, 11.0f, 2.2f, UI_CYAN);

    char title[96];
    snprintf(title, sizeof(title), "SOAL %02d / %02d",
             game->session.currentQuestion + 1, game->session.questionCount);
    UI_Text(game->fonts.ui, title, 24.0f, 30.0f, 26.0f, UI_TEXT);

    char subtitle[192];
    snprintf(subtitle, sizeof(subtitle), "%s  //  MODE %s",
             SUBTEST_NAMES[subtest], difficulty->name);
    UI_Text(game->fonts.small, subtitle, 24.0f, 58.0f, 12.0f, UI_MUTED);

    Rectangle left = ReviewLeftPanelRect();
    Rectangle right = ReviewRightPanelRect();
    UI_DrawPanel(left, (Color){ 10, 15, 26, 255 }, UI_BORDER);
    UI_DrawPanel(right, (Color){ 10, 15, 26, 255 }, UI_BORDER);

    UI_TextSpaced(game->fonts.small, "SOAL & OPSI", left.x + 18.0f, left.y + 16.0f,
                  10.0f, 1.8f, UI_CYAN);

    const char *status = "TIDAK DIJAWAB";
    Color statusColor = UI_MUTED;
    if (answered) {
        status = correct ? "BENAR" : "SALAH";
        statusColor = correct ? UI_SUCCESS : UI_ACCENT;
    }
    Vector2 statusSize = MeasureTextEx(game->fonts.small, status, 11.0f, 1.2f);
    UI_TextSpaced(game->fonts.small, status,
                  left.x + left.width - statusSize.x - 18.0f, left.y + 16.0f,
                  11.0f, 1.2f, statusColor);

    Rectangle passage = ReviewPassageRect();
    DrawRectangleRounded((Rectangle){ passage.x - 8.0f, passage.y - 6.0f,
                                     passage.width + 16.0f, passage.height + 12.0f },
                         0.06f, 8, (Color){ 13, 18, 30, 210 });
    UI_DrawTextWrappedScrolled(game->fonts.small, question->passage,
                               (Rectangle){ passage.x, passage.y,
                                            passage.width - 14.0f, passage.height },
                               15.0f, 0.25f, 21.0f, game->reviewScroll,
                               (Color){ 210, 217, 230, 255 });
    DrawScrollBar((Rectangle){ passage.x + passage.width - 4.0f, passage.y,
                               4.0f, passage.height },
                  game->reviewScroll, ReviewPassageMaxScroll(game));

    Rectangle prompt = ReviewPromptRect();
    UI_DrawTextWrapped(game->fonts.ui, question->prompt, prompt,
                       15.0f, 0.25f, 21.0f, UI_TEXT);

    for (int i = 0; i < question->optionCount; ++i) {
        Rectangle option = ReviewOptionRect(i);
        bool isCorrect = (i == question->correctIndex);
        bool isUser = (i == answer);

        Color fill = (Color){ 22, 28, 44, 235 };
        Color border = Fade(UI_BORDER, 0.8f);
        Color letterFill = Fade(UI_CYAN, 0.08f);
        Color letterText = UI_CYAN;
        Color textColor = (Color){ 205, 212, 227, 255 };

        if (isCorrect) {
            fill = Fade(UI_SUCCESS, 0.16f);
            border = Fade(UI_SUCCESS, 0.9f);
            letterFill = UI_SUCCESS;
            letterText = UI_BG_TOP;
            textColor = UI_TEXT;
        } else if (isUser) {
            fill = Fade(UI_ACCENT, 0.14f);
            border = Fade(UI_ACCENT, 0.85f);
            letterFill = UI_ACCENT;
            letterText = UI_TEXT;
        }

        DrawRectangleRounded(option, 0.18f, 8, fill);
        DrawRectangleRoundedLinesEx(option, 0.18f, 8, isCorrect ? 1.8f : 1.0f, border);

        Rectangle letter = { option.x + 10.0f, option.y + 8.0f, 28.0f, 28.0f };
        DrawRectangleRounded(letter, 0.25f, 8, letterFill);
        char letterLabel[2] = { (char)('A' + i), '\0' };
        UI_TextCentered(game->fonts.small, letterLabel, letter,
                        14.0f, 0.5f, letterText);

        UI_DrawTextWrapped(game->fonts.small, question->options[i],
                           (Rectangle){ option.x + 50.0f, option.y + 5.0f,
                                        option.width - 62.0f, option.height - 10.0f },
                           14.0f, 0.25f, 18.0f, textColor);
    }

    UI_TextSpaced(game->fonts.small, "ARISU SAKAYANAGI  //  PEMBAHASAN",
                  right.x + 20.0f, right.y + 16.0f, 10.0f, 1.8f, UI_CYAN);

    DrawPortrait(ArisuTextureFor(game, correct, answered), ReviewPortraitRect());

    char speechStatus[96];
    if (answered) {
        snprintf(speechStatus, sizeof(speechStatus), "KUNCI %c  //  JAWABANMU %c",
                 (char)('A' + question->correctIndex), (char)('A' + answer));
    } else {
        snprintf(speechStatus, sizeof(speechStatus), "KUNCI %c  //  BELUM DIJAWAB",
                 (char)('A' + question->correctIndex));
    }

    DrawSpeechBox(game, ReviewSpeechRect(), "ARISU", speechStatus,
                  question->explanation, statusColor,
                  game->explanationScroll);
    DrawScrollBar((Rectangle){ right.x + right.width - 24.0f,
                               right.y + 396.0f, 4.0f, 150.0f },
                  game->explanationScroll, ReviewExplanationMaxScroll(game));

    UI_Button(game->fonts.ui, ReviewPrevRect(), "SEBELUMNYA",
              false, game->session.currentQuestion > 0,
              UI_BUTTON_SECONDARY, UI_CYAN);
    UI_Button(game->fonts.ui, ReviewNextRect(), "BERIKUTNYA",
              false, game->session.currentQuestion + 1 < game->session.questionCount,
              UI_BUTTON_SECONDARY, UI_CYAN);
    UI_Button(game->fonts.ui, ReviewSummaryRect(), "RINGKASAN",
              false, true, UI_BUTTON_SECONDARY, UI_CYAN);
    UI_Button(game->fonts.ui, ReviewFinishRect(), "SELESAI",
              false, true, UI_BUTTON_PRIMARY, difficulty->accent);

    UI_TextCentered(game->fonts.small,
                    "A/D: SOAL   SCROLL: BACA   PAGEUP/PAGEDOWN: PEMBAHASAN   ESC: RINGKASAN",
                    (Rectangle){ 0.0f, 708.0f, SCREEN_WIDTH, 12.0f },
                    10.0f, 0.6f, Fade(UI_MUTED, 0.75f));
}

