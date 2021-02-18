MACHTYPE= 68k

all: test1 test2

test1: test1.o ../src/stack.o
	$(CC) $(CFLAGS) -o test1 test1.c ../src/stack.o

test2: test2.o ../src/stack.o
	$(CC) $(CFLAGS) -o test2 test2.c ../src/stack.o

../src/stack.o: ../src/stack.s
	cp ../src/stack.s.$(MACHTYPE) ../src/stack.s
	/lib/cpp <../src/stack.s | sed '/^#/d' >stack.ss
	as -o ../src/stack.o stack.ss
	rm stack.ss
