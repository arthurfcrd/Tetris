UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
CFLAGS = -I/opt/homebrew/include -D_THREAD_SAFE -std=c++14 -g -Werror -Wall
LDFLAGS = -L/opt/homebrew/lib -lSDL2 -lSDL2_image -lSDL2_net -lSDL2_ttf -lSDL2_mixer
endif

ifeq ($(UNAME), Linux)
CFLAGS= -g -Wall -Werror -std=c++14
LDFLAGS= -lSDL2 -lSDL2_image -lSDL2_net -lSDL2_ttf -lSDL2_mixer
endif

CC = g++

all: main.exe
	./main.exe
	rm -f *.o

main.exe: main.o graphics.o tetromino.o grid.o game.o keyboard.o point.o gui.o music.o online.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) -v
	rm -f *.o

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^

solo: main.cpp
	$(CC) $(CFLAGS) gui.cpp main.cpp -o main.exe $(LDFLAGS)
	./main.exe

clean:
	rm -f *.o

realclean: clean
	rm -f *.exe
