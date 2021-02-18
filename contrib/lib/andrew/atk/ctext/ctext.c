/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ctext/RCS/ctext.c,v 2.14 1991/09/12 16:11:55 bobg Exp $ */
/* $ACIS:ctext.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ctext/RCS/ctext.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ctext/RCS/ctext.c,v 2.14 1991/09/12 16:11:55 bobg Exp $";
#endif /* lint */


/* CText, a ``c'' mode for BE2. */

#include <class.h>
#include <andrewos.h> /* strings.h */
#include <setjmp.h>
#include <ctype.h>

#include <envrment.ih>
#include <stylesht.ih>
#include <style.ih>
#include <attribs.h>
#include <nstdmark.ih>
#include <tree23.ih>

#include <ctext.eh>

#define new(type) (type *) malloc(sizeof(type))
#define isident(c) (isalnum(c) || (c) == '_')

/* Variables used to control the indenting style. */

boolean useTabs = TRUE;
int levelIndent=4,
    contIndent=2,
    braceIndent=0,
    switchLevelIndent=8,
    switchLabelUndent=4;

void ctext__SetAttributes(self,atts)
struct ctext	*self;
struct attributes   *atts;
{
    super_SetAttributes(self,atts);

    while(atts!=NULL){
	if(strncmp(atts->key,"ctext-",6)==0){
	    char    *key=atts->key+6;

            if(strcmp(key,"level-indent")==0)
                levelIndent=atoi(atts->value.string);
            else if(strcmp(key,"cont-indent")==0)
                contIndent=atoi(atts->value.string);
            else if(strcmp(key,"brace-indent")==0)
                braceIndent=atoi(atts->value.string);
            else if(strcmp(key,"switch-level-indent")==0)
                switchLevelIndent=atoi(atts->value.string);
            else if(strcmp(key,"switch-label-undent")==0)
                switchLabelUndent=atoi(atts->value.string);
	    else if(strcmp(key,"use-tabs")==0)
		useTabs=atoi(atts->value.string);
	}

	atts=atts->next;
    }
}

static void SetupStyles(self)
    struct ctext *self;
{
    self->comment_style = stylesheet_Find(self->header.text.styleSheet, "comment");
    self->fnname_style = stylesheet_Find(self->header.text.styleSheet, "function");

    ctext_SetGlobalStyle(self, stylesheet_Find(self->header.text.styleSheet, "global"));
}

boolean ctext__InitializeObject(classID, self)
    struct classheader *classID;
    struct ctext *self;
{
    struct attributes templateAttribute;

    templateAttribute.key = "template";
    templateAttribute.value.string = "ctext";
    templateAttribute.next = NULL;
    ctext_SetAttributes(self, &templateAttribute);
    ctext_SetExportEnvironments(self, FALSE);

    SetupStyles(self);
    return TRUE;
}

void ctext__Clear(self)
struct ctext *self;
{
    super_Clear(self); /* This destroys all styles in the stylesht. */

    SetupStyles(self);
}

long ctext__Read(self, file, id)
struct ctext *self;
FILE *file;
long id;
{
    long tmpRetValue;

    tmpRetValue = super_Read(self, file, id);
    if (tmpRetValue == dataobject_NOREADERROR) {
        SetupStyles(self);
        ctext_RedoStyles(self);
    }
    return tmpRetValue;
}

long ctext__Write(self, file, id, level)
struct ctext *self;
FILE *file;
long id;
int level;
{
    int len = ctext_GetLength(self);
    if (len > 0 && ctext_GetChar(self, len - 1) != '\n')
        ctext_InsertCharacters(self, len, "\n", 1);
    return super_Write(self, file, id, level);
}

/****************************************************************
 *
 *  Style parsing code.
 *
 ****************************************************************/

#define NONE 0
#define SINGLE 1
#define DOUBLE 2

