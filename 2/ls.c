#include <stdio.h>
#include <dirent.h>

int main(int argc, char *argv[]) {
    const char *dir_name = ".";
    if (argc == 2) {
        dir_name = argv[1];
    }

    DIR *dir = opendir(dir_name);
    if (dir == NULL) {
        perror(" Не получилось");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
    return 0;
}