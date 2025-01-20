#include "raylib.h"
#include "structs.h"

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

/*interfejs graficzny*/
void UpdateHitbox(struct ship *s);                               // ustala pozycję i wymiary hitbox'u
void mouse_drag(int key, struct ship *s, Color color);           // ustala położenie obiektu po pozycji mysz}
void rotate(int key, Image *sprite, Texture2D *texture);         // ustala rotację obiektu
void UpdateShip(bool *isDragging, struct ship *s);               // wywołuje mouse_drag i rotate
void SnapToGrid(struct ship *s, int gridStartX, int gridStartY); // przypisuje statek do kratki
void PrintShipPositions(struct ship *);                          // funkcja pomocnicza do wypisywania koordynatóœ statku
void CheckShipPlacement(struct ship *ships);                     // sprawdza czy statek jest ustawiony zgodnie z zasadami gry
GameData* GameSet(GameState gameState, PauseMenu *pauseMenu);                           // uruchamia ustawianie statków
void scream();//wywołuje dźwięk zatapiania
void FreeSounds();
/*plansza i interakcja z plansza*/
board *initboard();
void delboard(board *boardtab);
void placeStatek(board *board, ship *curr_ship, pair begin, int direction);
void shoot(board *player, pair pairA);                                 // wymaga poprawnie zainicjowanego statku zeby dzialalo, a wiec potrzeba inicjatora
void printboard(board *boardtab);                                      // funkcja rysuje tablice danego gracza. Na razie drukuje tylko tile'a (funkcja potencjalnie do wymiany)
struct array_cordinals *Get_array_cordinals(int offsetX, int offsetY); // przekształca położenie myszki na położenie statku na planszy

/*gameplay*/
void NewGame(PauseMenu *pauseMenu);
GameState PreGame(PauseMenu *pauseMenu);                                                                     // funkcja do obslugi menu przed rozpoczeciem gry
void PlayGame(board *playerBoard, board *enemyBoard, ship *playerShip, ship *enemyShip, PauseMenu *pauseMenu); // funkcja do obslugi gry (inicjuje okienko oraz dodany jest jakis biedny postgame)
void ResetGame(board **playerBoard, board **enemyBoard, ship **playerShip, ship **enemyShip, GameState gameState, PauseMenu *pauseMenu); //basicowa funkcja resetujaca gre (pozniej trzeba wyrzucic stad playership i enemyship, zeby samo usuwalo - nikt nie bedzie tego recznie ustawial)

/*menu pauzy*/
Button InitButton(float x, float y, char* spriteLoc, char* text, int fontsize); //customowy przycisk do menu pauzy
void UpdateButton(Button *b);
Slider InitSlider(float y_pos, float left, float right, float sl_height, float hand_width, float hand_height, char* sl_spriteLoc, char* hand_spriteLoc); //tworzenie slidera
void UpdateSlider(Slider* s, void *context); // aktualizuje suwak służący do ustawiania np. głośności
void UnloadGeneralMenu(PauseMenu *pm);
void ReloadGeneralMenu(PauseMenu *pm);
void UnloadSoundMenu(PauseMenu *pm);
void ReloadSoundMenu(PauseMenu *pm);
PauseMenu* InitPauseMenu();
void UpdatePauseMenu(PauseMenu *pm);

/*do dodania*/
void PlayGame_PvP(board *player1Board, board *player2Board, ship *player1Ship, ship *player2Ship, PauseMenu *pauseMenu); // funkcja do obslugi gry gracz vs gracz
ship *initship(int type);
void delship(ship *statek);
board *init_ai_ships();
int random_ai_ships(int tab[10][10], int typeindex, int randomized[10]);
int islegal(int x, int y, int tab[10][10], int type);

#endif