static void wrapCommentStyle(self, start, len, incB, incE)
struct ctext *self;
long start, len;
boolean incB, incE;
{
    struct environment *env;
    if (self->comment_style == NULL)
        return;
    env = environment_InsertStyle(self->header.text.rootEnvironment, start,
      self->comment_style, TRUE);
    environment_SetLength(env, len);
    environment_SetStyle(env, incB, incE);
}

static void wrapFnnameStyle(self, start, len)
struct ctext *self;
long start, len;
{
    struct environment *env;
    if (self->fnname_style == NULL)
        return;
    env = environment_InsertStyle(self->header.text.rootEnvironment, start,
      self->fnname_style, TRUE);
    environment_SetLength(env, len);
}

/* Argument to comment is position of asterisk that starts */
/* the comment. Returns position one past ending slash. */

static long comment(self, start)
struct ctext *self;
long start;
{
    int prev, c = 0;
    long end = start, len = ctext_GetLength(self);
    long origStart = start;

    while (end < len) {
        prev = c;
        c = ctext_GetChar(self, end++);

        if (c == '/' && prev == '*') {
            wrapCommentStyle(self, start, end - start - 1, (start != origStart), FALSE);
            break;
        }
        if (c == '\n') {
            wrapCommentStyle(self, start, end - start - 1, (start != origStart), TRUE);

            while (end < len) {
                prev = c;
                c = ctext_GetChar(self, end++);
                if (c == ' ' || c == '\t' || c == '\n')
                    continue;
                break;
            }
            start = end - 1;
        }
    }

    return end;
}

static void fn_name(self, posn, last_comment)
struct ctext *self;
long posn, last_comment;
{
    long len = 0;

    {
        /* False alarm if it turns out to be a global array of pointers to */
        /* functions:  if '[' is first non-white char following end paren */
        long pos = posn, length = ctext_GetLength(self);
        while (ctext_GetChar(self, pos) != ')' && pos < length)
            pos++;
        pos++;
        while (ctext_GetChar(self, pos) <= ' ' && pos < length)
            pos++;
        if (pos < length && ctext_GetChar(self, pos) == '[')
            return;
    }

    /* Scan backwards for the end of preceding whitespace */
    while (--posn >= last_comment && ctext_GetChar(self, posn) <= ' ');

    /* Now find start of token */
    while (posn >= last_comment && isident(ctext_GetChar(self, posn))) {
        posn--;
        len++;
    }

    if (len > 0)
        wrapFnnameStyle(self, posn + 1, len);
}

static void DoFreeTree(self)
struct nestedmark *self;
{
    nestedmark_FreeTree(self);
}

void ctext__RedoStyles(self)
struct ctext *self;
{
    struct nestedmark *root = (struct nestedmark *)self->header.text.rootEnvironment;
    long last_comment = 0, posn, len = ctext_GetLength(self);
    int define = 0, prev = 0, c = '\n', braces = 0, string = NONE, escape=FALSE;
    /* c is initialize to a newline so the start of the file looks like the start of line. */

    /* Remove the old styles, but leave the root environemt in place. */
    if (root->children) {
        tree23int_Apply(root->children, DoFreeTree);
        tree23int_Destroy(root->children);
        root->children = NULL;
    }

    for (posn = 0; posn < len; posn++) {
        prev = c;
        c = ctext_GetChar(self, posn);
        if(escape)
            escape=FALSE;
        else
            switch (c) {
                case '#':
                    if (prev == '\n')
                        define = 1;
                    break;
                case '\\':
                    escape=TRUE;
		    break;
                case '\n':
                    define = 0;
                    break;
                case '*':
                    if (prev == '/' && string == NONE)
                        last_comment = posn = comment(self, posn) - 1;
                    break;
                case '{':
                    if (string == NONE)
                        braces++;
                    break;
                case '}':
                    if (string == NONE)
                        braces--;
                    break;
                case '(':
                    if (!define && string == NONE) {
                        if (braces == 0)
                            fn_name(self, posn, last_comment);
                        braces++;
                    }
                    break;
                case ')':
                    if (!define && string == NONE)
                        braces--;
                    break;
                case '\'':
                case '\"':
                    if (string == NONE)
                        if (c == '\"')
                            string = DOUBLE;
                        else
                            string = SINGLE;
                    else
                        if ((string == DOUBLE && c == '\"') || (string == SINGLE && c == '\''))
                            string = NONE;
                    break;
                default:
                    break;
            }
        }
}

