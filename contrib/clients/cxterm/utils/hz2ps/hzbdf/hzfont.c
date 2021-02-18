/* File: hzfont.c */

/*----------------------------------------------------------------------------

	Copyright (C) 1989, 1991  Fung F. Lee

	All rights reserved.
	Permission to copy and distribute verbatim copies of this
	document for non-commercial purposes is granted, but changing
	it is not allowed.

----------------------------------------------------------------------------*/

#include <stdio.h>

extern char fontdir[], hzfont[];
extern int hzbyte;
extern char *calloc();

#define GB0 0x2121	/* starting code of GB Level 1 symbols */
#define GB1 0x3021	/* starting code of GB Level 1 hanzi */
#define GB2 0x5821	/* starting code of GB Level 2 hanzi */
#define GBU 0x7821	/* starting code of GB user defined hanzi */
#define GBX 0x7921	/* starting code of my own GB extension */
#define GBXe 0x7A7E	/* ending code of my own GB extension */

#define GBbegin GB0
#define GBend   GBXe
#define GBbox   0x2175	/* GB "blank box" code */

#define BIG0 0xa140	/* starting code of BIG5 Level 1 symbols */
#define BIG1 0xa440	/* starting code of BIG5 Level 1 hanzi */
#define BIG2 0xc6a1	/* starting code of BIG5 Level 2 hanzi */
#define BIG2e 0xf9fe

#define BIGbegin BIG0
#define BIGend	 BIG2e
#define BIGbox   0xa1bc	/* BIG "blank box" code */

#define HB(code)	(((code)&0xff00) >> 8)
#define	LB(code)	((code)&0x00ff)

char Font0[80], Font[80], Font1[80], Font2[80], FontX[80];
char *bitmap, *bitmap0, *bitmap1, *bitmap2, *bitmapX;
long bn0, bn1, bn2, bnX, bn;

unsigned int GB2Index(), BIG2Index();
char *GBbitmap(), *BIGbitmap();
void loadGBfont(), loadBIGfont();


unsigned int GB2Index(code)
unsigned int code;
{
    return ((HB(code) - 0x21) * 94 + (LB(code)) - 0x21);
}


unsigned int BIG2Index(code)
unsigned int code;
{
    int hi, lo;

    return ((HB(code) - 0xA1) * 157 +
	    ((LB(code) < 0x7F)? LB(code)-0x40: LB(code)-0xA1+0x7F-0x40));
}


char *GBbitmap(code)
unsigned int code ;
{    
    char *ptr;

    if ((code >= GBbegin) && (code <= GBend))
      ptr = bitmap + GB2Index(code) * hzbyte;
    else
      {
	ptr = bitmap + GB2Index(GBbox) * hzbyte;
        fprintf(stderr, "Error: GB code out of range: %x\n", code);
      }
    return ptr;
}


char *BIGbitmap(code)
unsigned int code ;
{    
    char *ptr;

    if ((code >= BIGbegin) && (code <= BIGend))
	ptr = bitmap + BIG2Index(code) * hzbyte;
    else
      {
	ptr = bitmap + BIG2Index(BIGbox) * hzbyte;
        fprintf(stderr, "Error: BIG code out of range: %x\n", code);
      }
    return ptr;
}


