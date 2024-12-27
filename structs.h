#ifndef STRUCTS_H
#define STRUCTS_H

#include "raylib.h"

struct pair{
	float x;
	float y;
};

struct ship{
	struct pair pos;	//pozycja lewego górnego rogu obiektu
	Image sprite;
	Texture2D texture;
	Rectangle hitbox;
	bool isUpdating;	//true, gdy obiekt jest aktualnie przeciągany

	void (*updateHitbox)(struct ship*);	//ustala wymiary hitbox'u
	void (*updateShip)(bool* isDragging, struct ship*);	//aktualizuje pozycję i rotację obiektu; isDragging ma spwawdzać, czy jakiś statek nie jest już przeciągany
};

#endif // STRUCTS_H