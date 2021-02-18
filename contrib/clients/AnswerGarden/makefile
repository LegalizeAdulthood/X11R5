# $Header: makefile,v 1.1 86/10/05 15:19:16 design Exp $

#------------------------------------------------------------------------
# makefile for X course programs
#------------------------------------------------------------------------

DFLAGS= -g 
CFLAGS= -c ${DFLAGS}
IFLAGS= -I.
LFLAGS= 
WHEREFLAGS= ${IFLAGS} ${LFLAGS}
XLIBS = -lXaw -lXmu -lXt -lX11 -lXext
SHAPE= -DSHAPE
BAKUP = /mit/xv/working/ag/bakup
WORKING = /mit/xv/working/ag/current.work

#------------------------------------------------------------------------
# programs
#------------------------------------------------------------------------

# To compile Answer Garden

AG ag: AG.o Layout.o Grapher.o Knedit.o Util.o \
Discuss.o NodeServ.o FileServ.o GButton.o FuncObj.o StackObj.o Edit.o EditInfo.o \
EditFile.o QA.o Globals.o CommServ.o 
	cc -o AG ${WHEREFLAGS} Globals.o AG.o Layout.o Grapher.o \
	Knedit.o Util.o Discuss.o NodeServ.o GButton.o FuncObj.o StackObj.o \
	Edit.o FileServ.o EditInfo.o EditFile.o QA.o CommServ.o \
	${XLIBS}


#  To compile a version of Answer Garden with journal nodes 
#	(for me)
#
JAG jag: AG.o Layout.o Grapher.o Knedit.o Util.o \
Discuss.o NodeServ.o FileServ.o GButton.o FuncObj.o StackObj.o Edit.o EditInfo.o \
EditFile.o QA.o Globals.o journal.o CommServ.o cistk.o
	cc ${CFLAGS} ${IFLAGS} Globals.c -DJOURNAL
	cc -o AG ${WHEREFLAGS} Globals.o AG.o Layout.o Grapher.o \
	Knedit.o Util.o Discuss.o NodeServ.o GButton.o FuncObj.o StackObj.o \
	Edit.o FileServ.o EditInfo.o EditFile.o QA.o journal.o CommServ.o \
	cistk.o \
	${XLIBS}
	rm Globals.o


authoring:  mail_stats everybody_here

bakup: 
	cp -r ${BAKUP}/one_back/* ${BAKUP}/two_back
	cp -r ${BAKUP}/current/* ${BAKUP}/one_back
	cp -r * ${BAKUP}/current

AG.h: AGmacros.h
	touch AG.h

NodeServ.o: NodeServ.c AG.h
	cc ${CFLAGS} ${IFLAGS} NodeServ.c -DTEMPORARY_NODES

CommServ.o: CommServ.c AG.h
	cc ${CFLAGS} ${IFLAGS} CommServ.c

FileServ.o: FileServ.c AG.h
	cc ${CFLAGS} ${IFLAGS} FileServ.c

Discuss.o: Discuss.c AG.h
	cc ${CFLAGS} ${IFLAGS} Discuss.c

Util.o: Util.c AG.h
	cc ${CFLAGS} ${IFLAGS} Util.c

Grapher.o: Grapher.c Grapher.h GrapherP.h AGmacros.h
	cc ${CFLAGS} ${IFLAGS} Grapher.c

FuncObj.o: FuncObj.c FuncObj.h FuncObjP.h AGmacros.h
	cc ${CFLAGS} ${IFLAGS} FuncObj.c

StackObj.o: StackObj.c StackObj.h StackObjP.h AGmacros.h
	cc ${CFLAGS} ${IFLAGS} StackObj.c

Layout.o: Layout.c Layout.h LayoutP.h 
	cc ${CFLAGS} ${IFLAGS} Layout.c

Knedit.o: Knedit.c Knedit.h KneditP.h AGmacros.h
	cc ${CFLAGS} ${IFLAGS} Knedit.c

AG.o: AG.c AG.h 
	cc ${CFLAGS} ${IFLAGS} AG.c

Edit.o: Edit.c AG.h Edit.h
	cc ${CFLAGS} ${IFLAGS} Edit.c

EditInfo.o: EditInfo.c AG.h Edit.h
	cc ${CFLAGS} ${IFLAGS} EditInfo.c

EditFile.o: EditFile.c AG.h Edit.h
	cc ${CFLAGS} ${IFLAGS} EditFile.c

GButton.o: GButton.c GButton.h GButtonP.h AGmacros.h
	cc ${CFLAGS} ${IFLAGS} ${SHAPE} GButton.c

QA.o: QA.c AG.h
	cc ${CFLAGS} ${IFLAGS} QA.c


Globals.o: Globals.c AG.h
	cc ${CFLAGS} ${IFLAGS} Globals.c

stubs.o: stubs.c AG.h
	cc ${CFLAGS} ${IFLAGS} stubs.c

journal.o: journal.c AG.h
	cc ${CFLAGS} ${IFLAGS} journal.c

cistk.o: cistk.c AG.h
	cc ${CFLAGS} ${IFLAGS} cistk.c


everybody_here.o: everybody_here.c AG.h
	cc ${CFLAGS} ${IFLAGS} everybody_here.c


everybody_here: everybody_here.o NodeServ.o Util.o Globals.o \
stubs.o FileServ.o
	cc ${WHEREFLAGS} -o everybody_here \
	 everybody_here.o NodeServ.o Util.o Globals.o FileServ.o stubs.o \
	 ${XLIBS}

mail_stats: mail_stats.c AG.h Globals.o Util.o stubs.o NodeServ.o FileServ.o 
	cc ${WHEREFLAGS} -o mail_stats mail_stats.c Globals.o Util.o \
	stubs.o NodeServ.o FileServ.o ${XLIBS}






