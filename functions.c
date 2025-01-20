#include "raylib.h"
#include "structs.h"
#include "functions.h"
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>//dla usleep
#include <stddef.h>//dla definicji nulla 
#include <stdio.h>//do wywalenia ale to jak bedzie interfejs

Sound scr;//efekty dźwiękowe deklaruję jako zmienne globalne
int loadscr = 0;
Sound blind;
int loadblind = 0;
Sound shoted;
int loadshot = 0;

void UpdateHitbox(ship* s) {		//ustala pozycję i wymiary hitbox'u
    s->hitbox.x = s->pos.x;
    s->hitbox.y = s->pos.y;
    s->hitbox.width = s->texture.width;
    s->hitbox.height = s->texture.height;
}

void mouse_drag(int key, ship* s, Color color){	//ustala położenie obiektu po pozycji myszy
	if(IsMouseButtonDown(key)){
		s->pos.x = GetMouseX() - s->texture.width / 2;		//poprawka na pozycję myszy
		s->pos.y = GetMouseY() - s->texture.height / 2;
		ClearBackground(color);						//usunięcie poprzedniej kopii obiektu
	}
}

void rotate(int key, Image* sprite, Texture2D* texture){	//ustala rotację obiektu
	UnloadTexture(*texture);					//usuwa poprednią teksturę

	if(key=='E') ImageRotateCW(sprite);		//obrót zgodnie z ruchem wskazówek zegara
	else ImageRotateCCW(sprite);				//obrót przeciwnie z ruchem wskazówek zegara

	*texture = LoadTextureFromImage(*sprite);	//załaduj nową teksturę
}

void UpdateShip(bool* isDragging, ship* s)
{	//wywołuje mouse_drag i rotate

	if(*isDragging && !s->isUpdating) return; //jeśli jakiś statek jest już przeciągany, nie przeciągaj drugiego

	//przy kliknięciu prawego przycisku myszy na hitbox lub gdy przy przeciąganiu przytrzymywany jest prawy przycisk myszy
	if((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), s->hitbox)) || (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && s->isUpdating)){

		s->isUpdating = true;
		*isDragging = true;
	}
	else {
		s->isUpdating = false;
	}

	if(s->isUpdating)
	{
		mouse_drag(MOUSE_BUTTON_LEFT, s, SKYBLUE);
        s->updateHitbox(s);									//aktualizacja hitboxu

        if(IsKeyPressed('E')) {
			rotate('E', &s->sprite, &s->texture);
			s->kierunek=(s->kierunek+1)%4;
		}
        if(IsKeyPressed('Q')) {
			rotate('Q', &s->sprite, &s->texture);
			s->kierunek=!s->kierunek?3:s->kierunek-1;
		}
	}
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
	{
        s->isUpdating = false;
        *isDragging = false;
    }
}
void SnapToGrid(ship *s, int gridStartX, int gridStartY)
{
    s->pos.x = gridStartX + ((int)((s->pos.x - gridStartX + TILE_SIZE / 2) / TILE_SIZE)) * TILE_SIZE;
    s->pos.y = gridStartY + ((int)((s->pos.y - gridStartY + TILE_SIZE / 2) / TILE_SIZE)) * TILE_SIZE;

    // Ensure the ship does not go out of bounds
    if (s->pos.x < gridStartX) s->pos.x = gridStartX;
    if (s->pos.x > gridStartX + (BOARD_SIZE - 1) * TILE_SIZE) s->pos.x = gridStartX + (BOARD_SIZE - 1) * TILE_SIZE;
    if (s->pos.y < gridStartY) s->pos.y = gridStartY;
    if (s->pos.y > gridStartY + (BOARD_SIZE - 1) * TILE_SIZE) s->pos.y = gridStartY + (BOARD_SIZE - 1) * TILE_SIZE;

	s->updateHitbox(s);

	if (s->boardplace != NULL)
	{
		free(s->boardplace);
	}
	s->boardplace = (shiptile *)malloc(s->length * sizeof(shiptile));

	// Update boardplace array with grid coordinates
	for (int i = 0; i < s->length; i++)
	{
		// kierunek  to liczby z zakresu 0 - góra, 1 - prawo, 2 - dół, 3 - lewo
		if (s->kierunek == 0)
		{
			s->boardplace[i].cords.x = (s->pos.x - gridStartX) / TILE_SIZE;
			s->boardplace[i].cords.y = (s->pos.y - gridStartY) / TILE_SIZE + i;
		}
		else if (s->kierunek == 1)
		{
			s->boardplace[i].cords.x = (s->pos.x - gridStartX) / TILE_SIZE + i;
			s->boardplace[i].cords.y = (s->pos.y - gridStartY) / TILE_SIZE;
		}
		else if (s->kierunek == 2)
		{
			s->boardplace[i].cords.x = (s->pos.x - gridStartX) / TILE_SIZE;
			s->boardplace[i].cords.y = (s->pos.y - gridStartY) / TILE_SIZE + i;
		}
		else if(s->kierunek == 3)
		{
			s->boardplace[i].cords.x = (s->pos.x - gridStartX) / TILE_SIZE + i;
			s->boardplace[i].cords.y = (s->pos.y - gridStartY) / TILE_SIZE;
		}
		// s->boardplace[i].cords.x = (s->pos.x - gridStartX) / TILE_SIZE + i;
		// s->boardplace[i].cords.y = (s->pos.y - gridStartY) / TILE_SIZE;
		// s->boardplace[i].got_shot = false;
	}

	// do pomocniczego wypisywania
	//PrintShipPositions(s);
}

void CheckShipPlacement(ship *ships)
{
    int matrix[10][10] = {0};

    for (int i = 0; i < MAX_SHIPS; i++)
    {
        ships[i].invalidPlacement = false; // Reset invalidPlacement flag for all ships
    }

    for (int i = 0; i < MAX_SHIPS; i++)
    {
        for (int j = 0; j < ships[i].length; j++)
        {
            if(ships[i].isPlaced == true)
            {
                if(matrix[(int)ships[i].boardplace[j].cords.x][(int)ships[i].boardplace[j].cords.y] == 0)
                {
                    matrix[(int)ships[i].boardplace[j].cords.x][(int)ships[i].boardplace[j].cords.y] = i+1;
                }
                else
                {
                    ships[i].invalidPlacement = true;
                    ships[matrix[(int)ships[i].boardplace[j].cords.x][(int)ships[i].boardplace[j].cords.y] - 1].invalidPlacement = true;
                }
            }
        }
    }
    
    
    for (int y = 0; y <= 9; y++)
    {
        for (int x = 0; x <= 9; x++)
        {
            if (matrix[x][y] != 0)
            {
                // Horizontal/vertical checks
                if (x > 0 && matrix[x - 1][y] != 0 && matrix[x - 1][y] != matrix[x][y])
                {
                    ships[matrix[x][y] - 1].invalidPlacement = true;
                    ships[matrix[x - 1][y] - 1].invalidPlacement = true;
                }
                if (x < 9 && matrix[x + 1][y] != 0 && matrix[x + 1][y] != matrix[x][y])
                {
                    ships[matrix[x][y] - 1].invalidPlacement = true;
                    ships[matrix[x + 1][y] - 1].invalidPlacement = true;
                }
                if (y > 0 && matrix[x][y - 1] != 0 && matrix[x][y - 1] != matrix[x][y])
                {
                    ships[matrix[x][y] - 1].invalidPlacement = true;
                    ships[matrix[x][y - 1] - 1].invalidPlacement = true;
                }
                if (y < 9 && matrix[x][y + 1] != 0 && matrix[x][y + 1] != matrix[x][y])
                {
                    ships[matrix[x][y] - 1].invalidPlacement = true;
                    ships[matrix[x][y + 1] - 1].invalidPlacement = true;
                }

                // Diagonal checks (mark invalid if IDs are different)
                if (x > 0 && y > 0 && matrix[x - 1][y - 1] != 0 && matrix[x - 1][y - 1] != matrix[x][y])
                {
                    ships[matrix[x][y] - 1].invalidPlacement = true;
                    ships[matrix[x - 1][y - 1] - 1].invalidPlacement = true;
                }
                if (x < 9 && y > 0 && matrix[x + 1][y - 1] != 0 && matrix[x + 1][y - 1] != matrix[x][y])
                {
                    ships[matrix[x][y] - 1].invalidPlacement = true;
                    ships[matrix[x + 1][y - 1] - 1].invalidPlacement = true;
                }
                if (x > 0 && y < 9 && matrix[x - 1][y + 1] != 0 && matrix[x - 1][y + 1] != matrix[x][y])
                {
                    ships[matrix[x][y] - 1].invalidPlacement = true;
                    ships[matrix[x - 1][y + 1] - 1].invalidPlacement = true;
                }
                if (x < 9 && y < 9 && matrix[x + 1][y + 1] != 0 && matrix[x + 1][y + 1] != matrix[x][y])
                {
                    ships[matrix[x][y] - 1].invalidPlacement = true;
                    ships[matrix[x + 1][y + 1] - 1].invalidPlacement = true;
                }
            }
        }
    }

    // clear terminal
    /* system("cls");
    for (int y = 0; y <= 9; y++)
    {
        for (int x = 0; x <= 9; x++)
        {
            printf("%2d ", matrix[x][y]);
        }
        printf("\n");
    } */
}

void PrintShipPositions(ship *s)
{
	printf("Ship positions (grid coordinates):\n");
	for (int i = 0; i < s->length; i++)
	{
		printf("Kierunek: %d Tile %d: (%.0f, %.0f)\n", s->kierunek, i, s->boardplace[i].cords.x, s->boardplace[i].cords.y);
	}
}

