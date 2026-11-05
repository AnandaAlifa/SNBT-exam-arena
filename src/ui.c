#include "ui.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

void UI_DrawBackground(float time)
{
    int width = GetScreenWidth();
    int height = GetScreenHeight();

    DrawRectangleGradientV(0, 0, width, height, UI_BG_TOP, UI_BG_BOTTOM);

    for (int x = -height; x < width; x += 72) {
        DrawLine(x, 0, x + height, height, Fade(UI_CYAN, 0.025f));
    }

    for (int y = 96; y < height; y += 96) {
        DrawLine(0, y, width, y, Fade(UI_CYAN, 0.018f));
    }

    Vector2 redGlow = {
        width * 0.86f + sinf(time * 0.22f) * 22.0f,
        height * 0.20f + cosf(time * 0.18f) * 18.0f
    };
    for (int i = 8; i >= 1; --i) {
        DrawCircleV(redGlow, 26.0f * (float)i, Fade(UI_ACCENT, 0.006f));
    }

    Vector2 cyanGlow = {
        width * 0.08f + cosf(time * 0.16f) * 18.0f,
        height * 0.88f + sinf(time * 0.20f) * 14.0f
    };
    for (int i = 7; i >= 1; --i) {
        DrawCircleV(cyanGlow, 24.0f * (float)i, Fade(UI_CYAN, 0.005f));
    }

    DrawCircleLines((int)(width * 0.93f), (int)(height * 0.72f), 190.0f,
                    Fade(UI_ACCENT, 0.08f));
    DrawCircleLines((int)(width * 0.93f), (int)(height * 0.72f), 150.0f,
                    Fade(UI_ACCENT, 0.05f));

    DrawRectangle(0, 0, width, 2, Fade(UI_ACCENT, 0.65f));
    DrawRectangle(0, height - 2, width, 2, Fade(UI_CYAN, 0.18f));
}

void UI_DrawPanel(Rectangle bounds, Color fill, Color border)
{
    Rectangle shadow = { bounds.x + 7.0f, bounds.y + 9.0f, bounds.width, bounds.height };
    DrawRectangleRounded(shadow, 0.025f, 12, (Color){ 0, 0, 0, 90 });
    DrawRectangleRounded(bounds, 0.025f, 12, fill);
    DrawRectangleRoundedLinesEx(bounds, 0.025f, 12, 1.4f, border);
}

void UI_Text(Font font, const char *text, float x, float y, float fontSize, Color color)
{
    DrawTextEx(font, text, (Vector2){ x, y }, fontSize, fontSize * 0.025f, color);
}

void UI_TextSpaced(Font font, const char *text, float x, float y, float fontSize,
                   float spacing, Color color)
{
    DrawTextEx(font, text, (Vector2){ x, y }, fontSize, spacing, color);
}

void UI_TextCentered(Font font, const char *text, Rectangle bounds, float fontSize,
                     float spacing, Color color)
{
    Vector2 textSize = MeasureTextEx(font, text, fontSize, spacing);
    Vector2 position = {
        bounds.x + (bounds.width - textSize.x) * 0.5f,
        bounds.y + (bounds.height - textSize.y) * 0.5f
    };
    DrawTextEx(font, text, position, fontSize, spacing, color);
}

