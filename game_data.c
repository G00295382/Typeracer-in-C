#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "game_data.h"
#include "safe_alloc.h"

#define PER_PROMPT_WORD_LIMIT 100
#define INITIAL_CAPACITY 10

#define STRTOK_FAILURE 12

int splitIntoWords(char* inputStr, char** outputArr, int outputArrSize) {
    int wordCount = 0;
    char* context = NULL;
    // Create an dyn-allocated copy of the passed input string using strdup() before passing it to strtok_s to preserve the original string
    // strcpy() could be used here instead but the copied string would have to have a static size given from a constant and we don't know the size of the input string
    char* inputStrC = safe_strdup(inputStr);
    // Begin splitting input string copy into words using " " delimiter
    char* word = strtok_s(inputStrC, " ", &context);
    while (word != NULL && wordCount < outputArrSize) {
        outputArr[wordCount++] = safe_strdup(word); // Each individual word is also dyn-allocated because strtok_s tokens have pointers to the original string
        word = strtok_s(NULL, " ", &context); // Get next word by passing NULL to strtok_S
    }
    // if strtok returned no words, free array and exit the program
    if (wordCount == 0) {
        free(outputArr);
        exit(STRTOK_FAILURE);
    }
    // Free the input string copy from memory
    free(inputStrC);
    return wordCount;
}

PromptData createPrompt(const char* sentenceStr) {
    PromptData newPrompt;
    // Initially create a large dynamic array, we don't know how many words are in the prompt yet
    newPrompt.wordArray = safe_malloc(PER_PROMPT_WORD_LIMIT * sizeof(char*));
    // Split sentence into words and put them into wordArray, then return the total number of words into totalWords
    newPrompt.totalWords = splitIntoWords(sentenceStr, newPrompt.wordArray, PER_PROMPT_WORD_LIMIT);
    // Resize dynamic array to the number of elements within it
    newPrompt.wordArray = safe_realloc(newPrompt.wordArray, newPrompt.totalWords * sizeof(char*));
    return newPrompt;
}

LevelData createLevel() {
    LevelData newLevel;
    newLevel.totalPrompts = 0;
    newLevel.promptCapacity = INITIAL_CAPACITY;
    newLevel.promptArray = safe_malloc(newLevel.promptCapacity * sizeof(PromptData));
    return newLevel;
}

void addPromptToLevel(LevelData* level, PromptData prompt) {
    // If at prompt capacity, increment prompt capacity by INITIAL_CAPACITY constant and resize array
    if (level->totalPrompts >= level->promptCapacity) {
        level->promptCapacity += INITIAL_CAPACITY;
        level->promptArray = safe_realloc(level->promptArray, level->promptCapacity * sizeof(PromptData));
    }

    // Add prompt to the level's promptArray
    level->promptArray[level->totalPrompts++] = prompt;
}

void addPromptStrToLevel(LevelData* level, char* sentenceStr) {
    // If at prompt capacity, increment prompt capacity by INITIAL_CAPACITY constant and resize array
    if (level->totalPrompts >= level->promptCapacity) {
        level->promptCapacity += INITIAL_CAPACITY;
        level->promptArray = safe_realloc(level->promptArray, level->promptCapacity * sizeof(PromptData));
    }

    // Create prompt using sentence string
    PromptData prompt = createPrompt(sentenceStr);

    // Add prompt to the level's promptArray
    level->promptArray[level->totalPrompts++] = prompt;
}

void compactLevel(LevelData* level) {
    // Change level's promptCapacity to value of totalPrompts
    level->promptCapacity = level->totalPrompts;
    // Resize level's promptArray to the new value of promptCapacity
    level->promptArray = safe_realloc(level->promptArray, level->promptCapacity * sizeof(PromptData));
}

GamemodeData createGamemode() {
    GamemodeData newMode;
    newMode.totalLevels = 0;
    newMode.levelCapacity = INITIAL_CAPACITY;
    newMode.levelArray = safe_malloc(newMode.levelCapacity * sizeof(GamemodeData));
    return newMode;
}

void addLevelToGamemode(GamemodeData* gamemode, LevelData* level) {
    // If at level capacity, increment level capacity by INITIAL_CAPACITY constant and resize array
    if (gamemode->totalLevels >= gamemode->levelCapacity) {
        gamemode->levelCapacity += INITIAL_CAPACITY;
        gamemode->levelArray = safe_realloc(gamemode->levelArray, gamemode->levelCapacity * sizeof(PromptData));
    }
}

void compactGamemode(GamemodeData* gamemode) {
    // Change gamemodes's levelCapacity to value of totalLevels
    gamemode->levelCapacity = gamemode->totalLevels;
    // Resize level's promptArray to the new value of promptCapacity
    gamemode->levelArray = safe_realloc(gamemode->levelArray, gamemode->levelCapacity * sizeof(PromptData));
}