GameData* GameSet( GameState gameState, PauseMenu* pauseMenu)
{
    GameState mainGameState = gameState;

    Texture2D background = LoadTexture("textures/ustawianie_z_siatka.png");
    Texture2D startbattle = LoadTexture("textures/rozpocznij_bitwe.png");
    //DrawTexture(background, 0, 0, WHITE);
    //DrawTexture(startbattle, 0, 0, WHITE);
    int gridSize = 10; // Rozmiar planszy
    // int TILE_SIZE = 50; // Rozmiar pojedynczej kratki (w pikselach)

    // załadowanie tekstur statków
    const char *ship1Files[] = {"textures/1x1.png", "textures/1x1.png", "textures/1x1.png", "textures/1x1.png"};
    const char *ship2Files[] = {"textures/2x1.png", "textures/2x1.png", "textures/2x1.png"};
    const char *ship3Files[] = {"textures/3x1.png", "textures/3x1.png"};
    const char *ship4Files[] = {"textures/4x1.png"};

    Texture2D startBattleTexture = LoadTexture("textures/rozpocznij_bitwe.png");
    Texture2D randomShipGenTexture = LoadTexture("textures/ustaw_losowo.png");

    Image ship1Images[4];
    Texture2D ship1Textures[4];
    for (int i = 0; i < 4; i++)
    {
        ship1Images[i] = LoadImage(ship1Files[i]);
        ImageResize(&ship1Images[i], TILE_SIZE, TILE_SIZE); // zmniejsz do 1x1 kafelka
        ship1Textures[i] = LoadTextureFromImage(ship1Images[i]);
    }

    Image ship2Images[3];
    Texture2D ship2Textures[3];
    for (int i = 0; i < 3; i++)
    {
        ship2Images[i] = LoadImage(ship2Files[i]);
        ImageResize(&ship2Images[i], TILE_SIZE * 2, TILE_SIZE); // zmniejsz do 2x1 kafelka
        ship2Textures[i] = LoadTextureFromImage(ship2Images[i]);
    }

    Image ship3Images[2];
    Texture2D ship3Textures[2];
    for (int i = 0; i < 2; i++)
    {
        ship3Images[i] = LoadImage(ship3Files[i]);
        ImageResize(&ship3Images[i], TILE_SIZE * 3, TILE_SIZE); // zmniejsz do 3x1 kafelka
        ship3Textures[i] = LoadTextureFromImage(ship3Images[i]);
    }

    Image ship4Images[1];
    Texture2D ship4Textures[1];
    for (int i = 0; i < 1; i++)
    {
        ship4Images[i] = LoadImage(ship4Files[i]);
        ImageResize(&ship4Images[i], TILE_SIZE * 4, TILE_SIZE); // zmniejsz do 4x1 kafelka
        ship4Textures[i] = LoadTextureFromImage(ship4Images[i]);
    }

    // inicjalizacja tablicy gracza
    board *playerBoard = initboard();

    // incjalizacja statków
    ship *playerShips = malloc(MAX_SHIPS * sizeof(ship));

    int startX = SCREENWIDTH * 1 / 4 - (gridSize * TILE_SIZE) / 2; // wyśrodkowanie statków
    int startY = (SCREENHEIGHT - (gridSize * TILE_SIZE)) / 2;
    int gridStartX = SCREENWIDTH * 3 / 4 - (gridSize * TILE_SIZE) / 2;
    int gridStartY = (SCREENHEIGHT - (gridSize * TILE_SIZE)) / 2;
    int shipIndex = 0;

    for (int i = 0; i < 4; i++)
    {
        int spacing = TILE_SIZE * 1 + TILE_SIZE;
        playerShips[shipIndex] = (ship){
            .pos = {startX + i * spacing, startY},
            .sprite = ship1Images[i],
            .texture = ship1Textures[i],
            .hitbox = {startX + i * spacing, startY, ship1Textures[i].width, ship1Textures[i].height},
            .isUpdating = false,
            .isPlaced = false,
            .length = 1,
            .kierunek = 1,
            .updateHitbox = UpdateHitbox,
            .updateShip = UpdateShip,
            .boardplace = malloc(1 * sizeof(shiptile))};
        shipIndex++;
    }
    for (int i = 0; i < 3; i++)
    {
        int spacing = TILE_SIZE * 2 + TILE_SIZE;
        playerShips[shipIndex] = (ship){
            .pos = {startX + i * spacing, startY + TILE_SIZE * 2},
            .sprite = ship2Images[i],
            .texture = ship2Textures[i],
            .hitbox = {startX + i * spacing, startY + TILE_SIZE * 2, ship2Textures[i].width, ship2Textures[i].height},
            .isUpdating = false,
            .isPlaced = false,
            .length = 2,
            .kierunek = 1,
            .updateHitbox = UpdateHitbox,
            .updateShip = UpdateShip,
            .boardplace = malloc(2 * sizeof(shiptile))};
        shipIndex++;
    }
    for (int i = 0; i < 2; i++)
    {
        int spacing = TILE_SIZE * 3 + TILE_SIZE;
        playerShips[shipIndex] = (ship){
            .pos = {startX + i * spacing, startY + TILE_SIZE * 4},
            .sprite = ship3Images[i],
            .texture = ship3Textures[i],
            .hitbox = {startX + i * spacing, startY + TILE_SIZE * 4, ship3Textures[i].width, ship3Textures[i].height},
            .isUpdating = false,
            .isPlaced = false,
            .length = 3,
            .kierunek = 1,
            .updateHitbox = UpdateHitbox,
            .updateShip = UpdateShip,
            .boardplace = malloc(3 * sizeof(shiptile))};
        shipIndex++;
    }
    playerShips[shipIndex] = (ship){
        .pos = {startX, startY + TILE_SIZE * 6},
        .sprite = ship4Images[0],
        .texture = ship4Textures[0],
        .hitbox = {startX, startY + TILE_SIZE * 6, ship4Textures[0].width, ship4Textures[0].height},
        .isUpdating = false,
        .isPlaced = false,
        .length = 4,
        .kierunek = 1,
        .updateHitbox = UpdateHitbox,
        .updateShip = UpdateShip,
        .boardplace = malloc(4 * sizeof(shiptile))};
    shipIndex++;

    bool isDragging = false;

    Music calm = LoadMusicStream("music/The_calm_before_the_storm.ogg");
    calm.looping = true;

    SetExitKey(0);
    PlayMusicStream(calm);
    while (1)
    {
        SetMusicVolume(calm, 0.75f*pauseMenu->all_sound.val * pauseMenu->music.val);
        UpdateMusicStream(calm);
        if (WindowShouldClose())
        {
            CloseWindow();
            FreeSounds();
            // Do dodania zwalnianie pamięci
            exit(0);
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            if (gameState != GAME_PAUSED) {
                ReloadGeneralMenu(pauseMenu);
                UpdatePauseMenu(pauseMenu);
                gameState = GAME_PAUSED;
            } else {
                if(pauseMenu->isGeneral){
                    UnloadGeneralMenu(pauseMenu);
                    gameState = mainGameState;
                } else{
                    UnloadSoundMenu(pauseMenu);
                    ReloadGeneralMenu(pauseMenu);
                    pauseMenu->isGeneral = true;
                }
            }
        }
        if(gameState == GAME_PAUSED){
            BeginDrawing();
            UpdatePauseMenu(pauseMenu);
            EndDrawing();
            if (!pauseMenu->isActive) {
                   gameState = mainGameState;
            } else if (pauseMenu->toMainMenu){
                break;
            }
        }
        else{
            // Update ships
            for (int i = 0; i < MAX_SHIPS; i++)
            {
                // Sprawdzenie czy statek nie wyleciał poza okno (update uwzględniający kilka statków)
                if (playerShips[i].pos.x < 0 ||
                    (playerShips[i].kierunek == 1 || playerShips[i].kierunek == 3 ? playerShips[i].pos.x + playerShips[i].length * TILE_SIZE :
                    playerShips[i].pos.x + TILE_SIZE) > SCREENWIDTH ||
                    playerShips[i].pos.y < 0 ||
                    (playerShips[i].kierunek == 0 || playerShips[i].kierunek == 2 ? playerShips[i].pos.y + playerShips[i].length * TILE_SIZE :
                    playerShips[i].pos.y + TILE_SIZE) > SCREENHEIGHT)
                {
                    playerShips[i].pos.x = SCREENWIDTH / 4 - playerShips[i].texture.width / 2;
                    playerShips[i].pos.y = SCREENHEIGHT / 2 - playerShips[i].texture.height / 2;
                    playerShips[i].updateHitbox(&playerShips[i]);
                }

                playerShips[i].updateShip(&isDragging, &playerShips[i]);
                if (isDragging && playerShips[i].isUpdating)
                {

                    // Sprawdzenie czy cały statek mieści się na planszy
                    if (playerShips[i].kierunek == 0 || playerShips[i].kierunek == 2) // Kierunek pionowy
                    {
                        if (playerShips[i].pos.x >= gridStartX && playerShips[i].pos.x <= gridStartX + gridSize * TILE_SIZE &&
                            playerShips[i].pos.y >= gridStartY && playerShips[i].pos.y + (int)playerShips[i].length * (TILE_SIZE - 3) <= gridStartY + gridSize * TILE_SIZE)
                        {
                            // Jeśli tak, to ustawiamy statek na planszy, wyrównujemy do kratki i sprawdzamy kolizje
                            playerShips[i].isPlaced = true;
                            SnapToGrid(&playerShips[i], gridStartX, gridStartY);
                            CheckShipPlacement(playerShips);
                        }
                        else
                        {
                            playerShips[i].isPlaced = false;
                        }
                    }
                    else // Kierunek poziomy
                    {
                        if (playerShips[i].pos.x >= gridStartX && playerShips[i].pos.x + (int)playerShips[i].length * (TILE_SIZE - 3) <= gridStartX + gridSize * TILE_SIZE &&
                            playerShips[i].pos.y >= gridStartY && playerShips[i].pos.y <= gridStartY + gridSize * TILE_SIZE)
                        {
                            // Jeśli tak, to ustawiamy statek na planszy, wyrównujemy do kratki i sprawdzamy kolizje
                            playerShips[i].isPlaced = true;
                            SnapToGrid(&playerShips[i], gridStartX, gridStartY);
                            CheckShipPlacement(playerShips);
                        }
                        else
                        {
                            playerShips[i].isPlaced = false;
                        }
                    }
                }
            }

            BeginDrawing();

            DrawTexture(background, 0, 0, WHITE);
            
            Rectangle StartBattleButton = {SCREENWIDTH - 260, SCREENHEIGHT - 100, 220, 50};
            Rectangle RandomShipGenButton = {SCREENWIDTH - 560, SCREENHEIGHT - 100, 220, 50};

            Rectangle startBattleDrawRect = StartBattleButton;
            Rectangle randomShipGenDrawRect = RandomShipGenButton;

            // Sprawdź, czy myszka znajduje się nad przyciskiem
            Vector2 mousePos = GetMousePosition();

            if (CheckCollisionPointRec(mousePos, StartBattleButton)) {
                startBattleDrawRect.x -= StartBattleButton.width * 0.05f;   
                startBattleDrawRect.y -= StartBattleButton.height * 0.05f;  
                startBattleDrawRect.width = StartBattleButton.width * 1.1f; 
                startBattleDrawRect.height = StartBattleButton.height * 1.1f; 
            }

            if (CheckCollisionPointRec(mousePos, RandomShipGenButton)) {
                randomShipGenDrawRect.x -= RandomShipGenButton.width * 0.05f;
                randomShipGenDrawRect.y -= RandomShipGenButton.height * 0.05f;
                randomShipGenDrawRect.width = RandomShipGenButton.width * 1.1f;
                randomShipGenDrawRect.height = RandomShipGenButton.height * 1.1f;
            }
            DrawTexturePro(startBattleTexture,(Rectangle){0, 0, startBattleTexture.width, startBattleTexture.height}, startBattleDrawRect, (Vector2){0, 0}, 0.0f,  WHITE);
            DrawTexturePro(randomShipGenTexture,(Rectangle){0, 0, randomShipGenTexture.width, randomShipGenTexture.height},randomShipGenDrawRect,(Vector2){0, 0},0.0f, WHITE);

                if (CheckCollisionPointRec(GetMousePosition(), RandomShipGenButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    playerBoard = init_ai_ships();
                    printboard(playerBoard);
                    for (int alpha = 0; alpha <= 255; alpha += 5)
                    {
                        BeginDrawing();
                        DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, (Color){0, 0, 0, alpha});
                        //napis nad planszą
                        if(mainGameState == GAME_START)
                            DrawText("Twoja plansza", gridStartX + (gridSize * TILE_SIZE) / 2 - MeasureText("Twoja plansza", 20) / 2, gridStartY - 50, 20, BLACK);
                        else if(mainGameState == GAME_PREPARE1)
                            DrawText("Gracz 1", gridStartX + (gridSize * TILE_SIZE) / 2 - MeasureText("Gracz 1", 20) / 2, gridStartY - 50, 20, BLACK);
                        else if(mainGameState == GAME_PREPARE2)
                            DrawText("Gracz 2", gridStartX + (gridSize * TILE_SIZE) / 2 - MeasureText("Gracz 2", 20) / 2, gridStartY - 50, 20, BLACK);

                        for (int i = 0; i < gridSize; i++)
                        {
                            char label[3]; // Increased size to accommodate two-digit numbers
                            snprintf(label, sizeof(label), "%c", 'A' + i);
                            DrawText(label, gridStartX + i * TILE_SIZE + TILE_SIZE / 2 - 5, gridStartY - 30, 20, BLACK);
                            snprintf(label, sizeof(label), "%d", i + 1);
                            DrawText(label, gridStartX - 30, gridStartY + i * TILE_SIZE + TILE_SIZE / 2 - 10, 20, BLACK);
                        }

                        EndDrawing();
                        double startTime = GetTime();
                        while (GetTime() - startTime < 0.04)
                        {
                            SetMusicVolume(calm, 0.75f*pauseMenu->all_sound.val * pauseMenu->music.val);
                            UpdateMusicStream(calm);
                        }
                    }
                    break;
                }

                if (CheckCollisionPointRec(GetMousePosition(), StartBattleButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    bool allShipsPlaced = true;
                    for (int i = 0; i < MAX_SHIPS; i++)
                    {
                        if (playerShips[i].invalidPlacement == true)
                        {
                            // Jeśli statek jest źle ustawiony, to nie można rozpocząć bitwy. Wyświetlany jest komunikat o błędzie.
                            allShipsPlaced = false;
                            ClearBackground(RAYWHITE);
                            BeginDrawing();
                            DrawRectangle(SCREENWIDTH / 2, SCREENHEIGHT / 4, SCREENWIDTH / 2, SCREENHEIGHT / 2, RED);
                            DrawText("Statki sa zle ustawione!", SCREENWIDTH / 2 + MeasureText("Statki sa zle ustawione!", 30) / 2, SCREENHEIGHT / 2 - 15, 30, WHITE);
                            EndDrawing();
                            double startTime = GetTime();
                            while (GetTime() - startTime < 1.0)
                            {
                                SetMusicVolume(calm, 0.75f*pauseMenu->all_sound.val * pauseMenu->music.val);
                                UpdateMusicStream(calm);
                            }
                            break;
                        }

                        // Jeśli statek nie jest ustawiony, to nie można rozpocząć bitwy. Wyświetlany jest komunikat o błędzie.
                        if (playerShips[i].isPlaced == false)
                        {
                            allShipsPlaced = false;
                            ClearBackground(RAYWHITE);
                            BeginDrawing();
                            DrawRectangle(SCREENWIDTH / 2, SCREENHEIGHT / 4, SCREENWIDTH / 2, SCREENHEIGHT / 2, BLUE);
                            DrawText("Nie wszystkie statki sa ustawione!", 3 * SCREENWIDTH / 4 - MeasureText("Nie wszystkie statki sa ustawione!", 30) / 2, SCREENHEIGHT / 2 - 15, 30, WHITE);
                            EndDrawing();
                            // zamiast usleep użyje funkcji GetTime() z rayliba (jednak jest cos takiego)
                            double startTime = GetTime();
                            while (GetTime() - startTime < 1.0)
                            {
                                SetMusicVolume(calm, 0.75f*pauseMenu->all_sound.val * pauseMenu->music.val);
                                UpdateMusicStream(calm);
                            }
                            break;
                        }

                    }

                    if (allShipsPlaced)
                    {
                        for (int alpha = 0; alpha <= 255; alpha += 5)
                        {
                            BeginDrawing();
                            DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, (Color){0, 0, 0, alpha});
                            //napis nad planszą
                            if(mainGameState == GAME_START)
                                DrawText("Twoja plansza", gridStartX + (gridSize * TILE_SIZE) / 2 - MeasureText("Twoja plansza", 20) / 2, gridStartY - 50, 20, BLACK);
                            else if(mainGameState == GAME_PREPARE1)
                                DrawText("Gracz 1", gridStartX + (gridSize * TILE_SIZE) / 2 - MeasureText("Gracz 1", 20) / 2, gridStartY - 50, 20, BLACK);
                            else if(mainGameState == GAME_PREPARE2)
                                DrawText("Gracz 2", gridStartX + (gridSize * TILE_SIZE) / 2 - MeasureText("Gracz 2", 20) / 2, gridStartY - 50, 20, BLACK);

                            for (int i = 0; i < gridSize; i++)
                            {
                                char label[3]; // Increased size to accommodate two-digit numbers
                                snprintf(label, sizeof(label), "%c", 'A' + i);
                                DrawText(label, gridStartX + i * TILE_SIZE + TILE_SIZE / 2 - 5, gridStartY - 30, 20, BLACK);
                                snprintf(label, sizeof(label), "%d", i + 1);
                                DrawText(label, gridStartX - 30, gridStartY + i * TILE_SIZE + TILE_SIZE / 2 - 10, 20, BLACK);
                            }


                            // Draw ships
                            // for (int i = 0; i < MAX_SHIPS; i++) {
                            //    DrawTexture(playerShips[i].texture, (int)playerShips[i].pos.x, (int)playerShips[i].pos.y, WHITE);
                            //}
                            EndDrawing();
                            double startTime = GetTime();
                            while (GetTime() - startTime < 0.04)
                            {
                                SetMusicVolume(calm, 0.75f*pauseMenu->all_sound.val * pauseMenu->music.val);
                                UpdateMusicStream(calm);
                            }
                        }
                        DrawText("Zacznij bitwe!", StartBattleButton.x + 10, StartBattleButton.y + 10, 30, BLACK);
                        break;
                    }
                }
                //napis nad planszą
                if(mainGameState == GAME_START)
                    DrawText("Twoja plansza", gridStartX + (gridSize * TILE_SIZE) / 2 - MeasureText("Twoja plansza", 20) / 2, gridStartY - 50, 20, BLACK);
                else if(mainGameState == GAME_PREPARE1)
                    DrawText("Gracz 1", gridStartX + (gridSize * TILE_SIZE) / 2 - MeasureText("Gracz 1", 20) / 2, gridStartY - 50, 20, BLACK);
                else if(mainGameState == GAME_PREPARE2)
                    DrawText("Gracz 2", gridStartX + (gridSize * TILE_SIZE) / 2 - MeasureText("Gracz 2", 20) / 2, gridStartY - 50, 20, BLACK);

                for (int i = 0; i < gridSize; i++)
                {
                    char label[3]; // Increased size to accommodate two-digit numbers
                    snprintf(label, sizeof(label), "%c", 'A' + i);
                    DrawText(label, gridStartX + i * TILE_SIZE + TILE_SIZE / 2 - 5, gridStartY - 30, 20, BLACK);
                    snprintf(label, sizeof(label), "%d", i + 1);
                    DrawText(label, gridStartX - 30, gridStartY + i * TILE_SIZE + TILE_SIZE / 2 - 10, 20, BLACK);
                }

                // Draw ships
                for (int i = 0; i < MAX_SHIPS; i++)
                {
                    if (playerShips[i].invalidPlacement)
                        DrawTexture(playerShips[i].texture, (int)playerShips[i].pos.x, (int)playerShips[i].pos.y, RED); // Czerwony tint dla nieprawidłowo ustawionych statków
                    else
                        DrawTexture(playerShips[i].texture, (int)playerShips[i].pos.x, (int)playerShips[i].pos.y, WHITE); // Normalny tint dla statków ustawionych prawidłowo
                }
            EndDrawing();
        }
    }
    if (pauseMenu->toMainMenu){
        NewGame(pauseMenu);
        return NULL;
    }

    // Układanie statków na w zmiennej playerBoard
    for (int i = 0; i < MAX_SHIPS; i++)
    {
        int gridX = (playerShips[i].pos.x - gridStartX) / TILE_SIZE;
        int gridY = (playerShips[i].pos.y - gridStartY) / TILE_SIZE;

        if (gridX >= 0 && gridY >= 0)
        {
            int dl = playerShips[i].length;
            ship *playerS = initship(dl);
            playerS->kierunek = playerShips[i].kierunek;;
            placeStatek(playerBoard, playerS, (pair){gridX, gridY}, playerShips[i].kierunek);
        }
    }
	printf("Contents of playerBoard:\n");
    printboard(playerBoard);

    // Unload textures
    for (int i = 0; i < 4; i++) 
    {
        UnloadTexture(ship1Textures[i]);
    }
    for (int i = 0; i < 3; i++) 
    {
        UnloadTexture(ship2Textures[i]);
    }
    for (int i = 0; i < 2; i++) 
    {
        UnloadTexture(ship3Textures[i]);
    }
    for (int i = 0; i < 1; i++) 
    {
        UnloadTexture(ship4Textures[i]);
    }


    GameData* gameData;
    gameData = (GameData *)malloc(sizeof(GameData));
    gameData->playerBoard = playerBoard;
    gameData->playerShips = playerShips;
    gameData->playerShipCount = MAX_SHIPS;
    StopMusicStream(calm);
    return gameData;
}

