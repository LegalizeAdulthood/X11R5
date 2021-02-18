# Example makefile to print the documentation of GWM
# consult your local TeX/LaTeX guru if you need help
# (or just grab the postscript version: gwm.ps by ftp on expo.lcs.mit.edu)

# To adjust the placement of the text on the page, play with the
# hoffset/voffset dimensions at the top of the "gwm.tex" file...
# the game is to be able to make a matching 2-sides photocopy
# (check that the page numbers on odd & even pages are at the same
# distance from the outside margin)

# if the included postscript files upsets your dvi driver, uncomment the
# relevant line in the "gwm.tex" file

# NOTE: it is NORMAL to get on first pass a lot of :
#	LaTeX Warning: Reference `xxx' on page xx undefined.

all:	gwm.ps

print: gwm.dvi
	dvips  -h idraw.pro gwm.dvi

gwm.dvi: gwm.tex colas.sty title.tex usage.tex overview.tex wool_ref.tex \
	contents.tex quick_ref.tex index.tex standard.tex
#	rm -f *.aux	# clean dir in case of stange errors
	echo 'Rerun to get cross-references right'> gwm.log
	@echo '***** expect a LOT of LaTeX Warnings now *****'
	../gwm_util latex gwm

# These are EXAMPLES of how to print a .dvi file. Refer to your local TeX
# Wizard for more details
# you will need a dvi2ps understanding the \special{psfile=...} command
# best one is dvips v5.0, anonymous ftp on labrea.stanford.edu (36.8.0.47)
# DO NOT FORGET TO INCLUDE THE idraw.pro POSTSCRIPT HEADER!!!
# (strip_ps is a local filter to strip ps comments, you can omit it,
# or use "squeeze" coming with the dvips distribution)
# PS: strip_ps is not needed, it is just a comment remover(stripper) to
# decrease the ps file for distribution...

gwm.ps:	gwm.dvi idraw.pro
	../gwm_util latex gwm
#	-dvi2ps -i idraw.pro gwm.dvi > gwm.ps  		      #standard dvi2ps 
#	-w_dvi2ps -i idraw.pro -F 10 gwm.dvi |strip_ps> gwm.ps  # Wang's dvi2ps
	dvips -f -h idraw.pro < gwm.dvi | strip_ps > gwm.ps #a4 dvips5.0
#	dvips -f -h idraw.pro < gwm.dvi > gwm.ps            # normal way

# how to make 20-page chunks in the gwm.ps-split directory

gwm.ps-split:      gwm.dvi idraw.pro
	../gwm_util latex gwm
	rm -rf gwm.ps-split; mkdir gwm.ps-split
	for i in 001 021 041 061 081 101;do \
dvips -r -p $$i -n 20 -f -h idraw.pro<gwm.dvi|strip_ps>gwm.ps-split/psdoc-pages-$$i.ps;\
done
	tar cfv -  gwm.ps-split|compress>$$FTP/pub/gwm/gwm.ps-split.tar.Z

update: gwm.ps gwm.ps-split

# NOTE: if you cannot see page numbers in the postcript doc, it is because the
# page has too much offset upwards. To correct it, go into each postcript file,
# search for the first occurence of the line:
# 
#        TeXDict begin @a4
# 
# then 2 solutions:
# 
# - either change @a4 by @letter
# 
# - or insert BEFORE this line the line:
#        0 -30 translate
#   which brings down the page by 30 points (a point is a 72th of an inch).
#   You could in fact put X Y translate where X and Y are x,y offsets...
# 
