CC     = gcc
CFLAGS = -Wall -Wextra -g -I include

SRC = src/main.c \
      src/server.c \
      src/http.c \
      src/files.c \
      src/mime.c \
      src/passivesock.c \
      src/passiveTCP.c

OBJ = $(SRC:.c=.o)

TARGET = minihttpd

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean