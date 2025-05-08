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
int number_of_paths;

void print_prompt() {
    char username [256];
    getlogin_r(username, 256);

    char hostname [1024];
    gethostname(hostname, 1024);

    printf("[%s@%s]%s: ", username, hostname, g_current_directory);
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

void execute_program(char program_path [2048]) {
    int rc = fork();

    // fork failed
    if (rc < 0) {
        printf("iss: program failed to execute! \n");
        // child process
    } else if (rc == 0) {
        execl(program_path, "foo");
        // parent process
    } else {
        int status;
        wait(&status);
    }
}

void process_command(char command [256]) {
    char first_character = command[0];

    // handdle full path
    if(first_character == '/') {
        execute_program(command);
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

        execute_program(buffer_string);
        return;
    }

    // handdle path starting with ./
    if(first_character == '.' && second_character == '/') {
        char buffer_string [512];

        strcpy(buffer_string, g_current_directory);
        strcat(buffer_string, &command[2]);

        execute_program(buffer_string);
        return;
    }

    // handdle path starting with ../
    char third_character = command[2];
    if(first_character == '.' && second_character == '.' && third_character == '/') {
        char buffer_string [512];

        strcpy(buffer_string, g_current_directory);
        strcat(buffer_string, "../");
        strcat(buffer_string, &command[2]);

        execute_program(buffer_string);
        return;
    }

}

// contains the main program loop
int main() {
    while(true) {
        print_prompt();
        char user_input [1024];
        fgets(user_input, 1024, stdin);

        // parsing section
        char word_array [64][256];
        int word_number = parse_into_words(user_input, word_array, ' ');

        if (word_number == 1 && word_array[0][0] == '\0')
            continue;

        // testing execution

        //char test_path [2048] = "/home/ideale/cProjects/linkedList/test";
        process_command(word_array[0]);
    }
}


