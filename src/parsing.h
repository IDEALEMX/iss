#pragma once

int parse_into_words(char* to_be_parsed_string, char word_array [64][256], char char_to_be_skipped);

void print_parsed_words(char words[64][256], int n);

void replace_wave_char(char* string);
