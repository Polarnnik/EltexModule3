#include "chat.h"

int main() {
    key_t key = ftok("chat", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);

    if (msgid == -1) {
        perror("Не удалось создать очередь сообщений");
        exit(EXIT_FAILURE);
    }

    struct message msg;
    while (1) {
        printf("[Отправитель]: Введите сообщение: ");
        fgets(msg.text, MSG_SIZE, stdin);
        msg.text[strcspn(msg.text, "\n")] = 0;

        if (strcmp(msg.text, "exit") == 0) {
            msg.priority = END_PRIORITY;
            msgsnd(msgid, &msg, sizeof(msg.text), 0);
            break;
        }

        msg.priority = 1;
        if (msgsnd(msgid, &msg, sizeof(msg.text), 0) == -1) {
            perror("Ошибка отправки сообщения");
            exit(EXIT_FAILURE);
        }

        printf("[Отправитель]: Ожидание ответа...\n");
        if (msgrcv(msgid, &msg, sizeof(msg.text), 2, 0) == -1) {
            perror("Ошибка получения ответа");
            exit(EXIT_FAILURE);
        }

        printf("[Получатель]: %s\n", msg.text);
    }

    printf("[Отправитель]: Завершение работы.\n");
    return 0;
}
