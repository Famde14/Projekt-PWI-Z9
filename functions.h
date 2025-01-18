#include "raylib.h"
#include "structs.h"
#ifndef FUNCTIONS_H
#define FUNCTIONS_H
/*interfejs graficzny*/ 
void UpdateHitbox(struct ship* s);	//ustala pozycję i wymiary hitbox'u
void mouse_drag(int key, struct ship* s, Color color);//ustala położenie obiektu po pozycji mysz}
void rotate(int key, Image* sprite, Texture2D* texture);   //ustala rotację obiektu
void UpdateShip(bool* isDragging, struct ship* s);	//wywołuje mouse_drag i rotate
void SnapToGrid(struct ship* s, int gridStartX, int gridStartY); //przypisuje statek do kratki
void PrintShipPositions(struct ship*);//funkcja pomocnicza do wypisywania koordynatóœ statku
GameData GameSet(); //uruchamia ustawianie statków
/*plansza i interakcja z plansza*/
board* initboard();
void delboard(board* boardtab);
void placeStatek(board* board,ship* curr_ship,pair begin,int direction);
void shoot(board* player,pair pairA);//wymaga poprawnie zainicjowanego statku zeby dzialalo, a wiec potrzeba inicjatora
void printboard(board* boardtab);//funkcja rysuje tablice danego gracza. Na razie drukuje tylko tile'a (funkcja potencjalnie do wymiany)
struct array_cordinals* Get_array_cordinals(int offsetX, int offsetY);//przekształca położenie myszki na położenie statku na planszy
/*gameplay*/
void PlayGame(board *playerBoard, board *enemyBoard, ship *playerShip, ship *enemyShip); //funkcja do obslugi gry (inicjuje okienko oraz dodany jest jakis biedny postgame)
/*ustawienie*/
void UpdateSlider(struct slider* s); //aktualizuje suwak służący do ustawiania np. głośności
/*do dodania*/
void PlayGame_PvP(board *player1Board, board *player2Board, ship *player1Ship, ship *player2Ship); //funkcja do obslugi gry gracz vs gracz
ship* initship(int type);
void delship(ship** statek);
board* init_ai_ships();
int random_ai_ships(int tab[10][10], int typeindex, int randomized[10]);
int islegal(int x, int y, int tab[10][10], int type);
#endif
