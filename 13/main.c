#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>

#define SIZE 1024
#define ARRAY_OFFSET 3

volatile sig_atomic_t stop = 0;
int sets_processed = 0;

void handle_sigint(int sig) {
    stop = 1;
}

void handle_sigusr1(int sig) {
    // Пустая обработка для выхода из pause
}

void generate_random_numbers(int *array, int *size) {
    *size = rand() % 20 + 1;
    for (int i = 0; i < *size; i++) {
        array[i] = rand() % 100;
    }
}

void find_min_max(int *array, int size, int *min, int *max) {
    *min = array[0];
    *max = array[0];
    for (int i = 1; i < size; i++) {
        if (array[i] < *min) *min = array[i];
        if (array[i] > *max) *max = array[i];
    }
}

int main() {
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    signal(SIGUSR1, handle_sigusr1);
    srand(time(NULL));

    // Создание разделяемой памяти
    int fd = shm_open("/shared_memory", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open failed");
        exit(1);
    }

    if (ftruncate(fd, SIZE) == -1) {
        perror("ftruncate failed");
        exit(1);
    }

    int *shared_memory = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    shared_memory[0] = 0;  // size
    shared_memory[1] = 0;  // min
    shared_memory[2] = 0;  // max

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid > 0) {
        while (!stop) {
            int size;
            generate_random_numbers(shared_memory + ARRAY_OFFSET, &size);
            shared_memory[0] = size;

            printf("\nСгенерирован набор из %d чисел: ", size);
            for (int i = 0; i < size; i++) {
                printf("%d ", shared_memory[ARRAY_OFFSET + i]);
            }
            printf("\n");

            kill(pid, SIGUSR1);

            pause();

            printf("Набор %d - Мин: %d, Макс: %d\n",
                   sets_processed + 1, shared_memory[1], shared_memory[2]);
            sets_processed++;

            usleep(100000);
        }

        kill(pid, SIGINT);
        wait(NULL);

    } else {
        while (!stop) {
            pause();
            if (stop) break;

            int size = shared_memory[0];
            int min, max;
            find_min_max(shared_memory + ARRAY_OFFSET, size, &min, &max);

            shared_memory[1] = min;
            shared_memory[2] = max;

            kill(getppid(), SIGUSR1);
        }
    }

    if (pid > 0) {
        printf("\nВсего обработано наборов данных: %d\n", sets_processed);
    }

    munmap(shared_memory, SIZE);
    close(fd);
    if (pid > 0) {
        shm_unlink("/shared_memory");
    }

    return 0;
}