#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <stdbool.h>
#include <string.h>

#include "cursor.h"
#include "game_data.h"
#include "safe_alloc.h"

#define PER_LINE_CHAR_LIMIT 40
#define USER_INPUT_ARRAY_SIZE 20
#define TIME_DISPLAY_REFRESH_RATE 10

char userPrefix;

void promptSafeUnderlinedPrint(const char* str, int lineCharsLeft, boolean leadingSpace) {
    if (leadingSpace) {
        lineCharsLeft -= strlen(str) + 1;
        if (lineCharsLeft > 0) {
            printf("\033[4m%s\033[24m ", str);
        }
        else {
            printf("\n  \033[4m%s\033[24m ", str);
            lineCharsLeft = PER_LINE_CHAR_LIMIT;
        }
    }
    else {
        lineCharsLeft -= strlen(str);
        if (lineCharsLeft > 0) {
            printf("\033[4m%s\033[24m", str);
        }
        else {
            printf("\n  \033[4m%s\033[24m", str);
            lineCharsLeft = PER_LINE_CHAR_LIMIT;
        }
    }
}

void promptSafeProgressPrint(const char* str, int* lineCharsLeft, boolean leadingSpace) {
    if (leadingSpace) {
        *lineCharsLeft -= strlen(str) + 1;
        if (*lineCharsLeft > 0) {
            printf("%s ", str);
        }
        else {
            printf("\n  %s ", str);
            *lineCharsLeft = PER_LINE_CHAR_LIMIT;
        }
    }
    else {
        *lineCharsLeft -= strlen(str);
        if (*lineCharsLeft > 0) {
            printf("%s", str);
        }
        else {
            printf("\n  %s", str);
            *lineCharsLeft = PER_LINE_CHAR_LIMIT;
        }
    }
}

void printTextPromptByWord(char* wordArray[], int totalWords) {
    int lineCharsLeft = PER_LINE_CHAR_LIMIT;
    for (int i = 0; i < totalWords; i++) {
        if (i + 1 >= totalWords) { // If last word, don't count or print space
            lineCharsLeft -= strlen(wordArray[i]);
            if (lineCharsLeft > 0) {
                printf("%s", wordArray[i]);
            }
            else {
                printf("\n  %s", wordArray[i]);
                lineCharsLeft = PER_LINE_CHAR_LIMIT;
            }
        }
        else {
            lineCharsLeft -= strlen(wordArray[i]) + 1; // +1 for space
            if (lineCharsLeft > 0) {
                printf("%s ", wordArray[i]);
            }
            else {
                printf("\n  %s ", wordArray[i]);
                lineCharsLeft = PER_LINE_CHAR_LIMIT;
            }
        }
    }
}

void recursivePrint(const char* string, int i) {
    if (i > 0) {
        printf("%s", string);
        recursivePrint(string, i - 1);
    }
}

void initUserPrefix() {
    userPrefix = '>';
}

void changeUserPrefix(const char newPrefix) {
    strcpy_s(userPrefix, sizeof(userPrefix), newPrefix);
}

void formatTime(int total_seconds) {
    int hours = total_seconds / 3600;
    int minutes = (total_seconds % 3600) / 60;
    int seconds = total_seconds % 60;

    if (hours > 0) {
        printf("%02d:%02d:%02d\n", hours, minutes, seconds);
    }
    else {
        printf("%02d:%02d\n", minutes, seconds);
    }
}

void updateTime(time_t start_time, time_t current_time, time_t time_limit) {
    // Save the current cursor position
    printf("\x1B[s");

    // Hide cursor and move it to the top-left
    printf("\x1B[?25l\x1B[H");

    // Calculate total remaining seconds
    int tr_total_seconds = (long)time_limit - ((long)current_time - start_time);

    printf("Time Remaining  -  ");
    if (tr_total_seconds <= 10) { // Red text color when <= 30 seconds
        printf("\x1B[91m");
    }
    else if ((((double)tr_total_seconds / time_limit) * 100) <= 50) { // Yellow text color when <=50% time remaining
        printf("\x1B[33m");
    }
    formatTime(tr_total_seconds); // Print formatted time (hh:/mm:ss) using total remaining seconds
    printf("\x1B[0m\x1B[K"); // Return to normal text color and clear remaining parts of the line after cursor

    // Move cursor to the saved position and unhide it
    printf("\x1B[u\x1B[?25h");

    // Immediately update console output from buffer
    fflush(stdout);
}

