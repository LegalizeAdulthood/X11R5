#
# Makefile for pubdic dictionaries
#
#   Do simply 'make' for normal cases.
#   Do 'make project', if your site participates in the PUBDIC project.
#

WNNDIR=/usr3/proj/Wnn
#ATOD=$(WNNDIR)/jutil/atod
ATOD=atod
#ATOF=$(WNNDIR)/fuzokugo/atof
ATOF=atof
HINSI=hinsi.data
FZK_SRC=fzk.master fzk.attr fzk.jirattr fzk.fzkattr fzk.jircon fzk.con fzk.shuutan
FZK_FLAG=

dic=\
	kihon.dic \
	setsuji.dic \
	jinmei.dic \
	chimei.dic \
	koyuu.dic \
	special.dic \
	computer.dic \
	bio.dic \
	symbol.dic \
	tankan.dic

more=\
	kihon.more \
	setsuji.more \
	jinmei.more \
	chimei.more \
	koyuu.more \
	special.more \
	computer.more \
	bio.more \
	symbol.more \
	tankan.more

all: full.fzk kougo.fzk std.fzk $(dic)

project: all $(more)

clean:
	rm -f *.fzk *.dic *.more

std.fzk: $(FZK_SRC)
	/lib/cpp $(FZK_FLAG) fzk.master | egrep -v '^(# |$$)' | $(ATOF) $@

full.fzk: $(FZK_SRC)
	make FZK_FLAG=-DKANSAI std.fzk
	mv std.fzk $@

kougo.fzk: $(FZK_SRC)
	make FZK_FLAG=-DKOUGO std.fzk
	mv std.fzk $@

.SUFFIXES: .hd .u .dic .more

.u.dic:
	cat $*.hd $< | $(ATOD) -p Version -P Version $@

.hd.more:
	$(ATOD) $@ < $<

$(dic): $(HINSI)
