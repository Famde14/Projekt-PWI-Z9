#include "raylib.h"
#include "structs.h"
#include "functions.h"
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>//dla usleep
#include <stddef.h>//dla definicji nulla 
#include <stdio.h>//do wywalenia ale to jak bedzie interfejs

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
	s->pos.x = gridStartX + ((int)((s->pos.x - gridStartX) / TILE_SIZE)) * TILE_SIZE;
	s->pos.y = gridStartY + ((int)((s->pos.y - gridStartY) / TILE_SIZE)) * TILE_SIZE;
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

GameData GameSet( GameState gameState )
{
    int gridSize = 10; // Rozmiar planszy
    // int TILE_SIZE = 50; // Rozmiar pojedynczej kratki (w pikselach)

    // załadowanie tekstur statków
    const char *ship1Files[] = {"textures/1x1.png", "textures/1x1.png", "textures/1x1.png", "textures/1x1.png"};
    const char *ship2Files[] = {"textures/2x1.png", "textures/2x1.png", "textures/2x1.png"};
    const char *ship3Files[] = {"textures/3x1.png", "textures/3x1.png"};
    const char *ship4Files[] = {"textures/4x1.png"};

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

    PlayMusicStream(calm);
    while (1)
    {
        UpdateMusicStream(calm);
        if (WindowShouldClose())
        {
            CloseWindow();
            // Do dodania zwalnianie pamięci
            exit(0);
        }
        
        // Update ships
        for (int i = 0; i < MAX_SHIPS; i++)
        {
            // Sprawdzenie czy statek nie wyleciał poza okno
            if (playerShips[i].pos.x < 0 || playerShips[i].pos.x + (int)playerShips[i].length * (TILE_SIZE) > SCREENWIDTH ||
                playerShips[i].pos.y < 0 || playerShips[i].pos.y + TILE_SIZE > SCREENHEIGHT)
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

        ClearBackground(RAYWHITE);

 
            DrawLine(SCREENWIDTH / 2, 0, SCREENWIDTH / 2, SCREENHEIGHT, BLACK); // Pionowa linia

            Rectangle StartBattleButton = {SCREENWIDTH - 260, SCREENHEIGHT - 100, 220, 50};
            DrawRectangleRec(StartBattleButton, LIGHTGRAY);
            DrawText("Zacznij bitwe!", StartBattleButton.x + 10, StartBattleButton.y + 10, 30, BLACK);
        
            if (CheckCollisionPointRec(GetMousePosition(), StartBattleButton))
            {
                DrawRectangleLinesEx(StartBattleButton, 2, DARKBLUE);
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
                        usleep(1000000);
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
                        usleep(1000000);
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
                        if(gameState == GAME_START)
                            DrawText("Twoja plansza", gridStartX + (gridSize * TILE_SIZE) / 2 - MeasureText("Twoja plansza", 20) / 2, gridStartY - 50, 20, BLACK);
                        else if(gameState == GAME_PREPARE1)
                            DrawText("Gracz 1", gridStartX + (gridSize * TILE_SIZE) / 2 - MeasureText("Gracz 1", 20) / 2, gridStartY - 50, 20, BLACK);
                        else if(gameState == GAME_PREPARE2)
                            DrawText("Gracz 2", gridStartX + (gridSize * TILE_SIZE) / 2 - MeasureText("Gracz 2", 20) / 2, gridStartY - 50, 20, BLACK);
                        
                        for (int i = 0; i < gridSize; i++)
                        {
                            char label[3]; // Increased size to accommodate two-digit numbers
                            snprintf(label, sizeof(label), "%c", 'A' + i);
                            DrawText(label, gridStartX + i * TILE_SIZE + TILE_SIZE / 2 - 5, gridStartY - 30, 20, BLACK);
                            snprintf(label, sizeof(label), "%d", i + 1);
                            DrawText(label, gridStartX - 30, gridStartY + i * TILE_SIZE + TILE_SIZE / 2 - 10, 20, BLACK);
                        }

                        for (int i = 0; i < gridSize; i++)
                        {
                            for (int j = 0; j < gridSize; j++)
                            {
                                DrawRectangleLines(gridStartX + j * TILE_SIZE, gridStartY + i * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLACK);
                            }
                        }
                        // Draw ships
                        // for (int i = 0; i < MAX_SHIPS; i++) {
                        //    DrawTexture(playerShips[i].texture, (int)playerShips[i].pos.x, (int)playerShips[i].pos.y, WHITE);
                        //}
                        EndDrawing();
                        usleep(40000);
                    }
                    DrawText("Zacznij bitwe!", StartBattleButton.x + 10, StartBattleButton.y + 10, 30, BLACK);
                    break;
                }
            }
            //napis nad planszą
            if(gameState == GAME_START)
                DrawText("Twoja plansza", gridStartX + (gridSize * TILE_SIZE) / 2 - MeasureText("Twoja plansza", 20) / 2, gridStartY - 50, 20, BLACK);
            else if(gameState == GAME_PREPARE1)
                DrawText("Gracz 1", gridStartX + (gridSize * TILE_SIZE) / 2 - MeasureText("Gracz 1", 20) / 2, gridStartY - 50, 20, BLACK);
            else if(gameState == GAME_PREPARE2)
                DrawText("Gracz 2", gridStartX + (gridSize * TILE_SIZE) / 2 - MeasureText("Gracz 2", 20) / 2, gridStartY - 50, 20, BLACK);

            for (int i = 0; i < gridSize; i++)
            {
                char label[3]; // Increased size to accommodate two-digit numbers
                snprintf(label, sizeof(label), "%c", 'A' + i);
                DrawText(label, gridStartX + i * TILE_SIZE + TILE_SIZE / 2 - 5, gridStartY - 30, 20, BLACK);
                snprintf(label, sizeof(label), "%d", i + 1);
                DrawText(label, gridStartX - 30, gridStartY + i * TILE_SIZE + TILE_SIZE / 2 - 10, 20, BLACK);
            }

            for (int i = 0; i < gridSize; i++)
            {
                for (int j = 0; j < gridSize; j++)
                {
                    DrawRectangleLines(gridStartX + j * TILE_SIZE, gridStartY + i * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLACK);
                }
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

    // Układanie statków na w zmiennej playerBoard
    for (int i = 0; i < MAX_SHIPS; i++)
    {
        int gridX = (playerShips[i].pos.x - gridStartX) / TILE_SIZE;
        int gridY = (playerShips[i].pos.y - gridStartY) / TILE_SIZE;

        if (gridX >= 0 && gridY >= 0)
        {
            int dl = playerShips[i].length;
            ship *playerS = initship(dl);
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

    GameData gameData = {playerBoard, playerShips, MAX_SHIPS};
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
	for (int i = 0; i < curr_ship->type; i++)
	{
		if((curr_ship->boardplace[i]).cords.x==paira.x&&(curr_ship->boardplace[i]).cords.y==paira.y)//nic nie sugeruje ale to byloby mniej brzydsze jakby uzyc klas
		{(curr_ship->boardplace[i]).got_shot=1;}
	}
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
    }
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

void ResetGame(board **playerBoard, board **enemyBoard, ship **playerShip, ship **enemyShip) //basicowa funkcja resetujaca gre (pozniej trzeba wyrzucic stad playership i enemyship, zeby samo usuwalo - nikt nie bedzie tego recznie ustawial)
{
    //delboard(*playerBoard);
    //delboard(*enemyBoard);
    //delship(*playerShip);
    //delship(*enemyShip);

    *playerBoard = init_ai_ships();//w następnej rundzie wszystko niech będzie ustawione losowo
    *enemyBoard = init_ai_ships();
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
    static Texture2D shipTextures[4];
    static Texture2D ship_brokenTextures[4];
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
        shipTextures[0] = LoadTextureFromImage(shipImages[0]);
        ship_brokenImages[0] = LoadImage("textures/1x1_zepsuta.png");
        ship_brokenTextures[0] = LoadTextureFromImage(ship_brokenImages[0]);

        shipImages[1] = LoadImage("textures/2x1.png");
        shipTextures[1] = LoadTextureFromImage(shipImages[1]);
        ship_brokenImages[1] = LoadImage("textures/2x1_zepsuta.png");
        ship_brokenTextures[1] = LoadTextureFromImage(ship_brokenImages[1]);

        shipImages[2] = LoadImage("textures/3x1.png");
        shipTextures[2] = LoadTextureFromImage(shipImages[2]);
        ship_brokenImages[2] = LoadImage("textures/3x1_zepsuta.png");
        ship_brokenTextures[2] = LoadTextureFromImage(ship_brokenImages[2]);

        shipImages[3] = LoadImage("textures/4x1.png");
        shipTextures[3] = LoadTextureFromImage(shipImages[3]);
        ship_brokenImages[3] = LoadImage("textures/4x1_zepsuta.png");
        ship_brokenTextures[3] = LoadTextureFromImage(ship_brokenImages[3]);

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
                {
                    DrawTexture(ship_brokenTextures[currShip->type-1], offsetX + currShip->boardplace[0].cords.x * TILE_SIZE, offsetY + currShip->boardplace[0].cords.y * TILE_SIZE, WHITE);
                }
                else
                {
                    if(!isEnemy)
                    {
                        DrawTexture(shipTextures[currShip->type-1], offsetX + currShip->boardplace[0].cords.x * TILE_SIZE, offsetY + currShip->boardplace[0].cords.y * TILE_SIZE, WHITE);
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

void PlayGame(board *playerBoard, board *enemyBoard, ship *playerShip, ship *enemyShip) {
    Music sos = LoadMusicStream("music/SOS_Signal.ogg");
    sos.looping = true;
    PlayMusicStream(sos);
    int playerOffsetX = (SCREENWIDTH * 1/3)-20 - (BOARD_SIZE * TILE_SIZE) / 2;
    int playerOffsetY = (SCREENHEIGHT - (BOARD_SIZE * TILE_SIZE)) / 2;
    int enemyOffsetX = (SCREENWIDTH * 2/3)+20 - (BOARD_SIZE * TILE_SIZE) / 2;
    int enemyOffsetY = (SCREENHEIGHT - (BOARD_SIZE * TILE_SIZE)) / 2;

    bool playerTurn = true;
    GameState gameState = GAME_RUNNING;
    char message[128] = "";

    for (int alpha = 255; alpha >= 0; alpha-=5) {
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
        usleep(40000);
    }

    while (!WindowShouldClose()) {
        UpdateMusicStream(sos);
        BeginDrawing();

        if (IsKeyPressed(KEY_ESCAPE)) {
            CloseWindow();
            break;
        }

        ClearBackground(RAYWHITE);

        if (gameState == GAME_RUNNING) {
            DrawText("Twoja plansza", playerOffsetX, playerOffsetY - 30, 20, BLACK);
            DrawBoard(playerBoard, playerOffsetX, playerOffsetY, false);

            DrawText("Plansza przeciwnika", enemyOffsetX, enemyOffsetY - 30, 20, BLACK);
            DrawBoard(enemyBoard, enemyOffsetX, enemyOffsetY, true);

            DrawText(message, SCREENWIDTH / 2 - MeasureText(message, 20) / 2, SCREENHEIGHT - 50, 20, DARKGRAY);

            if (playerTurn) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    struct array_cordinals *cords = Get_array_cordinals(enemyOffsetX, enemyOffsetY);
                    if(cords==NULL) goto VALIDCLICK;//dobrze, że dr Paweł Laskoś-Grabowski tego nie sprawdza, cóż byłem do tego zmuszony
                    int x = cords->x;
                    int y = cords->y;
					free(cords);

                    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
                        pair shot = {x, y};
                        if (!enemyBoard->shots[x][y]) //jeśli pole puste lub niezestrzelone, to strzelaj
                        {
                            shoot(enemyBoard, shot);
                            playerTurn = false;
                            snprintf(message, sizeof(message), "Gracz strzelil w (%d, %d)", x, y);
                            if(enemyBoard->BOARD[x][y]!=NULL)
                            {
                                ship *currShip = enemyBoard->BOARD[x][y];
                                bool sunk = true;
                                for (int i = 0; i < currShip->type; i++)
                                {
                                    if (!currShip->boardplace[i].got_shot)
                                    {
                                        sunk = false;
                                        break;
                                    }
                                }
                                if (sunk)
                                {
                                    snprintf(message, sizeof(message), "Gracz zatopil statek!");
                                }
                                playerTurn=true;
                            }
                        } 
                        else 
                        {
                            snprintf(message, sizeof(message), "Strzelales juz tutaj!");
                        }
                    }
                }
            } else {
                pair shot = AITurn(playerBoard);
                snprintf(message, sizeof(message), "Przeciwnik strzela w (%d, %d)", (int)shot.x + 1, (int)shot.y + 1);

                int shotX = (int)shot.x;
                int shotY = (int)shot.y;

                if (shotX >= 0 && shotX < BOARD_SIZE && shotY >= 0 && shotY < BOARD_SIZE && playerBoard->BOARD[shotX][shotY] != NULL) {
                    ship *currShip = playerBoard->BOARD[shotX][shotY];
                    bool sunk = true;
                    for (int i = 0; i < currShip->type; i++) {
                        if (!currShip->boardplace[i].got_shot) {
                            sunk = false;
                            break;
                        }
                    }
                    if (sunk) {
                        snprintf(message, sizeof(message), "Przeciwnik zatopił Twój statek!");
                    }
                    playerTurn = false;
                } else 
                {
                    playerTurn = true;
                }
            }
            VALIDCLICK:

            if (CheckWinCondition(playerBoard)) {
                gameState = GAME_AI_WON;
            } else if (CheckWinCondition(enemyBoard)) {
                gameState = GAME_PLAYER1_WON;
            }
        } else {
            if (gameState == GAME_PLAYER1_WON) {
                DrawText("Wygrywasz!", SCREENWIDTH / 2 - MeasureText("Wygrywasz!", 40) / 2, SCREENHEIGHT / 2 - 20, 40, GREEN);
            } else if (gameState == GAME_AI_WON) {
                DrawText("Przegrywasz!", SCREENWIDTH / 2 - MeasureText("Przegrywasz!", 40) / 2, SCREENHEIGHT / 2 - 20, 40, RED);
            }

            Rectangle playAgainButton = {SCREENWIDTH / 2 - 150, SCREENHEIGHT / 2 + 50, 300, 50};
            const char* buttonText = "Zagraj ponownie";
            DrawRectangleRec(playAgainButton, LIGHTGRAY);
            int textWidth = MeasureText(buttonText, 30);
            int textHeight = 30;
            int textX = playAgainButton.x + (playAgainButton.width - textWidth) / 2;
            int textY = playAgainButton.y + (playAgainButton.height - textHeight) / 2;
            DrawText(buttonText, textX, textY, 30, BLACK);

            Rectangle closeButton = {SCREENWIDTH / 2 - 100, SCREENHEIGHT / 2 + 120, 200, 50};
            DrawRectangleRec(closeButton, LIGHTGRAY);
            DrawText("Zamknij", closeButton.x + 70, closeButton.y + 10, 30, BLACK);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, playAgainButton)) {
                    ResetGame(&playerBoard, &enemyBoard, &playerShip, &enemyShip);
                    gameState = GAME_RUNNING;
                    playerTurn = true;
                    message[0] = '\0';
                } else if (CheckCollisionPointRec(mousePos, closeButton)) {
                    CloseWindow();
                    break;
                }
            }
        }

        EndDrawing();
    }
    StopMusicStream(sos);
    CloseWindow();
}

