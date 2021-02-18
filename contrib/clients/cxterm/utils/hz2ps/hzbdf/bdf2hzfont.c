/*
  bdf2hzfont.c -- program to convert bdf (X window) font
                  format to hz font format

  Written by Ricky Yeung

  Copyright (c) 1989, 1991 Ricky Yeung

  All rights reserved.
  Permission to copy and distribute verbatim copies of this
  document for non-commercial purposes is granted, but changing
  it is not allowed.
  There is absolutely no warranty for this program.

  Please send your comments and suggestions to the author:
  	Ricky Yeung	yeung@decwrl.dec.com or yeung@atd.dec.com
*/
#include <stdio.h>

/* GB constants */

#define GB0 0x2121	/* starting code of GB1 symbols */
#define GB1 0x3021	/* starting code of GB1 hanzi */
#define GB2 0x5821	/* starting code of GB2 hanzi */
#define GBU 0x7821	/* starting code of GB user defined hanzi */
#define GBX 0x7921	/* starting code of my own GB extension */
#define GBXe 0x7A7E	/* ending code of my own GB extension */

#define GBbegin GB0
#define GBend   GBXe
#define GBbox   0x2175	/* ? GB "blank box" code */

/* BIG5 constants */

#define FSIZE   442432	/* source-file size */
#define MagicNo 20000

#define BIG0 0xa140	/* starting code of BIG5 Level 1 symbols */
#define BIG1 0xa440	/* starting code of BIG5 Level 1 hanzi */
#define BIG2 0xc6a1	/* starting code of BIG5 Level 2 hanzi */
#define BIGbegin BIG0
#define BIGend	 0xf9fe

#define HB(code)	(((code)&0xff00) >> 8)
#define	LB(code)	((code)&0x00ff)

#define NAMELEN	80

int indexSize;		/* total number of codes in range */
char Font0[NAMELEN], Font[NAMELEN], Font1[NAMELEN],
    Font2[NAMELEN], FontX[NAMELEN];
char *bitmap, *bitmap0, *bitmap1, *bitmap2, *bitmapX;
int bn0, bn1, bn2, bnX, bn;
int hzbyte, HZbm;

int GBmode = 1;		/* default GB order */

#define LEN 100

char line[LEN];

int GB2Index(code)
     unsigned int code;
{
  return ((HB(code) - 0x21) * 0x5E + (LB(code)) - 0x21);
}

int BIG2Index(code)
unsigned int code;
{
    int hi, lo;

    return ((HB(code) - 0xA1) * 157 +
	    ((LB(code) < 0x7F)? LB(code)-0x40: LB(code)-0xA1+0x7F-0x40));
}

int SkipTillStr (s)
    char *s;
{
    for (;;)
    {
	if (!gets(line)) return(0);
	if (strncmp(line,s,strlen(s))==0) return(1);
    }
}

void WriteFntFile (s, bm, n)
    char *s, *bm;
    int n;
{
    FILE *fp;
    
    if (!(fp=fopen(s,"w")))
    {
	fprintf(stderr, "Can't write to font file %s\n", s);
	exit(1);
    }
    if (fwrite(bm, 1, n, fp)!=n)
    {
	fprintf(stderr, "Error in writing to font file %s\n", s);
	exit(1);
    }
}

char2int (c)
    int c;
{
    if ((c>='0')&&(c<='9')) return(c-'0');
    else if ((c>='a')&&(c<='f')) return(c-'a'+10);
    else if ((c>='A')&&(c<='F')) return(c-'A'+10);
    else
    {
	fprintf (stderr, "Bit map error. \n");
	exit(1);
    }
}


