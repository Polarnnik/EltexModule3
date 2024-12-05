# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Файлы
SRC = cd.c fup.c main.c ls.c sum.c
OBJ = $(SRC:.c=)

# Правило по умолчанию
all: $(OBJ)

# Компиляция каждого файла отдельно
%: %.c
	$(CC) $(CFLAGS) $< -o $@

# Очистка
clean:
	rm -f $(OBJ)
