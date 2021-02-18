/* File: hz2ps.c */

static char version[] = "hz2ps version 2.0 (June 6, 1991)";

/*----------------------------------------------------------------------------


  	hz2ps 2.0: Convert a Hanzi (GB/BIG5) File to PostScript
	-------------------------------------------------------

	By Fung F. Lee

	Copyright (C) 1989,1991  Fung F. Lee

	All rights reserved.
	Permission to copy and distribute verbatim copies of this
	document for non-commercial purposes is granted, but changing
	it is not allowed.
	There is absolutely no warranty for this program.

	Please send your comments and suggestions to the author:
	Fung F. Lee	lee@milo.stanford.edu

----------------------------------------------------------------------------*/
  

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define True 1
#define False 0

#define GBcode 0
#define BIGcode 1

#define GBspace		0x2121
#define BIGspace	0xA140

#define MAX(a, b)	((a)>=(b)? (a) : (b))
#define DB(hi,lo)	(((hi)&0xFF) << 8 | (lo)&0xFF)
#define HGB2GB(hi,lo)	(((hi)&0x7F) << 8 | (lo)&0x7F)
/* test ranges of first and second bytes */
#define isGB1(c)	((c)>=0xA1 && (c)<=0xFE)
#define isGB2(c)	((c)>=0xA1 && (c)<=0xFE)
#define isBIG1(c)	((c)>=0xA1 && (c)<=0xF9)
#define isBIG2(c)	((c)>=0x40 && (c)<=0x7E || (c)>=0xA1 && (c)<=0xFE)


extern char *GBbitmap(), *BIGbitmap();
extern void loadGBfont(), loadBIGfont();
extern char *getenv();


int encoding = GBcode;
char pmode = 'h';
float leftmargin = 72,   /* 1 in */  rightmargin = 540, /* 7.5 in */
      bottommargin = 72, /* 1 in */  topmargin = 720;   /* 10 in */
int HZbm = 24;
int hzbyte;	/* number of bytes per hanzi image */
float HZpoint = 10, HZspacing = 1, HZskip = 15;
float Epoint = 10, Eskip = 6, Edescender=2, Evskip;
float vskip = 15, hskip = 15;
int hanziPerTab = 4;
float HZtab;
int pageNo;
float hpos, vpos;
float pnLeft = 300, pnBottom = 36;
int pnStart = 1;
int printPageNo = True;

static char HZDIR[] = "HZDIR";
static char HEADER[] = "hz2ps.pro";
static char nullName[] = "";
char progname[] = "hz2ps";
char hzdir[100], fontdir[100], headerName[100];
char hzfont[30];
char hzName[30]="Beijing";
char engfont[30]="Courier";
char *inputName;
FILE *fin;

void usage(), prHeader(), GBrange(), BIGrange();
void GBscanner(), BIGscanner(), fputNewPage(), fputNewLine();
void fputHanzi(), fputWord(), fputTab(), fputAscii(), fputDash();  
char GBvtest(), BIGvtest();
unsigned int GBreplace(), BIGreplace();
void GBvscanner(), BIGvscanner(), vfputNewPage(), vfputNewLine();
void vfputHanzi(), vfputWord(), vfputTab(), vfputAscii(), vfputDash();


void usage()
{
    FILE *f = stderr;

    fprintf(f, "%s\n\n", version);
    fprintf(f, "usage:\t%s ", progname);
    fprintf(f, "[-gb] [-big] [-h] [-v] \\\n");
    fprintf(f, "\t[-margin <left> <right> <bottom> <top>] \\\n" );
    fprintf(f, "\t[-ls <line-spacing>] [-htab <n>] \\\n");
    fprintf(f, "\t[-hf <hanzi-font> <point-size> <inter-hanzi-spacing>] ");
    fprintf(f, "[-hbm <m>] \\\n");
    fprintf(f, "\t[-af <ascii-font> <point-size> <width> <descender>] \\\n");
    fprintf(f, "\t[-pnp <left> <bottom>] [-pns <page-number>] [-nopn] \\\n");
    fprintf(f, "\t[file]\n");
    exit(1);
}


#define LEN 100
#define PtPerIn 72

