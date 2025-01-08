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

	if(key=='E') ImageRotateCW(sprite);		//obrót zgodnie z ruchem wskazówek zegara
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
		s->updateHitbox(s);									//aktualizacja hitboxu

        if(IsKeyPressed('E')) rotate('E', &s->sprite, &s->texture);
        if(IsKeyPressed('Q')) rotate('Q', &s->sprite, &s->texture);
	}
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
	{
        s->isUpdating = false;
        *isDragging = false;
    }
}
void SnapToGrid(struct ship* s, int gridStartX, int gridStartY, int cellSize) 		//przypisuje statek do kratki
{
    s->pos.x = gridStartX + ((int)((s->pos.x - gridStartX) / cellSize)) * cellSize;
    s->pos.y = gridStartY + ((int)((s->pos.y - gridStartY) / cellSize)) * cellSize;
    s->updateHitbox(s);
}
	ship* initship(int type)
	{													//trzeba bedzie zaktualizowac funkcje tak aby aktualizowala polozenie,hitbox i sprite w interfejsie graficznym. 
														//funkcja spelnia absolutne minimum do testowania mechanik
		ship* statek =malloc(sizeof(ship));
		statek->boardplace=malloc(type*sizeof(shiptile));
		for (int i = 0; i < type; i++)
		{
			statek->boardplace[i].got_shot=0;
		}
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
	void placeStatek(board* boardtab,ship* curr_ship,pair begin,int direction)//1-gora 2-prawo 3-dol 4-lewo 
	{ 	printf("typ: %d kierunek:%d\n",curr_ship->type,direction);															//nie kladzie statku jestli jest on zle polozony(nie zwraca bledu) 
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
		{	shiptile temp = {begin.x,(begin.y-i)};
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
		{	shiptile temp = {begin.x-i,(begin.y)};
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
	};
;
struct array_cordinals* Get_array_cordinals(int offsetX, int offsetY) {
    struct array_cordinals* cordinal = (struct array_cordinals*)malloc(sizeof(struct array_cordinals));
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
void PlayGame(board *playerBoard, board *enemyBoard, ship *playerShip, ship *enemyShip) //na razie biore po jednym statku, ale w wersji bardziej grywalnej bedzie ich tu z oczywistych powodow wiecej
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
                    struct array_cordinals *cords = Get_array_cordinals(enemyOffsetX, enemyOffsetY);
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
                gameState = GAME_AI_WON;
            } else if (CheckWinCondition(enemyBoard)) {
                gameState = GAME_PLAYER_WON;
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
};
