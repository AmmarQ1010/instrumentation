all: test test-inst calltraceviz

inst.o: inst.c
	gcc -Wall -c inst.c
	
test: test.c
	gcc -Wall -o test test.c

test-inst: test.c inst.o
	gcc -Wall -finstrument-functions test.c inst.o -o test-inst

calltraceviz: calltraceviz.c
	gcc -Wall calltraceviz.c -o calltraceviz

clean:
	rm -f test-inst inst.o calltraceviz
