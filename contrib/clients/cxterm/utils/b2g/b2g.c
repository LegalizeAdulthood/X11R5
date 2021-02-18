/*
 * FILE NAME: hutil.c
 *	See MODIFICATIONS entry below.
 *
 * NAME:
 *	HC - Hanzi Converter Version 1.2u
 *	Copyright @ 1988,1989,1990 by Fung F. Lee & Ricky Yeung
 *
 * SYNOPSIS:
 *	HC -m mode [-tabs t1 t2 ...]
 *		mode: can be BIGtoGB or GBtoBIG
 *		ti: conversion table filenames
 *
 * DESCRIPTION:
 *	`HC' converts BIG-5 codes into GB codes (with high bit set),
 *	and vice versa. BIG-5 is a popular hanzi code used in Taiwan,
 *	whereas GB (GuoBiao) is Mainland China's national standard.
 *	GB code (with high bit set) is adopted widely in many systems,
 *	such as CCDOS and Macintosh Chinese Operating System 6.0.3.
 *	
 * FILES:
 *	GB0toBIG, GB1toBIG, GB2toBIG, BIG0toGB, BIG1toGB, YiTiZi
 *	are HC mapping table files. They are either GB or BIG-5 files.
 *	The format is simple and allows easy modification with a Chinese
 *	editor. 
 *
 * AUTHORS:
 *	Ricky Yeung (yeung@june.cs.washington.edu)
 *	Fung F. Lee (lee@milo.stanford.edu)
 *
 * ACKNOWLEDGEMENT:
 *	Thanks to Edmund Lai for providing most of the data in `GB2toBIG'.
 *
 * DISTRIBUTION:
 *	This program and the data files are NOT in public domain.  
 *	The source file and the related table files as listed
 *	in FILES can be freely distributed for non-commercial purposes only.
 *
 *	Contact the authors before making any modification to the software
 *	and/or the table files.
 *
 *	This software is provided "as is" and the authors disclaim all
 *	warranties with regard to this software and the correctness of the
 *	mapping table files.  
 *
 * MODIFICATIONS:
 *	Modified Fri Dec 21 07:35:35 1990 by Mark Leisher
 *
 *	1. Consolidated GB[012]toBIG files into 'g2b.table' and
 *	   BIG[01]toGB into 'b2g.table'.
 *	2. Changed file name to 'b2g.c'.
 *	3. Removed unneeded functions, modified converter
 *	   function, modified function return types.
 */

#include <stdio.h>
#include <ctype.h>

#ifndef BTOG_TABLE
#define BTOG_TABLE "/usr/local/src/ch_hk_news/b2g/b2g.table"
#endif /* BTOG_TABLE */

#ifndef GTOB_TABLE
#define GTOB_TABLE "/usr/local/src/ch_hk_news/b2g/g2b.table"
#endif /* GTOB_TABLE */

#define	GBtoBIG	  1
#define BIGtoGB	  2
#define YITIZI    3

#define GBstart  0xA1A1	/* first address of GtoB table */
#define GBend	 0xFEFE	/* last address of GtoB table */
#define GBsize	 0x5E5E	/* size of GtoB table */
#define BIGstart 0xA140	/* first address of BtoG table */
#define BIGend	 0xF9FE	/* last address of BtoG table */
#define	BIGsize	 0x58BF	/* size of BtoG table */

#define GBbox	0xA1F5	/* GB code of blank box, default value of BtoG table */
#define BIGbox	0xA1BC	/* BIG code of blank box,default value of GtoB table */

#define HB(x)		((x>>8) & 0xFF)
#define LB(x)		(x & 0xFF)
#define DB(hi,lo)	((((hi)&0xFF) << 8) | ((lo)&0xFF))
#define isFirstByte(c)	(((c) & 0xFF)>=0xA1)
#define inGBrange(x)	(((x)>=GBstart) && ((x)<=GBend))
#define inSGBrange(x)   (((x)>=SGBstart) && ((x)<=SGBend))
#define inBIGrange(x)	(((x)>=BIGstart) && ((x)<=BIGend))

unsigned short int *BtoG, *GtoB;	/* Hanzi code mapping tables */

static int
SkipSpaces(fp)
FILE *fp;
{
    int c;

    for(;;)
    {
        c=fgetc(fp);
        if ((c&0x80) || !isspace(c)) break;
    }
    return (c);
}

static void
fatal(s)
char *s;
{
    fprintf(stderr, "%s\n", s);
    exit(1);
}

static FILE *
openfile(fn)
char *fn;
{
    FILE *fp = fopen(fn, "r");
    
    if (!fp)
      fprintf(stderr, "Unable to open file: %s\n", fn);
    return(fp);
}

static unsigned short int
makeYiTi(fp)
FILE *fp;
{
    unsigned int code1, code2, value;
    int		c1, c2;
    unsigned short int	total=0;
    
    for (;;)
    {
	if ((c1=SkipSpaces(fp))==EOF) break;
	c2 = fgetc(fp);
	code1 = DB(c1, c2);
	value = BtoG[code1 - BIGstart];
	for (;;)
	{
	    if ((c1=SkipSpaces(fp))==EOF) break;
	    c2 = fgetc(fp);
	    code2 = DB(c1, c2);
	    BtoG[code2 - BIGstart] = value;
	    total++;		
	    if ((c1=fgetc(fp))=='\n')
		break;
	    ungetc(c1, fp);
	}
    }
    return total;
}

