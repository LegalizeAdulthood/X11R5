# This is the Makefile for et2hku

CC = cc
CFLAGS = -O

INSTALL = cp

BINDIR = /usr/local/bin
MANDIR = /usr/local/man/man1

all: et2hkubig5

et2hkubig5: et2hkubig5.c
	$(CC) $(CFLAGS) -o et2hkubig5 et2hkubig5.c

install:
	$(INSTALL) et2hkubig5 $(BINDIR)

install.man:
	$(INSTALL) et2hkubig5.1 $(MANDIR)

clean:
	rm -f et2hkubig5 core a.out *BAK *CKP
