#include <time.h>
#include <stdlib.h>
#include <stdio.h>


// Sprawdza, czy można umieścić statek w danym miejscu
int islegal(int x, int y, int tab[10][10], int type) {
    if (x + type > 10) return 0;

    for (int i = x - 1; i <= x + type; i++) {
        for (int j = y - 1; j <= y + 1; j++) {
            if (i >= 0 && i < 10 && j >= 0 && j < 10) {
                if (tab[j][i] != 0) return 0;  // Zajęte pole
            }
        }
    }

    return 1;  // Legalne miejsce
}

// Funkcja do losowego rozmieszczania statków
int random_ai_ships(int tab[10][10], int typeindex, int randomized[10]) {
    if (typeindex > 10) return 0;  // Błąd danych
    if (typeindex == 10) return 1;  // Wszystkie statki rozmieszczone

    // Określenie rozmiaru statku
    int type;
    if (typeindex == 0) type = 4;
    else if (typeindex >= 1 && typeindex <= 2) type = 3;
    else if (typeindex >= 3 && typeindex <= 5) type = 2;
    else type = 1;  // Dla jednomasztowców

    int x, y;

    // Próba losowania współrzędnych
    for (int attempt = 0; attempt < 10; attempt++) {
        x = rand() % (11 - type);  // Losowanie współrzędnej X
        y = rand() % 10;           // Losowanie współrzędnej Y

        // Sprawdzanie, czy wiersz y jest już zajęty
        if (randomized[y] == -1) {
            continue;  // Wiersz już zajęty, próbujemy inny
        }

        // Sprawdzamy, czy wylosowane miejsce jest legalne
        if (islegal(x, y, tab, type)) {
            // Umieszczamy statek
            for (int i = x; i < x + type; i++) {
                tab[y][i] = type;
            }
            randomized[y] = -1;  // Zajmujemy wiersz

            // Rekurencyjnie rozmieszczamy kolejne statki
            if (random_ai_ships(tab, ++typeindex, randomized)) {
                return 1;  // Sukces
            } else {
                // Cofamy zmiany, jeżeli rozmieszczenie statku się nie udało
                for (int i = x; i < x + type; i++) {
                    tab[y][i] = 0;
                }
                randomized[y] = y;  // Zwolnienie wiersza
            }
        }
    }

    // Jeśli nie udało się rozmieszczać statku po kilku próbach, próbujemy iteracyjnie
    for (int i = 0; i < 10; i++) {
        if (i == x) continue;  // Pomijamy poprzednio wylosowane miejsce
        if (islegal(i, y, tab, type)) {
            // Umieszczamy statek
            for (int j = i; j < i + type; j++) {
                tab[y][j] = type;
            }
            randomized[y] = -1;  // Zajmujemy wiersz

            // Rekurencyjnie rozmieszczamy kolejne statki
            if (random_ai_ships(tab, ++typeindex, randomized)) {
                return 1;  // Sukces
            } else {
                // Cofamy zmiany, jeżeli rozmieszczenie statku się nie udało
                for (int j = i; j < i + type; j++) {
                    tab[y][j] = 0;
                }
                randomized[y] = y;  // Zwolnienie wiersza
            }
        }
    }

    return 0;  // Nie udało się rozmieszczone statki
}

int main() {
    srand(time(NULL));
    // Inicjalizacja planszy i zmiennych
    int board[10][10] = {0};  // 0 oznacza puste pole
    int randomized[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};  // Tablica indeksów wierszy
    random_ai_ships(board, 0, randomized);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%i ", board[i][j]);
        }
        putchar('\n');
    }
}
