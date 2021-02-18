.DEFAULT:
	$(LINK) ../../lib/$< $<

.c.o:
	$(CC) $(CFLAGS) -c -I../src $<

H=	../src/config.h\
	../src/object.h\
	../src/extern.h\
	../src/macros.h\
	../src/alloca.h\
	util/string.h\
	util/symbol.h

C=	string.c\
	when.c\
	chdir.c\
	hunk.c\
	monitor.c\
	struct.c\
	hack.c\
	debug.c\
	unix.c\
	c++.c

O=	string.o\
	when.o\
	chdir.o\
	hunk.o\
	monitor.o\
	struct.o\
	hack.o\
	debug.o\
	unix.o\
	c++.o

all:		$(O)

string.o:	$(H) string.c
when.o:		$(H) when.c
chdir.o:	$(H) chdir.c
hunk.o:		$(H) hunk.c
struct.o:	$(H) struct.c
hack.o:		$(H) hack.c
debug.o:	$(H) debug.c
unix.o:		$(H) unix.c
c++.o:		$(H) c++.c
monitor.o:	$(H) monitor.c
weak.o:		$(H) weak.c

util/string.h:
	$(LINK) ../../../lib/util/string.h util/string.h

util/symbol.h:
	$(LINK) ../../../lib/util/symbol.h util/symbol.h

lint:
	lint $(LINTFLAGS) -I../src -abxh $(C) | egrep -v '\?\?\?'

clean:
	rm -f *.o core a.out
