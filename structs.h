#include "raylib.h"
//#include "functions.c"
#ifndef STRUCTS_H
#define STRUCTS_H
typedef struct pair{
	float x;
	float y;
}pair;
typedef struct boardplace
{
	pair cords;
	bool got_shot;
}boardplace;

typedef struct ship{
	struct pair pos;	//pozycja lewego górnego rogu obiektu
	Image sprite;
	Texture2D texture;
	Rectangle hitbox;
	bool isUpdating;	//true, gdy obiekt jest aktualnie przeciągany
	boardplace* boardplace; 
	char type; // 1 - 1maszt 2 - 2-maszt 3 - 3maszt 4- 4maszt 
	void (*updateHitbox)(struct ship*);	//ustala wymiary hitbox'u
	void (*updateShip)(bool* isDragging, struct ship*);	//aktualizuje pozycję i rotację obiektu; isDragging ma spwawdzać, czy jakiś statek nie jest już przeciągany
}ship;
typedef struct board	// temp tablica (nie wiem jeszcz jak dokladnie zamierzamy sprawdzac poprawnosc polozenia)
{
	ship* BOARD[10][10];
}board;
#endif