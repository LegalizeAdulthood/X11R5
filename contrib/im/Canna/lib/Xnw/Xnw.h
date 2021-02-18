#ifndef _Xnw_h_
#define _Xnw_h_

#include <X11/Intrinsic.h>
/*#include <X11/Quarks.h>*/

#define MAXCUT			30000

/*#define XtNfont                 "font"*/
#define XtNkanjiFont            "kanjiFont"
#define XtNkanaFont             "kanaFont"
#define XtNmodeDisplay          "modeDisplay"
#define XtNcvtBySpace           "cvtBySpace"
#define XtNlabelKanjiFont	"labelKanjiFont"
#define XtNlabelKanaFont	"labelKanaFont"

/*#define XtCFont                 "Font"*/
#define XtCKanjiFont            "KanjiFont"
#define XtCKanaFont             "KanaFont"
#define XtCModeDisplay          "ModeDisplay"
#define XtCCvtBySpace           "CvtBySpace"

/*#define XtRFontStruct           "FontStruct"*/
#define XtRKanjiFontStruct      "KanjiFontStruct"
#define XtRKanaFontStruct       "KanaFontStruct"

#define XtEdefaultfont		"xtedefaultfont"
#define XtEdefaultkanjifont	"xtedefaultkanjifont"
#define XtEdefaultkanafont	"xtedefaultkanafont"

#define Xnw_Ascii   0
#define Xnw_Kanji   1
#define Xnw_Kana    2

void XnwCvtStringToFontStruct();
void XnwCvtStringToKanjiFontStruct();
void XnwCvtStringToKanaFontStruct();
void XnwCvtEucCodeToJisCode();
void XnwCvtJisCodeToEucCode();
Boolean XnwIsLocalInput();
int XnwMax3();

#define XnwDefaultFont		"neci16"
#define XnwDefaultKanjiFont	"neck16"
#define XnwDefaultKanaFont	"neckana16"

#define ISKANJICODE(c1, c2, c3) \
		(((c1 == 0x1b) && (c2 == '$') && (c3 == 'B')) || \
		 ((c1 == 0x1b) && (c2 == '$') && (c3 == '@'))) 
		  	
#define ISROMANCODE(c1, c2, c3) \
		(((c1 == 0x1b) && (c2 == '(') && (c3 == 'J')) || \
		 ((c1 == 0x1b) && (c2 == '(') && (c3 == 'H')) || \
		 ((c1 == 0x1b) && (c2 == '(') && (c3 == 'B'))) 
		  	
#define ISKANACODE(c1, c2, c3) \
		((c1 == 0x1b) && (c2 == '(') && (c3 == 'I')) 
		  	
#endif /* _Xnw_h_ */
