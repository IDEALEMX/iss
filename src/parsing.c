#include "parsing.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>

/*
 * given a string, an array of strings where to store the parsed data
 * and a character to be skipped, the function will parse the data
 * separating it everytime said character is encountered
 * and will return the number of words contained in the string
 */
int parse_into_words(char* to_be_parsed_string, char word_array [64][256], char char_to_be_skipped) {
    char current_word_buffer [128];
    int current_word_char = 0;

    int current_word_number = 0;

    // return a 0 if input was empty
    if (to_be_parsed_string[0] == '\0')
        return 0;
    
    for (int i = 0; ; i++) {
        char current_char = to_be_parsed_string[i];

        // handle overflows
        if (current_word_char >= 127) {
            current_word_buffer[127] = '\0';
        }

        if (current_word_number >= 64) {
            break; 
        }

        // for any general character
        
        if (current_char == '\n'){
            continue;
        }

        if (current_char != char_to_be_skipped && current_char != '\0') {
            current_word_buffer[current_word_char] = current_char;
            current_word_char ++;
            continue;
        }
 
        if (current_char == char_to_be_skipped && current_word_char == 0) {
            continue;
        }       

        // if we find the end of a word
        current_word_buffer[current_word_char] = '\0';

        strncpy(word_array[current_word_number], current_word_buffer, 256);

        current_word_number += 1;
        current_word_char = 0;

        if (current_char == '\0') {
            break;
        }
    }
    return current_word_number;
}


// for debugging
void print_parsed_words(char words[64][256], int n) {
    for (int i = 0; i<n; i++) {
        printf("%s \n", words[i]);
    }
}

void replace_wave_char(char* string) {
    if ((string [0] != '~' || string[1] != '/') && (string [0] != '~' || string[1] != '\0'))
        return;

    char username [256];
    getlogin_r(username, 256);

    char buffer_string [512];
    sprintf(buffer_string, "/home/%s/", username);

    strcat(buffer_string, &string[2]);

    strcpy(string, buffer_string);
}