ship* initship(int type)
{													//trzeba bedzie zaktualizowac funkcje tak aby aktualizowala polozenie,hitbox i sprite w interfejsie graficznym. 
													//funkcja spelnia absolutne minimum do testowania mechanik
	ship* statek =malloc(sizeof(ship));
	if(statek == NULL){
		return NULL;
	}
	statek->boardplace=malloc(type*sizeof(shiptile));
	if(statek->boardplace == NULL){
		free(statek);
		return NULL;
	}
	for (int i = 0; i < type; i++)
	{
		statek->boardplace[i].got_shot=0;
	}
    statek->kierunek=0;
	statek->type = type;
	return statek;
}

void delship(ship* statek)
{
	if(statek==NULL) return;
	if(statek ->boardplace != NULL){
		free(statek->boardplace);
	}
	free(statek);
}

board* initboard()
{
   	board *newBoard = (board*)malloc(sizeof(board));
	if(newBoard==NULL){
		return NULL;//w sumie warto sprawdzić, czy nie ma błędu alokacji pamięci
	}
   	for (int y = 0; y < BOARD_SIZE; y++)
	{
       	for (int x = 0; x < BOARD_SIZE; x++)
		{
           	newBoard->BOARD[x][y] = NULL;
           	newBoard->shots[x][y] = false;
       	}
   	}
    return newBoard;
}

void delboard(board* boardtab)
{
	if(boardtab!=NULL){free(boardtab);}//nie zwolnie statkow gdyz musialbym sledzic czy dany statek nie zostal zwolniony wczesniej. Normalnie to od tego bylyby smart pointery ale jako ze to c to bedzie to problem osoby inicjujacej statek
}

bool isLegal(board* player,pair tile)
{	
	if(tile.y>9||tile.y<0||tile.x>9||tile.x<0){
		//printf("OofB     ");
		return 0;
		}//jesli siega poza tabele return 0
	for (int i = tile.x-1; i <= tile.x+1; i++)
	{
		for (int k = tile.y-1; k <= tile.y+1; k++)
		{
			if (i>9||k>9||k<0||i<0||player->BOARD[i][k]==NULL)
			//napisalem tak bo bylo mi latwiej w glowie wymienic warunki, niech ktos(albo ja ) przepisze to tak, aby nie bytlo else'a
			//obecne warunki to - nie sasiaduje z innymi statkami
			{
				
			}
			else 
			{
				//printf("!null/etc");
				return 0;
            }
		}
	}
	//printf("legalne  ");
	return 1;
}

void placeStatek(board *boardtab, ship *curr_ship, pair begin, int direction) // 0-gora 1-prawo 2-dol 3-lewo
{
	printf("typ: %d kierunek:%d begin(%d, %d)\n", curr_ship->type, direction, (int)begin.x, (int)begin.y); // nie kladzie statku jestli jest on zle polozony(nie zwraca bledu)
	switch (direction)
	{
	case 0:
		for (int i = 0; i < (int)(curr_ship->type); i++)
		{
			pair tpair = {begin.x, (begin.y + i)};
			if (!isLegal(boardtab, tpair))
			{
				return;
			}
			else
			{
				break;
			}
		}
		for (int i = 0; i < (int)(curr_ship->type); i++)
		{
			shiptile temp = {{begin.x, (begin.y + i)}, 0};
			boardtab->BOARD[(unsigned int)begin.x][(unsigned int)begin.y + i] = curr_ship;
			curr_ship->boardplace[i] = temp;
		}

		break;
	case 1:
		for (int i = 0; i < (int)(curr_ship->type); i++)
		{
			pair tpair = {begin.x + i, (begin.y)};
			if (!isLegal(boardtab, tpair))
			{
				return;
			}
			else
			{
				break;
			}
		}
		for (int i = 0; i < (int)(curr_ship->type); i++)
		{
			shiptile temp = {{begin.x + i, (begin.y)}, 0};
			boardtab->BOARD[(unsigned int)begin.x + i][(unsigned int)begin.y] = curr_ship;
			curr_ship->boardplace[i] = temp;
		}

		break;
	case 2:
		for (int i = 0; i < (int)(curr_ship->type); i++)
		{
			pair tpair = {begin.x, (begin.y + i)};
			if (!isLegal(boardtab, tpair))
			{
				return;
			}
			else
			{
				break;
			}
		}
		for (int i = 0; i < (int)(curr_ship->type); i++)
		{
			shiptile temp = {{begin.x, (begin.y + i)},0};
			boardtab->BOARD[(unsigned int)begin.x][(unsigned int)begin.y + i] = curr_ship;
			curr_ship->boardplace[i] = temp;
		}

		break;
	case 3:
		for (int i = 0; i < (int)(curr_ship->type); i++)
		{
			pair tpair = {begin.x + i, (begin.y)};
			if (!isLegal(boardtab, tpair))
			{

				return;
			}
			else
			{
				break;
			}
		}
		for (int i = 0; i < (int)(curr_ship->type); i++)
		{
			shiptile temp = {{begin.x + i, (begin.y)},0};
			boardtab->BOARD[(unsigned int)begin.x + i][(unsigned int)begin.y] = curr_ship;
			curr_ship->boardplace[i] = temp;
		}

		break;
	default:
		break;
	}
}

