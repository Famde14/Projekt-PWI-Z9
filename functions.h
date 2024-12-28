#include "raylib.h"
#include "structs.h"
#ifndef FUNCTIONS_H
#define FUNCTIONS_H
/*interfejs graficzny*/ 
void UpdateHitbox(struct ship* s);	//ustala pozycję i wymiary hitbox'u
void mouse_drag(int key, struct ship* s, Color color);//ustala położenie obiektu po pozycji mysz}
void rotate(int key, Image* sprite, Texture2D* texture);	//ustala rotację obiektu
void UpdateShip(bool* isDragging, struct ship* s);	//wywołuje mouse_drag i rotate
/*plansza i interakcja z plansza*/ 
board* initboard();
void delboard(board* boardtab);
void placeStatek(board* board,ship* ship,pair begin,int direction);
void shoot(board* player,pair pair);//wymaga poprawnie zainicjowanego statku zeby dzialalo, a wiec potrzeba inicjatora
void drawboard(board* board,int screenwidth,int screenheight)//funkcja rysuje tablice danego gracza. Na razie drukuje tylko tile'a (funkcja potencjalnie do wymiany)
/*do dodania*/
//init_ship();
//del_ship();
#endif