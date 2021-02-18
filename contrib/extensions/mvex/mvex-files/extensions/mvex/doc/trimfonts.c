/* $Header: /usr/local/src/x11r5.mvex/RCS/trimfonts.c,v 1.3 1991/07/16 18:57:38 toddb Exp $ */
#include <stdio.h>
/*
 * This is a very simple minded program to read its standard input
 * and eliminate the following lines (of postscript).
 */

char *strings[] = {
    "(LubalinGraph-Book)",
    "(LubalinGraph-BookOblique)",
    "(LubalinGraph-Demi)",
    "(LubalinGraph-DemiOblique)",
    "(Souvenir-Light)",
    "(Souvenir-LightItalic)",
    "(Souvenir-Demi)",
    "(Souvenir-DemiItalic)",
    "(Optima)",
    "(Optima-Oblique)",
    "(Optima-Bold)",
    "(Optima-BoldOblique)",
    "(Garamond-Light)",
    "(Garamond-LightItalic)",
    "(Garamond-Bold)",
    "(Garamond-BoldItalic)",
    "(AvantGarde-Book)",
    "(AvantGarde-BookOblique)",
    "(AvantGarde-Demi)",
    "(AvantGarde-DemiOblique)",
    "(Bookman-Light)",
    "(Bookman-LightItalic)",
    "(Bookman-Demi)",
    "(Bookman-DemiItalic)",
    "(Helvetica-Narrow)",
    "(Helvetica-Narrow-Oblique)",
    "(Helvetica-Narrow-Bold)",
    "(Helvetica-Narrow-BoldOblique)",
    "(NewCenturySchlbk-Roman)",
    "(NewCenturySchlbk-Italic)",
    "(NewCenturySchlbk-Bold)",
    "(NewCenturySchlbk-BoldItalic)",
    "(Palatino-Roman)",
    "(Palatino-Italic)",
    "(Palatino-Bold)",
    "(Palatino-BoldItalic)",
    "(ZapfChancery-MediumItalic)",
};

#define PRECHECK 5
#define BUFFERSIZ 4096

char check[PRECHECK][256];

main()
{
    int i, j, n;
    unsigned char *p, buf[ BUFFERSIZ ];

    for (i = 0; i < sizeof(strings) / sizeof(char *); i++) {
	p = (unsigned char *)strings[ i ];
	for (j = 0; j < PRECHECK; j++)
	    check[ j ][ *p++ ] = 1;
    }

skipLine:
    while (fgets(buf, BUFFERSIZ, stdin)) {
	p = buf;
	for (p = buf-1; *p++;) {
	    if (check[ 0 ][ *p ]
	     && check[ 1 ][ *(p+1) ]
	     && check[ 2 ][ *(p+2) ]
	     && check[ 3 ][ *(p+3) ]
	     && check[ 4 ][ *(p+4) ]
	     && IsAString(p))
		goto skipLine;
	}
	fputs(buf, stdout);
    }
    exit(0);
}

IsAString(p)
    char *p;
{
    int i;

    for (i = 0; i < sizeof(strings) / sizeof(char *); i++) {
	if (strncmp(p, strings[ i ], strlen(strings[ i ])) == 0)
	    return(1);
    }
    return (0);
}

