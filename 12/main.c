#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#define SHM_SIZE 1024
#define MAX_NUMBERS 100

volatile sig_atomic_t stop = 0;
int sets_processed = 0;

typedef struct {
    int numbers[MAX_NUMBERS];
    int count;
    int min;
    int max;
    int ready;
} shared_data_t;

void sigint_handler(int signum) {
    stop = 1;
}

void parent_process(int shmid, pid_t child_pid) {
    shared_data_t *data = (shared_data_t *)shmat(shmid, NULL, 0);
    if (data == (void *)-1) {
        perror("Ошибка при подключении к разделяемой памяти (родитель)");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    while (!stop) {
        int num_count = rand() % 10 + 1;
        data->count = num_count;
        for (int i = 0; i < num_count; i++) {
            data->numbers[i] = rand() % 100;
        }
        data->ready = 1;

        printf("[Родитель] Сгенерировано %d чисел: ", num_count);
        for (int i = 0; i < num_count; i++) {
            printf("%d ", data->numbers[i]);
        }
        printf("\n");

        while (data->ready == 1) {
            usleep(1000);
        }

        printf("[Родитель] Найдено: min = %d, max = %d\n", data->min, data->max);
        sets_processed++;
        sleep(1);
    }

    printf("[Родитель] Получено сигнала SIGINT. Обработано наборов: %d\n", sets_processed);
    kill(child_pid, SIGINT);
    wait(NULL);
    shmdt(data);
}

void child_process(int shmid) {
    shared_data_t *data = (shared_data_t *)shmat(shmid, NULL, 0);
    if (data == (void *)-1) {
        perror("Ошибка при подключении к разделяемой памяти (дочерний)");
        exit(EXIT_FAILURE);
    }

    while (!stop) {
        if (data->ready == 1) {
            int min = data->numbers[0];
            int max = data->numbers[0];
            for (int i = 1; i < data->count; i++) {
                if (data->numbers[i] < min) min = data->numbers[i];
                if (data->numbers[i] > max) max = data->numbers[i];
            }
            data->min = min;
            data->max = max;
            data->ready = 0;
            printf("[Дочерний] Обработал данные: min = %d, max = %d\n", min, max);
        }
        usleep(1000);
    }

    printf("[Дочерний] Завершение работы по сигналу SIGINT.\n");
    shmdt(data);
    exit(EXIT_SUCCESS);
}

int main() {
    signal(SIGINT, sigint_handler);

    // Создание разделяемой памяти
    int shmid = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Ошибка при создании разделяемой памяти");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Ошибка при создании дочернего процесса");
        shmctl(shmid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        child_process(shmid);
    } else {
        // Родительский процесс
        parent_process(shmid, pid);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return EXIT_SUCCESS;
}
