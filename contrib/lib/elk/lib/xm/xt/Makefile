.DEFAULT:
	$(LINK) ../../../lib/xt/$< $<

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

../../xt-motif.o:	$(O) ../../xlib.o
	ld -r $(O) -u _XmIsMotifWMRunning $(MOTIF) $(XT) ../../xlib.o $(XLIB);
	mv a.out ../../xt-motif.o;
	chmod 644 ../../xt-motif.o

clean:
	rm -f *.o core a.out ../../xt-motif.o
