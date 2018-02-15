
all:
	gcc -o neshash neshash.c

clean:
	rm -f neshash

rebuild: clean all
