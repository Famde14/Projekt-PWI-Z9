#include "raylib.h"
#include "structs.h"
#include "functions.h"
#include <stdio.h>
#include <stddef.h>

int main()
{
    board *enemyBoard = init_ai_ships();        //druga plansza
    ship *enemyShip = NULL;
    ship *playerShip = NULL;
    
    /*
        Przekazywanie niezainicjalizowany zmiennych do funkcji to narażanie się na undefined 
        behaviour, więc nie chcąc wywracać kodu do góry nogami inicjalizuję je jako NULL.
    */

    InitWindow(SCREENWIDTH, SCREENHEIGHT, "The Statki Game");
    SetTargetFPS(60);

    GameData gameData = GameSet();          //uruchomienie ustawiania statków
    
    PlayGame(gameData.playerBoard, enemyBoard,playerShip,enemyShip);
}