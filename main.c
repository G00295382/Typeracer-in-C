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
    printf("\033[K"); // Clear initial line
    for (int i = 0; i < totalWords; i++) {
        if (i + 1 >= totalWords) { // If last word, don't count or print space
            lineCharsLeft -= strlen(wordArray[i]);
            if (lineCharsLeft > 0) {
                printf("%s", wordArray[i]);
            }
            else { // If not enough chars remaining on line for word, goto newline and clear the line before printing the word
                printf("\n\033[K  %s", wordArray[i]);
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
        printf("%02d:%02d:%02d", hours, minutes, seconds);
    }
    else {
        printf("%02d:%02d", minutes, seconds);
    }
}

void updateTime(CursorPos pos, time_t start_time, time_t current_time, time_t time_limit) {
    // Save the current cursor position
    CursorPos previousPos;
    saveCursor(&previousPos);

    // Hide cursor and move it to the top-left
    printf("\033[?25l");
    moveCursor(pos);

    // Calculate total remaining seconds
    int tr_total_seconds = (long)time_limit - ((long)current_time - start_time);

    printf("Time Remaining  -  ");
    if (tr_total_seconds <= 10) { // Red text color when <= 30 seconds
        printf("\033[91m");
    }
    else if ((((double)tr_total_seconds / time_limit) * 100) <= 50) { // Yellow text color when <=50% time remaining
        printf("\033[33m");
    }
    formatTime(tr_total_seconds); // Print formatted time (hh:/mm:ss) using total remaining seconds
    printf("\033[0m\033[K"); // Return to normal text color and clear remaining parts of the line after cursor

    // Move cursor to the saved position and unhide it
    moveCursor(previousPos);
    printf("\033[?25h");

    // Immediately update console output from buffer
    fflush(stdout);
}

int main() {

    initUserPrefix();

    GamemodeData* selectedGamemode;
    LevelData* currentLevel;
    PromptData* userPrompt;

    // Initialize cursor positions
    CursorPos homeCursor = createCursor(1, 1); // top-left of console window
    CursorPos promptStart;
    CursorPos currentPromptProgress;
    CursorPos userInputField;

    // Create and organize prompt data
    GamemodeData g1 = createGamemode();

    LevelData l1 = createLevel();
    addPromptStrToLevel(&l1, "First prompt");
    addPromptStrToLevel(&l1, "Second prompt");
    addPromptStrToLevel(&l1, "Third prompt");
    LevelData l2 = createLevel();
    addPromptStrToLevel(&l2, "Fourth prompt");
    addPromptStrToLevel(&l2, "Fifth prompt");
    addPromptStrToLevel(&l2, "Sixth prompt");
    LevelData l3 = createLevel();
    addPromptStrToLevel(&l3, "Seventh prompt");
    addPromptStrToLevel(&l3, "Eighth prompt");
    addPromptStrToLevel(&l3, "Ninth prompt");

    addLevelToGamemode(&g1, l1);
    addLevelToGamemode(&g1, l2);
    addLevelToGamemode(&g1, l3);


    char userResponse[USER_INPUT_ARRAY_SIZE];

	char ch;
	int time_limit = 40;
    int updateTimeLoop = 0;

    // Initialize time
    time_t start_time;
    time_t current_time;

    // Disable cursor blinking
    printf("\033[?12l");

    // Set loop variables
    int wordIndex;
    int promptIndex;
    int levelIndex = 0;
    int totalPromptChars = 0;
    int totalInputErrors = 0;
    int wordLength;
    int wordErrors;
    int wordInputIndex;
    boolean charLoop;
    boolean wordLoop;
    boolean promptLoop;
    boolean levelLoop;
    boolean playLoop;
    boolean gameLoop;
    int lineCharsLeft;
    int winState = -1;

    // Hide cursor
    printf("\033[?25l");

    levelLoop = true;

    selectedGamemode = &g1;

    // Cycle between the levels in the selected gamemode
    while (levelLoop) {
        // Move cursor to the top-left of the console window
        moveCursor(homeCursor);
        // Display timer on current line
        start_time = time(NULL);
        current_time = time(NULL);
        updateTime(homeCursor, start_time, current_time, time_limit);
        // Set current level to specified index of selected gamemode
        currentLevel = &selectedGamemode->levelArray[levelIndex];
        // Display level info on next line
        printf("\nLevel %d/%d\033[K", levelIndex + 1, selectedGamemode->totalLevels);
        // Navigate to where the prompt will be printed
        printf("\n\n  ");
        // Save current cursor position to promptStart
        saveCursor(&promptStart);

        // Reset prompt index
        promptIndex = 0;
        promptLoop = true;

        // Cycle between the prompts in each level
        while (promptLoop && levelLoop) {
            // Move cursor to the start of the prompt
            moveCursor(promptStart);
            // Set current prompt progress cursor to prompt start cursor
            currentPromptProgress = promptStart;
            // Clear all text from the screen past the cursor
            printf("\033[03");

            // Set userPrompt to prompt at specified index of current level
            userPrompt = &currentLevel->promptArray[promptIndex];
            // Procedurally print prompt word by word with newlines when line char limit is reached
            printTextPromptByWord(userPrompt->wordArray, userPrompt->totalWords);
            // Create input field and save it's position to a cursor
            printf("\n\n%c ", userPrefix);
            saveCursor(&userInputField);

            // Reset line chars left
            lineCharsLeft = PER_LINE_CHAR_LIMIT;
            // Reset word index
            wordIndex = 0;

            // Move cursor back to the start of the prompt
            moveCursor(promptStart);
            // Hide the cursor
            printf("\033[?25l");

            wordLoop = true;

            // Cycle between the words in each prompt
            while (wordLoop && promptLoop && levelLoop) {
                // Update word length to size of current word at index
                wordLength = strlen(userPrompt->wordArray[wordIndex]);
                // Set all elements of the userResponse array to the null character
                memset(userResponse, '\0', sizeof(userResponse));

                // Print underlined version of word at current index
                promptSafeUnderlinedPrint(userPrompt->wordArray[wordIndex], lineCharsLeft, true);
                // Move cursor back to start of input field
                moveCursor(userInputField);
                // Unhide cursor
                printf("\x1B[?25h");

                totalPromptChars += wordLength;
                wordErrors = 0;
                wordInputIndex = 0;

                charLoop = true;

                // Cycle between the characters in each word
                while (charLoop && wordLoop && promptLoop && levelLoop) {
                    while (!_kbhit() && levelLoop) {  // Wait for a key press without blocking
                        updateTimeLoop++; // Increment time display update loop count
                        current_time = time(NULL); // Update current time
                        if (current_time - start_time >= time_limit) { // Fail user if they exceeded the time limit
                            winState = 0;
                            levelLoop = false;
                        }
                        if (updateTimeLoop >= TIME_DISPLAY_REFRESH_RATE) { // Update time display if TDU loop count exceeds threshold
                            updateTimeLoop = 0;
                            updateTime(homeCursor, start_time, current_time, time_limit);
                        }
                        Sleep(1);
                    }
                    if (charLoop && wordLoop && promptLoop && levelLoop) {
                        ch = _getch();  // Read input once a key is pressed
                        if (ch == 8) { // If backspace
                            if (wordInputIndex > 0) { // If not at beginning of input
                                wordInputIndex--;
                                printf("%c \033[D", ch); // Overwrite character behind cursor with a space
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
                                    printf("\033[31m%c\x1B[0m", ch);
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
                moveCursor(userInputField);
                // Erase line after cursor
                printf("\033[K");

                // Print prompt progress if word loop is still active
                if (wordLoop && promptLoop && levelLoop) {
                    // Move cursor to where the progress printer left off
                    moveCursor(currentPromptProgress);

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
                        wordLoop = false;
                    }
                }
            }

            if (promptLoop && levelLoop) {
                promptIndex++;

                if (promptIndex >= currentLevel->totalPrompts) { // If loop is still enabled and user completed all the prompts
                    promptLoop = false;
                }
            }
        }

        if (levelLoop) {
            levelIndex++;

            if (levelIndex >= selectedGamemode->totalLevels) { // If loop is still enabled and user completed all the levels
                levelLoop = false;
            }
        }
    }

    // Move cursor to start of input field
    moveCursor(userInputField);

    if (winState == 0) {
        printf("\nTimes up! You lost.\n");
    }
    else {
        printf("\nYou won!\n");
    }

    Sleep(2000);

    printf("Press any key to continue...");
    _getch();

	return 0;
}