void beingshot(ship* curr_ship,pair paira)
{
    //Sound scream = LoadSound("Soundeffects/screaming_sinking.wav");
	for (int i = 0; i < curr_ship->type; i++)
	{
		if((curr_ship->boardplace[i]).cords.x==paira.x&&(curr_ship->boardplace[i]).cords.y==paira.y)//nic nie sugeruje ale to byloby mniej brzydsze jakby uzyc klas
		{
            (curr_ship->boardplace[i]).got_shot=1;
            //PlaySound(scream);
            //UnloadSound(scream);
            //return;
        }
	}
    //UnloadSound(scream);
    shoted = LoadSound("Soundeffects/shoted.wav");
    SetSoundVolume(shoted, 0.5f);//trzeba by było zmodyfikować suwak, pod zmianę dynamiki dźwięku
    loadshot = 1;
    PlaySound(shoted);
    //UnloadSound(shoted);
}

void shoot(board *playerBoard, pair shot)
{
    int x = shot.x;
    int y = shot.y;
    playerBoard->shots[x][y] = true; //zapisuje strzał na planszy gracza
    if (playerBoard->BOARD[x][y] != NULL)
	{
        ship *curr_ship = playerBoard->BOARD[x][y];
		beingshot(curr_ship,shot);
        return;
    }
    blind = LoadSound("Soundeffects/blind.wav");
    SetSoundVolume(blind, 0.5f);
    loadblind = 1;
    //SetSoundVolume(blind, 1.0f);
    PlaySound(blind);
    //UnloadSound(blind);
}

void printboard(board* boardA)//funkcja drukuje tablice gracza. Funkcja raczej testowa
{
	for (int k = 0; k < 10; k++)
	{
		for (int i = 0 ;i <10 ; i++)
		{
			char ch;
			if (boardA->BOARD[i][k]==NULL)
			{
				printf(" 0");
				//printf("null");
				continue;
			}
			switch ((int)(boardA->BOARD[i][k])->type)
			{
				case 1: //1maszt
					if ((boardA->BOARD[i][k])->boardplace[0].got_shot)
					{
						ch='J';
					}
					else
					{
						ch='j';
					}
				break;
				case 2:	//2maszt
					for (int a = 0; a < 2; a++)
					{
						if(((boardA->BOARD[i][k])->boardplace[a].cords.x==i)&&((boardA->BOARD[i][k])->boardplace[a].cords.y==k)){
							if (((boardA->BOARD[i][k])->boardplace[a].got_shot))
							{
								ch = 'D';
							}
							else{
								ch='d';
							}
							break;
						}						
					}					
				break; 
				case 3: //3maszt
					for (int a = 0; a < 3; a++)
					{
						if(((boardA->BOARD[i][k])->boardplace[a].cords.x==i)&&((boardA->BOARD[i][k])->boardplace[a].cords.y==k)){
							if (((boardA->BOARD[i][k])->boardplace[a].got_shot))
							{
								ch = 'T';
							}
							else{
								ch='t';
							}
							break;
						}
						
					}
				break; 
				case 4: //4maszt
					for (int a = 0; a < 4; a++)
					{
						if(((boardA->BOARD[i][k])->boardplace[a].cords.x==i)&&((boardA->BOARD[i][k])->boardplace[a].cords.y==k)){
							if (((boardA->BOARD[i][k])->boardplace[a].got_shot))
							{
								ch = 'C';
							}
							else{
								ch='c';
							}
							break;
						}
						
					}
				break;			
			
				default:
				printf("ERROR");	
				break;
			}
			printf(" %c",ch);
		//printf("nie null");
		}
		printf("\n");
	}
}
    
array_cordinals* Get_array_cordinals(int offsetX, int offsetY) {
    array_cordinals* cordinal = (array_cordinals*)malloc(sizeof(array_cordinals));
    if (cordinal == NULL) return NULL;

    int x = GetMouseX();
    int y = GetMouseY();
	
    x -= offsetX;
    y -= offsetY;

    x = x / TILE_SIZE;
    y = y / TILE_SIZE;
	//printf("%i, %i\n", x, y);
    if (x < 0 || x > 9 || y < 0 || y > 9)//9 jest jak najbardziej dopuszczalne!
    {
        free(cordinal);
        return NULL;
    }

    cordinal->x = x;
    cordinal->y = y;
    return cordinal;
};

void ResetGame(board **playerBoard, board **enemyBoard, ship **playerShip, ship **enemyShip, GameState gameState, PauseMenu *pauseMenu) //basicowa funkcja resetujaca gre (pozniej trzeba wyrzucic stad playership i enemyship, zeby samo usuwalo - nikt nie bedzie tego recznie ustawial)
{
    //delboard(*playerBoard);
    //delboard(*enemyBoard);
    //delship(*playerShip);
    //delship(*enemyShip);
    if(gameState == GAME_PREPARE1)
    {   *enemyBoard = init_ai_ships();
        GameData* gameData = GameSet(GAME_START, pauseMenu);
        *playerBoard = gameData->playerBoard;
    }
    else if(gameState == GAME_PREPARE2)
    {
        GameData* gameData1 = GameSet(GAME_PREPARE1, pauseMenu);
        GameData* gameData2 = GameSet(GAME_PREPARE2, pauseMenu);
        *playerBoard = gameData1->playerBoard;
        *enemyBoard = gameData2->playerBoard;
    }
	/*reczne dodawanie statkow, pozniej tego nie bedzie, bo zacznie sie funkcja z ustawianiem przez uzytkownika*/
    *playerShip = NULL;
    *enemyShip = NULL;
/*
    pair playerStart = {2, 2};
    pair enemyStart = {4, 4};

    placeStatek(*playerBoard, *playerShip, playerStart, 2);
    placeStatek(*enemyBoard, *enemyShip, enemyStart, 3);*/
};

void DrawBoard(board *playerBoard, int offsetX, int offsetY, bool isEnemy) //funkcja rysujaca plansze
{
    static bool texturesLoaded = false;
    static Texture2D shipTextures[16];
    static Texture2D ship_brokenTextures[16];
    static Texture2D XTexture;

    if (!texturesLoaded) { //ładowanie tekstur (tak żeby się nie powtarzało bez sensu)
        // Load "X" texture
        Image X = LoadImage("textures/X.png");
        XTexture = LoadTextureFromImage(X);
        UnloadImage(X); // Zwolnij obraz po załadowaniu tekstury

        // Load ship textures
        Image shipImages[4];
        Image ship_brokenImages[4];

        shipImages[0] = LoadImage("textures/1x1.png");
        ship_brokenImages[0] = LoadImage("textures/1x1_zepsuta.png");
        for(int i=0; i<4; i++) //0,3 - struktury 1x1
        {
            shipTextures[i] = LoadTextureFromImage(shipImages[0]);
            ship_brokenTextures[i] = LoadTextureFromImage(ship_brokenImages[0]);
            ImageRotateCW(&shipImages[0]);
            ImageRotateCW(&ship_brokenImages[0]);
        }

        shipImages[1] = LoadImage("textures/2x1.png");
        ship_brokenImages[1] = LoadImage("textures/2x1_zepsuta.png");
        for(int i=0; i<4; i++) //4,7 - struktury 2x1
        {
            shipTextures[4+i] = LoadTextureFromImage(shipImages[1]);
            ship_brokenTextures[4+i] = LoadTextureFromImage(ship_brokenImages[1]);
            ImageRotateCW(&shipImages[1]);
            ImageRotateCW(&ship_brokenImages[1]);
            
        }

        shipImages[2] = LoadImage("textures/3x1.png");
        ship_brokenImages[2] = LoadImage("textures/3x1_zepsuta.png");
        for(int i=0; i<4; i++) 
        {
            shipTextures[8+i] = LoadTextureFromImage(shipImages[2]);
            ship_brokenTextures[8+i] = LoadTextureFromImage(ship_brokenImages[2]);
            ImageRotateCW(&shipImages[2]);
            ImageRotateCW(&ship_brokenImages[2]);
        }

        shipImages[3] = LoadImage("textures/4x1.png");
        ship_brokenImages[3] = LoadImage("textures/4x1_zepsuta.png");
        
        for(int i=0; i<4; i++) 
        {
            shipTextures[12+i] = LoadTextureFromImage(shipImages[3]);
            ship_brokenTextures[12+i] = LoadTextureFromImage(ship_brokenImages[3]);
            ImageRotateCW(&shipImages[3]);
            ImageRotateCW(&ship_brokenImages[3]);
            
        }
        // Mark textures as loaded
        texturesLoaded = true;
    }

    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            Rectangle tile = {offsetX + x * TILE_SIZE, offsetY + y * TILE_SIZE, TILE_SIZE, TILE_SIZE}; //deklaracja pola

            if (playerBoard->BOARD[x][y] == NULL) //jeśli nie ma statku, to rysuj szare linie, pole puste
            {
                DrawRectangleLines(tile.x, tile.y, tile.width, tile.height, GRAY);
                if (playerBoard->shots[x][y]) // Check if the tile has been shot
                {
                    DrawRectangle(tile.x, tile.y, tile.width, tile.height, LIGHTGRAY); // Color for missed shots
                }
                
            }
            else 
            {
                DrawRectangleLines(tile.x, tile.y, tile.width, tile.height, GRAY); // Border lines
                ship *currShip = playerBoard->BOARD[x][y];
                int shots = 0;
                for (int i = 0; i < currShip->type; i++)
                {
                    if (currShip->boardplace[i].got_shot) 
                    {
                        shots++;
                    }
                }

                if (shots == currShip->type) // Check if the ship is fully destroyed
                { //0 -> prawo, 1 -> dol, 2 -> lewo, 3 -> gora
                    DrawTexture(ship_brokenTextures[(currShip->type-1)*4+(currShip->kierunek==0?3:currShip->kierunek-1)], offsetX + currShip->boardplace[0].cords.x * TILE_SIZE, offsetY + currShip->boardplace[0].cords.y * TILE_SIZE, WHITE);
                }
                else
                {
                    if(!isEnemy)
                    {
                        DrawTexture(shipTextures[(currShip->type-1)*4+(currShip->kierunek==0?3:currShip->kierunek-1)], offsetX + currShip->boardplace[0].cords.x * TILE_SIZE, offsetY + currShip->boardplace[0].cords.y * TILE_SIZE, WHITE);
                    }
                    
                    for (int i = 0; i < currShip->type; i++)
                    {
                        if (currShip->boardplace[i].got_shot) 
                        {
                            DrawTexture(XTexture, offsetX + currShip->boardplace[i].cords.x * TILE_SIZE, offsetY + currShip->boardplace[i].cords.y * TILE_SIZE, WHITE);
                        }
                    }
                }
                
            }
        }
    }
};

pair AITurn(board *playerBoard) //losuje do skutku, dopóki nie trafi w puste pole (mogę później zoptymalizować losowanie, ale na razie wystarcza)
{
    while (true)
    {
        int x = GetRandomValue(0, BOARD_SIZE - 1);
        int y = GetRandomValue(0, BOARD_SIZE - 1);
        pair shot = {x, y};
        if (!playerBoard->shots[x][y])
        {
            shoot(playerBoard, shot);
            return shot;
        }
    }
};

bool CheckWinCondition(board *playerBoard) //czy wszystkie statki zostały zestrzelone
{
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            if (playerBoard->BOARD[x][y] != NULL)
            {
                ship *currShip = playerBoard->BOARD[x][y];
                for (int i = 0; i < currShip->type; i++)
                {
                    if (!currShip->boardplace[i].got_shot) return false; //fałsz, jeśli jakiś statek nie został zestrzelony
                }
            }
        }
    }
    return true; //prawda, jeśli wszystkie statki zostały zestrzelone
};