Readbdf ()
{
    int code, bindex, i, j, c;
    char tmp[LEN];
    int odd = 1;
                
    while (SkipTillStr ("ENCODING "))
    {
	sscanf(line, "%s %d", tmp, &code);
	
	if (GBmode)
	{
	    code &= 0x7f7f;   /* turn off high bit of each byte */
	    bindex = GB2Index(code) * hzbyte;
	}
	else /* Big5 */
	    bindex = BIG2Index(code) * hzbyte;
	
	SkipTillStr("BITMAP");
	for (i=1;i<=HZbm;i++)
	{
	    for (j=1;j<=(HZbm/4);j++)
	    {
		c = char2int(getchar());
		if (!odd) 
		{
		    bitmap[bindex++] |= c;
		    odd = 1;
		}
		else
		{
		    bitmap[bindex] = bitmap[bindex] | (c<<4);
		    odd = 0;
		}
	    }
	    while ((c=getchar())!='\n');  /* skip rest of line */
	}
    }
}

main(argc, argv)
    int argc;
    char *argv[];
{
    char *calloc(), tmp[LEN];

    if ((argc>2)||(argc>1&&strcmp(argv[1],"-big5")&&strcmp(argv[1],"-gb")))
    {
	fprintf(stderr, "usage: %s [-big5 or -gb (default)]\n", argv[0]);
	exit(1);
    }

    if (argc==2)
    {
	GBmode = strcmp(argv[1], "-big5");
    }
        
    SkipTillStr("FONT ");
    sscanf(line, "%s %s", tmp, Font);
    SkipTillStr("SIZE ");
    sscanf(line, "%s %d", tmp, &HZbm);
    hzbyte = HZbm * HZbm / 8;
    printf("Font:%s Character size: %d x %d = %d bytes.\n",Font,HZbm,HZbm,hzbyte);
    strcat(Font, "-");

    if (GBmode)
    {
	indexSize = GB2Index(GBend) + 1;
	bn0 = (GB2Index(GB1)-GB2Index(GB0)) * hzbyte;
	bn1 = (GB2Index(GB2)-GB2Index(GB1)) * hzbyte;
	bn2 = (GB2Index(GBU)-GB2Index(GB2)) * hzbyte;
	bnX = (GB2Index(GBXe)-GB2Index(GBX)+1) * hzbyte;
	bn  = (GB2Index(GBend)-GB2Index(GBbegin)+1) * hzbyte;
    }
    else /* Big5 */
    {
	indexSize = BIG2Index(BIGend) + 1;
	bn0 = (BIG2Index(BIG1)-BIG2Index(BIG0)) * hzbyte;
	bn1 = (BIG2Index(BIG2)-BIG2Index(BIG1)) * hzbyte;
	bn2 = (BIG2Index(BIGend)-BIG2Index(BIG2)+1) * hzbyte;
	bn  = (BIG2Index(BIGend)-BIG2Index(BIGbegin)+1) * hzbyte + MagicNo;
    }
    
    if ((bitmap = calloc(bn, 1)) == NULL)
      {
	fprintf(stderr, "Can't allocate enough memory.\n");
	exit(1);
      }
    bitmap0 = bitmap;
    
    if (GBmode)
    {
	bitmap1 = bitmap + (GB2Index(GB1)-GB2Index(GB0)) * hzbyte;
	bitmap2 = bitmap + (GB2Index(GB2)-GB2Index(GB0)) * hzbyte;
	bitmapX = bitmap + (GB2Index(GBX)-GB2Index(GB0)) * hzbyte;
    }
    else 
    {
	bitmap1 = bitmap + (BIG2Index(BIG1)-BIG2Index(BIG0)) * hzbyte;
	bitmap2 = bitmap + (BIG2Index(BIG2)-BIG2Index(BIG0)) * hzbyte;
    }

    sprintf(tmp, "%d", HZbm);
    strcat(Font, tmp);
    strcpy(Font0, Font);
    strcat(Font0, ".0");
    strcpy(Font1, Font);
    strcat(Font1, ".1");
    strcpy(Font2, Font);
    strcat(Font2, ".2");
    strcpy(FontX, Font);
    strcat(FontX, ".X");
    printf("Font set size: %d\n",bn);

    Readbdf();

    WriteFntFile (Font0, bitmap0, bn0);
    WriteFntFile (Font1, bitmap1, bn1);
    WriteFntFile (Font2, bitmap2, bn2);
    if (GBmode) WriteFntFile (FontX, bitmapX, bnX);
}









