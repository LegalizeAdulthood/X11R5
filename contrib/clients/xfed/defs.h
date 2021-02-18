#include <X11/Xosdefs.h>
#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#else
extern char *malloc(), *realloc();
#endif

extern int errno;

struct box {
	int w, h;
	int x, y;
};

struct character {
	char    *charId;
	int     encoding;
	int	notadobe;
	int     swidth[2];
	int     dwidth[2];
	struct  box bbx;
	int     nrows;
	char    **rows;
};

struct font {
	int     rev, subrev;
	char    **comments;
	unsigned int	ncomments, szcomments;
	char    *fontname;
	int     sizes[3];
	struct box     boundingbox;
	char	**props;
	unsigned int    nprops;
	int     nchars;
	struct box maxbbx;
	struct  character *characters;
};

struct font font;
