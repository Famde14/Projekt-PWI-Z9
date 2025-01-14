#include "raylib.h"
#include "structs.h"
#include "functions.h"
#include <stdlib.h>
#include <stddef.h>//dla definicji nulla 
#include <stdio.h>//do wywalenia ale to jak bedzie interfejs

#define MAX_SHIPS 10

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
void SnapToGrid(ship *s, int gridStartX, int gridStartY, int cellSize)
{
	s->pos.x = gridStartX + ((int)((s->pos.x - gridStartX) / cellSize)) * cellSize;
	s->pos.y = gridStartY + ((int)((s->pos.y - gridStartY) / cellSize)) * cellSize;
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
			s->boardplace[i].cords.x = (s->pos.x - gridStartX) / cellSize;
			s->boardplace[i].cords.y = (s->pos.y - gridStartY) / cellSize + i;
		}
		else if (s->kierunek == 1)
		{
			s->boardplace[i].cords.x = (s->pos.x - gridStartX) / cellSize + i;
			s->boardplace[i].cords.y = (s->pos.y - gridStartY) / cellSize;
		}
		else if (s->kierunek == 2)
		{
			s->boardplace[i].cords.x = (s->pos.x - gridStartX) / cellSize;
			s->boardplace[i].cords.y = (s->pos.y - gridStartY) / cellSize + i;
		}
		else if(s->kierunek == 3)
		{
			s->boardplace[i].cords.x = (s->pos.x - gridStartX) / cellSize + i;
			s->boardplace[i].cords.y = (s->pos.y - gridStartY) / cellSize;
		}
		// s->boardplace[i].cords.x = (s->pos.x - gridStartX) / cellSize + i;
		// s->boardplace[i].cords.y = (s->pos.y - gridStartY) / cellSize;
		// s->boardplace[i].got_shot = false;
	}

	// do pomocniczego wypisywania
	PrintShipPositions(s);
}

void PrintShipPositions(ship *s)
{
	printf("Ship positions (grid coordinates):\n");
	for (int i = 0; i < s->length; i++)
	{
		printf("Kierunek: %d Tile %d: (%.0f, %.0f)\n", s->kierunek, i, s->boardplace[i].cords.x, s->boardplace[i].cords.y);
	}
}

