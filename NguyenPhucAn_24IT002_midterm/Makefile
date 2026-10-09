CC = cc
CFLAGS = -Wall -Wextra -Iinclude
SRCS = src/main.c src/options.c src/listing.c src/sorting.c src/display.c src/utils.c
OBJS = src/main.o src/options.o src/listing.o src/sorting.o src/display.o src/utils.o

all: myls
myls: $(OBJS)
	$(CC) $(CFLAGS) -o myls $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
src/main.o: src/main.c include/options.h include/listing.h
src/options.o: src/options.c include/options.h
src/listing.o: src/listing.c include/listing.h include/sorting.h include/display.h
src/sorting.o: src/sorting.c include/sorting.h
src/display.o: src/display.c include/display.h include/utils.h
src/utils.o: src/utils.c include/utils.h
clean:
	rm -f $(OBJS) myls myls.exe
test: myls
	sh tests/test.sh
.PHONY: all clean test
