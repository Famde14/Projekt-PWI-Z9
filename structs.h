#include "raylib.h"
//#include "functions.c"
#ifndef STRUCTS_H
#define STRUCTS_H

#define BOARD_SIZE 10
#define TILE_SIZE 40
#define MAX_SHIPS 10
#define SCREENWIDTH 1280
#define SCREENHEIGHT 720

typedef enum { //struct przechowujący stan gry
    GAME_START,
    GAME_PREPARE1,
    GAME_PREPARE2,
    GAME_RUNNING,
    GAME_PAUSED,
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
    struct pair pos; // pozycja lewego górnego rogu obiektu
    Image sprite;
    Texture2D texture;
    Rectangle hitbox;
    bool isUpdating;       // true, gdy obiekt jest aktualnie przeciągany
    bool isPlaced;         // true, gdy obiekt jest już ustawiony na planszy
    bool invalidPlacement; // true, gdy statek jest ustawiony niezgodnie z zasadami gry
    int length;            // długość statku
    shiptile *boardplace;
    char type;                                           // 1 - 1maszt 2 - 2-maszt 3 - 3maszt 4- 4maszt
    int kierunek;                                        // 1-gora 2-prawo 3-dol 4-lewo
    void (*updateHitbox)(struct ship *);                 // ustala wymiary hitbox'u
    void (*updateShip)(bool *isDragging, struct ship *); // aktualizuje pozycję i rotację obiektu; isDragging ma spwawdzać, czy jakiś statek nie jest już przeciągany
}ship;

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

typedef struct Button{
    pair pos;
    bool isActive;
    bool isUnderMouse;

    Image sprite;
    Texture2D texture;

    int fontsize;
    char *text;

    Rectangle hitbox;
    Rectangle draw;
}Button;

typedef struct slider{
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
}Slider;

typedef struct{
    bool isActive;
    bool isGeneral;
    bool toMainMenu;

    Color blur;
    Texture2D background;

    Image sprite;
    Image image;
    Texture2D texture;

    Button back;
    Button volume;
    Button menu;
    Button sound_back;

    Slider all_sound;
    Slider music;
    Slider effects;
}PauseMenu;

#endif
