.DEFAULT:
	$(LINK) ../../../lib/xt/$< $<

.c.o:
	$(CC) $(CFLAGS) -I../../src -c $<

H=	../../src/config.h\
	../../src/object.h\
	../../src/extern.h\
	../../src/macros.h\
	../../src/alloca.h\
	../util/objects.h\
	../util/string.h\
	../xlib/xlib.h\
	xt.h

C=	accelerator.c\
	action.c\
	callback.c\
	class.c\
	context.c\
	converter.c\
	error.c\
	function.c\
	identifier.c\
	objects.c\
	popup.c\
	resource.c\
	translation.c\
	widget.c\
	xt.c

O=	accelerator.o\
	action.o\
	callback.o\
	class.o\
	context.o\
	converter.o\
	error.o\
	function.o\
	identifier.o\
	objects.o\
	popup.o\
	resource.o\
	translation.o\
	widget.o\
	xt.o

../xt.o:	$(O) ../xlib.o
	ld -r $(O) $(XAW) ../xlib.o $(XLIB)
	mv a.out ../xt.o; chmod 644 ../xt.o

accelerator.o:	$(H) accelerator.c
action.o:	$(H) action.c
callback.o:	$(H) callback.c
class.o:	$(H) class.c
context.o:	$(H) context.c
converter.o:	$(H) converter.c
error.o:	$(H) error.c
function.o:	$(H) function.c
identifier.o:	$(H) identifier.c
objects.o:	$(H) objects.c
popup.o:	$(H) popup.c
resource.o:	$(H) resource.c
translation.o:	$(H) translation.c
widget.o:	$(H) widget.c
xt.o:		$(H) xt.c

lint:
	lint $(LINTFLAGS) -abxh $(C) -I../../src | egrep -v '\?\?\?'

clean:
	rm -f *.o core a.out ../xt.o
