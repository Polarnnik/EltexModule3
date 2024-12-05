#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 256
#define MAX_ARG_COUNT 10

void print_prompt() {
    printf("Shield> ");
}

int main() {
    char input[MAX_COMMAND_LENGTH];
    char *args[MAX_ARG_COUNT + 1];
    char *token;
    pid_t pid;
    int status;

    while (1) {
        print_prompt();

        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Ошибка чтения строки");
            continue;
        }

        input[strcspn(input, "\n")] = '\0';

        int i = 0;
        token = strtok(input, " ");
        while (token != NULL && i < MAX_ARG_COUNT) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (args[0] == NULL) continue;
        if (strcmp(args[0], "exit") == 0) break;

        pid = fork();
        if (pid == 0) {
            // Дочерний процесс
            if (execv(args[0], args) == -1) {
                perror("Ошибка выполнения команды");
            }
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("Fork не удался");
        } else {
            // Родительский процесс
            waitpid(pid, &status, 0);
        }
    }

    printf("Выход из интерпиритатора\n");
    return 0;
}
