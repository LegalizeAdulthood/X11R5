# Makefile for ndbm-clone: sdbm
#
# Copyright (c) 1991 by Ozan S. Yigit
#
# Permission to use, copy, modify, and distribute this software and its
# documentation for any purpose and without fee is hereby granted,
# provided that the above copyright notice appear in all copies and that
# both that copyright notice and this permission notice appear in
# supporting documentation.
#
# This file is provided AS IS with no warranties of any kind.  The author
# shall have no liability with respect to the infringement of copyrights,
# trade secrets or any patents by this file or any part thereof.  In no
# event will the author be liable for any lost revenue or profits or
# other special, indirect and consequential damages.
#
#			oz@nexus.yorku.ca
#
#			Ozan S. Yigit
#			Dept. of Computing and Communication Svcs.
#			Steacie Science Library. T103
#			York University
#			4700 Keele Street, North York
#			Ontario M3J 1P3
#			Canada

# DUFF: use duff's device (loop unroll) in parts of the code
#
CFLAGS = -O -DSDBM -DDUFF -DBSD42 -DNULLKEY

LFLAGS = -p

OBJS = sdbm.o hash.o
SRCS = sdbm.c hash.c dbu.c util.c
HDRS = tune.h sdbm.h 

sdbm: $(OBJS)
	ar cr libsdbm.a $(OBJS)
	ranlib libsdbm.a
###	cp libsdbm.a /usr/lib/libsdbm.a

all: dbu

dbu: dbu.o sdbm util.o
	cc $(LFLAGS) -o dbu dbu.o util.o libsdbm.a

dba.o: sdbm.h
dbu.o: sdbm.h
util.o:sdbm.h

$(OBJS): $(HDRS)

lint:
	lint -abchx $(SRCS)

clean:
	rm -f *.o mon.out core

purge: 	clean
	rm -f dbu libsdbm.a  *.dir *.pag