main(argc, argv)
int argc;
char *argv[];
{
    char s[LEN];
    int i;
    FILE *header;
    float ftmp;
    
    if (getenv(HZDIR) == NULL)
    {
	fprintf(stderr, "Please setenv %s appropriately.\n", HZDIR);
	exit(1);
    }
    strcpy(hzdir, getenv(HZDIR));
    strcat(hzdir, "/");
    strcpy(headerName, hzdir);
    strcat(headerName, HEADER);
    strcpy(fontdir, hzdir);
    inputName = nullName;
    
    for (i=1; i<argc; i++)
    {
	if (strcmp("-gb", argv[i]) == 0)
	    encoding = GBcode;
	else if (strcmp("-big", argv[i]) == 0)
	    encoding = BIGcode;
	else if (strcmp("-h", argv[i]) == 0)
	    pmode = 'h';
	else if (strcmp("-v", argv[i]) == 0)
	    pmode = 'v';
	else if (strcmp("-margin", argv[i]) == 0)
	{
	    if (++i >= argc) usage();
	    if (sscanf(argv[i], "%f", &ftmp) != 1) usage();
	    leftmargin = ftmp * PtPerIn;
	    if (++i >= argc) usage();
	    if (sscanf(argv[i], "%f", &ftmp) != 1) usage();
	    rightmargin = ftmp * PtPerIn;
	    if (++i >= argc) usage();
	    if (sscanf(argv[i], "%f", &ftmp) != 1) usage();
	    bottommargin = ftmp * PtPerIn;
	    if (++i >= argc) usage();
	    if (sscanf(argv[i], "%f", &ftmp) != 1) usage();
	    topmargin = ftmp * PtPerIn;
	}
	else if (strcmp("-ls", argv[i]) == 0)
	{
	    if (++i >= argc) usage();
	    if (sscanf(argv[i], "%f", &vskip) != 1) usage();
	    hskip = vskip;
	}
	else if (strcmp("-htab", argv[i]) == 0)
	{
	    if (++i >= argc) usage();
	    if (sscanf(argv[i], "%d", &hanziPerTab) != 1) usage();
	}
	else if (strcmp("-hf", argv[i]) == 0)
	{
	    if (++i >= argc) usage();
	    strcpy(hzName, argv[i]);
	    if (++i >= argc) usage();
	    if (sscanf(argv[i], "%f", &HZpoint) != 1) usage();
	    if (++i >= argc) usage();
	    if (sscanf(argv[i], "%f", &HZspacing) != 1) usage();
	}
	else if (strcmp("-hbm", argv[i]) == 0)
	{
	    if (++i >= argc) usage();
	    if (sscanf(argv[i], "%d", &HZbm) != 1) usage();
	}
	else if (strcmp("-af", argv[i]) == 0)
	{
	    if (++i >= argc) usage();
	    strcpy(engfont, argv[i]);
	    if (++i >= argc) usage();
	    if (sscanf(argv[i], "%f", &Epoint) != 1) usage();
	    if (++i >= argc) usage();
	    if (sscanf(argv[i], "%f", &Eskip) != 1) usage();
	    if (++i >= argc) usage();
	    if (sscanf(argv[i], "%f", &Edescender) != 1) usage();
	}
	else if (strcmp("-pnp", argv[i]) == 0)
	{
	    if (++i >= argc) usage();
	    if (sscanf(argv[i], "%f", &ftmp) != 1) usage();
	    pnLeft = ftmp * PtPerIn;
	    if (++i >= argc) usage();
	    if (sscanf(argv[i], "%f", &ftmp) != 1) usage();
	    pnBottom = ftmp * PtPerIn;
	}
	else if (strcmp("-pns", argv[i]) == 0)
	{
	    if (++i >= argc) usage();
	    if (sscanf(argv[i], "%d", &pnStart) != 1) usage();
	}
	else if (strcmp("-nopn", argv[i]) == 0)
	    printPageNo = False;
	else if (argv[i][0] == '-')
	    usage();
	else
	    inputName = argv[i];
    }
    
    HZskip = HZpoint + HZspacing;
    Evskip = Epoint * 1.2;
    HZtab = hanziPerTab * HZskip;
    
    if (leftmargin + MAX(HZskip, hskip) > rightmargin)
    {
	fprintf(stderr, "Insufficient space between left and right margins.\n");
	exit(1);
    }
    if (bottommargin + MAX(HZskip, vskip) > topmargin)
    {
	fprintf(stderr, "Insufficient space between top and bottom margins.\n");
	exit(1);
    }

    sprintf(s, "%d", HZbm);
    strcpy(hzfont, hzName);
    strcat(hzfont, s);
    
    hzbyte = (HZbm * HZbm) / 8;
    if (encoding == GBcode)
	loadGBfont();
    else if (encoding == BIGcode)
	loadBIGfont();
    
    if ((header = fopen(headerName, "r")) == 0)
    {
	fprintf(stderr, "Cannot open prolog file %s\n", headerName);
	exit(1);
    }
    if (strcmp(inputName, nullName) == 0)
	fin = stdin;
    else if ((fin = fopen(inputName, "r")) == 0)
    {
	fprintf(stderr, "Cannot open input file %s\n", inputName);
	exit(1);
    }
    prHeader(header, stdout);
    fclose(header);
    if (pmode == 'h') 
    {     
	hpos = leftmargin;
	vpos = topmargin;
	if (encoding == GBcode)
	    GBscanner(fin, stdout);
	else if (encoding == BIGcode)
	    BIGscanner(fin, stdout);
    }  
    else if (pmode == 'v')
    {
	hpos = rightmargin;
	vpos = topmargin;
	if (encoding == GBcode)
	    GBvscanner(fin, stdout);
	else if (encoding == BIGcode)
	    BIGvscanner(fin, stdout);
    }
    else
    {
	fprintf(stderr, "unknown printing mode %c\n", pmode);
	exit(1);
    }
}


