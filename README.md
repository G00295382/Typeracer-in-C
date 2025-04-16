# Typeracer-in-C

<br>

## Modules

### Safe Alloc

A simple wrapper for `malloc()`, `realloc()`, and `strdup()` that exits the program if there was an allocation failure.

### Cursor

Contains a `CursorPos` struct and some functions that allow a cursor position on the terminal to be stored internally and moved back to later.

* `CursorPos` only stores two intergers, the row and column of the cursor.

* `saveCursor(CursorPos*)` saves the current position of the cursor in the terminal (using Console Screen Buffer Info from the Windows API) into the `CursorPos` instance.

* `moveCursor(CursorPos*)` moves the cursor in the terminal window to the specific row and column specified in the passed CursorPos instance, using the ANSI escape sequence `\033[{row};{col}H`.

### Game Data

Contains several structs and functions that allow easy setup and clean organization for the individual prompts, levels, and gamemodes within the program.

#### Structs

* `GamemodeData` contains an integer tracking the total number of levels the gamemode contains, and a pointer to an array of `LevelData` structs.

* `LevelData` contains an integer tracking the total number of prompts in the level, and a pointer to an array of `PromptData` structs.

* `PromptData` contains an integer tracking the total number of words in the prompt, and a pointer to an array of char pointers that point to individual words.

#### Functions

* `createGamemode()` returns an instance of a new `GamemodeData` struct, initialized with `0` levels, an initial level capacity of the value of the `INITIAL_CAPACITY` constant, which is set globally to `10`, and a pointer to a dynamically allocated block of memory that will contain the array of `LevelData` struct pointers.
* `createLevel()` works exactly the same as `createGamemode()`, but will return a new instance of `LevelData`, and the array will be of `PromptData` struct pointers.
* `createPrompt(char*)` first dynamically allocates a memory block with the size of the `PER_PROMPT_WORD_LIMIT` constant (which is globally defined as `100`) multiplied by the byte size of a char pointer.
Then, it splits the passed char array parameter into an array of individual dynamically allocated words using `splitIntoWords()` and assigns them to the memory block allocated earlier. Finally, the memory block is resized to just small enough to fit the total number of words from the passed string.

<br>

* `addLevelToGameMode(GamemodeData*, LevelData*)` adds the passed `LevelData` struct pointer to the passed `GamemodeData` struct's `levelData` array. If doing so would exceed the `GamemodeData` struct's internal level capacity, it will expand the capacity by the value of the `INITIAL_CAPACITY` constant and re-allocate the array to be the new size of the capacity.

* `addPromptToLevel(LevelData*, PromptData*)` works exactly the same as `addLevelToGamemode()`, but it is adding the passed `PromptData` struct pointer to the passed `LevelData` struct's `promptArray`.

<br>

* `compactLevel(LevelData*)` and `compactGamemode(GamemodeData*)` both set the `capacity` integer of the respective passed structs to the total number of elements in their respective arrays, and resizes those arrays to be the new value of their `capacity` integer. Similar to how `createPrompt()` starts with a large array that is re-sized afterwards based on the number of words `splitIntoWords()` found, but here it is done manually to allow for better control when manually creating the data for levels and gamemodes.

<br>

* `splitIntoWords(char* inputStr, char** outputArr, int outputArrSize)` takes the passed input char array, copies it using `strdup()`, then splits the copy into words using `strtok()` with `" "` as the set delimiter. A dynamically allocated copy of each word is created using `strdup()` before the copy is assigned to the appropriate index of the output array.

## Main

### Functions

* `promptSafeUnderlinedPrint(char* str, int lineCharsLeft, bool leadingSpace)` prints the passed `str` parameter in an underlined format using the ANSI escape sequence `"\033[4m{str}\033[24m"`. However it does so in a "prompt safe" way by accounting for the number of allowed chars left in the current line using the passed `lineCharsLeft` integer. If printing the word would exceed the `lineCharsLeft` limit, the function will print on a newline instead of the same line. The boolean parameter `leadingSpace` simply detemines if the function should print a single space after it prints the word. For our purposes this boolean is always set to `True`.

* `promptSafeProgressPrint(char* str, int* lineCharsLeft, bool leadingSpace)` works almost exactly the same as `promptSafeUnderlinedPrint`, but the `str` parameter is printed with no formatting, and the passed parameter `lineCharsLeft` is an integer *pointer* instead of a normal integer. This is done so the length of the printed word will be subtracted from actual variable passed into the function (which would normally be beyond it's scope), and the number of chars remaining on the line can be accurately tracked. 

* `printTextPromptByWord(char* wordArray[], int totalWords)` prints every word, or char pointer, inside an array of words, without formatting, while tracking the maximum chars allowed on each line and doing a newline if a printed word would exceed it. `totalWords` is passed so the function can know when it is printing the last word and not print a leading space.
 
* `recursivePrint(char* string, int i)` is a *recursive* function that prints `string` without formatting `i` times. Mostly a utility function and currently unused.

<br>

* `initUserPrefix()` simply sets the global `userPrefix` variable to `>`, which is the default prefix.
* `changeUserPrefix(char newPrefix)` changes the global `userPrefix` variable to the passed char

<br>

* `formatTime(int total_seconds)` takes the passed `total_seconds` integer, converts it into hours, minutes, and seconds, then prints it in `HH:MM:SS` format. If the total seconds does not convert to at least an hour, it will instead print in `MM:SS` format.

* `updateTime(time_t start_time, time_t current_time, time_t time_limit)` updates the program's *"Time Remaining"* display. It uses ANSI escape sequences to save the current cursor position, then move it to "home" (or the very top left), then calculates the total remaining seconds using all three passed parameters, before preparing an ANSI escape sequence for a color code based on how many seconds are left (yellow if under 50% remaining, red if <= 30). Then prints the formatted time using `formatTime()`, clears the line after the cursor, and moves the cursor back to the saved position.

### Game Loop

<sup>\*How it currently works</sup>

The `main()` function initializes the user prefix and sets up a single level containing one prompt using `addPromptStrToLevel()`, which creates a new `PromptData` instance from a hardcoded prompt string and adds it to the level.

It then initializes various timing and loop control variables, sets up and stores several `CursorPos` structs for positioning the cursor (prompt start, progress tracking, and user input field), and disables the blinking cursor.

<br>

The current prompt is printed to the screen one word at a time using `printTextPromptByWord()`, and the user input field is positioned and saved. The actual game loop begins by hiding the cursor and jumping back to the start of the prompt using `moveCursor()`.

Each word in the prompt is processed one at a time. The word is printed underlined using `promptSafeUnderlinedPrint()`, and user input is collected character-by-character using `_getch()` while checking for timeouts via `updateTime()` in a non-blocking loop. Input is compared to the correct word in real-time, and incorrect characters are displayed in red. If the word is entered correctly and followed by a space, the loop continues to the next word.

After each word is completed, the input field is cleared, and the word is re-printed in either green (correct) or orange (incorrect) using `promptSafeProgressPrint()`, then the cursor position is saved again for the next word.

If all words are completed within the time limit, the game ends with a *"You won!"* message. If the timer expires first, *"Times up! You lost."* is shown. The cursor is then repositioned to the input field and the program waits for a final key press before exiting.
