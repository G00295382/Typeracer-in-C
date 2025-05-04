#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <stdbool.h>
#include <string.h>

#include "cursor.h"
#include "game_data.h"
#include "safe_alloc.h"
#include "game_loop.h"

int main() {
	// Disable cursor blinking
	printf("\033[?12l");

	char userResponse[USER_INPUT_ARRAY_SIZE];
    char ch;
    int inputIndex;
    boolean charLoop;
    boolean inputLoop;
    boolean gameLoop = true;

    while (gameLoop) {
        printf("Welcome to TypeRaCer!\n\n");

        printf("Please select the gamemode you wish to try : \n");
        printf("- Normal\n");
        printf("- Time Trials\n");
        printf("- Alt Time Trials\n");
        printf("- Sudden Death\n");

        printf("\n\n> ");

        inputIndex = 0;
        // Set all elements of the userResponse array to the null character
        memset(userResponse, '\0', sizeof(userResponse));

        inputLoop = true;

        while (inputLoop) {
            charLoop = true;

            while (charLoop) {
                ch = _getch();  // Read input once a key is pressed
                if (ch == 8) { // If backspace
                    if (inputIndex > 0) { // If not at beginning of input
                        userResponse[--inputIndex] = '\0';
                        printf("%c \033[D", ch); // Overwrite character behind cursor with a space
                    }
                }
                else if (ch == -32) { // If arrow keys
                    ch = _getch(); // Block input of second arrow key character
                }
                else if (ch == 10 || ch == 13) { // If enter/return
                    charLoop = false;
                }
                else {
                    if (inputIndex < USER_INPUT_ARRAY_SIZE) {
                        userResponse[inputIndex] = ch; // Update char array that tracks input
                        printf("\033[96m%c\033[0m", ch);
                        inputIndex++;
                    }
                }
            }

            if (strcmp(userResponse, "Normal") == 0) {
                inputLoop = false;
                normal_mode_loop(generateGamemodeData(0));
            }
            else if (strcmp(userResponse, "Time Trials") == 0) {
                inputLoop = false;
                time_trials_mode_loop(generateGamemodeData(0), 40);
            }
            else if (strcmp(userResponse, "Level Rush") == 0) {
                inputLoop = false;
                alt_time_trials_mode_loop(generateGamemodeData(0), 10);
            }
            else if (strcmp(userResponse, "Sudden Death") == 0) {
                inputLoop = false;
                sudden_death_mode_loop(generateGamemodeData(0), 10);
            }
        }
    }
}
