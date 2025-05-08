#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include <dirent.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "parsing.h"

// global variables
char g_paths[256][256];
int g_number_of_paths = 0;

void load_from_config() {
    char line[512];

    char config_path[256] = "~/.issrc";
    replace_wave_char(config_path);

    FILE* config_file = fopen(config_path, "r");

    if (config_file != NULL) {
        while (fgets(line, sizeof(line), config_file)) {
            char slots[2][256];
            int input_number = parse_into_words(line, slots, ' ');

            if (input_number != 2)
                continue;

            if (strcmp(slots[0], "path:") == 0) {
                replace_wave_char(slots[1]);
                strcpy(g_paths[g_number_of_paths], slots[1]);
                g_number_of_paths ++;
                //printf("loaded path: %s \n", slots[1]);
                continue;
            }
        }
        fclose(config_file);
    }
    else {
        fprintf(stderr, "iss: failed to load user config! \n");
    }

    g_number_of_paths = 2;
}

void print_prompt() {
    char username [256];
    getlogin_r(username, 256);

    char hostname [1024];
    gethostname(hostname, 1024);

    char current_dir [2048];
    getcwd(current_dir, 2048);

    printf("[%s@%s]%s: ", username, hostname, current_dir);
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

void print_pointer_array(char* array [], int number_of_element) {
    for (int i = 0; i < number_of_element; i++) {
        printf("index: %d, value: %s \n", i, array[i]);
    }
}

void execute_program(char* args[256]) {
    int rc = fork();

    // parse input for ~

    for (int i = 0; ;i++){
        if(args[i] == NULL)
            break;

        replace_wave_char(args[i]);
    }

    // fork failed
    if (rc < 0) {
        printf("iss: program failed to execute! \n");
        // child process
    } else if (rc == 0) {
        execvp(args[0], args);
        // parent process
    } else {
        int status;
        wait(&status);
    }
}

void process_command(char* word_list []) {
    char* command = word_list[0];
    char first_character = command[0];
    char second_character = command[1];
    char third_character = command[2];

    // handdle path possibilities
    if(    first_character == '/'
        || first_character == '.' && second_character == '/'
        || first_character == '.' && second_character == '.' && third_character == '/'
        || first_character == '~' && second_character == '/'
      ) {
        replace_wave_char(command);
        execute_program(word_list);
        return;
    }

    // handle "cd" command case
    if (strcmp(command, "cd") == 0) {
        replace_wave_char(word_list[1]);
        chdir(word_list[1]);
        return;
    }

    // throw error if there are no path variables
    if (g_number_of_paths == 0) {
        fprintf(stderr, "iss: no path variables! \n");
        return;
    }
    
    // look for command in path variables
    for (int i = 0; i < g_number_of_paths; i++) {
        FILE *file;
        char file_buffer [512];
        strcpy(file_buffer, g_paths[i]);
        strcat(file_buffer, command);

        //printf("attempting: %s \n", file_buffer);

        if((file = fopen(file_buffer,"r")) != NULL) {
            strcpy(command, file_buffer);
            execute_program(word_list);
            return;
        } 
    }
        
    fprintf(stderr, "iss: command not found! \n");
}

// contains the main program loop
int main() {
    load_from_config();

    while(true) {
        print_prompt();
        char user_input [1024];
        fgets(user_input, 1024, stdin);

        // parsing section
        char word_array [64][256];
        int word_number = parse_into_words(user_input, word_array, ' ');

        if (word_number == 1 && word_array[0][0] == '\0')
            continue;

        // calculating pointer array
        char* pointer_array [64];
        for (int i = 0; i < word_number; i++) {
            pointer_array[i] = word_array[i];
        }
        pointer_array[word_number] = NULL;

        process_command(pointer_array);
    }
}


