#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include <dirent.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

// GLOBAL VARIABLES
char g_current_directory [2048] = "/home/ideale/cProjects/iss/src/";

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
        
        if (current_char == '\n'){
            continue;
        }

        if (current_char != ' ' && current_char != '\0') {
            current_word_buffer[current_word_char] = current_char;
            current_word_char ++;
            continue;
        }
 
        if (current_char == ' ' && current_word_char == 0) {
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

int is_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int is_dir(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

void remove_last_char(char* str) {
  if (str != NULL && str[0] != '\0') {
    size_t len = strlen(str);
    str[len - 1] = '\0';
  }
}

void truncate_spaces(char* str) {
    for (int i = 0; ; i++) {
        if (str[i] == ' '){
            str[i] = '\0';
            return;
        }

        if (str[i] == '\0'){
            return;
        }

    }
}

void go_back_dir() {
    if (strcmp(g_current_directory, "/") == 0)
        return;

    int length = strlen(g_current_directory);
    g_current_directory[length - 1] = '\0';

    for (int i = length - 1; i >= 0; i--) {
        if (g_current_directory[i] == '/') {
            g_current_directory[i+1] = '\0';
            return;
        }
    }
}

void ls () {
    DIR *dir;
    struct dirent *ent;
    dir = opendir(g_current_directory);
    if (dir != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            char item_dir [2048];
            strcpy(item_dir, g_current_directory);
            strcat(item_dir, ent->d_name);

            if (is_file(item_dir))
                printf("*FILE*  %s\n", ent->d_name);
            else if (is_dir(item_dir))
                printf("*DIR*   %s\n", ent->d_name);
            else
                printf("*OTHER* %s\n", ent->d_name);
        }
        closedir(dir);
    }
}

void cd (char directory [256]) {
    
    // Edge cases
    if (strcmp(directory, ".") == 0) {
        return;
    }

    if (strcmp(directory, "..") == 0) {
        go_back_dir();
        return;
    }

    // Try the directory as is:
    char directory_buffer [2048];
    struct stat s = {0};

    strcpy(directory_buffer, directory);

    // If the file exists:
    if (!stat(directory, &s)) {
        strcpy(g_current_directory, directory_buffer);
        return;
    }

    // Try the directory where we are located:
    strcpy(directory_buffer, g_current_directory);
    strcat(directory_buffer, directory);
    strcat(directory_buffer, "/");

    if (!stat(directory_buffer, &s)) {
        strcpy(g_current_directory, directory_buffer);
        return;
    }

    // If both options fail
    fprintf(stderr, "cd: dir not found! \n");
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

        // Iss specific commands
        if (strcmp(word_array[0], "exit") == 0) {
            break;
        }

        if (strcmp(word_array[0], "ls") == 0) {
            if (word_number > 1) {
                fprintf(stderr, "ls: too many arguments! \n");
                continue;
            }
            ls();
            continue;
        }

        if (strcmp(word_array[0], "cd") == 0) {
            if (word_number > 2) {
                fprintf(stderr, "cd: too many arguments! \n");
                continue;
            }
            
            if (word_number == 1) {
                fprintf(stderr, "cd: too few arguments! \n");
                continue;
            }
            cd(word_array[1]);

            continue;
        }
        
        /*
        int rc = fork();
        
        // Fork failed
        if (rc < 0) {
            printf("iss: program failed to execute! \n");
        // Child process
        } else if (rc == 0) {
            execl("/home/ideale/cProjects/linkedList/test", "foo");
        // Parent process
        } else {
            int status;
            wait(&status);
        }
        */
    }
}


