#include "raylib.h"
#include "game.h"

int main(void)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "UTBK Tactics: SNBT Exam Arena");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    Game game;
    GameInit(&game);

    while (!WindowShouldClose() && !game.exitRequested) {
        GameUpdate(&game);

        BeginDrawing();
        GameDraw(&game);
        EndDrawing();
    }

    GameUnload(&game);
    CloseWindow();

    return 0;
}