void UI_DrawTextWrapped(Font font, const char *text, Rectangle bounds, float fontSize,
                        float spacing, float lineHeight, Color color)
{
    if (text == NULL || text[0] == '\0') {
        return;
    }

    char line[512] = { 0 };
    const char *cursor = text;
    float y = bounds.y;

    while (*cursor != '\0') {
        while (*cursor == ' ') {
            ++cursor;
        }
        if (*cursor == '\0') {
            break;
        }

        char word[128] = { 0 };
        int wordLength = 0;
        while (*cursor != '\0' && *cursor != ' ' && wordLength < (int)sizeof(word) - 1) {
            word[wordLength++] = *cursor++;
        }
        word[wordLength] = '\0';

        char candidate[640] = { 0 };
        if (line[0] != '\0') {
            snprintf(candidate, sizeof(candidate), "%s %s", line, word);
        } else {
            snprintf(candidate, sizeof(candidate), "%s", word);
        }

        Vector2 candidateSize = MeasureTextEx(font, candidate, fontSize, spacing);
        if (line[0] != '\0' && candidateSize.x > bounds.width) {
            DrawTextEx(font, line, (Vector2){ bounds.x, y }, fontSize, spacing, color);
            y += lineHeight;
            snprintf(line, sizeof(line), "%s", word);
        } else {
            snprintf(line, sizeof(line), "%s", candidate);
        }

        if (y > bounds.y + bounds.height) {
            return;
        }
    }

    if (line[0] != '\0') {
        DrawTextEx(font, line, (Vector2){ bounds.x, y }, fontSize, spacing, color);
    }
}

void UI_DrawSectionLabel(Font font, const char *text, float x, float y, Color accent)
{
    DrawRectangle((int)x, (int)y + 2, 4, 16, accent);
    UI_TextSpaced(font, text, x + 12.0f, y, 14.0f, 1.6f, UI_MUTED);
}

bool UI_IsHovered(Rectangle bounds)
{
    bool hovered = CheckCollisionPointRec(GetMousePosition(), bounds);
    if (hovered) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    }
    return hovered;
}

bool UI_IsClicked(Rectangle bounds)
{
    return UI_IsHovered(bounds) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void UI_Button(Font font, Rectangle bounds, const char *label, bool focused,
               bool enabled, UIButtonStyle style, Color accent)
{
    bool hovered = UI_IsHovered(bounds) && enabled;
    bool down = hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    Color fill = UI_PANEL_LIGHT;
    Color border = UI_BORDER;
    Color textColor = UI_TEXT;

    if (!enabled) {
        fill = (Color){ 24, 28, 38, 255 };
        border = (Color){ 45, 50, 64, 255 };
        textColor = (Color){ 88, 94, 110, 255 };
    } else if (style == UI_BUTTON_PRIMARY) {
        fill = accent;
        if (focused) {
            fill = ColorBrightness(accent, 0.16f);
        } else if (hovered) {
            fill = ColorBrightness(accent, 0.08f);
        }
        border = Fade(WHITE, focused ? 0.55f : 0.18f);
        textColor = (Color){ 11, 15, 24, 255 };
    } else if (style == UI_BUTTON_SECONDARY) {
        fill = focused ? (Color){ 39, 49, 71, 255 }
                       : (hovered ? (Color){ 33, 42, 62, 255 } : (Color){ 25, 33, 50, 255 });
        border = focused ? accent : (hovered ? Fade(accent, 0.65f) : UI_BORDER);
        textColor = focused ? UI_TEXT : UI_MUTED;
    } else {
        fill = hovered ? (Color){ 33, 23, 31, 255 } : Fade(UI_BG_TOP, 0.0f);
        border = focused ? Fade(accent, 0.85f) : (Color){ 62, 55, 68, 255 };
        textColor = focused ? (Color){ 245, 145, 158, 255 } : UI_MUTED;
    }

    if (down) {
        fill = ColorBrightness(fill, -0.08f);
    }

    Rectangle drawBounds = bounds;
    if (down) {
        drawBounds.y += 1.0f;
    }

    DrawRectangleRounded(drawBounds, 0.22f, 10, fill);
    DrawRectangleRoundedLinesEx(drawBounds, 0.22f, 10, focused ? 1.8f : 1.2f, border);

    if (focused && enabled) {
        Rectangle focusRing = {
            drawBounds.x - 3.0f,
            drawBounds.y - 3.0f,
            drawBounds.width + 6.0f,
            drawBounds.height + 6.0f
        };
        DrawRectangleRoundedLinesEx(focusRing, 0.22f, 10, 1.0f, Fade(accent, 0.35f));
    }

    UI_TextCentered(font, label, drawBounds, 19.0f, 1.0f, textColor);
}
