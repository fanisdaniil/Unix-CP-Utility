all:
	gcc -c -g main.c
	gcc -c -g userInterface.c

	gcc -o main main.o userInterface.o
	rm -rf ./tmp/*
clean:
	rm -rf *.o main
