.DEFAULT:
	$(LINK) ../../../lib/util/$< $<

H=	../../src/config.h\
	../../src/object.h\
	../../src/extern.h\
	../../src/macros.h\
	../../src/alloca.h

C=	objects.c\
	symbol.c

O=	objects.o\
	symbol.o

all:	$(O)

objects.o:	objects.c objects.h $(H)
	$(CC) $(CFLAGS) -c -I../../src $<
symbol.o:	symbol.c symbol.h $(H)
	$(CC) $(CFLAGS) -c -I../../src $<

lint:
	lint $(LINTFLAGS) -abxh -I../../src $(C) | egrep -v '\?\?\?'
clean:
	rm -f *.o core a.out
