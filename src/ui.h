#ifndef UI_H
#define UI_H

#include "raylib.h"
#include <stdbool.h>

#define UI_BG_TOP (Color){ 7, 10, 18, 255 }
#define UI_BG_BOTTOM (Color){ 18, 24, 39, 255 }
#define UI_PANEL (Color){ 20, 26, 42, 245 }
#define UI_PANEL_LIGHT (Color){ 30, 38, 58, 255 }
#define UI_BORDER (Color){ 54, 65, 90, 255 }
#define UI_TEXT (Color){ 232, 236, 245, 255 }
#define UI_MUTED (Color){ 139, 150, 172, 255 }
#define UI_ACCENT (Color){ 224, 74, 96, 255 }
#define UI_CYAN (Color){ 86, 204, 242, 255 }
#define UI_SUCCESS (Color){ 96, 211, 148, 255 }
#define UI_WARNING (Color){ 255, 176, 84, 255 }

typedef enum {
    UI_BUTTON_PRIMARY = 0,
    UI_BUTTON_SECONDARY,
    UI_BUTTON_GHOST
} UIButtonStyle;

void UI_DrawBackground(float time);
void UI_DrawPanel(Rectangle bounds, Color fill, Color border);

void UI_Text(Font font, const char *text, float x, float y, float fontSize, Color color);
void UI_TextSpaced(Font font, const char *text, float x, float y, float fontSize,
                   float spacing, Color color);
void UI_TextCentered(Font font, const char *text, Rectangle bounds, float fontSize,
                     float spacing, Color color);
void UI_DrawTextWrapped(Font font, const char *text, Rectangle bounds, float fontSize,
                        float spacing, float lineHeight, Color color);
void UI_DrawSectionLabel(Font font, const char *text, float x, float y, Color accent);

bool UI_IsHovered(Rectangle bounds);
bool UI_IsClicked(Rectangle bounds);
void UI_Button(Font font, Rectangle bounds, const char *label, bool focused,
               bool enabled, UIButtonStyle style, Color accent);

#endif
