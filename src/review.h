#ifndef REVIEW_H
#define REVIEW_H

#include "game.h"

void UpdateScoreSummary(Game *game);
void DrawScoreSummary(const Game *game);
void UpdateReviewPhase(Game *game);
void DrawReviewPhase(const Game *game);

#endif