// Sprawdza, czy można umieścić statek w danym miejscu - naprawdę potrzebuję tej funkcji - Bartłomiej Powierża
int islegal(int x, int y, int tab[10][10], int type) {
    if (x + type > 10) return 0;

    for (int i = x - 1; i <= x + type; i++) {
        for (int j = y - 1; j <= y + 1; j++) {
            if (i >= 0 && i < 10 && j >= 0 && j < 10) {
                if (tab[j][i] != 0) return 0;  // Zajęte pole
            }
        }
    }

    return 1;  // Legalne miejsce
}

// Funkcja do losowego rozmieszczania statków
int random_ai_ships(int tab[10][10], int typeindex, int randomized[10]) {
    //printf("%i\n", typeindex);
    //for(int i = 0; i < 10; i++){
    //    printf("%i ", randomized[i]);
    //}
    //putchar('\n');
    if (typeindex > 10) return 0;  // Błąd danych
    if (typeindex == 10) return 1;  // Wszystkie statki rozmieszczone

    // Określenie rozmiaru statku
    int type;
    if (typeindex == 0) type = 4;
    else if (typeindex >= 1 && typeindex <= 2) type = 3;
    else if (typeindex >= 3 && typeindex <= 5) type = 2;
    else type = 1;  // Dla jednomasztowców

    int x, y;

    // Próba losowania współrzędnych
    for (int attempt = 0; attempt < 10; attempt++) {
        x = rand() % (11 - type);  // Losowanie współrzędnej X
        y = rand() % 10;           // Losowanie współrzędnej Y

        // Sprawdzanie, czy wiersz y jest już zajęty
        if (randomized[y] == -1) {
            for(int i = 0; i<10; i++){
                if(randomized[i] != -1) y = i;
            }
        }

        // Sprawdzamy, czy wylosowane miejsce jest legalne
        if (islegal(x, y, tab, type)) {
            // Umieszczamy statek
            for (int i = x; i < x + type; i++) {
                tab[y][i] = type;
            }
            randomized[y] = -1;  // Zajmujemy wiersz

            // Rekurencyjnie rozmieszczamy kolejne statki
            if (random_ai_ships(tab, ++typeindex, randomized)) {
                return 1;  // Sukces
            } else {
                // Cofamy zmiany, jeżeli rozmieszczenie statku się nie udało
                for (int i = x; i < x + type; i++) {
                    tab[y][i] = 0;
                }
                randomized[y] = y;  // Zwolnienie wiersza
            }
        }
    }

    // Jeśli nie udało się rozmieszczać statku po kilku próbach, próbujemy iteracyjnie
    for (int i = 0; i < 10; i++) {
        if (i == x) continue;  // Pomijamy poprzednio wylosowane miejsce
        if (islegal(i, y, tab, type)) {
            // Umieszczamy statek
            for (int j = i; j < i + type; j++) {
                tab[y][j] = type;
            }
            randomized[y] = -1;  // Zajmujemy wiersz

            // Rekurencyjnie rozmieszczamy kolejne statki
            if (random_ai_ships(tab, ++typeindex, randomized)) {
                return 1;  // Sukces
            } else {
                // Cofamy zmiany, jeżeli rozmieszczenie statku się nie udało
                for (int j = i; j < i + type; j++) {
                    tab[y][j] = 0;
                }
                randomized[y] = y;  // Zwolnienie wiersza
            }
        }
    }
    return 0;  // Nie udało się rozmieszczone statki
}

board* init_ai_ships(){
    srand(time(NULL));
    int tablica[10][10] = {0};
    int randomized[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    TRYAGAIN:
    random_ai_ships(tablica, 0, randomized);
    for(int i=0; i<10; i++){
        if(randomized[i]!=-1){
            for(int a=0; a<10; a++){
                randomized[a] = a;
                for(int b = 0; b<10; b++){
                    tablica[a][b] = 0;
                }
            }
            goto TRYAGAIN;
        }
    }

    board* k = initboard();
    if (k == NULL) {
        printf("Error: initboard() returned NULL.\n");
        return NULL;
    }
    for(int i = 0; i<10; i++){
        for(int j = 0; j< 10; j++){
            printf("%i", tablica[i][j]);
        }
        putchar('\n');
    }
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (tablica[j][i] != 0) {
                //printf("Creating ship type %d at (%d, %d).\n", tablica[i][j], i, j);
                ship* enemy_ship = initship(tablica[j][i]);
                enemy_ship->kierunek = 1;
                if (enemy_ship == NULL) {
                    printf("Error: initship() returned NULL for type %d.\n", tablica[j][i]);
                    continue;
                }
                //printf("Placing ship type %d at (%d, %d).\n", tablica[i][j], i, j);
                placeStatek(k, enemy_ship, (pair){i, j}, 3);
            }
        }
    }
    return k;
}

void scream(){
    scr = LoadSound("Soundeffects/screaming_sinking.wav");
    SetSoundVolume(scr, 0.5f);
    loadscr = 1;
    PlaySound(scr);
}

void FreeSounds(){
    if(loadblind){
        puts("Zwalniam_blind");
        UnloadSound(blind);
        loadblind = 0;
    }
    if(loadshot){
        puts("Zwalniam_shoted");
        UnloadSound(shoted);
        loadshot = 0;
    }
    if(loadscr){
        puts("Zwalniam_scr");
        loadscr = 0;
        UnloadSound(scr);
    }
}
void PlayGame(board *playerBoard, board *enemyBoard, ship *playerShip, ship *enemyShip, PauseMenu *pauseMenu) {
    SetExitKey(0);
    Music sos = LoadMusicStream("music/SOS_Signal.ogg");
    Texture2D playAgainTexture = LoadTexture("textures/zagraj_ponownie.png");
    Texture2D closeTexture = LoadTexture("textures/wyjdz.png");
    sos.looping = true;
    PlayMusicStream(sos);
    int playerOffsetX = (SCREENWIDTH * 1/3)-20 - (BOARD_SIZE * TILE_SIZE) / 2;
    int playerOffsetY = (SCREENHEIGHT - (BOARD_SIZE * TILE_SIZE)) / 2;
    int enemyOffsetX = (SCREENWIDTH * 2/3)+20 - (BOARD_SIZE * TILE_SIZE) / 2;
    int enemyOffsetY = (SCREENHEIGHT - (BOARD_SIZE * TILE_SIZE)) / 2;

    bool playerTurn = true;
    GameState gameState = GAME_RUNNING;
    char message[128] = "";

    for (int alpha = 255; alpha >= 0; alpha -= 5) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Twoja plansza", playerOffsetX, playerOffsetY - 30, 20, BLACK);
        DrawBoard(playerBoard, playerOffsetX, playerOffsetY, false);

        DrawText("Plansza przeciwnika", enemyOffsetX, enemyOffsetY - 30, 20, BLACK);
        DrawBoard(enemyBoard, enemyOffsetX, enemyOffsetY, true);

        DrawText(message, SCREENWIDTH / 2 - MeasureText(message, 20) / 2, SCREENHEIGHT - 50, 20, DARKGRAY);

        // Draw the fading overlay
        DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, (Color){0, 0, 0, alpha});

        EndDrawing();

        // Add a small delay to make the transition visible
        double startTime = GetTime();
        while (GetTime() - startTime < 0.04) 
        {
            SetMusicVolume(sos, 0.33f*pauseMenu->all_sound.val * pauseMenu->music.val);
            UpdateMusicStream(sos);
        }
    }

    while (!WindowShouldClose()) {
        SetMusicVolume(sos, 0.33f*pauseMenu->all_sound.val * pauseMenu->music.val);
        UpdateMusicStream(sos);
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (IsKeyPressed(KEY_ESCAPE)) {
            if (gameState != GAME_PAUSED) {
                ReloadGeneralMenu(pauseMenu);
                UpdatePauseMenu(pauseMenu);
                gameState = GAME_PAUSED;
            } else{
                if(pauseMenu->isGeneral){
                    UnloadGeneralMenu(pauseMenu);
                    gameState = GAME_RUNNING;
                } else{
                    UnloadSoundMenu(pauseMenu);
                    ReloadGeneralMenu(pauseMenu);
                    pauseMenu->isGeneral = true;
                }
            }
        }

        if (gameState == GAME_RUNNING) {

            DrawText("Twoja plansza", playerOffsetX, playerOffsetY - 30, 20, BLACK);
            DrawBoard(playerBoard, playerOffsetX, playerOffsetY, false);

            DrawText("Plansza przeciwnika", enemyOffsetX, enemyOffsetY - 30, 20, BLACK);
            DrawBoard(enemyBoard, enemyOffsetX, enemyOffsetY, true);

            DrawText(message, SCREENWIDTH / 2 - MeasureText(message, 20) / 2, SCREENHEIGHT - 50, 20, DARKGRAY);

            if (playerTurn) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    struct array_cordinals *cords = Get_array_cordinals(enemyOffsetX, enemyOffsetY);
                    if (cords == NULL) goto VALIDCLICK; //dobre obejście sytuacji braku kordu
                    int x = cords->x;
                    int y = cords->y;
                    free(cords);

                    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
                        pair shot = {x, y};
                        if (!enemyBoard->shots[x][y]) //jeśli pole puste lub niezestrzelone, to strzelaj
                        {
                            shoot(enemyBoard, shot);
                            snprintf(message, sizeof(message), "Gracz strzelił w (%d, %d)", x, y);
                            if (enemyBoard->BOARD[x][y] != NULL) {
                                ship *currShip = enemyBoard->BOARD[x][y];
                                bool sunk = true;
                                for (int i = 0; i < currShip->type; i++) {
                                    if (!currShip->boardplace[i].got_shot) {
                                        sunk = false;
                                        break;
                                    }
                                }
                                if (sunk) {
                                    scream();
                                    snprintf(message, sizeof(message), "Gracz zatopił statek!");
                                }
                            } else {
                                playerTurn = false; // Tylko jeśli gracz nie trafi, zmienia się tura
                            }
                            double startTime = GetTime();
                            while (GetTime() - startTime < 1.0) {
                                UpdateMusicStream(sos); // Utrzymuje działanie muzyki w tle
                                BeginDrawing();
                                ClearBackground(RAYWHITE);

                                DrawText("Twoja plansza", playerOffsetX, playerOffsetY - 30, 20, BLACK);
                                DrawBoard(playerBoard, playerOffsetX, playerOffsetY, false);

                                DrawText("Plansza przeciwnika", enemyOffsetX, enemyOffsetY - 30, 20, BLACK);
                                DrawBoard(enemyBoard, enemyOffsetX, enemyOffsetY, true);

                                DrawText(message, SCREENWIDTH / 2 - MeasureText(message, 20) / 2, SCREENHEIGHT - 50, 20, DARKGRAY);

                                EndDrawing();
                            }
                        } else {
                            snprintf(message, sizeof(message), "Strzelałeś już tutaj!");
                        }
                    }
                }
            } else {
                pair shot = AITurn(playerBoard);
                snprintf(message, sizeof(message), "Przeciwnik strzela w (%d, %d)", (int)shot.x + 1, (int)shot.y + 1);

                int shotX = (int)shot.x;
                int shotY = (int)shot.y;

                if (shotX >= 0 && shotX < BOARD_SIZE && shotY >= 0 && shotY < BOARD_SIZE) {
                    if (playerBoard->BOARD[shotX][shotY] != NULL) {
                        ship *currShip = playerBoard->BOARD[shotX][shotY];
                        bool sunk = true;
                        for (int i = 0; i < currShip->type; i++) {
                            if (!currShip->boardplace[i].got_shot) {
                                sunk = false;
                                break;
                            }
                        }
                        if (sunk) {
                            scream();
                            //tutaj wstawimy dźwięk zatapiania
                            snprintf(message, sizeof(message), "Przeciwnik zatopił Twój statek!");
                        }
                    } else {
                        playerTurn = true; // Tylko jeśli przeciwnik nie trafi, zmienia się tura
                    }
                }

                double startTime = GetTime();
                while (GetTime() - startTime < 1.0) {
                    UpdateMusicStream(sos); // Utrzymuje działanie muzyki w tle
                    BeginDrawing();
                    ClearBackground(RAYWHITE);

                    DrawText("Twoja plansza", playerOffsetX, playerOffsetY - 30, 20, BLACK);
                    DrawBoard(playerBoard, playerOffsetX, playerOffsetY, false);

                    DrawText("Plansza przeciwnika", enemyOffsetX, enemyOffsetY - 30, 20, BLACK);
                    DrawBoard(enemyBoard, enemyOffsetX, enemyOffsetY, true);

                    DrawText(message, SCREENWIDTH / 2 - MeasureText(message, 20) / 2, SCREENHEIGHT - 50, 20, DARKGRAY);

                    EndDrawing();
                }
            }

        VALIDCLICK:

            if (CheckWinCondition(playerBoard)) {
                gameState = GAME_AI_WON;
            } else if (CheckWinCondition(enemyBoard)) {
                gameState = GAME_PLAYER1_WON;
            }
        } else if (gameState == GAME_PAUSED){
               UpdatePauseMenu(pauseMenu);
               if (!pauseMenu->isActive) {
                   gameState = GAME_RUNNING;
            } else if (pauseMenu->toMainMenu){
                break;
            }
        } else {
            if (gameState == GAME_PLAYER1_WON) {
                DrawText("Wygrywasz!", SCREENWIDTH / 2 - MeasureText("Wygrywasz!", 40) / 2, SCREENHEIGHT / 2 - 20, 40, GREEN);
            } else if (gameState == GAME_AI_WON) {
                DrawText("Przegrywasz!", SCREENWIDTH / 2 - MeasureText("Przegrywasz!", 40) / 2, SCREENHEIGHT / 2 - 20, 40, RED);
            }
                Rectangle playAgainButton = { SCREENWIDTH / 2 - 150, SCREENHEIGHT / 2 + 50, 300, 50 };
                Rectangle closeButton = { SCREENWIDTH / 2 - 100, SCREENHEIGHT / 2 + 120, 200, 50 };

                // Prostokąty do rysowania (początkowo takie same jak logiczne prostokąty przycisków)
                Rectangle drawPlayAgainButton = playAgainButton;
                Rectangle drawCloseButton = closeButton;

                // Pozycja myszy
                Vector2 mousePos = GetMousePosition();

                // Sprawdzenie kolizji i powiększenie prostokątów do rysowania
                if (CheckCollisionPointRec(mousePos, playAgainButton)) {
                    drawPlayAgainButton.x -= playAgainButton.width * 0.05f; 
                    drawPlayAgainButton.y -= playAgainButton.height * 0.05f;
                    drawPlayAgainButton.width *= 1.1f;                      
                    drawPlayAgainButton.height *= 1.1f;                     
                }

                if (CheckCollisionPointRec(mousePos, closeButton)) {
                    drawCloseButton.x -= closeButton.width * 0.05f;
                    drawCloseButton.y -= closeButton.height * 0.05f;
                    drawCloseButton.width *= 1.1f;
                    drawCloseButton.height *= 1.1f;
                }
                // Rysowanie przycisków
                DrawTexturePro(playAgainTexture,(Rectangle){0, 0, playAgainTexture.width, playAgainTexture.height}, drawPlayAgainButton, (Vector2){0, 0},  0.0f, WHITE);
                DrawTexturePro(closeTexture,(Rectangle){0, 0, closeTexture.width, closeTexture.height},drawCloseButton,(Vector2){0, 0}, 0.0f,WHITE);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, playAgainButton)) {
                    gameState = GAME_PREPARE1;
                    ResetGame(&playerBoard, &enemyBoard, &playerShip, &enemyShip, gameState, pauseMenu);
                    gameState = GAME_RUNNING;
                    playerTurn = true;
                    message[0] = '\0';
                } else if (CheckCollisionPointRec(mousePos, closeButton)) {
                    FreeSounds();
                    CloseWindow();
                    break;
                }
            }
        }

        EndDrawing();
    }
    UnloadTexture(playAgainTexture);
    UnloadTexture(closeTexture);
    StopMusicStream(sos);
    if (pauseMenu->toMainMenu){
        NewGame(pauseMenu);
    } if(IsWindowReady()){
        FreeSounds();
        CloseWindow();
    }
}

