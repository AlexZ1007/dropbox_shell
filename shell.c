#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>


/**
    * Function to execute the exit command.
    * It saves the history and exits the shell.
    * @param cmd The command string to be freed.
*/

void execute_exit(char *cmd) {
    printf("Exiting Dropbox Shell...\n");
    write_history(".dropbox_history");
    free(cmd);
    exit(0);
}


/**
    * Function to save the command to history
    * if the command is not empty and not a duplicate.
    * @param cmd The command string to be saved.
*/
void save_to_history(char *cmd) {
    if (!(cmd && *cmd))
        return;

    if(history_length > 0){
        HIST_ENTRY *last_entry = history_get(history_length);
        if (last_entry && strcmp(last_entry->line, cmd) == 0) 
            return; 
    }

    add_history(cmd);
    
}

/**
    * Function to read a command from the user.
    * If EOF, it calls execute_exit.
    * @return The command string entered by the user.
*/
char* read_command() {
    char *cmd = readline("dropbox> ");

    if (cmd == NULL) 
        execute_exit(cmd);
    

    save_to_history(cmd);

    return cmd;
}


/**
    * Function to generate arguments for the command.
    * It splits the command string into tokens.
    * @param cmd The command string to be tokenized.
    * @param args The array to store the command and its arguments.
*/
void generateArgs(char *cmd, char *args[]) {
    char *token = strtok(cmd, " ");
    int i = 1;
    while (token != NULL) {
        args[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    args[i] = NULL; 
}



/**
    * Function to execute the command.
    * It forks a new process to run the command using execvp.
    * If the command is "exit", it calls execute_exit.
    * @param cmd The command string to be executed.
*/
void execute_command(char *cmd) {
    if (strcmp(cmd, "exit") == 0) 
        execute_exit(cmd);
    

    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        return;
    } else if (pid == 0) {
        char *args[] = {"dbxcli"};
        generateArgs(cmd, args);
        execvp(args[0], args);
        perror("execvp failed");
        return;
    } else {
        wait(NULL); 
        // NOTE: the ls command does not print newline by default
        if(strcmp(cmd, "ls") == 0)
            printf("\n");
    }
}




int main() {
    printf("******* WELCOME TO DROPBOX SHELL *******\n");

    read_history(".dropbox_history");

    while (1) {   
        char *cmd = read_command();

        execute_command(cmd);

        free(cmd);

    }

    return 0;
}