/****************************************************************
 *
 * Paren balancer.
 *
 ****************************************************************/


struct paren_node {
    long type;
    struct paren_node *next;
};


static boolean Quoted(self, pos)
struct ctext *self;
long pos;
{
    /* returns true iff the character at pos is quoted (ie. "\"). Takes into account the slash being quoted. (ie "\\"). */

    boolean retval = FALSE;

    pos--;
    while (ctext_GetChar(self, pos) == '\\' && pos > 0) {
        retval = !retval;
	pos--;
    }

    return retval;
}

void ctext__RecommentBackward(self, pos)
struct ctext *self;
long pos;
{
    int end = pos - 2;

    pos -= 2;   /* Dot was positioned one past the slash */

    if (pos < 2 || ctext_GetChar(self, pos--) != '*')
        return;

    while (--pos >= 0) {
        int c = ctext_GetChar(self, pos);
        if (c == '/' && ctext_GetChar(self, pos + 1) == '*') {
            pos++;
            break;
        }
        /* Abort if reach end of another comment */
        if (c == '*' && ctext_GetChar(self, pos + 1) == '/')
            return;
    }

    if (pos >= 0) {
        int endpos;

	environment_Remove(self->header.text.rootEnvironment, pos, end - pos + 1, environment_Style, TRUE);

	endpos = comment(self, pos);
        ctext_RegionModified(self, pos, endpos - pos);
    }
}

long ctext__ReverseBalance(self, pos)
struct ctext *self;
long pos;
{
    boolean found = FALSE, incomment = FALSE, instring = FALSE, doublestring = FALSE, atleastone = FALSE;
    int thischar, prechar;
    char *parentype;
    struct paren_node *parenstack = NULL;
    static char *opens = "({[", *closes = ")}]";


    while ((parenstack != NULL || !atleastone) && (pos > 0)) {
        thischar = ctext_GetChar(self, --pos);
        prechar = pos > 0 ? ctext_GetChar(self, pos - 1) : 0;

	if (incomment) {
	    if(thischar == '*' && prechar == '/') {
                incomment = FALSE;
	    }
        }
	else if (!Quoted(self, pos)) {
            if (instring) {
		if ((thischar == '"' && doublestring) || (thischar == '\'' && !doublestring)) {
                    instring = FALSE;
		}
            }
            else if (thischar == '"') {
                instring = TRUE;
		doublestring = TRUE;
	    }
	    else if (thischar == '\'') {
		instring = TRUE;
		doublestring = FALSE;
	    }
	    else if (thischar == '/' && prechar == '*') {
		incomment = TRUE;
	    }
	    else if ((parentype = index(opens, thischar)) != NULL) {
		if (parenstack == NULL || parenstack->type != (parentype - opens)) {
		    break;
		}
		else {
		    struct paren_node *temp = parenstack;

		    parenstack = parenstack->next;
		    free(temp);
		    if ((prechar == '\n' || pos == '0') && parenstack != NULL) {
		        break;
		    }
		    else if (parenstack == NULL) {
		        found = TRUE;
			break;
		    }
		}
	    }
	    else if ((parentype = index(closes, thischar)) != NULL) {
		struct paren_node *temp = new(struct paren_node);

		temp->type = parentype - closes;
		temp->next = parenstack;
		parenstack = temp;
	    }
	}
    }

    if (found) {
	return pos;
    }
    else {
	return EOF;
    }
}

