.DEFAULT:
	$(LINK) ../../../lib/xlib/$< $<

.c.o:
	$(CC) $(CFLAGS) -I../../src -c $<

H=	../../src/config.h\
	../../src/object.h\
	../../src/extern.h\
	../../src/macros.h\
	../../src/alloca.h\
	../util/symbol.h\
	../util/string.h\
	../util/objects.h\
	xlib.h

C=	client.c\
	color.c\
	colormap.c\
	cursor.c\
	display.c\
	error.c\
	event.c\
	extension.c\
	font.c\
	gcontext.c\
	grab.c\
	graphics.c\
	key.c\
	objects.c\
	pixel.c\
	pixmap.c\
	property.c\
	text.c\
	type.c\
	util.c\
	window.c\
	wm.c\
	xlib.c

O=	client.o\
	color.o\
	colormap.o\
	cursor.o\
	display.o\
	error.o\
	event.o\
	extension.o\
	font.o\
	gcontext.o\
	grab.o\
	graphics.o\
	key.o\
	objects.o\
	pixel.o\
	pixmap.o\
	property.o\
	text.o\
	type.o\
	util.o\
	window.o\
	wm.o\
	xlib.o\
	../util/symbol.o\
	../util/objects.o

../xlib.o:	$(O)
	ld -r $(O) $(XLIB)
	mv a.out ../xlib.o; chmod 644 ../xlib.o

client.o:	$(H) client.c
color.o:	$(H) color.c
colormap.o:	$(H) colormap.c
cursor.o:	$(H) cursor.c
display.o:	$(H) display.c
error.o:	$(H) error.c
event.o:	$(H) event.c
extension.o:	$(H) extension.c
font.o:		$(H) font.c
gcontext.o:	$(H) gcontext.c
grab.o:		$(H) grab.c
graphics.o:	$(H) graphics.c
key.o:		$(H) key.c
objects.o:	$(H) objects.c
pixel.o:	$(H) pixel.c
pixmap.o:	$(H) pixmap.c
property.o:	$(H) property.c
text.o:		$(H) text.c
type.o:		$(H) type.c
util.o:		$(H) util.c
window.o:	$(H) window.c
wm.o:		$(H) wm.c
xlib.o:		$(H) xlib.c

lint:
	lint $(LINTFLAGS) -abxh $(C) -I../../src | egrep -v '\?\?\?'

clean:
	rm -f *.o core a.out ../xlib.o