GameData GameSet() {
    int screenWidth = 1200;
    int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "The Statki Game");
    SetTargetFPS(60);

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

    //inicjalizacja tablicy gracza
    board *playerBoard = initboard();

    //incjalizacja statków
    ship *playerShips = malloc(MAX_SHIPS * sizeof(ship));

    int startX = screenWidth * 1 / 4 - (gridSize * cellSize) / 2; //wyśrodkowanie statków
    int startY = (screenHeight - (gridSize * cellSize)) / 2;
    int gridStartX = screenWidth * 3 / 4 - (gridSize * cellSize) / 2;
    int gridStartY = (screenHeight - (gridSize * cellSize)) / 2;
    int shipIndex = 0;

    for (int i = 0; i < 4; i++) {
        int spacing = cellSize * 1 + cellSize;
        playerShips[shipIndex] = (ship){
            .pos = { startX + i * spacing, startY },
            .sprite = ship1Images[i],
            .texture = ship1Textures[i],
            .hitbox = { startX + i * spacing, startY, ship1Textures[i].width, ship1Textures[i].height },
            .isUpdating = false,
            .length = 1,
			.kierunek = 1,
            .updateHitbox = UpdateHitbox,
            .updateShip = UpdateShip,
            .boardplace = malloc(1 * sizeof(shiptile))
        };
        shipIndex++;
    }
    for (int i = 0; i < 3; i++) {
        int spacing = cellSize * 2 + cellSize;
        playerShips[shipIndex] = (ship){
            .pos = { startX + i * spacing, startY + cellSize },
            .sprite = ship2Images[i],
            .texture = ship2Textures[i],
            .hitbox = { startX + i * spacing, startY + cellSize, ship2Textures[i].width, ship2Textures[i].height },
            .isUpdating = false,
            .length = 2,
			.kierunek = 1,
            .updateHitbox = UpdateHitbox,
            .updateShip = UpdateShip,
            .boardplace = malloc(2 * sizeof(shiptile))
        };
        shipIndex++;
    }
    for (int i = 0; i < 2; i++) {
        int spacing = cellSize * 3 + cellSize;
        playerShips[shipIndex] = (ship){
            .pos = { startX + i * spacing, startY + 2 * cellSize },
            .sprite = ship3Images[i],
            .texture = ship3Textures[i],
            .hitbox = { startX + i * spacing, startY + 2 * cellSize, ship3Textures[i].width, ship3Textures[i].height },
            .isUpdating = false,
            .length = 3,
			.kierunek = 1,
            .updateHitbox = UpdateHitbox,
            .updateShip = UpdateShip,
            .boardplace = malloc(3 * sizeof(shiptile))
        };
        shipIndex++;
    }
    int spacing = cellSize * 4 + cellSize;
    playerShips[shipIndex] = (ship){
        .pos = { startX, startY + 3 * cellSize },
        .sprite = ship4Images[0],
        .texture = ship4Textures[0],
        .hitbox = { startX, startY + 3 * cellSize, ship4Textures[0].width, ship4Textures[0].height },
        .isUpdating = false,
        .length = 4,
		.kierunek = 1,
        .updateHitbox = UpdateHitbox,
        .updateShip = UpdateShip,
        .boardplace = malloc(4 * sizeof(shiptile))
    };
    shipIndex++;

    bool isDragging = false;

    while (!WindowShouldClose())
    {
        // Update ships
        for (int i = 0; i < MAX_SHIPS; i++) {
            playerShips[i].updateShip(&isDragging, &playerShips[i]);
            if (isDragging && playerShips[i].isUpdating) {
                if (playerShips[i].pos.x >= gridStartX && playerShips[i].pos.x <= gridStartX + gridSize * cellSize &&
                    playerShips[i].pos.y >= gridStartY && playerShips[i].pos.y <= gridStartY + gridSize * cellSize) {
                    SnapToGrid(&playerShips[i], gridStartX, gridStartY, cellSize);
                }
            }
        }


        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            for (int i = 0; i < MAX_SHIPS; i++) {
                if (playerShips[i].isUpdating) {
                    // Check if the ship is within the grid boundaries
                    if (playerShips[i].pos.x >= gridStartX && playerShips[i].pos.x <= gridStartX + gridSize * cellSize &&
                        playerShips[i].pos.y >= gridStartY && playerShips[i].pos.y <= gridStartY + gridSize * cellSize) {
                        SnapToGrid(&playerShips[i], gridStartX, gridStartY, cellSize);
                    }
                    playerShips[i].isUpdating = false;
                    isDragging = false;
                }
            }
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, BLACK); // Pionowa linia

        DrawText("Press ESC to exit", 10, 10, 20, DARKGRAY);

        for (int i = 0; i < gridSize; i++) {
            char label[3]; // Increased size to accommodate two-digit numbers
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

        // Draw ships
        for (int i = 0; i < MAX_SHIPS; i++) {
            DrawTexture(playerShips[i].texture, (int)playerShips[i].pos.x, (int)playerShips[i].pos.y, WHITE);
        }
		
        EndDrawing();
    }

    //Układanie statków na w zmiennej playerBoard
    for (int i = 0; i < MAX_SHIPS; i++) {

    int gridX = (playerShips[i].pos.x - gridStartX)/cellSize;
    int gridY = (playerShips[i].pos.y - gridStartY)/cellSize;

    if(gridX>=0&&gridY>=0){
		int dl=playerShips[i].length;
		ship *playerS = initship(dl);
		placeStatek(playerBoard, playerS, (pair){gridX, gridY}, playerShips[i].kierunek);
	}
}
	printf("Contents of playerBoard:\n");
    printboard(playerBoard);

    // Unload textures
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

    GameData gameData = {playerBoard, playerShips, MAX_SHIPS};
    return gameData;
}








