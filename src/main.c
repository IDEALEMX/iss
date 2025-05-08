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
char g_current_directory [2048] = "/home/ideale/";
char g_paths [256][256];
int g_number_of_paths = 0;

void load_paths() {
    strcpy(g_paths[0], "/home/ideale/scripts/");
    strcpy(g_paths[1], "/bin/");

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

void execute_program(char program_path [2048], char arg1 [256], char arg2 [256], char arg3 [256], char arg4 [256]) {
    int rc = fork();

    // fork failed
    if (rc < 0) {
        printf("iss: program failed to execute! \n");
        // child process
    } else if (rc == 0) {
        execl(program_path, arg1, arg2, arg3, arg4);
        // parent process
    } else {
        int status;
        wait(&status);
    }
}

void process_command(char command [256], char arg1 [256], char arg2 [256], char arg3 [256], char arg4 [256]) {
    char first_character = command[0];

    // handdle full path
    if(first_character == '/') {
        execute_program(command, arg1, arg2, arg3, arg4);
        return;
    }

    // handdle path starting with ~/
    char second_character = command[1];
    if(first_character == '~' && second_character == '/') {
        char username [256];
        getlogin_r(username, 256);

        char buffer_string [512];
        sprintf(buffer_string, "/home/%s/", username);

        strcat(buffer_string, &command[2]);

        execute_program(buffer_string, arg1, arg2, arg3, arg4);
        return;
    }

    // handdle path starting with ./
    if(first_character == '.' && second_character == '/') {
        char buffer_string [512];

        strcpy(buffer_string, g_current_directory);
        strcat(buffer_string, &command[2]);

        execute_program(buffer_string, arg1, arg2, arg3, arg4);
        return;
    }

    // handdle path starting with ../
    char third_character = command[2];
    if(first_character == '.' && second_character == '.' && third_character == '/') {
        char buffer_string [512];

        strcpy(buffer_string, g_current_directory);
        strcat(buffer_string, "../");
        strcat(buffer_string, &command[2]);

        execute_program(buffer_string, arg1, arg2, arg3, arg4);
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

        // printf("attempting: %s \n", file_buffer);

        if((file = fopen(file_buffer,"r")) != NULL) {
            execute_program(file_buffer, arg1, arg2, arg3, arg4);
            return;
        } 
    }
        
    fprintf(stderr, "iss: command not found! \n");

}

// contains the main program loop
int main() {
    load_paths();

    while(true) {
        print_prompt();
        char user_input [1024];
        fgets(user_input, 1024, stdin);

        // parsing section
        char word_array [64][256];
        int word_number = parse_into_words(user_input, word_array, ' ');

        if (word_number == 1 && word_array[0][0] == '\0')
            continue;

        process_command(word_array[0], word_array[1], word_array[2], word_array[3], word_array[4]);
    }
}


