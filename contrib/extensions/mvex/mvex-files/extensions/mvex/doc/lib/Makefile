# $Header: /n/lacey/usr/local/src/video/MVEX/doc/lib/RCS/Makefile,v 1.11 1991/09/26 04:55:58 toddb Exp $
  TROFFDEV = -Tpsc
DITROFFDEV = -TX75
       EQN = /usr/local/bin/eqn
       TBL = /usr/local/bin/tbl
   DITROFF = /usr/local/bin/ditroff
     TROFF = /usr/local/bin/psroff -t
   PRINTER = lw3
LIBFILES= ../macros.t\
	intro \
	datastr \
	proto \
	apputil \
	events \
	postproc
     PAGES=1-
      ONLY = -o$(PAGES)
#
# This is only necessary if your troff calls up fonts that it
# doesn't need.  See ../trimfonts.c for a list.
#
 TRIMFONTS = | ../trimfonts
 TRIM_EXEC = ../trimfonts


all: lib.ps index.ps

preview: lib.dit index.dit

lib.ps: $(LIBFILES) $(TRIM_EXEC)
	cat $(LIBFILES) \
		| $(EQN) $(TROFFDEV) \
		| $(TBL) \
		| $(TROFF) $(ONLY) $(TROFFDEV) -ms 2> index.raw.tmp \
		$(TRIMFONTS) > $@.tmp
	mv $@.tmp $@
	mv index.raw.tmp index.raw

lib.dit: $(LIBFILES)
	cat $(LIBFILES) \
		| $(EQN) $(DITROFFDEV) \
		| $(TBL) \
		| $(DITROFF) $(ONLY) $(DITROFFDEV) -ms \
		  > $@.tmp 2> index.raw.tmp
	mv $@.tmp $@
	mv index.raw.tmp index.raw

index: index.raw ../fixindex.awk ../block.awk
	sort -f -t: +1 -3 +0n -1n index.raw \
		| awk -f ../fixindex.awk \
		| awk -f ../block.awk > index
	
index.ps: lib.ps index ../indexmacros.t $(TRIM_EXEC)
	cat ../indexmacros.t index | ${TROFF} -me \
		$(TRIMFONTS) > index.ps.tmp
	mv index.ps.tmp index.ps

index.dit: lib.dit index ../indexmacros.t
	cat ../indexmacros.t index \
	| ${DITROFF} $(DITROFFDEV) -me  > index.dit.tmp
	mv index.dit.tmp index.dit

print: lib.ps index.ps
	lpr -P$(PRINTER) lib.ps
	sleep 30;
	lpr -P$(PRINTER) index.ps

clean:
	rm -f *.dit *.ps *.raw index index.page
