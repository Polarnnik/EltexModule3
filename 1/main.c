#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


void calculate(char* args[], int start, int end) {
    for (int i = start; i < end; i++) {
        int num = atoi(args[i]);
        printf("Квадрат %d: %d\n", num, num * num);
    }
}

int main(int argc, char* argv[]) {

    int middle = (argc - 1) /2;

    pid_t pid = fork();
    if (pid < 0) {
        perror("Ошибка при вызове fork: ");
        return 0;
    }
    else if (pid > 0) {
        calculate(argv, 1, middle + 1);
        wait(NULL);
    }
    else if (pid == 0) {
        calculate(argv, middle + 1, argc);
        return 0;
    }

    
    printf("Все сделанно\n");     
    return 0;
}