void PlayGame_PvP(board *player1Board, board *player2Board, ship *player1Ship, ship *player2Ship, PauseMenu *pauseMenu) {
    SetExitKey(0);
    Music sos = LoadMusicStream("music/SOS_Signal.ogg");
    Texture2D newTurnTexture = LoadTexture("textures/nowa_tura.png");
    Texture2D playAgainTexture = LoadTexture("textures/zagraj_ponownie.png");
    Texture2D closeTexture = LoadTexture("textures/wyjdz.png");
    sos.looping = true;
    PlayMusicStream(sos);
    SetMusicVolume(sos, 0.33f*pauseMenu->all_sound.val * pauseMenu->music.val);
    int player1OffsetX = (SCREENWIDTH * 1/3)-20 - (BOARD_SIZE * TILE_SIZE) / 2;
    int player1OffsetY = (SCREENHEIGHT - (BOARD_SIZE * TILE_SIZE)) / 2;
    int player2OffsetX = (SCREENWIDTH * 2/3)+20 - (BOARD_SIZE * TILE_SIZE) / 2;
    int player2OffsetY = (SCREENHEIGHT - (BOARD_SIZE * TILE_SIZE)) / 2;

    bool player1Turn = false;
    bool turnEnded = true; // Flaga do śledzenia końca tury
    GameState gameState = GAME_RUNNING;
    char message[128] = "";

    for (int alpha = 255; alpha >= 0; alpha -= 5) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Gracz 1:", player1OffsetX, player1OffsetY - 30, 20, BLACK);
        DrawBoard(player1Board, player1OffsetX, player1OffsetY, true);

        DrawText("Gracz 2:", player2OffsetX, player2OffsetY - 30, 20, BLACK);
        DrawBoard(player2Board, player2OffsetX, player2OffsetY, true);
        DrawText(message, SCREENWIDTH / 2 - MeasureText(message, 20) / 2, SCREENHEIGHT - 50, 20, DARKGRAY);

        // Draw the fading overlay
        DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, (Color){0, 0, 0, alpha});

        EndDrawing();

        // Add a small delay to make the transition visible
        double startTime = GetTime();
        while (GetTime() - startTime < 0.04) 
        {
            SetMusicVolume(sos, 0.33f*pauseMenu->all_sound.val * pauseMenu->music.val);
            UpdateMusicStream(sos);
        }
    }

    while (!WindowShouldClose()) {
        SetMusicVolume(sos, 0.33f*pauseMenu->all_sound.val * pauseMenu->music.val);
        UpdateMusicStream(sos);
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (IsKeyPressed(KEY_ESCAPE)) {
            if (gameState != GAME_PAUSED) {
                ReloadGeneralMenu(pauseMenu);
                UpdatePauseMenu(pauseMenu);
                gameState = GAME_PAUSED;
            } else{
                if(pauseMenu->isGeneral){
                    UnloadGeneralMenu(pauseMenu);
                    gameState = GAME_RUNNING;
                } else{
                    UnloadSoundMenu(pauseMenu);
                    ReloadGeneralMenu(pauseMenu);
                    pauseMenu->isGeneral = true;
                }
            }
        }

        if (gameState == GAME_RUNNING) {
            if (turnEnded) { // Wyświetlanie przycisku do zmiany tury, tak aby gracze nie widzieli plansz przeciwnika
                BeginDrawing();
                ClearBackground(RAYWHITE);

                Rectangle newTurnButton = { SCREENWIDTH / 2 - 100, SCREENHEIGHT / 2 + 50, 200, 50 };

                Rectangle drawNewTurnButton = newTurnButton;

                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, newTurnButton)) {
                    drawNewTurnButton.x -= newTurnButton.width * 0.05f;  
                    drawNewTurnButton.y -= newTurnButton.height * 0.05f; 
                    drawNewTurnButton.width *= 1.1f;                     
                    drawNewTurnButton.height *= 1.1f;                   
                }


                DrawTexturePro(newTurnTexture,(Rectangle){0, 0, newTurnTexture.width, newTurnTexture.height},drawNewTurnButton, (Vector2){0, 0}, 0.0f,  WHITE);

                if (CheckCollisionPointRec(GetMousePosition(), newTurnButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    player1Turn = !player1Turn;
                    turnEnded = false;
                }
            }
            else {
                // Wyświetlanie plansz i obsługa tury gracza
                if (player1Turn) {
                    DrawText("Gracz 1: Twoja plansza", player1OffsetX, player1OffsetY - 30, 20, BLACK);
                    DrawBoard(player1Board, player1OffsetX, player1OffsetY, false);

                    DrawText("Plansza przeciwnika", player2OffsetX, player2OffsetY - 30, 20, BLACK);
                    DrawBoard(player2Board, player2OffsetX, player2OffsetY, true);

                    DrawText(message, SCREENWIDTH / 2 - MeasureText(message, 20) / 2, SCREENHEIGHT - 50, 20, DARKGRAY);

                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        struct array_cordinals *cords = Get_array_cordinals(player2OffsetX, player2OffsetY);
                        if (cords != NULL) {
                            int x = cords->x;
                            int y = cords->y;
                            free(cords);

                            if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
                                pair shot = {x, y};
                                if (!player2Board->shots[x][y]) {
                                    shoot(player2Board, shot);
                                    snprintf(message, sizeof(message), "Gracz 1 strzelił w (%d, %d)", x, y);
                                    if (player2Board->BOARD[x][y] != NULL) {
                                        ship *currShip = player2Board->BOARD[x][y];
                                        bool sunk = true;
                                        for (int i = 0; i < currShip->type; i++) {
                                            if (!currShip->boardplace[i].got_shot) {
                                                sunk = false;
                                                break;
                                            }
                                        }
                                        if (sunk) {
                                            scream();
                                            snprintf(message, sizeof(message), "Gracz 1 zatopił statek!");
                                        }
                                    } else {
                                        turnEnded = true;
                                    }
                                    double startTime = GetTime();
                                    while (GetTime() - startTime < 1.0) {
                                        UpdateMusicStream(sos);
                                        BeginDrawing();
                                        ClearBackground(RAYWHITE);

                                        DrawText("Gracz 1: Twoja plansza", player1OffsetX, player1OffsetY - 30, 20, BLACK);
                                        DrawBoard(player1Board, player1OffsetX, player1OffsetY, false);

                                        DrawText("Plansza przeciwnika", player2OffsetX, player2OffsetY - 30, 20, BLACK);
                                        DrawBoard(player2Board, player2OffsetX, player2OffsetY, true);

                                        DrawText(message, SCREENWIDTH / 2 - MeasureText(message, 20) / 2, SCREENHEIGHT - 50, 20, DARKGRAY);

                                        EndDrawing();
                                    }

                                } else {
                                    snprintf(message, sizeof(message), "Strzelałeś już tutaj!");
                                }
                            }
                        }
                    }
                } else {
                    DrawText("Gracz 2: Twoja plansza", player2OffsetX, player2OffsetY - 30, 20, BLACK);
                    DrawBoard(player2Board, player2OffsetX, player2OffsetY, false);

                    DrawText("Plansza przeciwnika", player1OffsetX, player1OffsetY - 30, 20, BLACK);
                    DrawBoard(player1Board, player1OffsetX, player1OffsetY, true);

                    DrawText(message, SCREENWIDTH / 2 - MeasureText(message, 20) / 2, SCREENHEIGHT - 50, 20, DARKGRAY);

                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        struct array_cordinals *cords = Get_array_cordinals(player1OffsetX, player1OffsetY);
                        if (cords != NULL) {
                            int x = cords->x;
                            int y = cords->y;
                            free(cords);

                            if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
                                pair shot = {x, y};
                                if (!player1Board->shots[x][y]) {
                                    shoot(player1Board, shot);
                                    snprintf(message, sizeof(message), "Gracz 2 strzelił w (%d, %d)", x, y);
                                    if (player1Board->BOARD[x][y] != NULL) {
                                        ship *currShip = player1Board->BOARD[x][y];
                                        bool sunk = true;
                                        for (int i = 0; i < currShip->type; i++) {
                                            if (!currShip->boardplace[i].got_shot) {
                                                sunk = false;
                                                break;
                                            }
                                        }
                                        if (sunk) {
                                            scream();
                                            snprintf(message, sizeof(message), "Gracz 2 zatopił statek!");
                                        }
                                    } else {
                                        turnEnded = true;
                                    }
                                    double startTime = GetTime();
                                    while (GetTime() - startTime < 1.0) {
                                        UpdateMusicStream(sos);
                                        BeginDrawing();
                                        ClearBackground(RAYWHITE);

                                        DrawText("Gracz 2: Twoja plansza", player2OffsetX, player2OffsetY - 30, 20, BLACK);
                                        DrawBoard(player2Board, player2OffsetX, player2OffsetY, false);

                                        DrawText("Plansza przeciwnika", player1OffsetX, player1OffsetY - 30, 20, BLACK);
                                        DrawBoard(player1Board, player1OffsetX, player1OffsetY, true);

                                        DrawText(message, SCREENWIDTH / 2 - MeasureText(message, 20) / 2, SCREENHEIGHT - 50, 20, DARKGRAY);

                                        EndDrawing();
                                    }

                                } else {
                                    snprintf(message, sizeof(message), "Strzelałeś już tutaj!");
                                }
                            }
                        }
                    }
                }

                if (CheckWinCondition(player1Board)) {
                    gameState = GAME_PLAYER2_WON;
                } else if (CheckWinCondition(player2Board)) {
                    gameState = GAME_PLAYER1_WON;
                }
            }
        }else if (gameState == GAME_PAUSED){
            UpdatePauseMenu(pauseMenu);
            if (!pauseMenu->isActive) {
                gameState = GAME_RUNNING;
            } else if (pauseMenu->toMainMenu){
                break;
            }
        } else {
            // Wyświetlenie wyniku gry
            if (gameState == GAME_PLAYER1_WON) {
                DrawText("Gracz 1 Wygrywa!", SCREENWIDTH / 2 - MeasureText("Gracz 1 Wygrywa!", 40) / 2, SCREENHEIGHT / 2 - 20, 40, GREEN);
            } else if (gameState == GAME_PLAYER2_WON) {
                DrawText("Gracz 2 Wygrywa!", SCREENWIDTH / 2 - MeasureText("Gracz 2 Wygrywa!", 40) / 2, SCREENHEIGHT / 2 - 20, 40, GREEN);
            }

                Rectangle playAgainButton = { SCREENWIDTH / 2 - 150, SCREENHEIGHT / 2 + 50, 300, 50 };
                Rectangle closeButton = { SCREENWIDTH / 2 - 100, SCREENHEIGHT / 2 + 120, 200, 50 };

                // Prostokąty do rysowania (początkowo takie same jak logiczne prostokąty przycisków)
                Rectangle drawPlayAgainButton = playAgainButton;
                Rectangle drawCloseButton = closeButton;

                // Pozycja myszy
                Vector2 mousePos = GetMousePosition();

                // Sprawdzenie kolizji i powiększenie prostokątów do rysowania
                if (CheckCollisionPointRec(mousePos, playAgainButton)) {
                    drawPlayAgainButton.x -= playAgainButton.width * 0.05f; 
                    drawPlayAgainButton.y -= playAgainButton.height * 0.05f;
                    drawPlayAgainButton.width *= 1.1f;                      
                    drawPlayAgainButton.height *= 1.1f;                     
                }

                if (CheckCollisionPointRec(mousePos, closeButton)) {
                    drawCloseButton.x -= closeButton.width * 0.05f;
                    drawCloseButton.y -= closeButton.height * 0.05f;
                    drawCloseButton.width *= 1.1f;
                    drawCloseButton.height *= 1.1f;
                }
                // Rysowanie przycisków
                DrawTexturePro(playAgainTexture,(Rectangle){0, 0, playAgainTexture.width, playAgainTexture.height}, drawPlayAgainButton, (Vector2){0, 0},  0.0f, WHITE);
                DrawTexturePro(closeTexture,(Rectangle){0, 0, closeTexture.width, closeTexture.height},drawCloseButton,(Vector2){0, 0}, 0.0f,WHITE);

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, playAgainButton)) {
                    gameState = GAME_PREPARE2;
                    ResetGame(&player1Board, &player2Board, &player1Ship, &player2Ship,gameState, pauseMenu);
                    gameState = GAME_RUNNING;
                    player1Turn = true;
                    turnEnded = false;
                    message[0] = '\0';
                } else if (CheckCollisionPointRec(mousePos, closeButton)) {
                    FreeSounds();
                    CloseWindow();
                    break;
                }
            }
        }

        EndDrawing();
    }
    UnloadTexture(newTurnTexture);
    UnloadTexture(playAgainTexture);
    UnloadTexture(closeTexture);
    StopMusicStream(sos);

    if (pauseMenu->toMainMenu){
        NewGame(pauseMenu);
    } if(IsWindowReady()){
        FreeSounds();
        CloseWindow();
    }
}

