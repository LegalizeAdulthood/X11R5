.DEFAULT:
	$(LINK) ../../../lib/xm/$< $<

WIDGET_SET= xmotif

O= arrow-button.o\
   bulletin-board.o\
   cascade-button.o\
   command.o\
   drawing-area.o\
   drawn-button.o\
   file-selection.o\
   form.o\
   frame.o\
   label.o\
   list.o\
   main-window.o\
   message-box.o\
   paned-window.o\
   push-button.o\
   row-column.o\
   scale.o\
   scroll-bar.o\
   scrolled-window.o\
   selection-box.o\
   separator.o\
   shell.o\
   support.o\
   text.o\
   toggle-button.o

.SUFFIXES: .d .c .o

.d.c:
	../../src/scheme -l ../xt/make-widget $< $@ $(WIDGET_SET)

.d.o:
	../../src/scheme -l ../xt/make-widget $< $*.c $(WIDGET_SET)
	$(CC) $(CFLAGS) -I../../src -c $*.c

.c.o:
	$(CC) $(CFLAGS) -I../../src -c $<

all: $(O)

$(O): ../xt/make-widget

arrow-button.o:		arrow-button.d
bulletin-board.o:	bulletin-board.d
cascade-button.o:	cascade-button.d
command.o:		command.d
drawing-area.o:		drawing-area.d
drawn-button.o:		drawn-button.d
file-selection.o:	file-selection.d
form.o:			form.d
frame.o:		frame.d
label.o:		label.d
list.o:			list.d
main-window.o:		main-window.d
message-box.o:		message-box.d
paned-window.o:		paned-window.d
push-button.o:		push-button.d
row-column.o:		row-column.d
scale.o:		scale.d
scroll-bar.o:		scroll-bar.d
scrolled-window.o:	scrolled-window.d
selection-box.o:	selection-box.d
separator.o:		separator.d
shell.o:		shell.d
support.o:		support.d
text.o:			text.d
toggle-button.o:	toggle-button.d

../xt/make-widget:
	$(LINK) ../../../lib/xt/make-widget ../xt

lint:
	lint $(LINTFLAGS) -I../../src -abxh *.c | egrep -v '\?\?\?'

clean:
	rm -f *.o *.c