//tu gameplay

	ship* initship(int type)
	{													//trzeba bedzie zaktualizowac funkcje tak aby aktualizowala polozenie,hitbox i sprite w interfejsie graficznym. 
														//funkcja spelnia absolutne minimum do testowania mechanik
		ship* statek =malloc(sizeof(ship));
		statek->boardplace=malloc(type*sizeof(shiptile));
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
		free(statek->boardplace);
		free(statek);
	}
board* initboard()
{
   	board *newBoard = (board*)malloc(sizeof(board));
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
	};

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
					return 0;}
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
			shiptile temp = {begin.x, (begin.y + i)};
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
			shiptile temp = {begin.x + i, (begin.y)};
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
	};
;
array_cordinals* Get_array_cordinals(int offsetX, int offsetY) {
    array_cordinals* cordinal = (array_cordinals*)malloc(sizeof(array_cordinals));
    if (cordinal == NULL) return NULL;

    int x = GetMouseX();
    int y = GetMouseY();

    x -= offsetX;
    y -= offsetY;

    x = x / TILE_SIZE;
    y = y / TILE_SIZE;

    if (x < 0 || x >= 9 || y < 0 || y >= 9)
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
    delboard(*playerBoard);
    delboard(*enemyBoard);
    delship(*playerShip);
    delship(*enemyShip);

    *playerBoard = initboard();
    *enemyBoard = initboard();
	/*reczne dodawanie statkow, pozniej tego nie bedzie, bo zacznie sie funkcja z ustawianiem przez uzytkownika*/
    *playerShip = initship(3);
    *enemyShip = initship(3);

    pair playerStart = {2, 2};
    pair enemyStart = {4, 4};

    placeStatek(*playerBoard, *playerShip, playerStart, 2);
    placeStatek(*enemyBoard, *enemyShip, enemyStart, 3);
};

