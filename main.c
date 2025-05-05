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
        printf("Welcome to TypeRa\033[93mC\033[0mer!\n\n\n");

        printf("Please select the gamemode you wish to try : \n\n");
        printf("- Pangrams (\033[92mNormal\033[0m)\n\n");
        printf("- Gemma (\033[93mTimed\033[0m)\n\n");
        printf("- Moby Dick (\033[93mTimed\033[0m)\n\n");
        printf("- War and Peace (\033[93mTimed\033[0m, \033[91mSudden Death\033[0m)\n");

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

            if (strcmp(userResponse, "Pangrams") == 0) {
                inputLoop = false;
                normal_mode_loop(generateGamemodeData(0));
            }
            else if (strcmp(userResponse, "Gemma") == 0) {
                inputLoop = false;
                time_trials_mode_loop(generateGamemodeData(1), 110);
            }
            else if (strcmp(userResponse, "Moby Dick") == 0) {
                inputLoop = false;
                alt_time_trials_mode_loop(generateGamemodeData(3), 120);
            }
            else if (strcmp(userResponse, "War and Peace") == 0) {
                inputLoop = false;
                sudden_death_mode_loop(generateGamemodeData(2), 120);
            }
            else if (strcmp(userResponse, "Credits") == 0) {
                inputLoop = false;
                show_credits();
            }
        }
    }
}
