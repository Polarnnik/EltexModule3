#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define MAX_READERS 2

sem_t *write_sem;
sem_t *reader_mutex;
sem_t *reader_limit_sem;
int *reader_count;

void init_shared_resources() {
    write_sem = sem_open("/write_sem", O_CREAT | O_EXCL, 0644, 1);
    reader_mutex = sem_open("/reader_mutex", O_CREAT | O_EXCL, 0644, 1);
    reader_limit_sem = sem_open("/reader_limit_sem", O_CREAT | O_EXCL, 0644, MAX_READERS);

    if (write_sem == SEM_FAILED || reader_mutex == SEM_FAILED || reader_limit_sem == SEM_FAILED) {
        perror("Ошибка создания семафоров");
        exit(EXIT_FAILURE);
    }

    int fd = shm_open("/reader_count", O_CREAT | O_RDWR, 0644);
    if (fd == -1) {
        perror("Ошибка создания разделяемой памяти");
        exit(EXIT_FAILURE);
    }
    ftruncate(fd, sizeof(int));
    reader_count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    *reader_count = 0;
}

void cleanup_shared_resources() {
    sem_close(write_sem);
    sem_close(reader_mutex);
    sem_close(reader_limit_sem);
    sem_unlink("/write_sem");
    sem_unlink("/reader_mutex");
    sem_unlink("/reader_limit_sem");

    shm_unlink("/reader_count");
}

void reader_process(const char *filename, int id) {
    for (int i = 0; i < 3; i++) {
        sem_wait(reader_limit_sem);

        sem_wait(reader_mutex);
        (*reader_count)++;
        if (*reader_count == 1) {
            sem_wait(write_sem);
        }
        sem_post(reader_mutex);

        // Чтение из файла
        FILE *file = fopen(filename, "r");
        if (!file) {
            perror("[Читатель] Не удалось открыть файл");
            exit(EXIT_FAILURE);
        }
        printf("[Читатель %d] Читает файл:\n", id);
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), file)) {
            printf("[Читатель %d] %s", id, buffer);
        }
        fclose(file);
        usleep(100000);

        sem_wait(reader_mutex);
        (*reader_count)--;
        if (*reader_count == 0) {
            sem_post(write_sem);
        }
        sem_post(reader_mutex);

        sem_post(reader_limit_sem);
        sleep(1);
    }
    exit(EXIT_SUCCESS);
}

void writer_process(const char *filename) {
    for (int i = 0; i < 5; i++) {
        sem_wait(write_sem);

        FILE *file = fopen(filename, "a");
        if (!file) {
            perror("[Писатель] Не удалось открыть файл");
            exit(EXIT_FAILURE);
        }
        int number = rand() % 100;
        printf("[Писатель] Записывает: %d\n", number);
        fprintf(file, "[Запись: %d]\n", number);
        fclose(file);

        sem_post(write_sem);
        sleep(2);
    }
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <файл>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *output_file = argv[1];
    init_shared_resources();

    FILE *file = fopen(output_file, "w");
    if (file) fclose(file);

    pid_t pids[5];
    int num_readers = 3, num_writers = 2;

    for (int i = 0; i < num_readers; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            reader_process(output_file, i + 1);
        }
    }

    for (int i = 0; i < num_writers; i++) {
        pids[num_readers + i] = fork();
        if (pids[num_readers + i] == 0) {
            writer_process(output_file);
        }
    }

    for (int i = 0; i < num_readers + num_writers; i++) {
        waitpid(pids[i], NULL, 0);
    }

    cleanup_shared_resources();
    return EXIT_SUCCESS;
}
