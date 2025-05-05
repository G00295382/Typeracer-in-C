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

* `createGamemode()` returns an instance of a new `GamemodeData` struct, initialized with `0` levels, an initial level capacity of the value of the `INITIAL_CAPACITY` constant, which is set globally to `10`, and a pointer to a dynamically allocated block of memory that will contain the array of `LevelData` structs.
* `createLevel()` works exactly the same as `createGamemode()`, but will return a new instance of `LevelData`, and the array will be of `PromptData` structs.
* `createPrompt(char*)` first dynamically allocates a memory block with the size of the `PER_PROMPT_WORD_LIMIT` constant (which is globally defined as `100`) multiplied by the byte size of a char pointer.
Then, it splits the passed char array parameter into an array of individual dynamically allocated words using `splitIntoWords()` and assigns them to the memory block allocated earlier. Finally, the memory block is resized to just small enough to fit the total number of words from the passed string.

<br>

* `addLevelToGameMode(GamemodeData*, LevelData)` adds the passed `LevelData` struct to the passed `GamemodeData` struct's `levelData` array. If doing so would exceed the `GamemodeData` struct's internal level capacity, it will expand the capacity by the value of the `INITIAL_CAPACITY` constant and re-allocate the array to be the new size of the capacity.

* `addPromptToLevel(LevelData*, PromptData)` works exactly the same as `addLevelToGamemode()`, but it is adding the passed `PromptData` struct to the passed `LevelData` struct's `promptArray`.

<br>

* `compactLevel(LevelData*)` and `compactGamemode(GamemodeData*)` both set the `capacity` integer of the respective passed structs to the total number of elements in their respective arrays, and resizes those arrays to be the new value of their `capacity` integer. Similar to how `createPrompt()` starts with a large array that is re-sized afterwards based on the number of words `splitIntoWords()` found, but here it is done manually to allow for better control when manually creating the data for levels and gamemodes.

<br>

* `splitIntoWords(char* inputStr, char** outputArr, int outputArrSize)` takes the passed input char array, copies it using `strdup()`, then splits the copy into words using `strtok()` with `" "` as the set delimiter. A dynamically allocated copy of each word is created using `strdup()` before the copy is assigned to the appropriate index of the output array.

### Game Loop

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

### Time Trials Mode

The `time_trials_mode_loop()` function begins by initializing the user prefix and creating a `GamemodeData` object that contains three levels. Each level contains three unique prompt strings, added via `addPromptStrToLevel()`. These levels are added to the gamemode using `addLevelToGamemode()`.

The program then defines various control variables, timing structures, and cursor positions (`homeCursor`, `promptStart`, `currentPromptProgress`, `userInputField`) for layout management. Cursor blinking is also disabled for a cleaner user interface.

<br>

The outermost loop (`levelLoop`) iterates through all levels in the selected gamemode. At the start of each level, the cursor is moved to the top-left, the timer is initialized and displayed using `updateTime()`, and the level index is printed at the top.

The next loop (`promptLoop`) goes through each prompt within the current level. Before showing the new prompt, the screen area is cleared from the prompt start position using the ANSI escape sequence `\033[J`, which clears to the end of the screen. The current prompt is printed word-by-word with automatic line wrapping using `printTextPromptByWord()`.

Once the prompt is displayed, an input field is shown below with the user prefix, and its position is saved. The `wordLoop` begins, iterating through each word in the prompt.

For each word:

* It is printed underlined at the current prompt progress location using `promptSafeUnderlinedPrint()`.

* The program waits for character-by-character input in a non-blocking charLoop using `_kbhit()` and `_getch()`.

* During this loop, the `updateTime()` function is called periodically to refresh the timer.

* Input is compared to the expected word in real time. Correct characters are printed normally, while incorrect ones are shown in red.
    * Special inputs, like Backspace, Arrow keys, and Escape, are handled separately. Notably, Backspace will simulate actually removing the character from the user's response array, while Escape will exit the game early and take the user back to the welcome screen.

* If the word is correctly completed and followed by a space, the input is accepted and the loop breaks.

Once the word is complete:

* The input line is cleared.

* The word is reprinted at the prompt location in green (if correct) or orange (if there were mistakes) using `promptSafeProgressPrint()`.

* The prompt progress position is updated and saved.

* The WPM display is updated with the calculated WPM based on the time it took for the user to complete the word

After all words in the prompt are completed, the `promptLoop` proceeds to the next prompt. After all prompts in the level are done, the `levelLoop` advances to the next level.

If the timer runs out at any point, the loop flags are updated and the game ends with a "Times up! You lost." message. If the player successfully finishes all levels, it ends with "You won!"

Finally, the program pauses and waits for a final keypress before exiting.

### Level Rush Mode

The `alt_time_trials_mode_loop()` function has the same exact structure as `time_trials_mode_loop()`, but the timer resets after every level.

### Sudden Death Mode

The `sudden_death_mode_loop()` function has the same exact structure as `time_trials_mode_loop()`, but it fails the user if they make a single mistake when typing the prompt.

### Normal Mode

The `normal_mode_loop()` function has generally similar structure to `time_trials_mode_loop()`, but it does not display or make use of a timer that fails the player if it reaches zero.

## Main

The `main()` function first disables cursor blinking, sets up some loop variables and puts the user in a nested `while` loop.

The outermost loop (`gameLoop`) displays the program's welcome screen and lists gamemode options to select from, which use a combination of the gamemode loop functions, pre-generated prompt data, and a set time limit.

Then it goes into the inner loop (`inputLoop`), and then the innermost loop (`charLoop`), which tracks and stores the user's input. Once the user presses enter, they exit the `charLoop` and the user's inputted response is checked against an `if-else` branch using `strcmp()` to see if it matches any of the available gamemodes. If it does, the `inputLoop` variable is set to false and the user enters the gamemode they specified. If it doesn't, `inputLoop` loops and the user will go back into `charLoop`.
