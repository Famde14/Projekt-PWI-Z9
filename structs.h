#include "raylib.h"
//#include "functions.c"
#ifndef STRUCTS_H
#define STRUCTS_H
#define BOARD_SIZE 10
#define TILE_SIZE 40
typedef enum { //struct przechowujący stan gry
    GAME_RUNNING,
    GAME_PLAYER_WON,
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

typedef struct ship {
    struct pair pos;    // pozycja lewego górnego rogu obiektu
    Image sprite;
    Texture2D texture;
    Rectangle hitbox;
    bool isUpdating;    // true, gdy obiekt jest aktualnie przeciągany
    int length;         // długość statku
    shiptile* boardplace;
    char type;          // 1 - 1maszt 2 - 2-maszt 3 - 3maszt 4- 4maszt
    int kierunek;       // 1-gora 2-prawo 3-dol 4-lewo 
    void (*updateHitbox)(struct ship*);    // ustala wymiary hitbox'u
    void (*updateShip)(bool* isDragging, struct ship*);    // aktualizuje pozycję i rotację obiektu; isDragging ma spwawdzać, czy jakiś statek nie jest już przeciągany
} ship;

typedef struct {
    ship *BOARD[BOARD_SIZE][BOARD_SIZE]; //plansza gracza
    bool shots[BOARD_SIZE][BOARD_SIZE]; //strzały gracza
} board;

//struct z hit_headers.h
typedef struct array_cordinals{//współrzędne w sensie trafiania statku
    int x;//od 0 do 9
    int y;//od 0 do 9
}array_cordinals;

typedef struct {
    board* playerBoard;
    ship* playerShips;
    int playerShipCount;
} GameData;
#endif