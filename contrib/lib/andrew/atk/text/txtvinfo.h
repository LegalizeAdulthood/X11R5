/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/txtvinfo.h,v 2.12 1991/09/12 20:03:38 bobg Exp $ */
/* $ACIS:txtvinfo.h 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/txtvinfo.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtextviewinfo = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/txtvinfo.h,v 2.12 1991/09/12 20:03:38 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#include <txtstvec.h>

/* Redisplay structure giving font and position info in a large buffer. */

enum li_Type {
    li_Plain,
    li_Expose,
    li_Octal,
    li_View
};

struct viewitem {
    struct view *view;          /* Pointer to view object */
    short width, height;        /* Space allocated in text */
};

struct textitem {
    short rSpaces;              /* # rubber spaces in item */
    short lineBufPos;		/* Start pos in line buffer */
    struct fontdesc *font;	/* Font of any text in item */
    short *fontWidths;		/* Cached ptr to font widths tbl */
    short styleFlags;           /* style_Underline, etc., ORed */
    short hasTab;               /* flag */
    char *color;		/* color for text */
};

struct lineitem  {
    long docPos;		/* Start pos in document */
    short scripting;		/* Displacement added to y */
    short xPos;			/* X pos of the line start */
    enum li_Type type;          /* Specifies union member */
    union {
        struct viewitem viewitem;
        struct textitem textitem;
    } itemdata;
};

#define vi_view itemdata.viewitem.view
#define vi_width itemdata.viewitem.width
#define vi_height itemdata.viewitem.height

#define ti_rSpaces itemdata.textitem.rSpaces
#define ti_lineBufPos itemdata.textitem.lineBufPos
#define ti_font itemdata.textitem.font
#define ti_fontWidths itemdata.textitem.fontWidths
#define ti_styleFlags itemdata.textitem.styleFlags
#define ti_hasTab itemdata.textitem.hasTab
#define ti_color itemdata.textitem.color

#define	NTITEMS	60  /* Max env changes per line */
#define	LBSIZE 512  /* Max total chars in all line items */

/*
 * Variables representing the window manager state.  They are used
 * as global implicit parameters within the real-time justification code
 * which must run extremely fast.
 */

struct formattinginfo  {
    long lineLength;		/* Length (in chars) of screen line */
    long markLength;		/* # things considered in choosing line */
    long totalWidth;		/* Total width of text in screen line */
    long rubberSpaces;		/* Total # expandable spaces on line */
    struct text_statevector sv;	/* Current state vector */
    short *myWidths;		/* Width table for current font */
    boolean pBreak;		/* True if char is first in a paragraph */
    boolean endOfPara;		/* True if line is last line in a para. */
    int	below;			/* Below of line including spacing/spread */
    int lineBelow;		/* Below of tallest font/view on line */
    int textBelow;		/* Below of tallest font on line */
    int lineAbove;		/* Above of tallest font on line */
    int spaceBump;		/* Extra space added per rubber space */
    int	lineBP;			/* Pointer into line buffer array */
    unsigned char lineBuffer[LBSIZE];	/* Line buffer array */
    int	lineIP;			/* Pointer into lineitem array */
    struct lineitem *clp;	/* Pointer to lineitems[lineip-1] */
    struct lineitem lineItems[NTITEMS];	/* Line item array */
    int	xDim;			/* X dimension to draw in */
    int locateX;		/* Locatehit stores best x pos here */
    int cursorY;
    int	bLM;			/* Left margin at beg. of line */
    enum style_Justification just;	/* Just. mode at beg. of line */
    int bIndent;		/* Indentation at beg. of line */
    int	sawTab;			/* True if line contains a tab (breaks justifier) */
    int	inContinueMode;
    struct view *foundView;
    long predrawnEnd;
    struct lineitem *itemAtDot;
    long lastFontPos;
    boolean continued;		/* True if line extends past the end of page */
};

/* defines used by charType in txtvcmds.c */
#define	WHITESPACE	0
#define	WORD		1
#define	SPECIAL		2

