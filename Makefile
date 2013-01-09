CFLAGS=-Wall -g

all:
	cc $(CFLAGS) ex1.c -o build/ex1
	cc $(CFLAGS) ex3.c -o build/ex3
	cc $(CFLAGS) ex10.c -o build/ex10
clean:
	rm -f build/*
