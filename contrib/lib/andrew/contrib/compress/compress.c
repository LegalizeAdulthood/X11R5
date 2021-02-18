/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/compress/RCS/compress.c,v 1.4 1991/09/12 16:49:47 bobg Exp $
*/

/*RSKmod butchered from 'fnote.c'*/

#include <class.h>
#include <envrment.ih>
#include "text.ih"
#include "style.ih"
#include <viewref.ih>
#include <stylesht.ih>
#include "smpltext.ih"    /*RSKadd*/
#include "compress.eh"

static char rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/compress/RCS/compress.c,v 1.4 1991/09/12 16:49:47 bobg Exp $";
static char CopyRight[] = " Copyright (c) IBM Corp. 1989";

/*
struct compress **stack , **endstack;
struct text *tmptext;
long notecount;
boolean donumber;
struct style *Style = NULL;
struct style *CompressStyle = NULL;
*//*RSKmod remove*/


#define BACKWARD 0
#define FORWARD 1
#define NOPROCESS 2
#define MAXNOTES 500
#define OPEN TRUE
#define CLOSE FALSE
#define FLIP 42

void compress__GetStyle(self,txt)
struct compress *self;
struct text *txt;
{
    struct style *Style = NULL;
    if(txt != self->parenttext){
	self->compressed_style = NULL;
	self->parenttext = txt;
    }
    if(self->compressed_style) return;
    if(txt && (Style = stylesheet_Find(txt->styleSheet, "compressed")) == NULL){
	Style = style_New();
	style_SetName(Style, "compressed");
	style_AddHidden(Style);
/*	style_AddUnderline(Style);*//*RSKmod: replace Hidden with Underline to verify that wrapping hidden style is a drawtxtv.c bug*/
	/*style_AddReadOnly(Style);*//*ReadOnly Style will not be implemented anyway*/
	stylesheet_Add(txt->styleSheet, Style);
    }
    self->compressed_style = Style;
}

boolean compress__InitializeObject(ClassID,self)    /*RSKmod*/
struct classheader *ClassID;
struct compress *self;
{
    self->cpmark=NULL;
    self->parenttext = NULL;
    self->compressed_style = NULL;
    compress_SetExportEnvironments(self, FALSE);
    return TRUE;
}

void compress__FinalizeObject(ClassID,self)    /*RSKadd*/
struct classheader *ClassID;
struct compress *self;
{
/*    if (self->viewbox!=NULL) compressv_UnlinkTree(self->viewbox);*/
    return;
}

long compress__Read(self,file,id)
struct compress *self;
FILE *file;
long id;
{
    long foo;
    foo = super_Read(self,file,id);
    return foo;
}

boolean compress__InitializeClass(ClassID)
struct classheader *ClassID;
{
/*
    stack = NULL;
    endstack = NULL;
    tmptext = NULL;
    notecount = 0;
    donumber = 0;
    Style = NULL;
    CompressStyle = NULL;
*//*RSKmod remove*/
    return TRUE;
}

long nextNL(ct,pos,endtxt)
/*returns (pos of next '\n')*/
/* NOTE! If pos is pointing to a '\n' upon entry to this routine, pos remains pointing to that '\n' as it is considered the "next" '\n' character.  This is necessary to catch multiple '\n's in the line count.  RLQ*/
struct text *ct;
long pos,endtxt;
{
    while((pos<endtxt)&&(text_GetChar(ct,pos)!='\n')) pos++;
    return pos;
}

long prevNL(ct,pos)
/*returns (pos of prev '\n')*/
struct text *ct;
long pos;
{
    if (text_GetChar(ct,pos)=='\n') pos--;
    while((pos>=0)&&(text_GetChar(ct,pos)!='\n')) pos--;
    return pos;
}

int is_whitespace(ch)
/* Returns 1 if whitespace or 0 if not whitespace.  NOTE: '\n' is NOT considered whitespace */
char ch;
{
    return((ch==' ')||(ch=='\t'));
}

long skipwhitespace(ct,pos,endtxt)
/* Skips over whitespace until non whitespace, '\n', or end of text is found */
struct text *ct;
long pos,endtxt;
{
    while((pos<endtxt)&&is_whitespace(text_GetChar(ct,pos))) pos++;
    return pos;
}

int currentIndent(txt,pos)
/* Returns current indentation on this line. */
/* NOTE !!! - REQUIREMENT for entry is that pos is pointing to the first position of the line (ie previous '\n'+1) in which the indent is to be calculated. */ 
struct text *txt;
long pos;
{
    int c,indent=0;

    if (text_GetChar(txt,pos)=='\n') return indent;
    while(is_whitespace(c=text_GetChar(txt,pos))||(c>127)) /*RLQmod*/
    {
	pos++;
	if (c=='\t')
	    indent=(indent+8)&~7;
	else
	    if (c<=127) /*RSKmod: don't count compress box as a char*/
		indent++;
    }
    return indent;
}