void PlayGame_PvP(board *player1Board, board *player2Board, ship *player1Ship, ship *player2Ship) {
    Music sos = LoadMusicStream("music/SOS_Signal.ogg");
    sos.looping = true;
    PlayMusicStream(sos);
    int player1OffsetX = (SCREENWIDTH * 1/3)-20 - (BOARD_SIZE * TILE_SIZE) / 2;
    int player1OffsetY = (SCREENHEIGHT - (BOARD_SIZE * TILE_SIZE)) / 2;
    int player2OffsetX = (SCREENWIDTH * 2/3)+20 - (BOARD_SIZE * TILE_SIZE) / 2;
    int player2OffsetY = (SCREENHEIGHT - (BOARD_SIZE * TILE_SIZE)) / 2;

    bool player1Turn = true;
    bool turnEnded = true; // Flaga do śledzenia końca tury
    GameState gameState = GAME_RUNNING;
    char message[128] = "";

    for (int alpha = 255; alpha >= 0; alpha-=5) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Gracz 1: Twoja plansza", player1OffsetX, player1OffsetY - 30, 20, BLACK);
        DrawBoard(player1Board, player1OffsetX, player1OffsetY, false);

         DrawText("Plansza przeciwnika", player2OffsetX, player2OffsetY - 30, 20, BLACK);
        DrawBoard(player2Board, player2OffsetX, player2OffsetY, true);
        DrawText(message, SCREENWIDTH / 2 - MeasureText(message, 20) / 2, SCREENHEIGHT - 50, 20, DARKGRAY);

        // Draw the fading overlay
        DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, (Color){0, 0, 0, alpha});

        EndDrawing();

        // Add a small delay to make the transition visible
        usleep(40000);
    }

    while (!WindowShouldClose()) {
        UpdateMusicStream(sos);
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (IsKeyPressed(KEY_ESCAPE)) {
            CloseWindow();
            break;
        }

        if (gameState == GAME_RUNNING) {
            if (turnEnded) { // Wyświetlanie przycisku do zmiany tury, tak aby gracze nie widzieli plansz przeciwnika
                BeginDrawing();
                ClearBackground(RAYWHITE);

                Rectangle newTurnButton = {SCREENWIDTH / 2 - 100, SCREENHEIGHT / 2 + 50, 200, 50};
                DrawRectangleRec(newTurnButton, LIGHTGRAY);
                DrawText("Nowa tura", newTurnButton.x + 30, newTurnButton.y + 10, 30, BLACK);

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
                                            snprintf(message, sizeof(message), "Gracz 1 zatopił statek!");
                                        }
                                    }
                                    turnEnded = true;
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
                                            snprintf(message, sizeof(message), "Gracz 2 zatopił statek!");
                                        }
                                    }
                                    turnEnded = true;
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
        } else {
            // Wyświetlenie wyniku gry
            if (gameState == GAME_PLAYER1_WON) {
                DrawText("Gracz 1 Wygrywa!", SCREENWIDTH / 2 - MeasureText("Gracz 1 Wygrywa!", 40) / 2, SCREENHEIGHT / 2 - 20, 40, GREEN);
            } else if (gameState == GAME_PLAYER2_WON) {
                DrawText("Gracz 2 Wygrywa!", SCREENWIDTH / 2 - MeasureText("Gracz 2 Wygrywa!", 40) / 2, SCREENHEIGHT / 2 - 20, 40, GREEN);
            }

            Rectangle playAgainButton = {SCREENWIDTH / 2 - 150, SCREENHEIGHT / 2 + 50, 300, 50};
                DrawRectangleRec(playAgainButton, LIGHTGRAY);
                DrawText("Zagraj ponownie", playAgainButton.x + 30, playAgainButton.y + 10, 30, BLACK);

                Rectangle closeButton = {SCREENWIDTH / 2 - 100, SCREENHEIGHT / 2 + 120, 200, 50};
                DrawRectangleRec(closeButton, LIGHTGRAY);
                DrawText("Zamknij", closeButton.x + 70, closeButton.y + 10, 30, BLACK);

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, playAgainButton)) {
                    ResetGame(&player1Board, &player2Board, &player1Ship, &player2Ship);
                    gameState = GAME_RUNNING;
                    player1Turn = true;
                    turnEnded = false;
                    message[0] = '\0';
                } else if (CheckCollisionPointRec(mousePos, closeButton)) {
                    CloseWindow();
                    break;
                }
            }
        }

        EndDrawing();
    }
    StopMusicStream(sos);
    CloseWindow();
}

