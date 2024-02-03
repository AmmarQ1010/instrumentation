all: test test-inst

test: test.c inst.o
	gcc -Wall -o test test.c inst.o

test-inst: test.c inst.o
	gcc -Wall -o test-inst test.c inst.o

inst.o: inst.c inst.h
	gcc -Wall -c inst.c -o inst.o

clean:
	rm -f test test-inst *.o
.PHONY: all clean
