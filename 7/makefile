
CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGETS = chatOne chatTwo

all: $(TARGETS)

sender: sender.c chat.h
	$(CC) $(CFLAGS) -o sender sender.c

receiver: receiver.c chat.h
	$(CC) $(CFLAGS) -o receiver receiver.c

clean:
	rm -f $(TARGETS)

distclean: clean
	rm -f chat
