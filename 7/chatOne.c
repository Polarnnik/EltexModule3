#include "chat.h"

int main() {
    struct mq_attr attr = {
            .mq_flags = 0,
            .mq_maxmsg = 10,
            .mq_msgsize = MSG_SIZE,
            .mq_curmsgs = 0
    };

    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0666, &attr);
    if (mq == (mqd_t)-1) {
        perror("Не удалось открыть очередь сообщений");
        exit(EXIT_FAILURE);
    }

    char buffer[MSG_SIZE];
    while (1) {
        printf("[Отправитель]: Введите сообщение: ");
        fgets(buffer, MSG_SIZE, stdin);

        if (strcmp(buffer, "exit") == 0) {
            if (mq_send(mq, buffer, strlen(buffer) + 1, END_PRIORITY) == -1) {
                perror("Ошибка отправки сообщения");
            }
            break;
        }

        if (mq_send(mq, buffer, strlen(buffer) + 1, 1) == -1) {
            perror("Ошибка отправки сообщения");
            exit(EXIT_FAILURE);
        }

        printf("[Отправитель]: Ожидание ответа...\n");

        ssize_t bytes_read = mq_receive(mq, buffer, MSG_SIZE, NULL);
        if (bytes_read == -1) {
            perror("Ошибка получения ответа");
            exit(EXIT_FAILURE);
        }

        printf("[Получатель]: %s\n", buffer);
    }

    mq_close(mq);
    printf("[Отправитель]: Завершение работы.\n");
    return 0;
}