int main() {

    initUserPrefix();

    LevelData l1 = createLevel();
    addPromptStrToLevel(&l1, "Five quacking zephyrs jolt my wax bed. The five boxing wizards jump quickly. Five or six big jet planes zoomed quickly by the tower. Jim quickly realized that the beautiful gowns are expensive.");

    char userResponse[USER_INPUT_ARRAY_SIZE];

	char ch;
	int time_limit = 50;
    int updateTimeLoop = 0;

    PromptData* userPrompt = &l1.promptArray[0];

    // Initialize time
    time_t start_time = time(NULL);
    time_t current_time = time(NULL);
    updateTime(start_time, current_time, time_limit);

    // Initialize prompt cursor positions
    CursorPos promptStart;
    CursorPos currentPromptProgress;
    CursorPos userInputField;

    // Disable cursor blinking
    printf("\033[?12l");

    // Prompt spacing
    printf("\n\n  ");

    // Word-by-word printed prompt

    // Save current cursor position
    saveCursor(&promptStart);
    currentPromptProgress = promptStart;

    // Procedurally print prompt word by word with newlines when line char limit is reached
    printTextPromptByWord(userPrompt->wordArray, userPrompt->totalWords);

    // Input spacing
    printf("\n\n%c ", userPrefix);
    saveCursor(&userInputField);

    // Set loop variables
    int wordIndex = 0;
    int promptIndex = 0;
    int levelIndex = 0;
    int totalPromptChars = 0;
    int totalInputErrors = 0;
    int wordLength;
    int wordErrors;
    int wordInputIndex;
    boolean charLoop = true;
    boolean wordLoop = true;
    boolean promptLoop = true;
    int lineCharsLeft = PER_LINE_CHAR_LIMIT;
    int winState = -1;
    
    // Hide cursor and move it to start of prompt
    printf("\033[?25l");
    moveCursor(&promptStart);

    // Begin word cycling loop
    while (wordLoop) {
        // Update word length to size of current word at index
        wordLength = strlen(userPrompt->wordArray[wordIndex]);
        // Set all elements of the userResponse array to the null character
        memset(userResponse, '\0', sizeof(userResponse));

        // Print underlined version of word at current index
        promptSafeUnderlinedPrint(userPrompt->wordArray[wordIndex], lineCharsLeft, true);
        // Move cursor back to start of input field
        moveCursor(&userInputField);
        // Unhide cursor
        printf("\x1B[?25h");

        totalPromptChars += wordLength;
        wordErrors = 0;
        wordInputIndex = 0;

        charLoop = true;

        // Begin word input loop
        while (charLoop && wordLoop) {
            while (!_kbhit() && wordLoop) {  // Wait for a key press without blocking
                updateTimeLoop++; // Increment time display update loop count
                current_time = time(NULL); // Update current time
                if (current_time - start_time >= time_limit) { // Fail player if they exceeded the time limit
                    winState = 0;
                    wordLoop = false;
                }
                if (updateTimeLoop >= TIME_DISPLAY_REFRESH_RATE) { // Update time display if TDU loop count exceeds threshold
                    updateTimeLoop = 0;
                    updateTime(start_time, current_time, time_limit);
                }
                Sleep(1);
            }
            if (charLoop && wordLoop) {
                ch = _getch();  // Read input once a key is pressed
                if (ch == 8) { // If backspace
                    if (wordInputIndex > 0) { // If not at beginning of input
                        wordInputIndex--;
                        printf("%c \x1B[D", ch); // Backspace on console printed characters would simply move the cursor back, so make sure the console properly overwrites the character
                    }
                }
                else if (ch == -32) { // If arrow keys
                    ch = _getch(); // Block input of second arrow key character
                }
                else if (!(ch == 10 || ch == 13)) { // If not \n or \r (enter)
                    if (wordInputIndex == wordLength && ch == 32) { // If user at final index of prompt and presses space
                        //int result = strcmp(userPrompt, userResponse);
                        //printf("\n\n%s\n%s\n%d", userPrompt, userResponse, result);
                        if (strcmp(userPrompt->wordArray[wordIndex], userResponse) == 0) {
                            charLoop = false;
                        }
                    }
                    if (wordInputIndex < wordLength) { // If user not at final index of prompt
                        userResponse[wordInputIndex] = ch; // Update char array that tracks input
                        if (ch == userPrompt->wordArray[wordIndex][wordInputIndex]) { // If input was correct, print normally
                            printf("%c", ch);
                        }
                        else { // If input was incorrect, print character in light red text
                            printf("\x1B[31m%c\x1B[0m", ch);
                            wordErrors++;
                        }
                        wordInputIndex++;
                    }
                }
            }
        }

        // Hide cursor
        printf("\033[?25l");
        // Move cursor to start of input field
        moveCursor(&userInputField);
        // Erase line after cursor
        printf("\033[K");

        if (wordLoop == true) {
            // Move cursor to where the progress printer left off
            moveCursor(&currentPromptProgress);

            // Print light green / orange version of the word at the current index
            if (wordErrors > 0) {
                printf("\033[38;5;208m");
            }
            else {
                printf("\033[92m");
            }
            promptSafeProgressPrint(userPrompt->wordArray[wordIndex], &lineCharsLeft, true);
            printf("\033[0m");
            // Save progress printer position
            saveCursor(&currentPromptProgress);

            totalInputErrors += wordErrors;
            wordIndex++;

            if (wordIndex >= userPrompt->totalWords) { // If loop is still enabled and user completed all the words
                winState = 1;
                wordLoop = false;
            }
        }
    }

    // Move cursor to start of input field
    moveCursor(&userInputField);

    if (winState == 1) {
        printf("\nYou won!\n");
    }
    else if (winState == 0) {
        printf("\nTimes up! You lost.\n");
    }

    Sleep(2000);

    printf("Press any key to continue...");
    _getch();

	return 0;
}