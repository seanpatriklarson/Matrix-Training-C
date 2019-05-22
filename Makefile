all: learn

learn:
	gcc -g -Wall -Werror -fsanitize=address learn.c -o learn -lm

clean:
	rm -rf learn