void prHeader(fin, f)
FILE *fin, *f;
{
    int c;
    
    fprintf(f, "%%!PS-Adobe-3.0\n");
    fprintf(f, "%%%%Title: %s\n", inputName);
    fprintf(f, "%%%%Creator: (hz2ps 2.0)\n");
    fprintf(f, "%%%%Pages: (atend)\n");
    fprintf(f, "%%%%PageOrder: Ascend\n");
    fprintf(f, "%%%%Orientation: Portrait\n");
    fprintf(f, "%%%%BoundingBox: %d %d %d %d\n", (int)leftmargin,
	    (int)bottommargin, (int)rightmargin, (int)topmargin);
    fprintf(f, "%%%%DocumentData: Clean7Bit\n");
    fprintf(f, "%%%%LanguageLevel: 1\n");
    fprintf(f, "%%%%EndComments\n");
    
    fprintf(f, "%%%%BeginProlog\n");
    while ((c=fgetc(fin)) != EOF)
	putc(c, f);
    
    fprintf(f, "/HZbm %d def\n", HZbm);
    fprintf(f, "/HZpoint %6.2f def\n", HZpoint);
    fprintf(f, "/HZspacing %6.2f def\n", HZspacing);
    fprintf(f, "/HZskip %6.2f def\n", HZskip);
    fprintf(f, "/Epoint %6.2f def\n", Epoint);
    fprintf(f, "/Evskip %6.2f def\n", Evskip);
    fprintf(f, "/Edescender %6.2f def\n", Edescender);
    fprintf(f, "\/%s Epoint chooseFont\n", engfont);
    fprintf(f, "/pnLeft %6.2f def\n", pnLeft);
    fprintf(f, "/pnBottom %6.2f def\n", pnBottom);
    fprintf(f, "%%%%EndProlog\n");
}


void GBrange(hi, lo)
int hi, lo;
{
    if (!isGB1(hi) || !isGB2(lo))
    {
	fprintf(stderr, "GB code out of range: %c%c (hex: %2x%2x)\n",
		hi, lo, hi, lo);
	exit(1);
    }
}


void BIGrange(hi, lo)
int hi, lo;
{
    if (!isBIG1(hi) || !isBIG2(lo))
    {
	fprintf(stderr, "BIG code out of range: %c%c (hex: %2x%2x)\n",
		hi, lo, hi, lo);
	exit(1);
    }
}

#define SLEN 80		/* maximum English string length */

