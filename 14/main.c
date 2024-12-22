#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFLEN 512
#define PORT1 8888
#define PORT2 8889

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Использование: %s <1|2>\n", argv[0]);
        return 1;
    }

    int client_num = atoi(argv[1]);
    if (client_num != 1 && client_num != 2) {
        printf("Аргумент должен быть 1 или 2\n");
        return 1;
    }

    struct sockaddr_in my_addr, other_addr;
    int sock, my_port, other_port;
    socklen_t slen = sizeof(other_addr);
    char buf[BUFLEN];

    if (client_num == 1) {
        my_port = PORT1;
        other_port = PORT2;
    } else {
        my_port = PORT2;
        other_port = PORT1;
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("socket");
        return 1;
    }

    memset((char *)&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(my_port);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) {
        perror("bind");
        return 1;
    }

    memset((char *)&other_addr, 0, sizeof(other_addr));
    other_addr.sin_family = AF_INET;
    other_addr.sin_port = htons(other_port);
    other_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Чат запущен. Для выхода введите 'exit'\n");

    while (1) {
        printf("Введите сообщение: ");
        fgets(buf, BUFLEN, stdin);
        buf[strcspn(buf, "\n")] = 0;

        if (sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&other_addr, slen) == -1) {
            perror("sendto");
            return 1;
        }

        if (strcmp(buf, "exit") == 0)
            break;

        memset(buf, 0, BUFLEN);
        if (recvfrom(sock, buf, BUFLEN, 0, (struct sockaddr *)&other_addr, &slen) == -1) {
            perror("recvfrom");
            return 1;
        }

        if (strcmp(buf, "exit") == 0)
            break;

        printf("Получено сообщение: %s\n", buf);
    }

    close(sock);
    return 0;
}