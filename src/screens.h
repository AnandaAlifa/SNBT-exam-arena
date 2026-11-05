#ifndef SCREENS_H
#define SCREENS_H

#include "game.h"

void UpdateMenu(Game *game);
void DrawMenu(const Game *game);

void UpdateDifficulty(Game *game);
void DrawDifficulty(const Game *game);

void UpdateSubtestSelect(Game *game);
void DrawSubtestSelect(const Game *game);

void UpdatePlaceholder(Game *game);
void DrawPlaceholder(const Game *game);

#endif