/*****************************************************************
 * Indenter.  This is a completely new one
 * 15 July 1987 - Miles Bader
 *****************************************************************/

long ctext__Indent(self,mark)
struct ctext *self;
struct mark *mark;
{
    long end,pos=mark_GetPos(mark);
    boolean isregion=(mark_GetLength(mark)>0);
    int c;

    while(pos-->0 && ctext_GetChar(self,pos)!='\n')
        ;

    pos++;

    do{
        pos=ctext_TabAndOptimizeWS(self,pos,indentation(self,pos,FALSE,!isregion));

        for(end=pos; (c=ctext_GetChar(self,end))==' ' || c=='\t'; end++)
            ;
        if(end>pos)
            ctext_DeleteCharacters(self,pos,end-pos);

        end=mark_GetEndPos(mark);
        while(pos<end && (c=ctext_GetChar(self,pos++))!=EOF && c!='\n')
            ;
    }while(pos<end && c!=EOF);

    return pos;
}

static boolean InsideComment(self, pos)
struct ctext *self;
long pos;
{
    boolean slash = FALSE, star = FALSE;

    for (; pos >= 0; pos--) {
        int i = ctext_GetChar(self, pos);
        if (i == '*') {
            if (slash)
                return FALSE;
            star = TRUE;
        } else if (i == '/') {
            if (star)
                return TRUE;
            slash = TRUE;
        } else
            star = slash = FALSE;
    }

    return FALSE;
}

void ctext__ReindentLine(self,pos)
struct ctext *self;
long pos;
{
    long end;
    int c;

    if (InsideComment(self, pos))
        return;

    while (pos>0 && ctext_GetChar(self, --pos) != '\n')
        ;

    pos++;

    pos=ctext_TabAndOptimizeWS(self,pos,indentation(self,pos,TRUE,TRUE));

    for(end=pos; (c=ctext_GetChar(self,end))==' ' || c=='\t'; end++)
        ;

    if(end>pos)
        ctext_DeleteCharacters(self,pos,end-pos);
}

/* assumes that pos points to 1 BEFORE ending '/' */
int maybeBackwardSkipComment(self,pos)
struct ctext *self;
int pos;
{
    if(pos>=0 && ctext_GetChar(self,pos)=='*')
        while(--pos>=0)
             if(ctext_GetChar(self,pos)=='*' && (--pos<0 || ctext_GetChar(self,pos++)=='/')){
                 pos-=2;
                 break;
	     }
    return pos;
}

/* assumes that pos points to 1 BEFORE ending delimiter */
int backwardSkipString(self,pos,delim)
struct ctext *self;
int pos;
char delim;
{
    while(pos>=0)
        if(ctext_GetChar(self,pos--)==delim){
            boolean real=TRUE;
            while(pos>=0 && ctext_GetChar(self,pos)=='\\'){
                pos--;
                real=!real;
            }
            if(real)
                break;
        }

    return pos;
}

/* beg and end must be constants (for switch) */
/* assumes ending delimiter AFTER pos */
int backwardSkipDelimited(self,pos,beg,end)
struct ctext *self;
int pos;
char beg, end;
{
    int level=1;
    while(pos>=0 && level>0){
	int c;
	switch(c=ctext_GetChar(self,pos--)){
	    case '\'':
	    case '"':
		pos=backwardSkipString(self,pos,c);
		break;
	    case '/':
		pos=maybeBackwardSkipComment(self,pos);
		break;
	    default:
		if(c==beg)
		    level--;
		else if(c==end)
		    level++;
	}
    }

    return pos;
}

backwardSkipJunk(self,pos)
struct ctext *self;
long pos;
{
    while(pos>=0){
        register int c=ctext_GetChar(self,pos--);

        switch(c){
            case '/':
                pos=maybeBackwardSkipComment(self,pos);
                break;
            case '\n':
		pos=maybeBackwardSkipCppLines(self,pos);
		/* fall through */
            case ' ':
            case '\t': 
                /* nothing */
                break;
            default:
                return pos+1;
        }
    }

    return -1;
}