void GBscanner(fin, fout)
FILE *fin, *fout;
{
    int c1, c2, i;
    unsigned int code;
    char *image, s[SLEN];
    
    pageNo = 1;
    fprintf(fout, "%%%%Page: page%d %d\n", pageNo, pageNo);
    fprintf(fout, "beginpage\n");
    fputNewLine(fout);
    while ((c1=fgetc(fin)) != EOF)
    {
	if (isGB1(c1))
	{
	    c2 = fgetc(fin);
	    code = HGB2GB(c1, c2);
	    if (!isGB2(c2))
	    {
		fprintf(stderr, "unrecognized code: 0x%2x%2x\n", c1, c2);
		code = GBspace;	/* default error recovery */
	    }
	    switch (code)
	    {
	    case 0x212A: fputDash(0.5, fout); break;
	    case 0x235F: fputDash(0.0, fout); break;
	    case 0x237E: fputDash(1.0, fout); break;
	    default:
		image = GBbitmap(code);
		fputHanzi(image, fout);
		break;
	    }
	}
	else if (c1 & 0x80)
	    fprintf(stderr, "unrecognized character code: 0x%2x\n", c1);
	else if (isalnum(c1))
	{
	    s[0] = c1;
	    i = 1;
	    while (i<SLEN-1 && (c1=fgetc(fin)) && isalnum(c1))
		s[i++] = c1;
	    s[i++] = '\0';
	    if (i<SLEN)
		ungetc(c1, fin);
	    fputWord(s, fout);
	}
	else
	    fputAscii(c1, fout);
    }
    if (printPageNo)
	fprintf(fout, "(%d) prPageNo\n", pageNo + pnStart - 1);
    fprintf(fout, "endpage\n");
    fprintf(fout, "%%%%Trailer\n");
    fprintf(fout, "%%%%Pages: %d\n", pageNo);
    fprintf(fout, "%%%%EOF\n");
}


void BIGscanner(fin, fout)
FILE *fin, *fout;
{
    int c1, c2, i;
    unsigned int code;
    char *image, s[SLEN];
    
    pageNo = 1;
    fprintf(fout, "%%%%Page: page%d %d\n", pageNo, pageNo);
    fprintf(fout, "beginpage\n");
    fputNewLine(fout);
    while ((c1=fgetc(fin)) != EOF)
    {
	if (isBIG1(c1))
	{
	    c2 = fgetc(fin);
	    code = DB(c1, c2);
	    if (!isBIG2(c2))
	    {
		fprintf(stderr, "unrecognized code: 0x%2x%2x\n", c1, c2);
		code = BIGspace;	/* default error recovery */
	    }
	    switch (code)
	    {
	    case 0xA277:
	    case 0xA299:
	    case 0xA156:
	    case 0xA158: fputDash(0.5, fout); break;
	    default:
		image = BIGbitmap(code);
		fputHanzi(image, fout);
		break;
	    }
	}
	else if (c1 & 0x80)
	    fprintf(stderr, "unrecognized character code: 0x%2x\n", c1);
	else if (isalnum(c1))
	{
	    s[0] = c1;
	    i = 1;
	    while (i<SLEN-1 && (c1=fgetc(fin)) && isalnum(c1))
		s[i++] = c1;
	    s[i++] = '\0';
	    if (i<SLEN)
		ungetc(c1, fin);
	    fputWord(s, fout);
	}
	else
	    fputAscii(c1, fout);
    }
    if (printPageNo)
	fprintf(fout, "(%d) prPageNo\n", pageNo + pnStart - 1);
    fprintf(fout, "endpage\n");
    fprintf(fout, "%%%%Trailer\n");
    fprintf(fout, "%%%%Pages: %d\n", pageNo);
    fprintf(fout, "%%%%EOF\n");
}


void fputNewPage(f)
FILE *f;
{
    if (printPageNo)
	fprintf(f, "(%d) prPageNo\n", pageNo + pnStart - 1);
    fprintf(f, "endpage\n");
    pageNo++;
    fprintf(f, "%%%%Page: page%d %d\n", pageNo, pageNo);
    fprintf(f, "beginpage\n");
    hpos = leftmargin;
    vpos = topmargin;
    fputNewLine(f);
}


void fputNewLine(f)
FILE *f;
{
    hpos = leftmargin;
    vpos -= vskip;
    if (vpos < bottommargin)
	fputNewPage(f);
    else
	fprintf(f, "%6.2f %6.2f moveto\n", hpos, vpos);
}


