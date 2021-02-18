#	$Header: /d/sam/tiff/RCS/Makefile,v 1.10 91/09/20 15:07:34 sam Exp $

default:

clean:
	-cd libtiff; make clean
	-cd tools; make clean

tiff.tar: clean
	tar cvf tiff.tar \
	    Makefile \
	    HOWTO \
	    README \
	    CHANGES-v2.3 \
	    CHANGES-v2.4 \
	    VERSION \
	    Copyright \
	    doc \
	    man/man1/*.1 \
	    man/man3/*.3t \
	    libtiff/README \
	    libtiff/*.[ch] \
	    libtiff/Makefile* \
	    tools/*.[ch] \
	    tools/Makefile* \
	    contrib/dbs/README \
	    contrib/dbs/*.c \
	    contrib/dbs/Imakefile \
	    contrib/dbs/xtiff/Imakefile \
	    contrib/dbs/xtiff/README \
	    contrib/dbs/xtiff/*.[ch] \
	    contrib/ras
	compress tiff.tar
tiffpics.tar:
	tar cvf tiffpics.tar \
	    pics/README \
	    pics/cramps.tif \
	    pics/cramps-tile.tif \
	    pics/fax2d.tif \
	    pics/g3test.tif \
	    pics/jello.tif \
	    pics/logo-picio.tif \
	    pics/logo.tif \
	    pics/oxford.tif \
	    pics/quad-lzw.tif \
	    pics/quad-tile.tif \
	    pics/text.tif \
	    pics/fax2d.g3 \
	    pics/g3test.g3
	compress tiffpics.tar

both:	tiff.tar tiffpics.tar
