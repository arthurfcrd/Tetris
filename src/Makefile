UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
CFLAGS = -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE -std=c++14 -g
LDFLAGS = -L/opt/homebrew/lib -lSDL2 -lSDL2_image -lSDL2_net -lSDL2_ttf
endif

ifeq ($(UNAME), Linux)
CFLAGS= -Werror -std=c++14
LDFLAGS= -lSDL2 -lSDL2_image -lSDL2_net -lSDL2_ttf
endif

CC = g++

all: main.exe
	./main.exe
	rm -f *.o

main.exe: main.o graphics.o tetromino.o grid.o game.o keyboard.o point.o gui.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) -v
	rm -f *.o

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f *.o

realclean: clean
	rm -f *.exe