void UpdateSlider(struct slider* s){
	s->hitbox.x = s->left - s->hand_texture.width / 2;
    s->hitbox.y = s->y_pos;
	s->hitbox.width = s->sl_texture.width + s->hand_texture.width;
	s->hitbox.height = s->sl_texture.height;

	if(s->isActive){
		if((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), s->hitbox)) || (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && s->isUpdating)){

		s->isUpdating = true;

		}
		else {
			s->isUpdating = false;
		}

		if(s->isUpdating){
			if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){

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
		}
		DrawTexture(s->sl_texture, (int)s->left, (int)s->y_pos, RAYWHITE);
		DrawTexture(s->hand_texture, (int)s->handle, (int)s->y_pos, RAYWHITE);

		snprintf(s->valText, sizeof(s->valText), "%d", (int)(s->val * 100));
		DrawText(s->valText, (int)(s->right + s->hand_texture.width / 2), (int)(s->y_pos), s->hitbox.height, BLACK);

	}
}

GameState PreGame() 
{
    Music pirent = LoadMusicStream("music/Pirates-entertaiment.ogg");
    pirent.looping = true;
    PlayMusicStream(pirent);

    Texture2D titleTexture = LoadTexture("Napisy/tytul.png");
    Texture2D onePlayerTexture = LoadTexture("Napisy/jedengracz.png");
    Texture2D twoPlayersTexture = LoadTexture("Napisy/dwochgraczy.png");
    Texture2D backgroundTexture = LoadTexture("Napisy/tlo.png");
    Texture2D buttonTexture = LoadTexture("Napisy/przycisk.png");

    while (!WindowShouldClose()) 
    {
        UpdateMusicStream(pirent);

        BeginDrawing();
        DrawTexture(backgroundTexture, -20, -20, WHITE);

        // Skalowanie i rysowanie tytułu gry
        float titleScale = 1.5f; // Skala tytułu
        int titlePosX = SCREENWIDTH / 2 - (titleTexture.width * titleScale) / 2;
        int titlePosY = SCREENHEIGHT / 2 - 280; 
        DrawTextureEx(titleTexture, (Vector2){titlePosX, titlePosY+30}, 0.0f, titleScale, WHITE);
    
        int buttonSpacing = 50;
        int buttonWidth = 300;  // Docelowa szerokość przycisków
        int buttonHeight = 100; // Docelowa wysokość przycisków
        float buttonScaleX = (float)buttonWidth / buttonTexture.width;
        //float buttonScaleY = (float)buttonHeight / buttonTexture.height; unused variable /shrug

        Rectangle buttonOnePlayer = { SCREENWIDTH / 2 - buttonWidth / 2, SCREENHEIGHT / 2 - buttonHeight / 2, buttonWidth, buttonHeight+43};
        Rectangle buttonTwoPlayers = { SCREENWIDTH / 2 - buttonWidth / 2, SCREENHEIGHT / 2 - buttonHeight / 2 + buttonHeight + buttonSpacing , buttonWidth, buttonHeight+43};

        // Rysowanie tekstur przycisków
        DrawTextureEx(buttonTexture, (Vector2){buttonOnePlayer.x, buttonOnePlayer.y}, 0.0f, buttonScaleX, WHITE);
        DrawTextureEx(buttonTexture, (Vector2){buttonTwoPlayers.x, buttonTwoPlayers.y}, 0.0f, buttonScaleX, WHITE);

        // Rysowanie obrazów wewnątrz przycisków
        float onePlayerImageScale = fminf((float)buttonWidth / onePlayerTexture.width - 0.15, (float)buttonHeight / onePlayerTexture.height);
        float twoPlayersImageScale = fminf((float)buttonWidth / twoPlayersTexture.width - 0.15, (float)buttonHeight / twoPlayersTexture.height);

        int onePlayerImageX = buttonOnePlayer.x + (buttonWidth - onePlayerTexture.width * onePlayerImageScale) / 2;
        int onePlayerImageY = buttonOnePlayer.y + (buttonHeight - onePlayerTexture.height * onePlayerImageScale) / 2;
        int twoPlayersImageX = buttonTwoPlayers.x + (buttonWidth - twoPlayersTexture.width * twoPlayersImageScale) / 2;
        int twoPlayersImageY = buttonTwoPlayers.y + (buttonHeight - twoPlayersTexture.height * twoPlayersImageScale) / 2;

        DrawTextureEx(onePlayerTexture, (Vector2){onePlayerImageX, onePlayerImageY+20}, 0.0f, onePlayerImageScale, WHITE);
        DrawTextureEx(twoPlayersTexture, (Vector2){twoPlayersImageX, twoPlayersImageY+20}, 0.0f, twoPlayersImageScale, WHITE);


        // Wykrywanie kliknięcia myszką
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, buttonOnePlayer) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            EndDrawing();
            StopMusicStream(pirent);
            return GAME_PREPARE1;
        } 
        else if (CheckCollisionPointRec(mousePoint, buttonTwoPlayers) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            EndDrawing();
            StopMusicStream(pirent);
            return GAME_PREPARE2;
        }

        // Zmiana koloru przycisku po najechaniu myszką
        if (CheckCollisionPointRec(mousePoint, buttonOnePlayer)) {
            DrawRectangleLinesEx(buttonOnePlayer, 2, DARKBLUE);
        }
        if (CheckCollisionPointRec(mousePoint, buttonTwoPlayers)) {
            DrawRectangleLinesEx(buttonTwoPlayers, 2, DARKBLUE);
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
    return GAME_RUNNING; // Default state if the window is closed
}
