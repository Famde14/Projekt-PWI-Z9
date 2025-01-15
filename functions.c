#include "raylib.h"
#include "structs.h"
#include "functions.h"
#include <stdlib.h>
#include <stddef.h>//dla definicji nulla 
#include <stdio.h>//do wywalenia ale to jak bedzie interfejs

void UpdateHitbox(struct ship* s) {		//ustala pozycję i wymiary hitbox'u
    s->hitbox.x = s->pos.x;
    s->hitbox.y = s->pos.y;
    s->hitbox.width = s->texture.width;
    s->hitbox.height = s->texture.height;
}

void mouse_drag(int key, struct ship* s, Color color){	//ustala położenie obiektu po pozycji myszy
	if(IsMouseButtonDown(key)){
		s->pos.x = GetMouseX() - s->texture.width / 2;		//poprawka na pozycję myszy
		s->pos.y = GetMouseY() - s->texture.height / 2;
		ClearBackground(color);						//usunięcie poprzedniej kopii obiektu
	}
}

void rotate(int key, Image* sprite, Texture2D* texture){	//ustala rotację obiektu
	UnloadTexture(*texture);					//usuwa poprednią teksturę

	if(key=='E') ImageRotateCCW(sprite);		//obrót zgodnie z ruchem wskazówek zegara
	else ImageRotateCCW(sprite);				//obrót przeciwnie z ruchem wskazówek zegara

	*texture = LoadTextureFromImage(*sprite);	//załaduj nową teksturę
}

void UpdateShip(bool* isDragging, struct ship* s)
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

        if(IsKeyPressed('E')) rotate('E', &s->sprite, &s->texture);
        if(IsKeyPressed('Q')) rotate('Q', &s->sprite, &s->texture);
	}
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

