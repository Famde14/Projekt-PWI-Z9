#include "raylib.h"
#include "structs.h"
#include "functions.h"
#include "stdio.h"

#define MAX_SHIPS 10

int main(void)
{
    int screenWidth = 1280;
    int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "The Statki Game");
    SetTargetFPS(60);

    int game_phase = 0;


    int gridSize = 10; // Rozmiar planszy
    int cellSize = 50; // Rozmiar pojedynczej kratki (w pikselach)

    //załadowanie tekstur statków
    const char* ship1Files[] = {"textures/ship1.png", "textures/ship1.png", "textures/ship1.png", "textures/ship1.png"};
    const char* ship2Files[] = {"textures/ship2.png", "textures/ship2.png", "textures/ship2.png"};
    const char* ship3Files[] = {"textures/ship3.png", "textures/ship3.png"};
    const char* ship4Files[] = {"textures/ship4.png"};

    Image ship1Images[4];
    Texture2D ship1Textures[4];
    for (int i = 0; i < 4; i++) {
        ship1Images[i] = LoadImage(ship1Files[i]);
        ImageResize(&ship1Images[i], cellSize, cellSize); //zmniejsz do 1x1 kafelka
        ship1Textures[i] = LoadTextureFromImage(ship1Images[i]);
    }

    Image ship2Images[3];
    Texture2D ship2Textures[3];
    for (int i = 0; i < 3; i++) {
        ship2Images[i] = LoadImage(ship2Files[i]);
        ImageResize(&ship2Images[i], cellSize * 2, cellSize); //zmniejsz do 2x1 kafelka
        ship2Textures[i] = LoadTextureFromImage(ship2Images[i]);
    }

    Image ship3Images[2];
    Texture2D ship3Textures[2];
    for (int i = 0; i < 2; i++) {
        ship3Images[i] = LoadImage(ship3Files[i]);
        ImageResize(&ship3Images[i], cellSize * 3, cellSize); //zmniejsz do 3x1 kafelka
        ship3Textures[i] = LoadTextureFromImage(ship3Images[i]);
    }

    Image ship4Images[1];
    Texture2D ship4Textures[1];
    for (int i = 0; i < 1; i++) {
        ship4Images[i] = LoadImage(ship4Files[i]);
        ImageResize(&ship4Images[i], cellSize * 4, cellSize); //zmniejsz do 4x1 kafelka
        ship4Textures[i] = LoadTextureFromImage(ship4Images[i]);
    }

    struct ship ships[MAX_SHIPS];
    int shipIndex = 0;

    int startX = screenWidth * 1 / 4 - (gridSize * cellSize) / 2; //wyśrodkowanie statków
    int startY = (screenHeight - (gridSize * cellSize)) / 2;

    //statki 1-masztowe
    for (int i = 0; i < 4; i++) {
        int spacing = cellSize * 1 + cellSize;
        ships[shipIndex++] = (struct ship){
            .pos = { startX + i * spacing, startY },
            .sprite = ship1Images[i],
            .texture = ship1Textures[i],
            .hitbox = { startX + i * spacing, startY, ship1Textures[i].width, ship1Textures[i].height },
            .isUpdating = false,
            .length = 1,
            .updateHitbox = UpdateHitbox,
            .updateShip = UpdateShip
        };
    }

    //statki 2-masztowe
    for (int i = 0; i < 3; i++) {
        int spacing = cellSize * 2 + cellSize;
        ships[shipIndex++] = (struct ship){
            .pos = { startX + i * spacing, startY + 2 * cellSize },
            .sprite = ship2Images[i],
            .texture = ship2Textures[i],
            .hitbox = { startX + i * spacing, startY + 2 * cellSize, ship2Textures[i].width, ship2Textures[i].height },
            .isUpdating = false,
            .length = 2,
            .updateHitbox = UpdateHitbox,
            .updateShip = UpdateShip
        };
    }

    //statki 3-masztowe
    for (int i = 0; i < 2; i++) {
        int spacing = cellSize * 3 + cellSize;
        ships[shipIndex++] = (struct ship){
            .pos = { startX + i * spacing, startY + 2 * (2 * cellSize) },
            .sprite = ship3Images[i],
            .texture = ship3Textures[i],
            .hitbox = { startX + i * spacing, startY + 2 * (2 * cellSize), ship3Textures[i].width, ship3Textures[i].height },
            .isUpdating = false,
            .length = 3,
            .updateHitbox = UpdateHitbox,
            .updateShip = UpdateShip
        };
    }

    //statki 4-masztowe
    int spacing = cellSize * 4 + cellSize;
    ships[shipIndex++] = (struct ship){
        .pos = { startX, startY + 3 * (2 * cellSize) },
        .sprite = ship4Images[0],
        .texture = ship4Textures[0],
        .hitbox = { startX, startY + 3 * (2 * cellSize), ship4Textures[0].width, ship4Textures[0].height },
        .isUpdating = false,
        .length = 4,
        .updateHitbox = UpdateHitbox,
        .updateShip = UpdateShip
    };

        bool isDragging = false;
    int gridStartX = screenWidth * 3 / 4 - (gridSize * cellSize) / 2;
    int gridStartY = (screenHeight - (gridSize * cellSize)) / 2;

    while (!WindowShouldClose())
    {
        //update ships
        for (int i = 0; i < MAX_SHIPS; i++) {
            ships[i].updateShip(&isDragging, &ships[i]);
            if (isDragging && ships[i].isUpdating) {
                //sprawdź czy statek jest w obrębie planszy
                if (ships[i].pos.x >= gridStartX && ships[i].pos.x <= gridStartX + gridSize * cellSize &&
                    ships[i].pos.y >= gridStartY && ships[i].pos.y <= gridStartY + gridSize * cellSize) {
                    SnapToGrid(&ships[i], gridStartX, gridStartY, cellSize);
                }
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            for (int i = 0; i < MAX_SHIPS; i++) {
                if (ships[i].isUpdating) {
                    // Check if the ship is within the grid boundaries
                    if (ships[i].pos.x >= gridStartX && ships[i].pos.x <= gridStartX + gridSize * cellSize &&
                        ships[i].pos.y >= gridStartY && ships[i].pos.y <= gridStartY + gridSize * cellSize) {
                        SnapToGrid(&ships[i], gridStartX, gridStartY, cellSize);
                    }
                    ships[i].isUpdating = false;
                    isDragging = false;
                }
            }
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        if (game_phase == 0) {
            // Rysowanie ekranu startowego
            DrawText("Welcome to Statki The Game!", screenWidth / 2 - MeasureText("Welcome to Statki The Game!", 40) / 2, screenHeight / 2 - 50, 40, DARKBLUE);
            DrawText("Press SPACE, to start", screenWidth / 2 - MeasureText("Press SPACE to start", 20) / 2, screenHeight / 2 + 20, 20, DARKGRAY);

            // Przejście do gry po wciśnięciu spacji
            if (IsKeyPressed(KEY_SPACE)) {
                game_phase = 1;
            }
        } else if (game_phase == 1) {

            DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, BLACK); // Pionowa linia

            DrawText("Press ESC to exit", 10, 10, 20, DARKGRAY);

            for (int i = 0; i < gridSize; i++) {
                char label[3];
                snprintf(label, sizeof(label), "%c", 'A' + i);
                DrawText(label, gridStartX + i * cellSize + cellSize / 2 - 5, gridStartY - 30, 20, BLACK);
                snprintf(label, sizeof(label), "%d", i + 1);
                DrawText(label, gridStartX - 30, gridStartY + i * cellSize + cellSize / 2 - 10, 20, BLACK);
            }

            for (int i = 0; i < gridSize; i++) {
                for (int j = 0; j < gridSize; j++) {
                    DrawRectangleLines(gridStartX + j * cellSize, gridStartY + i * cellSize, cellSize, cellSize, BLACK);
                }
            }

            //draw ships
            for (int i = 0; i < MAX_SHIPS; i++) {
                DrawTexture(ships[i].texture, (int)ships[i].pos.x, (int)ships[i].pos.y, WHITE);
            }
        }
        EndDrawing();
    }

    //unload textures
    for (int i = 0; i < 4; i++) {
        UnloadTexture(ship1Textures[i]);
    }
    for (int i = 0; i < 3; i++) {
        UnloadTexture(ship2Textures[i]);
    }
    for (int i = 0; i < 2; i++) {
        UnloadTexture(ship3Textures[i]);
    }
    for (int i = 0; i < 1; i++) {
        UnloadTexture(ship4Textures[i]);
    }

    CloseWindow();

    return 0;
}