int compress__CurrentIndentation(classID,txt,pos)
struct classheader *classID;
struct text *txt;
long pos;
{
    int tmp;
    pos = prevNL(txt, pos)+1;
    tmp=currentIndent(txt,pos);
    return tmp;
}

boolean emptyLine(txt,pos,endtxt)
/* Returns TRUE if line is empty (eg whitespace up to the '\n' character), or FALSE if line has a non-whitespace character in it before the '\n' character. */
    struct text *txt;
    long pos,endtxt;
    {
	pos=skipwhitespace(txt,prevNL(txt,pos)+1,endtxt);
	if (text_GetChar(txt,pos)!='\n' && pos<endtxt) return FALSE;
	else
	    return TRUE;
    }

struct environment *getEnvironment(txt,pos)    /*butchered from mtext.c*/
struct text *txt;
long pos;
{
    struct environment *me;
    if(me=txt->/*header.text.*/rootEnvironment) me=environment_GetEnclosing(me,pos);
    return me;
}

struct style *getStyle(txt,pos)    /*butchered from mtext.c*/
struct text *txt;
long pos;
{
    struct environment *me;
    if(me=getEnvironment(txt,pos)) return me->data.style;
    return NULL;
}

boolean compress__IsHidden(self,txt,pos)
struct compress *self;
struct text *txt;
long pos;
{
    boolean retval;
    retval=style_IsHiddenAdded(getStyle(txt,pos));
    return retval;
}

/* Compress(self,txt,pos,len) compresses the region if len is non-zero, otherwise it compresses according to indentation */
void compress__Compress(self,txt,pos,len)   
struct compress *self;
struct text *txt;
long pos,len;
{
    struct environment *te; /* This is needed for checking styles if desired */
    long endregion=pos+len;
    long endtxt=text_GetLength(txt);
    long forwardpos=nextNL(txt,pos,endtxt)+1;
    int  indent=0;
    long nlines=1;
    long starthide=0;
    long temp;

    pos = prevNL(txt, pos)+1;
    /* Check for any style and adjust pos to (start of style) if pos is within a style- RLQ*/
    te = txt->rootEnvironment;	/* now its the root */
    if (te)
	te = environment_GetEnclosing(te, pos);	/* now its not */
    if (te) {
	if (te != txt->rootEnvironment) {
	    temp = pos;
	    pos = environment_Eval(te); /* set pos to the beginning of style */
	    while (temp > pos) {
		if (text_GetChar(txt,temp)=='\n') nlines++; /* catch any '\n's skipped over */
		temp--;
	    }
	    pos = prevNL(txt, pos)+1; /* set pos to beginning of this line */
	}
    }

    indent = currentIndent(txt,pos);

    if (len==0)	{ /* COMPRESS ACCORDING TO INDENT OF CURRENT LINE */
	temp = pos;
	pos--; /* pos now points to the prev NL */
	while (pos>0 && (currentIndent(txt,(pos=prevNL(txt,pos)+1))>=indent || emptyLine(txt,pos,endtxt))) {
	    temp = pos;
	    pos--; /* catch consecutive '\n's */
	    nlines++;
	}
	pos = temp;  
	starthide = pos;
	te = txt->rootEnvironment;	/* now its the root */
	if (te) 
	    te = environment_GetEnclosing(te, pos);	/* now its not */
	if (te) {
	    if (te != txt->rootEnvironment) {
		temp = pos;
		pos = environment_Eval(te) +  environment_GetLength(te);
		while (temp < pos) {
		    if (text_GetChar(txt,temp)=='\n') nlines--; /* catch any '\n's skipped over */
		    temp++;
		}
		starthide=nextNL(txt,pos,endtxt)+1;
		nlines--;
	    }
	}
	pos = forwardpos;
	while (pos<=endtxt && (currentIndent(txt,pos)>=indent || emptyLine(txt,pos,endtxt))) {
	    pos=nextNL(txt,pos,endtxt)+1;
	    nlines++;
	}
    }

    else
    {	/* COMPRESS THE REGION MARKED BY THE CURSOR */
	starthide = pos;
	pos = forwardpos;
	while (pos < endregion) {
	    if (text_GetChar(txt,pos)=='\n')
		nlines++;
	    pos++;
	}
	if (text_GetChar(txt,pos-1) != '\n') {
	    pos=nextNL(txt,pos,endtxt)+1;
	    nlines++;
	}
    }
/* COMMON WRAP-UP FOR COMPRESS ACCORDING TO INDENT OF CURRENT LINE AND  COMPRESS THE REGION MARKED BY THE CURSOR */
    pos--;
    te = txt->rootEnvironment;	/* now its the root */
    if (te) 
	te = environment_GetEnclosing(te, pos);	/* now its not */
    if (te) {
	if (te != txt->rootEnvironment) {
	    temp = pos;
	    pos = environment_Eval(te) +  environment_GetLength(te);
	    while (temp < pos) {
		if (text_GetChar(txt,temp)=='\n') nlines++; /* catch any '\n's skipped over */
		temp++;
	    }
	    pos=nextNL(txt,pos,endtxt);
	}
    }
    len = pos-starthide;
    self->cpmark=text_CreateMark(txt,starthide,len);
    self->lines=nlines;
    compress_GetStyle(self,txt);
}
    
