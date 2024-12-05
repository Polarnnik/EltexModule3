#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>

void generate_random_numbers(int count, int write_fd) {
    srand(time(NULL) ^ getpid());
    for (int i = 0; i < count; i++) {
        int number = rand() % 100;
        write(write_fd, &number, sizeof(int));
    }
    close(write_fd);
}

void process_numbers(int count, int read_fd, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Неполучилось открыть файл");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < count; i++) {
        int number;
        read(read_fd, &number, sizeof(int));
        printf("Получил: %d\n", number);
        fprintf(file, "%d\n", number);
    }

    fclose(file);
    close(read_fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <количество> <файл>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int count = atoi(argv[1]);
    if (count <= 0) {
        fprintf(stderr, "Количество должно быть больше нуля.\n");
        return EXIT_FAILURE;
    }

    const char *output_file = argv[2];

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("Не удалось создать pipe");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Не удалось создать дочерний процесс");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        close(pipe_fd[0]);
        generate_random_numbers(count, pipe_fd[1]);
        exit(EXIT_SUCCESS);
    } else {
        close(pipe_fd[1]);
        process_numbers(count, pipe_fd[0], output_file);
        wait(NULL);
    }

    return EXIT_SUCCESS;
}