void placeStatek(board* boardtab,ship* curr_ship,pair begin,int direction)//1-gora 2-prawo 3-dol 4-lewo 
	{ 	
        printf("typ: %d kierunek:%d\n",curr_ship->type,direction);															//nie kladzie statku jestli jest on zle polozony(nie zwraca bledu) 
		switch (direction)
		{
		case 1:
		for (int i = 0; i < (int)(curr_ship->type); i++)
		{	pair tpair = {begin.x,(begin.y+i)};
			if (!isLegal(boardtab,tpair))
			{
				return;
			}
			//else{break;}
		}
		for (int i = 0; i < (int)(curr_ship->type); i++)
		{	shiptile temp = {{begin.x,(begin.y+i)},0};
			boardtab->BOARD[(unsigned int)begin.x][(unsigned int)begin.y+i]=curr_ship;
			curr_ship->boardplace[i]=temp;
		}
		
			break;
		case 2:
		for (int i = 0; i < (int)(curr_ship->type); i++)
		{	pair tpair = {begin.x+i,(begin.y)};
			if (!isLegal(boardtab,tpair))
			{
				return;
			}
			//else{break;}
		}
		for (int i = 0; i < (int)(curr_ship->type); i++)
		{	shiptile temp = {{begin.x+i,(begin.y)},0};
			boardtab->BOARD[(unsigned int)begin.x+i][(unsigned int)begin.y]=curr_ship;
			curr_ship->boardplace[i]=temp;
		}
		
			break;
			case 3:
		for (int i = 0; i < (int)(curr_ship->type); i++)
		{	pair tpair = {begin.x,(begin.y-i)};
			if (!isLegal(boardtab,tpair))
			{
				return;
			}
			//else{break;}
		}
		for (int i = 0; i < (int)(curr_ship->type); i++)
		{	shiptile temp = {{begin.x,(begin.y-i)}, false};//struktura zagnieżdżona musi mieć {}
			boardtab->BOARD[(unsigned int)begin.x][(unsigned int)begin.y-i]=curr_ship;
			curr_ship->boardplace[i]=temp;
		}
		
			break;
			case 4:
		for (int i = 0; i < (int)(curr_ship->type); i++)
		{	pair tpair = {begin.x-i,(begin.y)};
			if (!isLegal(boardtab,tpair))
			{
				
				return;
			}
			//else{break;}
		}
		for (int i = 0; i < (int)(curr_ship->type); i++)
		{	shiptile temp = {{begin.x-i,(begin.y)}, false};
			boardtab->BOARD[(unsigned int)begin.x-i][(unsigned int)begin.y]=curr_ship;
			curr_ship->boardplace[i]=temp;
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
    
struct array_cordinals* Get_array_cordinals(int offsetX, int offsetY) {
    struct array_cordinals* cordinal = (struct array_cordinals*)malloc(sizeof(struct array_cordinals));
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

void PlayGame(board *playerBoard, board *enemyBoard, ship *playerShip, ship *enemyShip) {
    int playerOffsetX = 50;
    int playerOffsetY = 100;
    int enemyOffsetX = 500;
    int enemyOffsetY = 100;

    const int screenWidth = 2 * BOARD_SIZE * TILE_SIZE + 3 * playerOffsetX;
    const int screenHeight = BOARD_SIZE * TILE_SIZE + 2 * playerOffsetY;

    InitWindow(screenWidth, screenHeight, "The Statki Game");

    bool playerTurn = true;
    GameState gameState = GAME_RUNNING;
    char message[128] = "";

    while (!WindowShouldClose()) {
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

            DrawText(message, screenWidth / 2 - MeasureText(message, 20) / 2, screenHeight - 50, 20, DARKGRAY);

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
                            } else {
                                snprintf(message, sizeof(message), "Strzelałeś już tutaj!");
                            }
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
                } else {
                    playerTurn = true;
                }
            }
			      VALIDCLICK:
            // Check win conditions
            if (CheckWinCondition(playerBoard)) {
                gameState = GAME_AI_WON;
            } else if (CheckWinCondition(enemyBoard)) {
                gameState = GAME_PLAYER1_WON;
            }
        } else {
            if (gameState == GAME_PLAYER1_WON) {
                DrawText("Wygrywasz!", screenWidth / 2 - MeasureText("Wygrywasz!", 40) / 2, screenHeight / 2 - 20, 40, GREEN);
            } else if (gameState == GAME_AI_WON) {
                DrawText("Przegrywasz!", screenWidth / 2 - MeasureText("Przegrywasz!", 40) / 2, screenHeight / 2 - 20, 40, RED);
            }

            Rectangle playAgainButton = {screenWidth / 2 - 150, screenHeight / 2 + 50, 300, 50};
            const char* buttonText = "Zagraj ponownie";
            DrawRectangleRec(playAgainButton, LIGHTGRAY);
            int textWidth = MeasureText(buttonText, 30);
            int textHeight = 30;
            int textX = playAgainButton.x + (playAgainButton.width - textWidth) / 2;
            int textY = playAgainButton.y + (playAgainButton.height - textHeight) / 2;
            DrawText(buttonText, textX, textY, 30, BLACK);

            Rectangle closeButton = {screenWidth / 2 - 100, screenHeight / 2 + 120, 200, 50};
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

    CloseWindow();
}

void PlayGame_PvP(board *player1Board, board *player2Board, ship *player1Ship, ship *player2Ship) {
    int player1OffsetX = 50;
    int player1OffsetY = 100;
    int player2OffsetX = 500;
    int player2OffsetY = 100;

    const int screenWidth = 2 * BOARD_SIZE * TILE_SIZE + 3 * player1OffsetX;
    const int screenHeight = BOARD_SIZE * TILE_SIZE + 2 * player1OffsetY;

    InitWindow(screenWidth, screenHeight, "The Statki Game - Gracz vs Gracz");

    bool player1Turn = true;
    bool turnEnded = true; // Flaga do śledzenia końca tury
    GameState gameState = GAME_RUNNING;
    char message[128] = "";

    while (!WindowShouldClose()) {
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

                Rectangle newTurnButton = {screenWidth / 2 - 100, screenHeight / 2 + 50, 200, 50};
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

                    DrawText(message, screenWidth / 2 - MeasureText(message, 20) / 2, screenHeight - 50, 20, DARKGRAY);

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

                    DrawText(message, screenWidth / 2 - MeasureText(message, 20) / 2, screenHeight - 50, 20, DARKGRAY);

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
                DrawText("Gracz 1 Wygrywa!", screenWidth / 2 - MeasureText("Gracz 1 Wygrywa!", 40) / 2, screenHeight / 2 - 20, 40, GREEN);
            } else if (gameState == GAME_PLAYER2_WON) {
                DrawText("Gracz 2 Wygrywa!", screenWidth / 2 - MeasureText("Gracz 2 Wygrywa!", 40) / 2, screenHeight / 2 - 20, 40, GREEN);
            }

            Rectangle playAgainButton = {screenWidth / 2 - 150, screenHeight / 2 + 50, 300, 50};
                DrawRectangleRec(playAgainButton, LIGHTGRAY);
                DrawText("Zagraj ponownie", playAgainButton.x + 30, playAgainButton.y + 10, 30, BLACK);

                Rectangle closeButton = {screenWidth / 2 - 100, screenHeight / 2 + 120, 200, 50};
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

    CloseWindow();
}