static void
init_tables()
{
    int i;

    GtoB = (unsigned short int *) malloc(sizeof(short int) * GBsize);
    BtoG = (unsigned short int *) malloc(sizeof(short int) * BIGsize);
    
    for (i=0; i<GBsize; i++)
      GtoB[i] = BIGbox;
    for (i=0; i<BIGsize; i++)
      BtoG[i] = GBbox;
}

int
load_hconv_table(fn)
char *fn;
{
    unsigned int codeS, codeT, cfirst, clast;
    int c1, c2, mode = GBtoBIG;
    unsigned short int total = 0;
    FILE *fp;
    char buf[255];

    if (!(fp = openfile(fn)))
      return(0);
    
    for (;;)
    {
	fgets(buf, 255, fp); /* no check for EOF */
	if (buf[0]!='\n' && (buf[0]!='#') && (buf[0]!=' ')) break;
    }

    /* header in new HCF format: e.g. '%GB->BIG', '%BIG->GB', '%YITIZI' */
    if (buf[0] == '%')
    {
	switch(buf[1])
	{
	  case 'G': mode = GBtoBIG; break;
	  case 'B': mode = BIGtoGB; break;
	  case 'Y': mode = YITIZI; break;
	  default: fatal("unknown HCF format\n"); break;
	}
    }
    
    if (mode == YITIZI) {
	total = makeYiTi(fp);
        fclose(fp);
	return total;
    }
    
    for (;;)
    {
	if (fscanf(fp, "%x %x", &cfirst, &clast) != 2)
	    break;
	for (codeS = cfirst; codeS <= clast; codeS++)
	{
	    if ((c1=SkipSpaces(fp))==EOF) break;
	    c2 = fgetc(fp);
	    codeT = DB(c1, c2);
	    if (mode == GBtoBIG)
	    {
		if (!inBIGrange(codeT))
		    fatal("error: invalid code mapping table entry\n");
		if (codeT != BIGbox)
		{
		    GtoB[codeS - GBstart] = codeT;
		    BtoG[codeT - BIGstart] = codeS;
		    total++;
		}
	    }
	    else if (mode == BIGtoGB)
	    {
		if (!inGBrange(codeT))
		    fatal("error: invalid code mapping table entry\n");
		if (codeT != GBbox)
		{
		    BtoG[codeS - BIGstart] = codeT;
		    GtoB[codeT - GBstart] = codeS;
		    total++;
		}
	    }
	}
    }
    fclose(fp);
    return total;
}

int
converter(mode, c1, c2, oc1, oc2)
int mode;
unsigned char c1, c2, *oc1, *oc2;
{
    unsigned short int codeS, codeT;
    unsigned char ch1, ch2;
    int miss = 0; 

    codeS = DB(c1, c2);
    codeT = codeS;
    switch(mode) {
      case GBtoBIG:
        if (inGBrange(codeS)) 
          codeT = GtoB[codeS - GBstart];
        else
          codeT = BIGbox;
        if ((codeT == BIGbox) && (codeS != GBbox))
	  miss++ ;
        break;
      case BIGtoGB:
        if (inBIGrange(codeS))
          codeT = BtoG[codeS - BIGstart];
        else
          codeT = GBbox;
	if ((codeT == GBbox) && (codeS != BIGbox))
	  miss++ ;
        break;
    }

    *oc1 = HB(codeT);
    *oc2 = LB(codeT);

    return (miss);
}

static int
btog(fp, mode)
FILE *fp;
int mode;
{
    int c1, c2;
    unsigned char oc1, oc2;
    long int unconverted = 0L;

    while((c1 = getc(fp)) != EOF) {
        if (!isFirstByte(c1))
          putchar(c1);
        else {
            c2 = getc(fp);
            unconverted += converter(mode, c1, c2, &oc1, &oc2);
            putchar(oc1);
            putchar(oc2);
        }
    }

    return (unconverted);
}

main(argc, argv)
int argc;
char **argv;
{
    FILE *fp;
    char *program;
    int mode = BIGtoGB;
    int missed;

    init_tables();
    load_hconv_table(BTOG_TABLE);
    load_hconv_table(GTOB_TABLE);

    program = argv[0];

    while(*++argv) {
        if (argv[0][0] == '-') {
            switch(argv[0][1]) {
              case 'g':
              case 'G':
                mode = BIGtoGB;
                break;
              case 'b':
              case 'B':
                mode = GBtoBIG;
                break;
              default:
                fprintf(stderr, "%s: Uknown switch: %c. Ignored.\n", program,
                        argv[0][1]);
                fprintf(stderr, "Usage: %s [-{g|b}] [file ...]\n", program);
                break;
            }
            argc--;
        } else {
            if (fp = fopen(argv[0], "r+b")) {
		missed = btog(fp, mode);
                if (missed > 0)
		    fprintf(stderr,"%s: number of hanzi not converted = %ld\n",
			    argv[0], missed);
                fclose(fp);
            } else
              perror(argv[0]);
        }
    }
    if (argc == 1) {
       missed = btog(stdin, mode);
       if (missed > 0)
	    fprintf(stderr,"number of hanzi not converted = %ld\n", missed);
    }
}
