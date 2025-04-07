#pragma once

typedef struct {
    int totalWords;
    char** wordArray;
} PromptData;

typedef struct {
    int totalPrompts;
    int promptCapacity;
    PromptData* promptArray;
} LevelData;

typedef struct {
    int totalLevels;
    int levelCapacity;
    LevelData* levelArray;
} GamemodeData;

int splitIntoWords(char* inputStr, char** outputArr, int outputArrSize);

PromptData createPrompt(const char* sentenceStr);

LevelData createLevel();

void addPromptToLevel(LevelData* level, PromptData prompt);

void addPromptStrToLevel(LevelData* level, char* sentenceStr);

void compactLevel(LevelData* level);

GamemodeData createGamemode();

void addLevelToGamemode(GamemodeData* gamemode, LevelData* level);

void compactGamemode(GamemodeData* gamemode);