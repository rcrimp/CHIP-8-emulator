OBJS = main.cpp chip8.cpp
CC = g++
CFLAGS = -W -Wall -ansi -pedantic -g
LFLAGS = -lSDL2 -lGL
EXEC = c8-emu

all : $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LFLAGS) -o $(EXEC)
