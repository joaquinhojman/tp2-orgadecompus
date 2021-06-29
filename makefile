CC=gcc
CFLAGS=-g -Wall -Wconversion -Wtype-limits -pedantic -O0 

main: *.c *.h
	$(CC) $(CFLAGS) *.c -o tp2 -lm
