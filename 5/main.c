#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

volatile sig_atomic_t access_allowed = 1;

void sigusr1_handler(int signum) {
    (void)signum;
    access_allowed = 0;
}

void sigusr2_handler(int signum) {
    (void)signum;
    access_allowed = 1;
}

void generate_random_numbers(int count, int write_fd) {
    srand(time(NULL) ^ getpid());
    for (int i = 0; i < count; i++) {
        int number = rand() % 100;
        write(write_fd, &number, sizeof(int));
    }
    close(write_fd);
}

void child_process(int count, int read_fd, int write_fd, const char *filename) {
    struct sigaction sa1 = {0};
    sa1.sa_handler = sigusr1_handler;
    sigaction(SIGUSR1, &sa1, NULL);

    struct sigaction sa2 = {0};
    sa2.sa_handler = sigusr2_handler;
    sigaction(SIGUSR2, &sa2, NULL);

    for (int i = 0; i < count; i++) {
        int number;
        read(read_fd, &number, sizeof(int));
        printf("[Дочерний] Получил: %d\n", number);

        while (!access_allowed) {
            usleep(1000);
        }

        FILE *file = fopen(filename, "a");
        if (!file) {
            perror("[Дочерний] Не удалось открыть файл");
            exit(EXIT_FAILURE);
        }
        fprintf(file, "%d\n", number);
        fclose(file);

        int new_number = number + 1;
        write(write_fd, &new_number, sizeof(int));
    }

    close(read_fd);
    close(write_fd);
    exit(EXIT_SUCCESS);
}

void parent_process(int count, int read_fd, int write_fd, const char *filename, pid_t child_pid) {
    for (int i = 0; i < count; i++) {
        int number = rand() % 100;
        write(write_fd, &number, sizeof(int));
        printf("[Родитель] Отправил: %d\n", number);

        kill(child_pid, SIGUSR1);

        // Модификация файла
        FILE *file = fopen(filename, "a");
        if (!file) {
            perror("[Родитель] Не удалось открыть файл");
            exit(EXIT_FAILURE);
        }
        fprintf(file, "[Родительская запись: %d]\n", number * 2);
        fclose(file);

        kill(child_pid, SIGUSR2);

        int new_number;
        read(read_fd, &new_number, sizeof(int));
        printf("[Родитель] Получил новое число: %d\n", new_number);
    }

    close(read_fd);
    close(write_fd);
    wait(NULL);
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

    int pipe1[2], pipe2[2];
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Не удалось создать pipe");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Не удалось создать дочерний процесс");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        close(pipe1[1]);
        close(pipe2[0]);
        child_process(count, pipe1[0], pipe2[1], output_file);
    } else {
        close(pipe1[0]);
        close(pipe2[1]);
        parent_process(count, pipe2[0], pipe1[1], output_file, pid);
    }

    return EXIT_SUCCESS;
}
