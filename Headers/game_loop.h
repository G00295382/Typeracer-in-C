#pragma once

#define PER_LINE_CHAR_LIMIT 40
#define USER_INPUT_ARRAY_SIZE 20
#define TIME_DISPLAY_REFRESH_RATE 10

void promptSafeUnderlinedPrint(const char* str, int lineCharsLeft, boolean leadingSpace);

void promptSafeProgressPrint(const char* str, int* lineCharsLeft, boolean leadingSpace);

void printTextPromptByWord(char* wordArray[], int totalWords);

void recursivePrint(const char* string, int i);

void initUserPrefix();

void changeUserPrefix(const char newPrefix);

void formatTime(int total_seconds);

void updateTime(CursorPos pos, time_t start_time, time_t current_time, time_t time_limit);

GamemodeData generateGamemodeData(index);

void time_trials_mode_loop(GamemodeData gamemode, int time_limit);

void alt_time_trials_mode_loop(GamemodeData gamemode, int time_limit);

void sudden_death_mode_loop(GamemodeData gamemode);

void normal_mode_loop(GamemodeData gamemode);

void show_credits();