// Button InitButton(float x, float y, void* function, char* spriteLoc, char* text, int fontsize){
Button InitButton(float x, float y, char* spriteLoc, char* text, int fontsize){
    Button b;
    b.isActive = false;
    b.isUnderMouse = false;

    // b.function = function;
    b.sprite = LoadImage(spriteLoc);
    if (&b.sprite == NULL) {
        TraceLog(LOG_ERROR, "Failed to load button sprite");
    }
    ImageResize(&b.sprite, 360, 90);
    b.texture = LoadTextureFromImage(b.sprite);

    b.pos.x = x - b.texture.width / 2;
    b.pos.y = y - b.texture.height / 2;

    b.text = strdup(text);
    b.fontsize = fontsize;

    b.hitbox.x = b.pos.x;
    b.hitbox.y = b.pos.y;
    b.hitbox.width = b.texture.width;
    b.hitbox.height = b.texture.height;

    b.draw = b.hitbox;

    return b;
}

void UpdateButton(Button *b) {
    if (b->isActive) {
        if (CheckCollisionPointRec(GetMousePosition(), b->hitbox) && !b->isUnderMouse){
            b->draw.x -= b->hitbox.width * 0.05f;
            b->draw.y -= b->hitbox.height * 0.05f;
            b->draw.width = b->hitbox.width * 1.1f;
            b->draw.height = b->hitbox.height * 1.1f;
            b->isUnderMouse = true;
        } else if(!CheckCollisionPointRec(GetMousePosition(), b->hitbox) && b->isUnderMouse){
            b->draw = b->hitbox;
            b->isUnderMouse = false;
        }

        DrawTexturePro(b->texture,(Rectangle){0, 0, b->texture.width, b->texture.height}, b->draw, (Vector2){0,0}, 0.0f, WHITE);
        DrawText(b->text,
                 (int)(b->hitbox.x + (b->hitbox.width / 2) - (MeasureText(b->text, b->fontsize) / 2)),
                 (int)(b->hitbox.y + (b->hitbox.height / 2) - (b->fontsize / 2)),
                 b->fontsize, BLACK);
    }
}

Slider InitSlider(float y_pos, float left, float right, float sl_height, float hand_width, float hand_height, char* sl_spriteLoc, char* hand_spriteLoc){
	Slider s;
	s.val = 1.0f;

	s.isActive = false;
	s.isUpdating = false;

	s.y_pos = y_pos;

	s.left = left;
	s.right = right;

	s.sl_sprite = LoadImage(sl_spriteLoc);
	if (s.sl_sprite.data == NULL) {
        TraceLog(LOG_ERROR, "Failed to load slider sprite");
    }
	ImageResize(&s.sl_sprite, (int)(right - left), (int)sl_height);
    s.sl_texture = LoadTextureFromImage(s.sl_sprite);

	s.hand_sprite = LoadImage(hand_spriteLoc);
	if (s.hand_sprite.data == NULL) {
        TraceLog(LOG_ERROR, "Failed to load handle sprite");
    }
	ImageResize(&s.hand_sprite, (int)hand_width, (int)hand_height);
    s.hand_texture = LoadTextureFromImage(s.hand_sprite);

	s.handle = s.right - s.hand_texture.width / 2;

	s.hitbox.x = left - s.hand_texture.width / 2;
    s.hitbox.y = y_pos;
	s.hitbox.width = s.sl_texture.width + s.hand_texture.width;
	s.hitbox.height = s.sl_texture.height;

	return s;
}

void UpdateSlider(Slider* s, void *context){
	if(s->isActive){
		if((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), s->hitbox)) || (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && s->isUpdating)){
		s->isUpdating = true;
		}
		else {
			s->isUpdating = false;
		}

		if(s->isUpdating && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
			if(((GetMouseX() - s->hand_texture.width / 2) >= (s->left - s->hand_texture.width / 2))
				&& ((GetMouseX() - s->hand_texture.width / 2) <= (s->right - s->hand_texture.width / 2))){
				s->handle = GetMouseX() - s->hand_texture.width / 2;
				s->val = (s->handle - s->left + s->hand_texture.width / 2) / (s->right - s->left);

				s->val = floor(100*s->val)/100;
			}
			else if((GetMouseX() - s->hand_texture.width / 2) < (s->left - s->hand_texture.width / 2)){
				s->handle = s->left - s->hand_texture.width / 2;
				s->val = 0.0f;
			}
			else{
				s->handle = s->right - s->hand_texture.width / 2;
				s->val = 1.0f;
			}
		}
		DrawTexture(s->sl_texture, (int)s->left, (int)s->y_pos, RAYWHITE);
		DrawTexture(s->hand_texture, (int)s->handle, (int)(s->y_pos - (s->hand_texture.height - s->hitbox.height) / 2), RAYWHITE);
		//DrawRectangleLines(s->hitbox.x, s->hitbox.y, s->hitbox.width, s->hitbox.height, RED);

		snprintf(s->valText, sizeof(s->valText), "%d", (int)(s->val * 100));
		DrawText(s->valText, (int)(s->right + s->hand_texture.width / 2), (int)(s->y_pos), s->hitbox.height, BLACK);
	}
}

void UnloadGeneralMenu(PauseMenu *pm){
    pm->back.isActive = false;
    pm->volume.isActive = false;
    pm->menu.isActive = false;
}

void ReloadGeneralMenu(PauseMenu *pm){
    pm->isActive = true;
    pm->back.isActive = true;
    pm->volume.isActive = true;
    pm->menu.isActive = true;
}

void UnloadSoundMenu(PauseMenu *pm){
    pm->sound_back.isActive = false;
    pm->all_sound.isActive = false;
    pm->music.isActive = false;
    pm->effects.isActive = false;
}

void ReloadSoundMenu(PauseMenu *pm){
    pm->isActive = true;
    pm->sound_back.isActive = true;
    pm->all_sound.isActive = true;
    pm->music.isActive = true;
    pm->effects.isActive = true;
}

PauseMenu* InitPauseMenu(){
    PauseMenu *pm;

    pm = (PauseMenu *) malloc(sizeof(PauseMenu));

    pm->isGeneral = true;
    pm->toMainMenu = false;
    pm->blur = (Color){0, 0, 0, 128};
    pm->background = LoadTexture("textures/ustawianie_bez_siatki.png");
    pm->back = InitButton(SCREENWIDTH / 2, SCREENHEIGHT / 4 * 3, "textures/3x1.png", "Powrót", 40);
    pm->volume = InitButton(SCREENWIDTH / 2, SCREENHEIGHT / 2, "textures/3x1.png", "Glosnosc", 40);
    pm->menu = InitButton(SCREENWIDTH / 2, SCREENHEIGHT / 4, "textures/3x1.png", "Menu Glowne", 40);

    pm->sound_back = InitButton(SCREENWIDTH / 2, SCREENHEIGHT / 6*5, "textures/3x1.png", "Powrót", 40);
    pm->all_sound = InitSlider(SCREENHEIGHT / 6, SCREENWIDTH / 7*2, SCREENWIDTH / 7*5, 50.0f, 60.0f, 60.0f, "textures/slider.png", "textures/1x1.png");
    pm->music = InitSlider(SCREENHEIGHT / 3, SCREENWIDTH / 7*2, SCREENWIDTH / 7*5, 50.0f, 60.0f, 60.0f, "textures/slider.png", "textures/1x1.png");
    pm->effects = InitSlider(SCREENHEIGHT / 2, SCREENWIDTH / 7*2, SCREENWIDTH / 7*5, 50.0f, 60.0f, 60.0f, "textures/slider.png", "textures/1x1.png");

    return pm;
}


