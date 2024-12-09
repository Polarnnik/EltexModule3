#include "chat.h"

int main() {
    struct mq_attr attr = {
            .mq_flags = 0,
            .mq_maxmsg = 10,
            .mq_msgsize = MSG_SIZE,
            .mq_curmsgs = 0
    };

    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
    if (mq == (mqd_t)-1) {
        perror("Не удалось открыть очередь сообщений");
        exit(EXIT_FAILURE);
    }

    char buffer[MSG_SIZE];
    unsigned int priority;
    while (1) {
        printf("[Получатель]: Ожидание сообщения...\n");

        ssize_t bytes_read = mq_receive(mq, buffer, MSG_SIZE, &priority);
        if (bytes_read == -1) {
            perror("Ошибка получения сообщения");
            exit(EXIT_FAILURE);
        }

        if (priority == END_PRIORITY) {
            printf("[Получатель]: Получено сообщение о завершении. Завершение работы.\n");
            break;
        }

        printf("[Отправитель]: %s\n", buffer);

        printf("[Получатель]: Введите ответ: ");
        fgets(buffer, MSG_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if (mq_send(mq, buffer, strlen(buffer) + 1, 1) == -1) {
            perror("Ошибка отправки ответа");
            exit(EXIT_FAILURE);
        }
    }

    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    return 0;
}
