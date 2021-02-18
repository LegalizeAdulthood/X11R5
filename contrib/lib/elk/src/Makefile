.DEFAULT:
	$(LINK) ../../src/$< $<

H=	config.h\
	object.h\
	extern.h\
	macros.h\
	alloca.h\
	sparc.h\
	scheme.h

C=	autoload.c\
	bignum.c\
	bool.c\
	char.c\
	cont.c\
	debug.c\
	dump.c\
	env.c\
	error.c\
	feature.c\
	fixmul.c\
	heap.c\
	io.c\
	list.c\
	load.c\
	main.c\
	math.c\
	prim.c\
	print.c\
	proc.c\
	promise.c\
	read.c\
	special.c\
	stab.c\
	string.c\
	symbol.c\
	type.c\
	vector.c

O=	alloca.o\
	autoload.o\
	bignum.o\
	bool.o\
	char.o\
	cont.o\
	debug.o\
	dump.o\
	env.o\
	error.o\
	feature.o\
	fixmul.o\
	heap.o\
	io.o\
	list.o\
	load.o\
	main.o\
	math.o\
	prim.o\
	print.o\
	proc.o\
	promise.o\
	read.o\
	special.o\
	stab.o\
	stack.o\
	string.o\
	symbol.o\
	type.o\
	vector.o

scheme:	$(O)
	$(CC) -o scheme $(CFLAGS) $(O) $(LDFLAGS) 

stack.o:	stack.s.$(MACHTYPE)
	cp stack.s.$(MACHTYPE) stack.s
	/lib/cpp stack.s | sed '/^#/d' >stack.ss
	as -o stack.o stack.ss
	rm stack.ss

stack.s.port:	stack.c
	$(CC) -S $(CFLAGS) stack.c
	mv stack.s stack.s.port

alloca.o:	alloca.s.$(MACHTYPE)
	cp alloca.s.$(MACHTYPE) alloca.s
	/lib/cpp alloca.s | sed '/^#/d' >alloca.ss
	as -o alloca.o alloca.ss
	rm alloca.ss

alloca.s.port:	$(H) alloca.c
	$(CC) $(CFLAGS) -S alloca.c
	mv alloca.s alloca.s.port

autoload.o:	$(H) autoload.c
bignum.o:	$(H) bignum.c
bool.o:		$(H) bool.c
char.o:		$(H) char.c
cont.o:		$(H) cont.c
debug.o:	$(H) debug.c
dump.o:		$(H) dump.c
env.o:		$(H) env.c
error.o:	$(H) error.c
feature.o:	$(H) feature.c
fixmul.o:	$(H) fixmul.c
heap.o:		$(H) heap.c
io.o:		$(H) io.c
list.o:		$(H) list.c
load.o:		$(H) load.c
main.o:		$(H) main.c
math.o:		$(H) math.c
prim.o:		$(H) prim.c
print.o:	$(H) print.c
proc.o:		$(H) proc.c
promise.o:	$(H) promise.c
read.o:		$(H) read.c
special.o:	$(H) special.c
stab.o:		$(H) stab.c
string.o:	$(H) string.c
symbol.o:	$(H) symbol.c
type.o:		$(H) type.c
vector.o:	$(H) vector.c

lint:
	lint $(LINTFLAGS) -abxh $(C) | egrep -v '\?\?\?'

clean:
	rm -f *.o core stack.s stack.s.port alloca.s alloca.s.port
