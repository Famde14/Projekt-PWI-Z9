#include "raylib.h"
//#include "functions.c"
#ifndef STRUCTS_H
#define STRUCTS_H
#define BOARD_SIZE 10
#define TILE_SIZE 40

typedef enum { //struct przechowujący stan gry
    GAME_RUNNING,
    GAME_PLAYER1_WON,
	GAME_PLAYER2_WON,
    GAME_AI_WON
} GameState;

typedef struct pair{
	float x;
	float y;
}pair;

typedef struct shiptile
{
	pair cords;
	bool got_shot;
}shiptile;

typedef struct ship{
	struct pair pos;	//pozycja lewego górnego rogu obiektu
	Image sprite;
	Texture2D texture;
	Rectangle hitbox;
	bool isUpdating;	//true, gdy obiekt jest aktualnie przeciągany
	shiptile* boardplace; 
	char type; // 1 - 1maszt 2 - 2-maszt 3 - 3maszt 4- 4maszt 
	void (*updateHitbox)(struct ship*);	//ustala wymiary hitbox'u
	void (*updateShip)(bool* isDragging, struct ship*);	//aktualizuje pozycję i rotację obiektu; isDragging ma spwawdzać, czy jakiś statek nie jest już przeciągany
}ship;

typedef struct {
    ship *BOARD[BOARD_SIZE][BOARD_SIZE]; //plansza gracza
    bool shots[BOARD_SIZE][BOARD_SIZE]; //strzały gracza
} board;
//struct z hit_headers.h
struct array_cordinals{//współrzędne w sensie trafiania statku
    int x;//od 0 do 9
    int y;//od 0 do 9
};

struct slider{
	float val;			//wartość od 0.0 fo 1.0
	char valText[4];	//służy do wyświetlania wartości
	bool isActive;		//czy suwak został włączony (jeśli będzie menu pauzy to się przyda)
						//unload assetów do zrobienia
	bool isUpdating;	//true, gdy hand_texture jest aktualnie przeciągane
	float y_pos;		//pozycja góry suwaka
	float left;			//koordynaty lewej granicy
	float right;		//koordynaty prawej granicy
	Image sl_sprite;	//sprite tyłu suwaka
	Texture2D sl_texture;
	float handle;		//koordynaty główki
	Image hand_sprite;	//sprite główki
	Texture2D hand_texture;
	Rectangle hitbox;	//hitbox tyłu suwaka
};

#endif
