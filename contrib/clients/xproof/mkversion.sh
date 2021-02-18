#!/bin/sh
# $Header: /src/X11/uw/xproof/RCS/mkversion.sh,v 1.4 89/07/17 12:04:33 solomon Distrib $
files='Imakefile
	Proof.c
	Proof.h
	ProofP.h
	README
	adobe.h
	bdfheader.h
	dev.h
	draw.c
	drawhacks.c
	fixfont.c
	getdevinfo.c
	index.c
	main.c
	mkversion.sh
	patchlevel.h
	showpage.c
	xfontinfo.c
	ximake
	xproof.h
	xproof.man'
mv version.c version.c.bak
awk '$1 == "#if" { exit }
$2 == "MinorVersion" { print "int MinorVersion = " $4+1 " ;" ; next }
{ print }' version.c.bak > version.c
echo "#if 0" >> version.c
fgrep '$Header' $files | fgrep -v fgrep | tr -d '$'>> version.c
echo "#endif" >> version.c
