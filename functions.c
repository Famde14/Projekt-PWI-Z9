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
		board* boardtab=malloc(sizeof(board));
		for (int i = 0; i < 10; i++)
		{
			for (int k = 0; k < 10; k++)
			{
				boardtab->BOARD[i][k]=NULL;
			}
			
		}
		return boardtab;
	};
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

	void shoot(board* player,pair paira){//funkcja nie zwraca trafienia.
		ship* curr_ship =player->BOARD[(unsigned int)paira.x][(unsigned int)paira.y];
		if(curr_ship!=NULL)
		{
			beingshot(curr_ship,paira);
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

