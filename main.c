#include "raylib.h"
#include "structs.h"
#include "functions.h"
#include "resource_dir.h"	
#include <string.h>
#include <stdio.h>
#include <ctype.h>
/*
techniczne rzeczy co do sprawdzarki
nalezy odpalic plik w konsoli.
sterowanie jest nastepujace 
kierunki:
1 - gora 
2 -prawo
3 -dol 
4 -lewo
Litera jest oznaczeniem osi X(nieintuicyjne,wiem), liczba osi Y

j d t c to kolejno 1-masztowy 2-masztowy 3-masztowy 4-masztowy statek
J D T C to elementy trafione

Na razie gra odbywa sie miedzy P1 a P1 

Na razie w tej grze gracz strzela do siebie samego(gracza z tej samej tury) 
*/ 
board* playerturn(board* p1, board* p2,int turn)
{
	if(turn%2==1)
	{
		return p1;
	}
	else{return p2;}
}
#define MAX_INPUT_LENGTH 1024

// Funkcja sprawdzająca poprawność koordynatów
int valid_coordinates(char letter, int number) {
    return isupper(letter) && (letter >= 'A' && letter <= 'J') && (number >= 1 && number <= 10);
}

// Funkcja przetwarzająca komendę `place`
void process_place(board* player,int length, int direction, char letter, int number) {
    if (length < 1 || length > 4) {
        printf("Błąd: długość statku musi być liczbą od 1 do 4.\n");
        return;
    }
    if (direction < 1 || direction > 4) {
        printf("Błąd: kierunek musi być liczbą od 1 do 4.\n");
        return;
    }
    if (!valid_coordinates(letter, number)) {
        printf("Błąd: nieprawidłowe koordynaty. Litera musi być od A do J, a liczba od 1 do 10.\n");
        return;
    }

    // Jeśli dane są poprawne
    printf("Poprawne polecenie PLACE: długość=%d, kierunek=%d, koordynaty=%c%d\n",
           length, direction, letter, number);
		   pair para={(int)(letter-'A'),number-1};
		placeStatek(player,initship(length),para,direction);
		printboard(player);
}

// Funkcja przetwarzająca komendę `shoot`
void process_shoot(board* player,char letter, int number) {
    if (!valid_coordinates(letter, number)) {
        printf("Błąd: nieprawidłowe koordynaty. Litera musi być od A do J, a liczba od 1 do 10.\n");
        return;
    }

    // Jeśli dane są poprawne
    printf("Poprawne polecenie SHOOT: koordynaty=%c%d\n", letter, number);
	pair para = {letter-'A',number-1};
	shoot(player,para);//tak, gracz strzela w swoja wlasna plansze. I co mi teraz zrobisz
	printboard(player);
}

int main() {
    char input[MAX_INPUT_LENGTH];

    printf("Wpisz polecenie (place lub shoot):\n");
	int turn=1;
	board* P1=initboard();
	board* P2=initboard();
    while (fgets(input, sizeof(input), stdin)) {
        char command[10];
        char letter;
        int number, length, direction;

        // Sprawdź polecenie PLACE
        if (sscanf(input, "place %d %d %c%d", &length, &direction, &letter, &number) == 4) {
            process_place(playerturn(P1,P1,turn),length, direction, letter, number);
        }
        // Sprawdź polecenie SHOOT
        else if (sscanf(input, "shoot %c%d", &letter, &number) == 2) {
            process_shoot(playerturn(P1,P1,turn),letter, number);
        }
        // Nieprawidłowe polecenie
        else {
            printf("Błąd: Nieprawidłowe polecenie. Użyj 'place <dlugosc> <kierunek> <koordynaty>' lub 'shoot <koordynaty>'.\n");
        }

        printf("\nWpisz kolejne polecenie (place lub shoot):\n");
		turn++;
    }

    return 0;//jak widac program nie zwalnia pamieci, glownie dlatego ze przy obecnym designie jest to technicznie niemozliwe, liczymy na to ze pamiec wyczysci sie sama i nie wywali nikomu kompa
	//oczywiscie wersja finalna nie moze taka byc 
}

