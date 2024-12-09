#include "chat.h"

int main() {
    key_t key = ftok("chat", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT); // Подключаемся к очереди сообщений

    if (msgid == -1) {
        perror("Не удалось подключиться к очереди сообщений");
        exit(EXIT_FAILURE);
    }

    struct message msg;
    while (1) {
        printf("[Получатель]: Ожидание сообщения...\n");
        if (msgrcv(msgid, &msg, sizeof(msg.text), 1, 0) == -1) {
            perror("Ошибка получения сообщения");
            exit(EXIT_FAILURE);
        }

        if (msg.priority == END_PRIORITY) {
            printf("[Получатель]: Получено сообщение о завершении. Завершение работы.\n");
            break;
        }

        printf("[Отправитель]: %s\n", msg.text);

        printf("[Получатель]: Введите ответ: ");
        fgets(msg.text, MSG_SIZE, stdin);
        msg.text[strcspn(msg.text, "\n")] = 0; // Убираем символ новой строки

        msg.priority = 2; // Приоритет для получателя
        if (msgsnd(msgid, &msg, sizeof(msg.text), 0) == -1) {
            perror("Ошибка отправки ответа");
            exit(EXIT_FAILURE);
        }
    }

    msgctl(msgid, IPC_RMID, NULL); // Удаляем очередь сообщений
    return 0;
}