/*-----------------------------------------------------------------------------*/
/*    printf("\nnth pos = %d, starthide = %d, len = %d, indent = %d, nlines = %d, endtxt = %d",
		  pos,      starthide,      len,      indent,      nlines,      endtxt);
    fflush(stdout); */
/*-----------------------------------------------------------------------------*/

/* NextCompressLoc(self,txt,startpos,indent) compresses a region that has the appropriate indentation.  It returns zero if no more regions are found to compress, otherwise it returns the location of the region it compressed */
long compress__NextCompressLoc(self,txt,pos,indent)    /*RSKadd*/
    struct compress *self;
    struct text *txt;
    long pos;
    int indent;

    {
	struct environment *te; /* This is needed for checking styles if desired */
	long endtxt=text_GetLength(txt);
	long len=0;
	long temp=0;
	long starthide=0;
	int  nlines=1;
	int  minlines=3; /* Don't compress unless there is AT LEAST minlines lines to compress.  This should never be <1, recommend never < 2.  RLQ */
	boolean loop;
	
	if (pos == 1) pos=0; /* hokey fix to catch first call with startpos set to 1 */

	while (nlines<minlines) { 
	    loop=TRUE;
	    while (loop) {
		while (pos<=endtxt && currentIndent(txt,pos)<indent && !(emptyLine(txt,pos,endtxt)) && loop){
		    pos=nextNL(txt,pos,endtxt)+1;
		}
		loop=FALSE;
		te = txt->rootEnvironment;	/* now its the root */
		if (te) 
		    te = environment_GetEnclosing(te, pos);	/* now its not */
		if (te) {
		    if (te != txt->rootEnvironment) {
			pos = environment_Eval(te) +  environment_GetLength(te);
			pos=nextNL(txt,pos,endtxt)+1;
			loop=TRUE;
		    }
		}
	    }
	    starthide=pos;
	    pos=nextNL(txt,pos,endtxt)+1;
	    while (pos<=endtxt && (currentIndent(txt,pos)>=indent || emptyLine(txt,pos,endtxt))) {
		pos=nextNL(txt,pos,endtxt)+1;
		nlines++;
	    }
	    te = txt->rootEnvironment;	/* now its the root */
	    if (te) 
		te = environment_GetEnclosing(te, pos-1);	/* now its not */
	    if (te) {
		if (te != txt->rootEnvironment) {
		    temp = pos-1;
		    pos = environment_Eval(te) +  environment_GetLength(te);
		    while (temp < pos) {
			if (text_GetChar(txt,temp)=='\n') nlines++; /* catch any '\n's skipped over */
			temp++;
		    }
		    pos=nextNL(txt,pos,endtxt)+1;
		}
	    }
	    if ((pos-1)>=endtxt) break;
	    if (nlines<minlines) nlines=1;
	}
	pos--; 
	if (pos>endtxt || nlines<minlines)
	    return 0; 
	len = pos-starthide;
	self->cpmark=text_CreateMark(txt,starthide,len);
	self->lines=nlines;
	compress_GetStyle(self,txt);
	return pos;
    }
/* ------------------------------------------------------------- */
/*	printf ("\n#01 pos = %d, endtxt = %d, indent = %d, nlines = %d, starthide = %d, len = %d",
		    pos,          endtxt,          indent,          nlines,          starthide,       len);
        fflush(stdout); 
*/
/* ------------------------------------------------------------- */

/* Decompress(self,txt) removes the hidden style, restoring the compressed text to normal */
void compress__Decompress(self,txt)    /*RSKadd*/
    struct compress *self;
    struct text *txt;
    {
    if (environment_Remove(txt->rootEnvironment,    mark_GetPos(self->cpmark),mark_GetLength(self->cpmark),environment_Style,FALSE)) text_SetModified(txt);
    text_RegionModified(txt,mark_GetPos(self->cpmark), mark_GetLength(self->cpmark));
    text_NotifyObservers(txt,0);
    }
