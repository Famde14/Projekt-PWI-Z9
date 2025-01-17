#include "raylib.h"
#include "structs.h"
#include "functions.h"
#include <stdio.h>
#include <stddef.h>

int main()
{
    board *enemyBoard = initboard();        //druga plansza
    ship *enemyShip = initship(3);
    pair enemyStart = {4, 4};

    placeStatek(enemyBoard, enemyShip, enemyStart, 1);

    ship *playerShip;

    InitWindow(SCREENWIDTH, SCREENHEIGHT, "The Statki Game");
    SetTargetFPS(60);

    GameData gameData = GameSet();          //uruchomienie ustawiania statków
    
    PlayGame(gameData.playerBoard, enemyBoard,playerShip,enemyShip);
}