/* returns the length of a *constant* string */
#define cstrlen(str) (sizeof(str)-1)

/* pos should pointer to 2nd from last character-- last character won't be
 * matched; strconst *must* be a constant string, so that sizeof works
 */
#define backwardMatch(self,pos,strConst) \
    (pos>=cstrlen(strConst)-1 && match(self,pos-(cstrlen(strConst)-1)+1,strConst))

#define match(self,pos,str) ((pos==0 || !isalnum(ctext_GetChar(self,pos-1))) && !isident(ctext_GetChar(self,pos+cstrlen(str))) && domatch(self,pos,str,cstrlen(str)))

domatch(self,pos,str,len)
struct ctext *self;
int pos;
char *str;
int len;
{
    while(len>0 && ctext_GetChar(self,pos++)==*str++)
        len--;

    return len==0;
}

int maybeBackwardSkipCppLines(self,pos)
struct ctext *self;
int pos;
{
    int currentEOL;

    do{
	currentEOL=pos;
	pos=backwardSkipString(self,pos,'\n');
    }while(pos>=0 && ctext_GetChar(self,pos+2)=='#'); /* character AFTER newline */

    return currentEOL;
}

indentation(self,pos,reindent,indentEmptyLines)
struct ctext *self;
long pos;
boolean reindent,indentEmptyLines;
{
    int c;
    long savedPos=(-1),
        parensFollowedBy=(-1),
        colonFollowedBy=(-1);
    int elseCatch=0;
    boolean onestatement=TRUE,
      skipend=FALSE,
      braceFlag=FALSE,
      commaFlag=FALSE;
    int braceExtra=0,
      levelExtra=levelIndent,
      switchLevelExtra=switchLevelIndent,
      labelExtra=0;

    pos--; /* parse everything *before* pos */

    /* see if indenting any special cases */
    {
        long newpos=pos+1;

        while((c=ctext_GetChar(self,newpos))==' ' || c=='\t')
            newpos++;
        switch(c){
            case '}':
		braceFlag=TRUE; /* kluge ? */
                pos=backwardSkipDelimited(self,newpos-1,'{','}');
                /* fall through */
            case '{':
                levelExtra=braceIndent;
		switchLevelExtra=braceIndent;
                break;
	    case '#':
		return 0;
            case 'e':
                if(match(self,newpos,"else"))
                    elseCatch=1;
            case 'c':
            case 'd':
                if(match(self,newpos,"case") || match(self,newpos,"default"))
                    labelExtra= -switchLabelUndent;
                break;
	    case '\n':
	    case EOF:
		if(!indentEmptyLines)
		    return 0;
		break;
        }
    }

    /* sometimes, *only* want to re-indent special cases */
    if(reindent && levelExtra==levelIndent && labelExtra==0 && elseCatch==0)
        return currentIndent(self,pos);

    pos=backwardSkipJunk(self,pos);

    for(;;){
	switch(ctext_GetChar(self,pos)){
	    case '}':
		pos=backwardSkipDelimited(self,--pos,'{','}'); /* avoid hitting the { */
		onestatement=FALSE;
		braceFlag=TRUE;
		pos=backwardSkipJunk(self,pos);
		continue;
	    case '{':
		braceExtra+=levelIndent;
		braceFlag=TRUE;
		pos=backwardSkipJunk(self,--pos);
		continue;
	}
	break;
    }

    if(ctext_GetChar(self,pos)==';'){
	savedPos=pos--;
	onestatement=FALSE;
    }

    for(;;){
        int c;

        if(pos<0)
            c=EOF;
        else
            c=ctext_GetChar(self,pos--);

        switch(c){
            case '/':
                pos=maybeBackwardSkipComment(self,pos);
                break;
            case '"':
            case '\'':
                pos=backwardSkipString(self,pos,c);
                savedPos=pos;
                break;
	    case '*':
		/* primitive comment support */
		if(pos>=0 && ctext_GetChar(self,pos)=='/')
		    return currentColumn(self,pos+1);
		break;
            case '\n':
		/* must check to avoid cpp lines */
		pos=maybeBackwardSkipCppLines(self,pos);
		/* fall through */
            case ' ':
            case '\t':
                /* nothing */
                break;
            case 'f': /* if */
                if((onestatement || elseCatch>0) && backwardMatch(self,pos,"if"))
                    if(elseCatch==0)
                        if(parensFollowedBy==-1)
                            return currentIndent(self,pos)+levelExtra+labelExtra;
                        else
                            return currentIndent(self,pos)+levelExtra+contIndent;
                    else if(--elseCatch==0)
                        return currentIndent(self,pos); /* line else up with if */
                savedPos=pos;
                break;
            case 'o': /* do */
                if(onestatement && backwardMatch(self,pos,"do"))
                    if(savedPos==-1)
                        return currentIndent(self,pos)+levelExtra+labelExtra;
                    else
                        return currentIndent(self,pos)+levelExtra+contIndent;
                savedPos=pos;
                break;
            case 'e': /* else, case or while */
                if(backwardMatch(self,pos,"case"))
                    if(!onestatement || colonFollowedBy==-1)
			return currentIndent(self,pos) + switchLevelIndent - (switchLevelIndent - switchLabelUndent) + labelExtra; 
                    else
                        return currentIndent(self,pos) + (switchLevelIndent - switchLabelUndent) + contIndent;
                else if(onestatement && backwardMatch(self,pos,"while"))
                    if(parensFollowedBy==-1)
                        return currentIndent(self,pos)+levelExtra+labelExtra;
                    else
                        return currentIndent(self,pos)+levelExtra+contIndent;
                else if(backwardMatch(self,pos,"else"))
                    if(onestatement)
                        if(savedPos==-1)
                            return currentIndent(self,pos)+levelExtra+labelExtra;
                        else
                            return currentIndent(self,pos)+levelExtra+contIndent;
                    else{
                        skipend=TRUE;
                        if(elseCatch>0)
                            elseCatch++;
                    }
                savedPos=pos;
                break;
            case 'r':
                if(onestatement && backwardMatch(self,pos,"for"))
                    if(parensFollowedBy==-1)
                        return currentIndent(self,pos)+levelExtra+labelExtra;
                    else
                        return currentIndent(self,pos)+levelExtra+contIndent;
                savedPos=pos;
                break;
            case 'h':
                if(onestatement && backwardMatch(self,pos,"switch"))
                    if(parensFollowedBy==-1)
                        return currentIndent(self,pos)+switchLevelExtra+labelExtra;
                    else
                        return currentIndent(self,pos)+switchLevelExtra+contIndent;
                savedPos=pos;
                break;
            case 't':
                if(backwardMatch(self,pos,"default"))
                    if(!onestatement || colonFollowedBy==-1)
			return currentIndent(self,pos) + switchLevelIndent - (switchLevelIndent - switchLabelUndent) + labelExtra; 
                    else
                        return currentIndent(self,pos) + (switchLevelIndent - switchLabelUndent) + contIndent;
                savedPos=pos;
                break;
            case ':':
                colonFollowedBy=savedPos;
                savedPos=pos;
                break;
	    case ',':
		/* this is a kluge, which assumes that commas occuring outside of parens
		* imply a separate statement (i.e., equivalent indentation).  This is mostly true.
		*/
		commaFlag=TRUE;
		savedPos=pos;
		break;
            case '{':
                /* Since skipend being set here would indicate a syntax
                 * error in the code, we assume it isn't.
                 */
                /* fall through */
            case '}':
                if(skipend)
                    pos=backwardSkipDelimited(self,pos,'{','}');
                /* fall through */
	    case ';':
	    case EOF:
		if(skipend){
		    skipend=FALSE;
		    savedPos=pos;
		}else if(onestatement){
		    int cin=currentIndent(self,savedPos);
		    /* savedPos set at beginning */

		    if(braceFlag)
			return cin+braceExtra;
		    else if(cin==0)
			return 0;
		    else if(commaFlag)
			return cin;
		    else
			return cin+contIndent;
		}else
		    /* CAN'T get here until after something that sets savedPos */
		    return currentIndent(self,savedPos)+labelExtra+braceExtra;
		break;
	    case ')':
		pos=backwardSkipDelimited(self,pos,'(',')');
		/* save to do continuation lines correctly */
		parensFollowedBy=savedPos;
		savedPos=pos;
		break;
	    case ']':
		pos=backwardSkipDelimited(self,pos,'[',']');
		savedPos=pos;
		break;
	    case '(':
	    case '[':
		return currentColumn(self,pos+1)+1;
	    default:
                savedPos=pos;
	}
    }
}