void fputHanzi(image, f)
char *image;	/* image points to hzbyte bytes of bitmap */
FILE *f;
{
    char c;
    int i;
    
    if (hpos + HZskip > rightmargin)
	fputNewLine(f);
    fprintf(f, "<");
    for (i=0; i<hzbyte; i++)
    {
	c = *image++;
	c = ~c;		/* invert the bitmap */
	fprintf(f, "%x%x", (c&0xF0)>>4, c&0x0F);
    }
    fprintf(f, "> HZshow\n");
    hpos += HZskip;
}


void fputWord(s, f)
char *s;
FILE *f;
{
    if (hpos + strlen(s) * Eskip > rightmargin)
	fputNewLine(f);
    fprintf(f, "(%s) Eshow\n", s);
    hpos += strlen(s) * Eskip;
}


void fputTab(f)
FILE *f;
{
    hpos = ((hpos-leftmargin)/HZtab + 1) * HZtab + leftmargin;
    if (hpos > rightmargin)
	fputNewLine(f);
    else
	fprintf(f, "%6.2f %6.2f moveto\n", hpos, vpos);
}


void fputAscii(c, f)
char c;
FILE *f;
{
    if (c=='\n')
    {
	fputNewLine(f);
	return;
    }
    if (c=='\f')
    {
	fputNewPage(f);
	return;
    }
    if (c=='\t')
    {
	fputTab(f);
	return;
    }
    if (hpos + Eskip > rightmargin)
	fputNewLine(f);
    if (c==' ')
    {
	fprintf(f, "( ) Eshow\n");
	hpos += Eskip;
    }
    else if (c=='(' ||  c==')')
    {
	fprintf(f,  "(\\%c) Eshow\n", c);
	hpos += Eskip;
    }
    else if (c=='\\')
    {
	fprintf(f,  "(\\\\) Eshow\n");
	hpos += Eskip;
    }
    else if (isgraph(c))
    {
	fprintf(f, "(%c) Eshow\n", c);
	hpos += Eskip;
    }
    /* ignore other non-graphical (unprintable) ascii characters */	
}


void fputDash(scale, f)
float scale;
FILE *f;
{
    if (hpos + HZskip > rightmargin)
	fputNewLine(f);
    fprintf(f, "%3f dash\n", scale);
    hpos += HZskip;
}


/* similar code for vertical printing mode */

char GBvtest(code)
unsigned int code;
{
    char mode;
    
    switch (code)
    {
    case 0x212A:	/* middle-dash */
    case 0x212B:	/*  ~  */
    case 0x212D:	/* ... */
    case 0x2132:
    case 0x2133:
    case 0x2134:
    case 0x2135:
    case 0x2136:
    case 0x2137:
    case 0x2138:
    case 0x2139:
    case 0x213A:
    case 0x213B:
    case 0x213C:
    case 0x213D:
    case 0x213E:
    case 0x213F:
    case 0x217A:	/* --> */
    case 0x217B:	/* <-- */
    case 0x217C:	/* up-arrow */
    case 0x217D:	/* down-arrow */
    case 0x2328:	/* ( */
    case 0x2329:	/* ) */
    case 0x232D:	/* - */
    case 0x235B:	/* [ */
    case 0x235D:	/* ] */
    case 0x235F:	/* lower-dash */
    case 0x237B:	/* { */
    case 0x237C:	/* | */
    case 0x237D:	/* } */
    case 0x237E:	/* upper-dash */
	mode = 'R'; break;
    case 0x2122:	/* dun-hao */
    case 0x2123:	/* period */
    case 0x2321:	/* ! */
    case 0x232C:	/* , */
    case 0x232E:	/* . */
    case 0x233A:	/* : */
    case 0x233B:	/* ; */
    case 0x233F:	/* ? */
	mode = 'Q'; break;
    default:
	mode = 'V'; break;
    }
    return mode;
}


unsigned int GBreplace(code)
unsigned int code;
{
    unsigned int out;
    switch (code)
    {
    case 0x212E: out = 0x2138; break;
    case 0x212F: out = 0x2139; break;
    case 0x2130: out = 0x213A; break;
    case 0x2131: out = 0x213B; break;
    default: out = code; break;
    }
    return out;
}


