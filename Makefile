CC      = gcc
CFLAGS  = -Wall -Wextra -Iinclude
LDFLAGS = -lsqlite3
TARGET  = parqueo
SRCS    = main.c src/db.c
OBJS    = $(SRCS:.c=.o)

.PHONY: all clean run valgrind

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) parqueo.db

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)