int currentIndent(self,pos)
struct ctext *self;
long pos;
{
    int c,ind=0;

    while(pos>=0 && (c=ctext_GetChar(self,pos))!='\n')
        pos--;

    while(isspace(c=ctext_GetChar(self,pos++)))
        switch(c){
            case ' ':
                ind++;
                break;
            case '\t':
                ind=(ind+8)&~7;
                break;
        }

    return ind;
}

int currentColumn(self,pos)
struct ctext *self;
long pos;
{
    int ind=0,oldPos=pos;

    while(pos>=0 && (ctext_GetChar(self,pos))!='\n')
        pos--;

    while(pos<oldPos)
        if(ctext_GetChar(self,pos++)=='\t')
            ind=(ind+8)&~7;
        else
            ind++;

    return ind;
}

/*
 * optimized whitespace insertion
 * 7 June 1987 - Miles Bader
 */

static char *spaces="        ";

#define TABOVER(self,oldpos,pos,oldcol,col) \
    if(col>oldcol){\
	ctext_DeleteCharacters(self,oldpos,pos-oldpos);\
	pos=oldpos;\
	if(useTabs){\
	    int nc;\
	    while(col>=(nc=(oldcol+8)&~7)){\
		oldcol=nc;\
		ctext_InsertCharacters(self,pos++,"\t",1);\
	    }\
	}else{\
	    int count=(col-oldcol)/8;\
	    while(count--){\
		ctext_InsertCharacters(self,pos,spaces,8);\
		oldcol+=8;\
		pos+=8;\
	    }\
	}\
	ctext_InsertCharacters(self,pos,spaces,col-oldcol);\
	pos+=(col-oldcol);\
    }

long ctext__TabAndOptimizeWS(self,pos,inc)
struct ctext    *self;
long	pos;
int	inc;
{
    int     home=0, oldPos, col=0, oldCol=0, target;

    while(--pos>=0 && ctext_GetChar(self,pos)!='\n')
        home++;

    oldPos= ++pos;
    while(home--){
        switch(ctext_GetChar(self,pos)){
            case '\t':
                col=(col+8)&~7;
                break;
            case ' ':
                col++;
                break;
            default:
                if(col>oldCol)
		    TABOVER(self,oldPos,pos,oldCol,col);

                oldPos=pos+1;
                oldCol= ++col;
        }
        pos++;
    }

    /* the column that we are aiming for */
    if(inc==0)
	target=col;
    else
	target=(col+inc)-(col%inc);

    /* skip over existing white space */
    while(col<target){
	int	nc;
        int	c=ctext_GetChar(self,pos);

        if(c==' '){
            col++;
            pos++;
        }else if(c=='\t' && (nc=(col+8)&~7)<=target){
            col=nc;
            pos++;
        }else
            break;
    }

    /* add new whitespace */
    TABOVER(self,oldPos,pos,oldCol,target);

    return pos;
}

/* --- */
