#include "raylib.h"
#include "structs.h"
#include "functions.h"
#include <stdio.h>
#include <stddef.h>

int main()
{
    GameState gameState = GAME_START;

    board *enemyBoard = init_ai_ships();        //druga plansza
    ship *enemyShip = NULL;
    ship *playerShip = NULL;

    InitAudioDevice(); 

    /*
        Przekazywanie niezainicjalizowany zmiennych do funkcji to narażanie się na undefined 
        behaviour, więc nie chcąc wywracać kodu do góry nogami inicjalizuję je jako NULL.
    */

    InitWindow(SCREENWIDTH, SCREENHEIGHT, "The Statki Game");
    SetTargetFPS(60);

    gameState = PreGame();

    if(gameState == GAME_PREPARE1)
    {
        GameData gameData = GameSet(GAME_START);
        PlayGame(gameData.playerBoard, enemyBoard,playerShip,enemyShip);
    }
    else if(gameState == GAME_PREPARE2)
    {
        GameData gameData1 = GameSet(GAME_PREPARE1);
        GameData gameData2 = GameSet(GAME_PREPARE2);
        PlayGame_PvP(gameData1.playerBoard, gameData2.playerBoard,playerShip,enemyShip);
    }
}