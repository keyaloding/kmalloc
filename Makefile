all: test

test: test.c kmalloc.c
	gcc -o test test.c kmalloc.c -I. -Werror

clean:
	rm -f test
