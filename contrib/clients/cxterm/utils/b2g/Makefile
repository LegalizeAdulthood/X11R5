# M. Leisher Fri Dec 21 08:57:56 1990

CC = cc
CFLAGS = -O

SRCS = b2g.c
OBJS = b2g.o

BINDIR = /usr/local/bin
MANDIR = /usr/local/man/man1

# Before you make, you need to change the following two line to 
# where you put these two table!
BTOG_TABLE = `pwd`/b2g.table
GTOB_TABLE = `pwd`/g2b.table

all: b2g

b2g: $(OBJS)
	$(CC) $(CFLAGS) -o b2g $(OBJS)

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -DBTOG_TABLE=\"$(BTOG_TABLE)\" \
                        -DGTOB_TABLE=\"$(GTOB_TABLE)\" -c b2g.c

install:
	install -g system -m 775 b2g $(BINDIR)

install.man:
	install -g system -m 664 b2g.1 $(MANDIR)

clean:
	rm -f *.o core *BAK *CKP .*BAK .*CKP b2g
