/* File: hku2hz.c */

/*
hku2hz: a simple utility to convert "chinese.16" (provided by HKU)
	into files of the format expected by "hz2ps".

By	Fung F. Lee (lee@milo.stanford.edu)

This file can be freely copied, distributed and modified.

*/


#include <stdio.h>
#include <string.h>

#define FSIZE   442432	/* source-file size */
#define MagicNo 20000

#define BIG0 0xa140	/* starting code of BIG5 Level 1 symbols */
#define BIG1 0xa440	/* starting code of BIG5 Level 1 hanzi */
#define BIG2 0xc6a1	/* starting code of BIG5 Level 2 hanzi */
#define BIGbegin BIG0
#define BIGend	 0xf9fe

#define HB(code)	(((code)&0xff00) >> 8)
#define	LB(code)	((code)&0x00ff)

char prog[] = "split";
int hzbyte;
int bn0, bn1, bn2, bn;
char *bitmap, *bitmap0, *bitmap1, *bitmap2;
char Font[80], Font0[80], Font1[80], Font2[80];

char *calloc();

int BIG2Index(code)
unsigned int code;
{
    return ((HB(code) - 0xA1) * 157 +
	    ((LB(code) < 0x7F)? LB(code)-0x40: LB(code)-0xA1+0x7F-0x40));
}


main(argc, argv)
int argc;
char *argv[];
{
    char *fname1, *fname2;

    if (argc != 3)
    {
	fprintf(stderr, "usage: %s <source-file> <destination-file>\n", prog);
	exit(1);
    }
    fname1 = argv[1];
    fname2 = argv[2];

    hzbyte = 32;
    bn0 = (BIG2Index(BIG1)-BIG2Index(BIG0)) * hzbyte;
    bn1 = (BIG2Index(BIG2)-BIG2Index(BIG1)) * hzbyte;
    bn2 = (BIG2Index(BIGend)-BIG2Index(BIG2)+1) * hzbyte;
    bn  = (BIG2Index(BIGend)-BIG2Index(BIGbegin)+1) * hzbyte + MagicNo;

    /* assert (bn >= FSIZE) */
    if ((bitmap = calloc(bn, 1)) == 0)
    {
	fprintf(stderr, "Can't allocate enough memory.\n");
	exit(1);
    }
    bitmap0 = bitmap + 256;
    bitmap1 = bitmap0 + 765 * hzbyte;
    bitmap2 = bitmap1 + bn1;

    ReadBM(fname1);
    WriteBM(fname2);
}


ReadBM(fname)
char *fname;
{
    FILE *f;

    if ((f = fopen(fname, "r")) == 0)
    {
	fprintf(stderr, "Can't open font file %s\n", fname);
	exit(1);
    }
    if (fread(bitmap, 1, FSIZE, f) != FSIZE)
    {
	fprintf(stderr, "Error in reading font file %s\n", fname);
	exit(1);
    }
    fclose(f);
}


WriteBM(fname)
char *fname;
{
    FILE *f;

    strcat(Font, fname);
    strcpy(Font0, Font);
    strcat(Font0, ".0");
    strcpy(Font1, Font);
    strcat(Font1, ".1");
    strcpy(Font2, Font);
    strcat(Font2, ".2");
    
    if ((f = fopen(Font0, "w")) == 0)
    {
	fprintf(stderr, "Can't create font file %s\n", Font0);
	exit(1);
    }
    if (fwrite(bitmap0, 1, bn0, f) != bn0)
    {
	fprintf(stderr, "Error in writing font file %s\n", Font0);
	exit(1);
    }
    fclose(f);
    if ((f = fopen(Font1, "w")) == 0)
    {
	fprintf(stderr, "Can't create font file %s\n", Font1);
	exit(1);
    }
    if (fwrite(bitmap1, 1, bn1, f) != bn1)
    {
	fprintf(stderr, "Error in writing font file %s\n", Font1);
	exit(1);
    }
    fclose(f);
    if ((f = fopen(Font2, "w")) == 0)
    {
	fprintf(stderr, "Can't create font file %s\n", Font2);
	exit(1);
    }
    if (fwrite(bitmap2, 1, bn2, f) != bn2)
    {
	fprintf(stderr, "Error in writing font file %s\n", Font2);
	exit(1);
    }
    fclose(f);
}


