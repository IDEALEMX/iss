#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

// GLOBAL VARIABLES
char g_current_directory [2048] = "/home/ideale/";

void print_prompt() {
    char username [256];
    getlogin_r(username, 256);

    char hostname [1024];
    gethostname(hostname, 1024);

    printf("[%s@%s]%s: ", username, hostname, g_current_directory);
}

// Parses the user input into an array of strings and returns the number of words
int parse_into_words(char* to_be_parsed_string, char word_array [64][256]) {
    char current_word_buffer [128];
    int current_word_char = 0;

    int current_word_number = 0;

    // return a 0 if input was empty
    if (to_be_parsed_string[0] == '\0')
        return 0;
    
    for (int i = 0; ; i++) {
        char current_char = to_be_parsed_string[i];

        // Handle overflows
        if (current_word_char >= 127) {
            current_word_buffer[127] = '\0';
        }

        if (current_word_number >= 64) {
            break; 
        }

        // For any general character
        if (current_char != ' ' && current_char != '\0') {
            current_word_buffer[current_word_char] = current_char;
            current_word_char ++;
            continue;
        }
        
        // If we find the end of a word
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


// Contains the main program loop
int main() {
    while(true) {
        print_prompt();
        char user_input [1024];
        fgets(user_input, 1024, stdin);

        // Parsing section
        char word_array [64][256];
        int word_number = parse_into_words(user_input, word_array);
        //print_parsed_words(word_array, word_number);
        
    }
}


