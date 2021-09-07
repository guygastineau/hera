CC = gcc

TARGET = hera

LFLAGS =

LIBS =

CFLAGS  = -std=c11
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -Werror
CFLAGS += -Wmissing-declarations
CFLAGS += -DUNITY_SUPPORT_64

ASANFLAGS  = -fsanitize=address
ASANFLAGS += -fno-common
ASANFLAGS += -fno-omit-frame-pointer

all: build test main
	@echo "All finished!"

test: tests.out
	@./tests.out

memcheck: test/*.c src/*.c src/*.h
	@echo Compiling $@
	@$(CC) $(ASANFLAGS) $(CFLAGS) src/*.c test/vendor/unity.c test/*.c -o memcheck.out $(LIBS)
	@./memcheck.out
	@echo "Memory check passed"

build.all: main
	@echo "Building library and binary for project: $(TARGET)..."

main: main.c src/*.h src/*.c
	$(CC) $(CFLAGS) -O3 main.c src/*.c -o $(TARGET) $(LIBS) $(DEFS)

tests.out: test/*.c src/*.c src/*.h
	@echo Compiling $@
	@$(CC) $(CFLAGS) src/*.c test/vendor/unity.c test/*.c -o tests.out $(LIBS) $(DEFS)

clean:
	@rm -rf *.out

deep.clean: clean
	@rm -rf *.o *.a *.S
