#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_CONTACTS 100
#define MAX_STRING_LENGTH 50
#define FILE_NAME "contacts.dat" //Файл для открытия

typedef struct {
    char lastName[MAX_STRING_LENGTH];
    char firstName[MAX_STRING_LENGTH];
    char workPlace[MAX_STRING_LENGTH];
    char position[MAX_STRING_LENGTH];
    char phone[MAX_STRING_LENGTH];
    char email[MAX_STRING_LENGTH];
    char socialMedia[MAX_STRING_LENGTH];
    char messenger[MAX_STRING_LENGTH];
} Contact;

Contact contacts[MAX_CONTACTS];
int contactCount = 0;

void readInput(char *buffer, int length) {
    fgets(buffer, length, stdin);
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
}

void loadContacts() {
    int fd = open(FILE_NAME, O_RDONLY);
    if (fd == -1) {
        if (errno == ENOENT) {
            fd = open(FILE_NAME, O_WRONLY | O_CREAT, 0644);
            if (fd == -1) {
                perror("Ошибка при создании файла");
                return;
            }
            printf("Файл не найден. Создан новый файл для хранения контактов.\n");
            close(fd);
            return;
        } else {
            perror("Ошибка при открытии файла");
            return;
        }
    }

    ssize_t bytesRead = read(fd, &contactCount, sizeof(contactCount));
    if (bytesRead != sizeof(contactCount)) {
        perror("Ошибка чтения количества контактов");
        close(fd);
        return;
    }

    bytesRead = read(fd, contacts, contactCount * sizeof(Contact));
    if (bytesRead != contactCount * sizeof(Contact)) {
        perror("Ошибка чтения контактов");
    }

    close(fd);
}

void saveContacts() {
    int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Ошибка при открытии файла для записи");
        return;
    }

    ssize_t bytesWritten = write(fd, &contactCount, sizeof(contactCount));
    if (bytesWritten != sizeof(contactCount)) {
        perror("Ошибка записи количества контактов");
        close(fd);
        return;
    }

    bytesWritten = write(fd, contacts, contactCount * sizeof(Contact));
    if (bytesWritten != contactCount * sizeof(Contact)) {
        perror("Ошибка записи контактов");
    }

    close(fd);
}

void addContact() {
    if (contactCount >= MAX_CONTACTS) {
        printf("Телефонная книга заполнена.\n");
        return;
    }

    Contact newContact;

    printf("Введите фамилию: ");
    readInput(newContact.lastName, MAX_STRING_LENGTH);
    printf("Введите имя: ");
    readInput(newContact.firstName, MAX_STRING_LENGTH);

    printf("Введите место работы (необязательно): ");
    readInput(newContact.workPlace, MAX_STRING_LENGTH);
    printf("Введите должность (необязательно): ");
    readInput(newContact.position, MAX_STRING_LENGTH);
    printf("Введите номер телефона (необязательно): ");
    readInput(newContact.phone, MAX_STRING_LENGTH);
    printf("Введите email (необязательно): ");
    readInput(newContact.email, MAX_STRING_LENGTH);
    printf("Введите ссылку на соц. сети (необязательно): ");
    readInput(newContact.socialMedia, MAX_STRING_LENGTH);
    printf("Введите мессенджер (необязательно): ");
    readInput(newContact.messenger, MAX_STRING_LENGTH);

    contacts[contactCount++] = newContact;
    printf("Контакт добавлен!\n");
}

void displayContacts() {
    if (contactCount == 0) {
        printf("Телефонная книга пуста.\n");
        return;
    }
    printf("\n-------------------------\n");

    for (int i = 0; i < contactCount; i++) {
        printf("Контакт %d:\n", i + 1);
        printf("Фамилия: %s\n", contacts[i].lastName);
        printf("Имя: %s\n", contacts[i].firstName);
        printf("Место работы: %s\n", contacts[i].workPlace[0] ? contacts[i].workPlace : "Не указано");
        printf("Должность: %s\n", contacts[i].position[0] ? contacts[i].position : "Не указана");
        printf("Телефон: %s\n", contacts[i].phone[0] ? contacts[i].phone : "Не указан");
        printf("Email: %s\n", contacts[i].email[0] ? contacts[i].email : "Не указан");
        printf("Соц. сети: %s\n", contacts[i].socialMedia[0] ? contacts[i].socialMedia : "Не указано");
        printf("Мессенджер: %s\n", contacts[i].messenger[0] ? contacts[i].messenger : "Не указан");
        printf("-------------------------\n");
    }
}

void deleteContact() {
    int index;
    printf("Введите номер контакта для удаления: ");
    scanf("%d", &index);
    getchar();
    if (index < 1 || index > contactCount) {
        printf("Некорректный номер контакта.\n");
        return;
    }

    for (int i = index - 1; i < contactCount - 1; i++) {
        contacts[i] = contacts[i + 1];
    }
    contactCount--;
    printf("Контакт удален!\n");
}

int main() {
    loadContacts();

    int choice;
    while (1) {
        printf("\nТелефонная книга:\n");
        printf("1. Добавить контакт\n");
        printf("2. Показать все контакты\n");
        printf("3. Удалить контакт\n");
        printf("4. Выйти\n");
        printf("Выберите действие: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                addContact();
                break;
            case 2:
                displayContacts();
                break;
            case 3:
                deleteContact();
                break;
            case 4:
                saveContacts();
                printf("Выход из программы.\n");
                return 0;
            default:
                printf("Некорректный выбор, попробуйте снова.\n");
        }
    }
}
