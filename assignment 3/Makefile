all: mandel mandelmovie

mandel: mandel.o bitmap.o
	gcc mandel.o bitmap.o -o mandel -lpthread

mandelmovie: mandelmovie.o bitmap.o
	gcc mandelmovie.o bitmap.o -o mandelmovie -lpthread

mandel.o: mandel.c
	gcc -Wall -g -c mandel.c -o mandel.o

mandelmovie.o: mandelmovie.c
	gcc -Wall -g -c mandelmovie.c -o mandelmovie.o

bitmap.o: bitmap.c
	gcc -Wall -g -c bitmap.c -o bitmap.o

clean:
	rm -f mandel.o mandelmovie.o bitmap.o mandel mandelmovie
