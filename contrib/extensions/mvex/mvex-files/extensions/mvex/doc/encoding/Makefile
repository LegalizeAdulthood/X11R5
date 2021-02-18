# $Header: /usr/local/src/x11r5/mit/extensions/RCS/Makefile,v 1.5 1991/09/19 22:25:33 toddb Exp $
  TROFFDEV = -Tpsc
DITROFFDEV = -TX100
   DITROFF = /usr/local/bin/ditroff
     TROFF = /usr/local/bin/psroff -t
PROTOFILES = ../macros.t encoding
     PAGES=1-
      ONLY = -o$(PAGES)
#
# This is only necessary if your troff calls up fonts that it
# doesn't need.  See ../trimfonts.c for a list.
#
 TRIMFONTS = | ../trimfonts
 TRIM_EXEC = ../trimfonts

all: encoding.ps index.ps

preview: encoding.dit index.dit

encoding.ps: $(PROTOFILES) $(TRIM_EXEC)
	cat $(PROTOFILES) \
		| $(TROFF) $(ONLY) $(TROFFDEV) -ms 2> index.raw.tmp \
		$(TRIMFONTS) > $@.tmp
	mv $@.tmp $@
	mv index.raw.tmp index.raw

encoding.dit: $(PROTOFILES)
	cat $(PROTOFILES) \
		| $(DITROFF) $(ONLY) $(DITROFFDEV) -ms \
		  > $@.tmp 2> index.raw.tmp
	mv $@.tmp $@
	mv index.raw.tmp index.raw

index: index.raw ../fixindex.awk ../block.awk
	sort -f -t: +1 -3 +0n -1n index.raw \
		| awk -f ../fixindex.awk \
		| awk -f ../block.awk > index
	
index.ps: encoding.ps index ../indexmacros.t
	cat ../indexmacros.t index \
		| ${TROFF} -me $(TRIMFONTS) > index.ps.tmp
	mv index.ps.tmp index.ps

index.dit: encoding.dit index ../indexmacros.t
	cat ../indexmacros.t index \
		| ${DITROFF} $(DITROFFDEV) -me  > index.dit.tmp
	mv index.dit.tmp index.dit

print: encoding.ps index.ps
	lpr -qps encoding.ps
	sleep 30;
	lpr -qps index.ps