void loadGBfont()
{
    FILE *f;
    char *fname;

    bn0 = (GB2Index(GB1)-GB2Index(GB0)) * hzbyte;
    bn1 = (GB2Index(GB2)-GB2Index(GB1)) * hzbyte;
    bn2 = (GB2Index(GBU)-GB2Index(GB2)) * hzbyte;
    bnX = (GB2Index(GBXe)-GB2Index(GBX)+1) * hzbyte;
    bn  = (GB2Index(GBend)-GB2Index(GBbegin)+1) * hzbyte;
    if ((bitmap = calloc(bn, 1)) == NULL)
      {
	fprintf(stderr, "Can't allocate enough memory.\n");
	exit(1);
      }
    bitmap0 = bitmap;
    bitmap1 = bitmap0 + bn0;
    bitmap2 = bitmap1 + bn1;
    bitmapX = bitmap2 + bn2;

    strcpy(Font, fontdir);
    strcat(Font, hzfont);
    strcpy(Font0, Font);
    strcat(Font0, ".0");
    strcpy(Font1, Font);
    strcat(Font1, ".1");
    strcpy(Font2, Font);
    strcat(Font2, ".2");
    strcpy(FontX, Font);
    strcat(FontX, ".X");

    if ((f = fopen(Font0, "r")) == 0)
    {
	fprintf(stderr, "Can't open font file %s\n", Font0);
	exit(1);
    }
    if (fread(bitmap0, 1, bn0, f) != bn0)
    {
#if EXACT
	fprintf(stderr, "Error in reading font file %s\n", Font0);
	exit(1);
#endif
    }
    fclose(f);
    if ((f = fopen(Font1, "r")) == 0)
    {
	fprintf(stderr, "Can't open font file %s\n", Font1);
	exit(1);
    }
    if (fread(bitmap1, 1, bn1, f) != bn1)
    {
#if EXACT
	fprintf(stderr, "Error in reading font file %s\n", Font1);
	exit(1);
#endif
    }
    fclose(f);
    if ((f = fopen(Font2, "r")) == 0)
    {
	fprintf(stderr, "Can't open font file %s\n", Font2);
	exit(1);
    }
    if (fread(bitmap2, 1, bn2, f) != bn2)
    {
#if EXACT
	fprintf(stderr, "Error in reading font file %s\n", Font2);
	exit(1);
#endif
    }
    fclose(f);
    if ((f = fopen(FontX, "r")) == 0)
    {
	fprintf(stderr, "Can't open font file %s\n", FontX);
	return;
#ifdef XGB
	exit(1);
#endif
    }
    if (fread(bitmapX, 1, bnX, f) != bnX)
    {
#ifdef XGB
	fprintf(stderr, "Error in reading font file %s\n", FontX);
	exit(1);
#endif
    }
    fclose(f);
}


void loadBIGfont()
{
    FILE *f;
    char *fname;
    char *ptr;

    bn0 = (BIG2Index(BIG1)-BIG2Index(BIG0)) * hzbyte;
    bn1 = (BIG2Index(BIG2)-BIG2Index(BIG1)) * hzbyte;
    bn2 = (BIG2Index(BIG2e)-BIG2Index(BIG2)+1) * hzbyte;
    bn  = (BIG2Index(BIGend)-BIG2Index(BIGbegin)+1) * hzbyte;
    if ((bitmap = calloc(bn, 1)) == NULL)
      {
	fprintf(stderr, "Can't allocate enough memory.\n");
	exit(1);
      }
    bitmap0 = bitmap;
    bitmap1 = bitmap0 + bn0;
    bitmap2 = bitmap1 + bn1;

    strcpy(Font, fontdir);
    strcat(Font, hzfont);
    strcpy(Font0, Font);
    strcat(Font0, ".0");
    strcpy(Font1, Font);
    strcat(Font1, ".1");
    strcpy(Font2, Font);
    strcat(Font2, ".2");
    strcpy(FontX, Font);
    strcat(FontX, ".X");

    if ((f = fopen(Font0, "r")) == 0)
    {
	fprintf(stderr, "Can't open font file %s\n", Font0);
	exit(1);
    }
    if (fread(bitmap0, 1, bn0, f) != bn0)
    {
#if EXACT
	fprintf(stderr, "Error in reading font file %s\n", Font0);
	exit(1);
#endif
    }
    fclose(f);
    if ((f = fopen(Font1, "r")) == 0)
    {
	fprintf(stderr, "Can't open font file %s\n", Font1);
	exit(1);
    }
    if (fread(bitmap1, 1, bn1, f) != bn1)
    {
#if EXACT
	fprintf(stderr, "Error in reading font file %s\n", Font1);
	exit(1);
#endif
    }
    fclose(f);
    if ((f = fopen(Font2, "r")) == 0)
    {
	fprintf(stderr, "Can't open font file %s\n", Font2);
	exit(1);
    }
    if (fread(bitmap2, 1, bn2, f) != bn2)
    {
#if EXACT
	fprintf(stderr, "Error in reading font file %s\n", Font2);
	exit(1);
#endif
    }
    fclose(f);
}

