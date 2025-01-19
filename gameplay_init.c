#include "raylib.h"
#include "structs.h"
#include "functions.h"
#include <stdlib.h>
/*main do testowania gameplayu*/
int main()
{
    board *playerBoard = initboard();
    if(playerBoard == NULL){//przyda sie sprawdzic, czy alokacja sie powiodla
        return 1;
    }
    board *enemyBoard = initboard();
    if(enemyBoard == NULL){//przyda sie sprawdzic, czy alokacja sie powiodla
        delboard(playerBoard);
        return 1;
    }

    //reczne dodanie statkow do debugowania (w przyszlosci zmienione na to co dostaniemy od gracza)
    ship *playerShip = initship(3);
    if(playerShip == NULL){
        delboard(playerBoard);
        delboard(enemyBoard);
        return 1;
    }
    ship *enemyShip = initship(3);
    if(playerShip == NULL){
        delboard(playerBoard);
        delboard(enemyBoard);
        delship(playerShip);
        return 1;
    }
    pair playerStart = {2, 2};
    pair enemyStart = {4, 4};
    placeStatek(playerBoard, playerShip, playerStart, 2);
    placeStatek(enemyBoard, enemyShip, enemyStart, 3);

    //funkcja od grania - wywoluje tez playagain (czyli w zasadzie reset gry, w ktorym czysci i zaczyna od poczatku)
    PlayGame(playerBoard, enemyBoard,playerShip,enemyShip);

    //sprzatam i wychodze

    delboard(playerBoard);
    delboard(enemyBoard);
    delship(playerShip);
    delship(enemyShip);

    return 0;
}