void UpdatePauseMenu(PauseMenu *pm){
    DrawTexture(pm->background, 0, 0, WHITE);
    DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, pm->blur);

    if(pm->isGeneral){
        UpdateButton(&pm->back);
        UpdateButton(&pm->volume);
        UpdateButton(&pm->menu);

        if (CheckCollisionPointRec(GetMousePosition(),pm->back.hitbox) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            pm->isActive = false;
            return;
        }

        if (CheckCollisionPointRec(GetMousePosition(),pm->volume.hitbox) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            UnloadGeneralMenu(pm);
            ReloadSoundMenu(pm);
            pm->isGeneral = false;
        }

        if (CheckCollisionPointRec(GetMousePosition(),pm->menu.hitbox) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            pm->toMainMenu = true;
            return;
        }
    }

    else {
        UpdateButton(&pm->sound_back);
        UpdateSlider(&pm->all_sound, pm);
        UpdateSlider(&pm->music, pm);
        UpdateSlider(&pm->effects, pm);

        if(pm->isMainMenu){
            SetExitKey(0);
            if(IsKeyPressed(KEY_ESCAPE)){
                UnloadSoundMenu(pm);
                pm->isActive = false;
                SetExitKey(KEY_ESCAPE);
            }
        }

        if (CheckCollisionPointRec(GetMousePosition(),pm->sound_back.hitbox) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
             if(!pm->isMainMenu){
                 UnloadSoundMenu(pm);
                 ReloadGeneralMenu(pm);
                 pm->isGeneral = true;
            } else{
                UnloadSoundMenu(pm);
                pm->isActive = false;
                SetExitKey(KEY_ESCAPE);
            }

        }
    }
}

GameState PreGame(PauseMenu *pauseMenu)
{
    GameState gameState = GAME_START;
    pauseMenu->toMainMenu = true;
    Music pirent = LoadMusicStream("music/Pirates-entertaiment.ogg");
    pirent.looping = true;
    PlayMusicStream(pirent);

    Texture2D titleTexture = LoadTexture("Napisy/tytul.png");
    Texture2D onePlayerTexture = LoadTexture("Napisy/jedengracz.png");
    Texture2D twoPlayersTexture = LoadTexture("Napisy/dwochgraczy.png");
    Texture2D backgroundTexture = LoadTexture("Napisy/tlo.png");
    Texture2D buttonTexture = LoadTexture("Napisy/przycisk.png");
    Texture2D soundSettingsTexture = LoadTexture("Napisy/glosnosc.png");

    while (!WindowShouldClose()) 
    {
        SetMusicVolume(pirent, 0.5f * pauseMenu->all_sound.val * pauseMenu->music.val);
        UpdateMusicStream(pirent);

        BeginDrawing();

        if(gameState == GAME_START){
            DrawTexture(backgroundTexture, -20, -20, WHITE);

            // Skalowanie i rysowanie tytułu gry
            float titleScale = 1.5f; // Skala tytułu
            int titlePosX = SCREENWIDTH / 2 - (titleTexture.width * titleScale) / 2;
            int titlePosY = SCREENHEIGHT / 2 - 280;
            DrawTextureEx(titleTexture, (Vector2){titlePosX, titlePosY+30}, 0.0f, titleScale, WHITE);

            int buttonSpacing = 20;
            int buttonWidth = 350;  // Docelowa szerokość przycisków
            int buttonHeight = 150; // Docelowa wysokość przycisków
            float buttonScaleX = (float)buttonWidth / buttonTexture.width;

            // Prostokąty dla przycisków
            Rectangle buttonOnePlayer = { SCREENWIDTH / 2 - buttonWidth / 2, SCREENHEIGHT / 2 - buttonHeight / 2, buttonWidth, buttonHeight };
            Rectangle buttonTwoPlayers = { SCREENWIDTH / 2 - buttonWidth / 2, SCREENHEIGHT / 2 - buttonHeight / 2 + buttonHeight + buttonSpacing, buttonWidth, buttonHeight };
            Rectangle buttonSoundSettings = { SCREENWIDTH/16*15, SCREENHEIGHT/32*30, soundSettingsTexture.width, soundSettingsTexture.height };

            // Zmienne skalowania przycisków
            Rectangle scaledButtonOnePlayer = buttonOnePlayer;
            Rectangle scaledButtonTwoPlayers = buttonTwoPlayers;
            Rectangle scaledButtonSoundSettings = buttonSoundSettings;

            // Skalowanie przycisków o 10% przy najechaniu myszką
            Vector2 mousePos = GetMousePosition();

            if (CheckCollisionPointRec(mousePos, buttonOnePlayer)) {
                scaledButtonOnePlayer.x -= buttonOnePlayer.width * 0.05f;  // Przesunięcie w lewo
                scaledButtonOnePlayer.y -= buttonOnePlayer.height * 0.05f; // Przesunięcie w górę
                scaledButtonOnePlayer.width *= 1.1f;                       // Zwiększenie szerokości
                scaledButtonOnePlayer.height *= 1.1f;                      // Zwiększenie wysokości
            }

            if (CheckCollisionPointRec(mousePos, buttonTwoPlayers)) {
                scaledButtonTwoPlayers.x -= buttonTwoPlayers.width * 0.05f;
                scaledButtonTwoPlayers.y -= buttonTwoPlayers.height * 0.05f;
                scaledButtonTwoPlayers.width *= 1.1f;
                scaledButtonTwoPlayers.height *= 1.1f;
            }

            if (CheckCollisionPointRec(mousePos, buttonSoundSettings)) {
                scaledButtonSoundSettings.x -= buttonSoundSettings.width * 0.05f;
                scaledButtonSoundSettings.y -= buttonSoundSettings.height * 0.05f;
                scaledButtonSoundSettings.width *= 1.1f;
                scaledButtonSoundSettings.height *= 1.1f;
            }

            // Rysowanie tła przycisków
            DrawTexturePro(
                buttonTexture,
                (Rectangle){0, 0, buttonTexture.width, buttonTexture.height},
                scaledButtonOnePlayer,
                (Vector2){0, 0},
                0.0f,
                WHITE
            );

            DrawTexturePro(
                buttonTexture,
                (Rectangle){0, 0, buttonTexture.width, buttonTexture.height},
                scaledButtonTwoPlayers,
                (Vector2){0, 0},
                0.0f,
                WHITE
            );

            DrawTexturePro(
                soundSettingsTexture,
                (Rectangle){0, 0, soundSettingsTexture.width, soundSettingsTexture.height},
                scaledButtonSoundSettings,
                (Vector2){0, 0},
                0.0f,
                WHITE
            );

            // Skalowanie obrazów wewnątrz przycisków
            float onePlayerImageScale = fminf(
                (scaledButtonOnePlayer.width / onePlayerTexture.width),
                (scaledButtonOnePlayer.height / onePlayerTexture.height)
            );

            float twoPlayersImageScale = fminf(
                (scaledButtonTwoPlayers.width / twoPlayersTexture.width),
                (scaledButtonTwoPlayers.height / twoPlayersTexture.height)
            );

            float soundSettingsImageScale = fminf(
                (scaledButtonSoundSettings.width / soundSettingsTexture.width),
                (scaledButtonSoundSettings.height / soundSettingsTexture.height)
            );

            // Obliczenie pozycji obrazów wewnętrznych
            int onePlayerImageX = scaledButtonOnePlayer.x + (scaledButtonOnePlayer.width - onePlayerTexture.width * onePlayerImageScale) / 2;
            int onePlayerImageY = scaledButtonOnePlayer.y + (scaledButtonOnePlayer.height - onePlayerTexture.height * onePlayerImageScale) / 2;
            int twoPlayersImageX = scaledButtonTwoPlayers.x + (scaledButtonTwoPlayers.width - twoPlayersTexture.width * twoPlayersImageScale) / 2;
            int twoPlayersImageY = scaledButtonTwoPlayers.y + (scaledButtonTwoPlayers.height - twoPlayersTexture.height * twoPlayersImageScale) / 2;
            int soundSettingsImageX = scaledButtonSoundSettings.x + (scaledButtonSoundSettings.width - soundSettingsTexture.width * soundSettingsImageScale) / 2;
            int soundSettingsImageY = scaledButtonSoundSettings.y + (scaledButtonSoundSettings.height - soundSettingsTexture.height * soundSettingsImageScale) / 2;

            // Rysowanie obrazów wewnątrz przycisków
            DrawTextureEx(onePlayerTexture, (Vector2){onePlayerImageX+26, onePlayerImageY+10}, 0.0f, onePlayerImageScale-0.16, WHITE);
            DrawTextureEx(twoPlayersTexture, (Vector2){twoPlayersImageX+26, twoPlayersImageY+10}, 0.0f, twoPlayersImageScale-0.16, WHITE);



            if (CheckCollisionPointRec(mousePos, buttonOnePlayer) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                EndDrawing();
                StopMusicStream(pirent);
                pauseMenu->toMainMenu = false;
                return GAME_PREPARE1;
            }
            else if (CheckCollisionPointRec(mousePos, buttonTwoPlayers) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                EndDrawing();
                StopMusicStream(pirent);
                pauseMenu->toMainMenu = false;
                return GAME_PREPARE2;
            }
            else if (CheckCollisionPointRec(mousePos, buttonSoundSettings) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                ReloadSoundMenu(pauseMenu);
                gameState = GAME_PAUSED;
            }

        } else{
            UpdatePauseMenu(pauseMenu);
            if (!pauseMenu->isActive) {
                gameState = GAME_START;
            }
        }

        EndDrawing();
    }

    // Zwolnienie tekstur
    UnloadTexture(titleTexture);
    UnloadTexture(onePlayerTexture);
    UnloadTexture(twoPlayersTexture);
    UnloadTexture(backgroundTexture);
    UnloadTexture(buttonTexture);
    StopMusicStream(pirent);
    pauseMenu->toMainMenu = false;
    return GAME_RUNNING; // Default state if the window is closed
}

void NewGame(PauseMenu *pauseMenu){
    SetExitKey(KEY_ESCAPE);
    GameState gameState = GAME_START;

    board *enemyBoard = init_ai_ships();        //druga plansza
    ship *enemyShip = NULL;
    ship *playerShip = NULL;

    /*
        Przekazywanie niezainicjalizowany zmiennych do funkcji to narażanie się na undefined
        behaviour, więc nie chcąc wywracać kodu do góry nogami inicjalizuję je jako NULL.
    */

    pauseMenu->isGeneral = false;
    pauseMenu->isMainMenu = true;
    gameState = PreGame(pauseMenu);
    pauseMenu->isMainMenu = false;
    pauseMenu->isGeneral = true;

    if(gameState == GAME_PREPARE1)
    {
        GameData* gameData = GameSet(GAME_START, pauseMenu);
        if(gameData != NULL){
            PlayGame(gameData->playerBoard, enemyBoard,playerShip,enemyShip,pauseMenu);
            free(gameData);
        }
    }
    else if(gameState == GAME_PREPARE2)
    {
        GameData* gameData1 = GameSet(GAME_PREPARE1, pauseMenu);
        GameData* gameData2 = GameSet(GAME_PREPARE2, pauseMenu);
        if(gameData1 != NULL && gameData2 != NULL){
            PlayGame_PvP(gameData1->playerBoard, gameData2->playerBoard,playerShip,enemyShip,pauseMenu);
            free(gameData1);
            free(gameData2);
        }
    }
    free(enemyBoard);
    free(enemyShip);
    free(playerShip);
}
