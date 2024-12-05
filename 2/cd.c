#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Использование: cd <директория>\n");
        return 1;
    }

    if (chdir(argv[1]) == 0) {
        printf("Директория поменяна на %s\n", argv[1]);
    } else {
        perror("Не получилось");
    }

    return 0;
}