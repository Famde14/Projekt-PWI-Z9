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
	void (*updateShip)(struct ship*);	//aktualizuje pozycję i rotację obiektu
};