void DrawBoard(board *playerBoard, int offsetX, int offsetY, bool isEnemy) {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            Rectangle tile = {offsetX + x * TILE_SIZE, offsetY + y * TILE_SIZE, TILE_SIZE, TILE_SIZE};

            if (playerBoard->BOARD[x][y] == NULL) {
                if (playerBoard->shots[x][y]) {
                    DrawRectangle(tile.x, tile.y, tile.width, tile.height, LIGHTGRAY); // Color for missed shots
                }
                DrawRectangleLines(tile.x, tile.y, tile.width, tile.height, GRAY);
            }
            else {
                ship *currShip = playerBoard->BOARD[x][y];
                bool partShot = false;
                for (int i = 0; i < currShip->type; i++) {
                    if (currShip->boardplace[i].cords.x == x && currShip->boardplace[i].cords.y == y && currShip->boardplace[i].got_shot) {
                        partShot = true;
                        break;
                    }
                }

                if (partShot) {
                    DrawRectangle(tile.x, tile.y, tile.width, tile.height, RED);
                }
                else {
                    Color color = (isEnemy) ? WHITE : BLUE;
                    DrawRectangle(tile.x, tile.y, tile.width, tile.height, color);
                }
                DrawRectangleLines(tile.x, tile.y, tile.width, tile.height, DARKGRAY); // Border lines
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
void PlayGame(board *playerBoard, board *enemyBoard, ship *playerShips, ship *enemyShip) //na razie biore po jednym statku, ale w wersji bardziej grywalnej bedzie ich tu z oczywistych powodow wiecej
{
    //być może ta część wypadałoby żeby była w #define, ale na razie napisałam tak
    int playerOffsetX = 50;
    int playerOffsetY = 100;
    int enemyOffsetX = 500;
    int enemyOffsetY = 100;

    const int screenWidth = 2*BOARD_SIZE*TILE_SIZE + 3*playerOffsetX; //szerokość okna zależna od ustawień powyższych funkcji (offsetX, plasza_gracza, offsetX, plansza_enemy, offsetX)
    const int screenHeight = BOARD_SIZE*TILE_SIZE + 2*playerOffsetY; //wysokość okna zależna od ustawień powyższych funkcji (offsetY, plansza_gracza, offsetY)

    InitWindow(screenWidth, screenHeight, "The Statki Game");

    bool playerTurn = true;
    GameState gameState = GAME_RUNNING;
    char message[128] = "";

    while (!WindowShouldClose()) {
        BeginDrawing();

        if (IsKeyPressed(KEY_ESCAPE))
        {
            CloseWindow();
            break;
        }

        ClearBackground(RAYWHITE);

        if (gameState == GAME_RUNNING) {
            DrawText("Twoja plansza", playerOffsetX, playerOffsetY - 30, 20, BLACK);
            DrawBoard(playerBoard, playerOffsetX, playerOffsetY, false);

            DrawText("Plansza przeciwnika", enemyOffsetX, enemyOffsetY - 30, 20, BLACK);
            DrawBoard(enemyBoard, enemyOffsetX, enemyOffsetY, true);

            DrawText(message, screenWidth / 2 - MeasureText(message, 20) / 2, screenHeight - 50, 20, DARKGRAY);

            if (playerTurn) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    array_cordinals *cords = Get_array_cordinals(enemyOffsetX, enemyOffsetY);
                    if(cords==NULL) break;
                    int x = cords->x;
                    int y = cords->y;

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
                snprintf(message, sizeof(message), "Przeciwnik strzela w (%d, %d)", ((int)shot.x)+1, ((int)shot.y)+1);
                if(playerBoard->BOARD[(int)shot.x][(int)shot.y]!=NULL)
                {
                    ship *currShip = playerBoard->BOARD[(int)shot.x][(int)shot.y];
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
                        snprintf(message, sizeof(message), "Przeciwnik zatopil twoj statek!");
                    }
                    playerTurn=false;
                }
                else playerTurn = true;
            }

            // Check win conditions
            if (CheckWinCondition(playerBoard)) {
                //gameState = GAME_AI_WON;
            } else if (CheckWinCondition(enemyBoard)) {
                //gameState = GAME_PLAYER_WON;
            }
        } else {
            if (gameState == GAME_PLAYER_WON) {
                DrawText("Wygrywasz!", screenWidth / 2 - MeasureText("Wygrywasz!", 40) / 2, screenHeight / 2 - 20, 40, GREEN);
            } else if (gameState == GAME_AI_WON) {
                DrawText("Przegrywasz!", screenWidth / 2 - MeasureText("Przegrywasz!", 40) / 2, screenHeight / 2 - 20, 40, RED);
            }

            //tanio zrobiony playagainbutton - można poprawić jeśli ktoś ma ochotę
            Rectangle playAgainButton = {screenWidth / 2 - 150, screenHeight / 2 + 50, 300, 50};
			const char* buttonText = "Zagraj ponownie";
			DrawRectangleRec(playAgainButton, LIGHTGRAY);

			int textWidth = MeasureText(buttonText, 30);
			int textHeight = 30;
			
			int textX = playAgainButton.x + (playAgainButton.width - textWidth) / 2;
			int textY = playAgainButton.y + (playAgainButton.height - textHeight) / 2;

			DrawText(buttonText, textX, textY, 30, BLACK);

            //równie tani co poprzedni - closebutton
            Rectangle closeButton = {screenWidth / 2 - 100, screenHeight / 2 + 120, 200, 50};
            DrawRectangleRec(closeButton, LIGHTGRAY);
            DrawText("Zamknij", closeButton.x + 70, closeButton.y + 10, 30, BLACK);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mousePos = GetMousePosition();
                if (CheckCollisionPointRec(mousePos, playAgainButton)) {
                    ResetGame(&playerBoard, &enemyBoard, &playerShips, &enemyShip);
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

    CloseWindow();
};
