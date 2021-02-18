# $Header: /usr/local/src/x11r5/mit/extensions/RCS/Makefile,v 1.7 1991/09/19 22:20:27 toddb Exp $
  TROFFDEV = -Tpsc
DITROFFDEV = -TX75
       EQN = /usr/local/bin/eqn
       TBL = /usr/local/bin/tbl
   DITROFF = /usr/local/bin/ditroff
     TROFF = /usr/local/bin/psroff -t
 HIGHLIGHT = ../highlight index.table
   PRINTER = qms
PROTOFILES = ../macros.t protocol
     PAGES=1-
      ONLY = -o$(PAGES)
#
# This is only necessary if your troff calls up fonts that it
# doesn't need.  See ../trimfonts.c for a list.
#
 TRIMFONTS = | ../trimfonts
 TRIM_EXEC = ../trimfonts

all: protocol.ps index.ps

preview: protocol.dit index.dit

protocol.ps: $(HIGHLIGHT) $(PROTOFILES) $(TRIM_EXEC)
	cat $(PROTOFILES) \
		| $(HIGHLIGHT) \
		| $(EQN) $(TROFFDEV) \
		| $(TBL) \
		| $(TROFF) $(ONLY) $(TROFFDEV) -ms 2> index.raw.tmp \
		$(TRIMFONTS) > $@.tmp
	mv $@.tmp $@
	mv index.raw.tmp index.raw

protocol.dit: $(HIGHLIGHT) index.table $(PROTOFILES)
	cat $(PROTOFILES) \
		| $(HIGHLIGHT) \
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
	
index.ps: protocol.ps index ../indexmacros.t
	cat ../indexmacros.t index \
		| ${TROFF} -me $(TRIMFONTS) > index.ps.tmp
	mv index.ps.tmp index.ps

index.dit: protocol.dit index ../indexmacros.t
	cat ../indexmacros.t index \
		| ${DITROFF} $(DITROFFDEV) -me  > index.dit.tmp
	mv index.dit.tmp index.dit

print: protocol.ps index.ps
	lpr -Plw3 protocol.ps
	lpr -Plw3 index.ps