void GBvscanner(fin, fout)
FILE *fin, *fout;
{
    int c1, c2, i;
    unsigned int code;
    char *image, s[SLEN];
    int vmode = False;
    
    pageNo = 1;
    fprintf(fout, "%%%%Page: page%d %d\n", pageNo, pageNo);
    fprintf(fout, "beginpage\n");
    vfputNewLine(fout);
    while ((c1=fgetc(fin)) != EOF)
    {
	if (isGB1(c1))
	{
	    c2 = fgetc(fin);
	    code = HGB2GB(c1, c2);
	    if (!isGB2(c2))
	    {
		fprintf(stderr, "unrecognized code: 0x%2x%2x\n", c1, c2);
		code = GBspace;	/* default error recovery */
	    }
	    switch (code)
	    {
	    case 0x212A: vfputDash(0.5, fout); break;
	    case 0x235F: vfputDash(0.0, fout); break;
	    case 0x237E: vfputDash(1.0, fout); break;
	    default:
		code = GBreplace(code);
		image = GBbitmap(code);
		vmode = GBvtest(code);
		vfputHanzi(image, fout, vmode);
		break;
	    }
	}
	else if (c1 & 0x80)
	    fprintf(stderr, "unrecognized character code: 0x%2x\n", c1);
	else if (isalnum(c1))
	{
	    s[0] = c1;
	    i = 1;
	    while (i<SLEN-1 && (c1=fgetc(fin)) && isalnum(c1))
		s[i++] = c1;
	    s[i++] = '\0';
	    if (i<SLEN)
		ungetc(c1, fin);
	    vfputWord(s, fout);
	}
	else
	    vfputAscii(c1, fout);
    }
    if (printPageNo)
	fprintf(fout, "(%d) prPageNo\n", pageNo + pnStart - 1);
    fprintf(fout, "endpage\n");
    fprintf(fout, "%%%%Trailer\n");
    fprintf(fout, "%%%%Pages: %d\n", pageNo);
    fprintf(fout, "%%%%EOF\n");
}


char BIGvtest(code)
unsigned int code;
{
    char mode;
    
    switch (code)
    {
    case 0xA156:	/* middle-dash */
    case 0xA158:
    case 0xA14B:
    case 0xA14C:
    case 0xA155:
    case 0xA157:
    case 0xA159:
    case 0xA15B:
    case 0xA15C:
    case 0xA1E3:
	mode = 'R'; break;
    case 0xA142:	/* dun-hao */
    case 0xA14E:
    case 0xA143:	/* period */
    case 0xA144:
    case 0xA145:
    case 0xA14F:
    case 0xA150:
    case 0xA141:	/* , */
    case 0xA14D:
	mode = 'Q'; break;
    default:
	mode = 'V'; break;
    }
    if (code >= 0xA15B && code <= 0xA1A4)
	mode = 'R';
    else if (code >= 0xA1F4 && code <= 0xA1FB)
	mode = 'R';
    return mode;
}


unsigned int BIGreplace(code)
unsigned int code;
{
    unsigned int out;

    switch (code)
    {
    case 0xA1A5: out = 0xA175; break;
    case 0xA1A6: out = 0xA176; break;
    case 0xA1A7: out = 0xA179; break;
    case 0xA1A8: out = 0xA17A; break;
    case 0xA1A9: out = 0xA179; break;
    case 0xA1AA: out = 0xA17A; break;
    default: out = code; break;
    }
    return out;
}


void BIGvscanner(fin, fout)
FILE *fin, *fout;
{
    int c1, c2, i;
    unsigned int code;
    char *image, s[SLEN];
    int vmode = False;
    
    pageNo = 1;
    fprintf(fout, "%%%%Page: page%d %d\n", pageNo, pageNo);
    fprintf(fout, "beginpage\n");
    vfputNewLine(fout);
    while ((c1=fgetc(fin)) != EOF)
    {
	if (isBIG1(c1))
	{
	    c2 = fgetc(fin);
	    code = DB(c1, c2);
	    if (!isBIG2(c2))
	    {
		fprintf(stderr, "unrecognized code: 0x%2x%2x\n", c1, c2);
		code = BIGspace;	/* default error recovery */
	    }
	    switch (code)
	    {
	    case 0xA277:
	    case 0xA299:
	    case 0xA156:
	    case 0xA158: vfputDash(0.5, fout); break;
	    case 0xA15A: vfputDash(0.0, fout); break;
	    default:
		code = BIGreplace(code);
		image = BIGbitmap(code);
		vmode = BIGvtest(code);
		vfputHanzi(image, fout, vmode);
		break;
	    }
	}
	else if (c1 & 0x80)
	    fprintf(stderr, "unrecognized character code: 0x%2x\n", c1);
	else if (isalnum(c1))
	{
	    s[0] = c1;
	    i = 1;
	    while (i<SLEN-1 && (c1=fgetc(fin)) && isalnum(c1))
		s[i++] = c1;
	    s[i++] = '\0';
	    if (i<SLEN)
		ungetc(c1, fin);
	    vfputWord(s, fout);
	}
	else
	    vfputAscii(c1, fout);
    }
    if (printPageNo)
	fprintf(fout, "(%d) prPageNo\n", pageNo + pnStart - 1);
    fprintf(fout, "endpage\n");
    fprintf(fout, "%%%%Trailer\n");
    fprintf(fout, "%%%%Pages: %d\n", pageNo);
    fprintf(fout, "%%%%EOF\n");
}


