
CFLAGS = $(CDEBUGFLAGS) $(INCPATH)

INCPATH = -I../../..
LIBPATH = -L../../../lib/X -L../lib

ALL = vset

all:: $(ALL)

vset:  vset.o
	rm -f $@
	cc -o $@ $(CFLAGS) $(LDFLAGS) $(LIBPATH) $@.o  -lXv -lX11

clean::
	rm *.o core #* *~ $(ALL)
