#include "raylib.h"
#include "structs.h"
#include "functions.h"
#include <stddef.h>//dla definicji nulla 
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
	board* initboard()
	{
		board* board=malloc(sizeof(board));
		for (int i = 0; i < 10; i++)
		{
			for (int k = 0; k < 10; k++)
			{
				board->BOARD[i][k]=NULL;
			}
			
		}
		return board;
	};
	board delboard(board* boardtab)
	{
		if(boardtab!=NULL){free(boardtab);}//nie zwolnie statkow gdyz musialbym sledzic czy dany statek nie zostal zwolniony wczesniej. Normalnie to od tego bylyby smart pointery ale jako ze to c to bedzie to problem osoby inicjujacej statek
	};
	void placeStatek(board* board,ship* ship,pair begin,int direction)//0-gora 1-prawo 2-dol 3-lewo 
	{ 																//nie kladzie statku jestli jest on zle polozony(nie zwraca bledu) 
		switch (direction)
		{
		case 0:
		for (int i = 0; i < (int)(ship->type); i++)
		{	pair pair = {begin.x,(begin.y+i)};
			if (!isLegal(board,pair))
			{
				return;
			}
			//else{break;}
		}
		for (int i = 0; i < (int)(ship->type); i++)
		{
			board->BOARD[(unsigned int)begin.x][(unsigned int)begin.y+i]=ship;
		}
		
			break;
		case 1:
		for (int i = 0; i < (int)(ship->type); i++)
		{	pair pair = {begin.x+i,(begin.y)};
			if (!isLegal(board,pair))
			{
				return;
			}
			//else{break;}
		}
		for (int i = 0; i < (int)(ship->type); i++)
		{
			board->BOARD[(unsigned int)begin.x+i][(unsigned int)begin.y]=ship;
		}
		
			break;
			case 2:
		for (int i = 0; i < (int)(ship->type); i++)
		{	pair pair = {begin.x,(begin.y-i)};
			if (!isLegal(board,pair))
			{
				return;
			}
			//else{break;}
		}
		for (int i = 0; i < (int)(ship->type); i++)
		{
			board->BOARD[(unsigned int)begin.x][(unsigned int)begin.y-i]=ship;
		}
		
			break;
			case 3:
		for (int i = 0; i < (int)(ship->type); i++)
		{	pair pair = {begin.x-i,(begin.y)};
			if (!isLegal(board,pair))
			{
				
				return;
			}
			//else{break;}
		}
		for (int i = 0; i < (int)(ship->type); i++)
		{
			board->BOARD[(unsigned int)begin.x-i][(unsigned int)begin.y]=ship;
		}
		
			break;
		default:
			break;
		}
	}

	bool isLegal(board* player,pair tile)
	{	
		if(tile.y>9||tile.y<0||tile.x>9||tile.x<9){return 0;}//jesli siega poza tabele return 0
		for (int i = tile.x-1; i <= tile.x+1; i++)
		{
			for (int k = tile.y-1; k <= tile.y+1; k++)
			{
				if (i>9||k>9||k<0||i<0||player->BOARD[i][k]==NULL)
				//napisalem tak bo bylo mi latwiej w glowie wymienic warunki, niech ktos(albo ja ) przepisze to tak, aby nie bytlo else'a
				//obecne warunki to - nie sasiaduje z innymi statkami
				{
					
				}
			}
		}
		return 1;
	}
	void shoot(board* player,pair pair){//funkcja nie zwraca trafienia.
		ship* ship =player->BOARD[(unsigned int)pair.x][(unsigned int)pair.y];
		if(ship!=NULL)
		{
		beingshot(ship,pair);
		}
	}
	void beingshot(ship* ship,pair pair)
	{
		for (int i = 0; i < ship->type; i++)
		{
			if((ship->boardplace[i]).cords.x==pair.x&&(ship->boardplace[i]).cords.y==pair.y)//mic nie sugeruje ale to byloby mniej brzydsze jakby uzyc klas
			{(ship->boardplace[i]).got_shot=1;}
		}
	}

	void drawboard(board* board,int screenwidth,int screenheight)//funkcja rysuje tablice danego gracza. Nie mmay jeszcze spritow, alej jak bedziemy mieli to wystarczy zmienic tutaj by drukowalo
	{
		for (int i = 0; i < 10; i++)
		{
			for (int k = 0 ;k <10 ; k++)
			{
				if(board->BOARD[i][k]==NULL){
					DrawRectangle(screenwidth/12*(i+1),screenheight/12*(k+1),screenwidth/12,screenheight/12,BLACK);
					DrawRectangle(screenwidth/12*(i+1)+1,screenheight/12*(k+1)+1,screenwidth/12-2,screenheight/12-2,WHITE);
					//DrawCircle(300,300,30,YELLOW);
				}
			}
		}
	};
;

