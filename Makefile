
CC = cc
CFLAGS = -Wall -Wextra -Iinclude

TARGET = myls

SRCS = src/main.c \
       src/options.c \
       src/listing.c \
       src/sorting.c \
       src/display.c \
       src/utils.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

test: $(TARGET)
	sh tests/test.sh

.PHONY: all clean test