void vfputNewPage(f)
FILE *f;
{
    if (printPageNo)
	fprintf(f, "(%d) prPageNo\n", pageNo + pnStart - 1);
    fprintf(f, "endpage\n");
    pageNo++;
    fprintf(f, "%%%%Page: page%d %d\n", pageNo, pageNo);
    fprintf(f, "beginpage\n");
    hpos = rightmargin;
    vpos = topmargin;
    vfputNewLine(f);
}


void vfputNewLine(f)
FILE *f;
{
    vpos = topmargin - HZskip;
    hpos -= hskip;
    if (hpos < leftmargin)
    	vfputNewPage(f);
    else
	fprintf(f, "%6.2f %6.2f moveto\n", hpos, vpos);
}


void vfputHanzi(image, f, vmode)
char *image;	/* image points to hzbyte bytes of bitmap */
FILE *f;
char vmode;
{
    char c;
    int i;
    
    if (vpos < bottommargin)
        vfputNewLine(f);
    fprintf(f, "<");
    for (i=0; i<hzbyte; i++)
    {
	c = *image++;
	c = ~c;		/* invert the bitmap */
	fprintf(f, "%x%x", (c&0xF0)>>4, c&0x0F);
    }
    switch (vmode)
    {
    case 'R': fprintf(f, "> HZvshowR\n"); break;
    case 'Q': fprintf(f, "> HZvshowQ\n"); break;
    default:  fprintf(f, "> HZvshow\n"); break;
    }
    vpos -= HZskip;
}


void vfputWord(s, f)
char *s;
FILE *f;
{
    if (vpos + HZskip - strlen(s) * Evskip < bottommargin)
        vfputNewLine(f);
    fprintf(f, "(%s) Evshow\n", s);
    vpos -= strlen(s) * Evskip;
}


void vfputTab(f)
FILE *f;
{
    vpos = topmargin - ((topmargin-vpos-HZskip)/HZtab + 1)*HZtab - HZskip;
    if (vpos < bottommargin)
	vfputNewLine(f);
    else
	fprintf(f, "%6.2f %6.2f moveto\n", hpos, vpos);
}


void vfputAscii(c, f)
char c;
FILE *f;
{
    if (c=='-')
	c = '|';
    else if (c=='|')
	c = '-';

    if (c=='\n')
    {
	vfputNewLine(f);
	return;
    }
    if (c=='\f')
    {
	vfputNewPage(f);
	return;
    }
    if (c=='\t')
    {
	vfputTab(f);
	return;
    }
    if (vpos + HZskip - Epoint - Evskip < bottommargin)
	vfputNewLine(f);
    if (c==' ')
    {
	fprintf(f, "( ) Evshow\n");
    }
    else if (c=='(' ||  c==')')
    {
	fprintf(f,  "(\\%c) Evshow\n", c);
	vpos -= Epoint;
    }
    else if (c=='\\')
    {
	fprintf(f,  "(\\\\) Evshow\n");
	vpos -= Epoint;
    }
    else if (isgraph(c))
    {
	fprintf(f, "(%c) Evshow\n", c);
	vpos -= Epoint;
    }
    /* ignore the unprintable ascii characters */	
}


void vfputDash(scale, f)
float scale;
FILE *f;
{
    if (vpos < bottommargin)
	vfputNewLine(f);
    fprintf(f, "%3f vdash\n", scale);
    vpos -= HZskip;
}


