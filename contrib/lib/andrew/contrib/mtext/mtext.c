/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mtext/RCS/mtext.c,v 1.14 1991/09/12 16:53:10 bobg Exp $ */
/* $ACIS:mtext.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mtext/RCS/mtext.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mtext/RCS/mtext.c,v 1.14 1991/09/12 16:53:10 bobg Exp $";
#endif /* lint */

/* Mtext, a Modula-2 mode for BE2. */

#include <class.h>
#include <andrewos.h>
#include <setjmp.h>
#include <ctype.h>

#include <envrment.ih>
#include <stylesht.ih>
#include <style.ih>
#include <attribs.h>
#include <nstdmark.ih>
#include <tree23.ih>
#include <mtext.eh>

/*
 * Parsing (NPDict, NPLookup) by Chris Newman, 1988
 */

#define	NPTOUPPER(c) (islower(c) ? toupper(c) : (c))

typedef struct {
    char    *stng;
    short   val;
} NPDict;

#define new(type) (type *) malloc(sizeof(type))

#define NONE 0
#define SINGLE '\''
#define DOUBLE '\"'

/*
 * Varables used to control the indenting style.
 */

boolean useTabs = FALSE;
int levelIndent=4,
contIndent=2,
braceIndent=0,
caseLevelIndent=4;

NPDict words[]={
    {"ABS",0},
    {"ADR",0},
    {"AND",0},
    {"ARRAY",0},
    {"BEGIN",9},
    {"BITSET",0},
    {"BOOLEAN",0},
    {"BY",0},
    {"CAP",0},
    {"CARDINAL",0},
    {"CASE",65},
    {"CHAR",0},
    {"CHR",0},
    {"CONST",9},
    {"DEC",0},
    {"DEFINITION",0},
    {"DISPOSE",0},
    {"DO",0},
    {"ELSE",4},
    {"ELSIF",36},
    {"END",50},
    {"EXCL",0},
    {"EXPORT",0},
    {"FALSE",0},
    {"FLOAT",0},
    {"FOR",1},
    {"FROM",0},
    {"FUNCTION",0},
    {"HALT",0},
    {"HIGH",0},
    {"IF",1},
    {"IMPLEMENTATION",0},
    {"IMPORT",0},
    {"INC",0},
    {"INCL",0},
    {"INTEGER",0},
    {"INTERFACE",0},
    {"MOD",0},
    {"MODULE",8},
    {"NEW",0},
    {"NOT",0},
    {"ODD",0},
    {"OF",0},
    {"OR",0},
    {"ORD",0},
    {"POINTER",0},
    {"PROC",0},
    {"PROCEDURE",136},
    {"QUALIFIED",0},
    {"REAL",0},
    {"RECORD",1},
    {"REPEAT",1},
    {"RETURN",0},
    {"SET",0},
    {"SIZE",0},
    {"TO",0},
    {"THEN",0},
    {"TRUE",0},
    {"TRUNC",0},
    {"TSIZE",0},
    {"TYPE",9},
    {"UNTIL",50},
    {"VAL",0},
    {"VAR",9},
    {"WHILE",1},
    {"WITH",1},
    {NULL,0}
};

void keyword();
int is_whitespace(ch)
char ch;
{
    return((ch==' ')||(ch=='\t'));
}

long skipwhitespace(ct,pos,end)
struct mtext *ct;
long pos,end;
{
    while((pos<end)&&is_whitespace(mtext_GetChar(ct,pos))) pos++;
    return pos;
}



long copyword(ct,pos,end,buffer)
struct mtext *ct;
long pos,end;
char buffer[];
{
    char ch;
    int count=0;
    buffer[0]=0;
    pos=skipwhitespace(ct,pos,end);
    while((count<=255)&&(pos<end)&& isalnum(ch=mtext_GetChar(ct,pos++))) buffer[count++]=NPTOUPPER(ch);
    buffer[count]=0;
    return pos;
}

long backwardcopyword(ct,from,to,buffer)
struct mtext *ct;
long from,to;
char buffer[];
{
    int count=0,i=0,j;
    buffer[0]=0;
    while((from-i>=to)&& isalnum(mtext_GetChar(ct,from-i))) i++;
    j=from-i;
    if(from-i>=to-1) for(;i>0 && count<255;i--,count++) {
	buffer[count]=NPTOUPPER(mtext_GetChar(ct,from-i+1));
    }
    buffer[count]=0;
    return j;
}

void makeupper(str)
char *str;
{
    while(*str!=0) {
	if(islower(*str)) *str=toupper(*str);
	str++;
    }
}

NPDict *NPlookup(dict, word)
NPDict *dict;
char *word;
{
    char *ptr1;
    if(dict!=NULL) {
	ptr1=dict->stng;
	while((ptr1!=NULL)&&(strcmp(ptr1,word)!=0)) ptr1=(++dict)->stng;
	return dict;
    }
}

struct environment *mtext__GetEnvironment(self,pos)
struct mtext *self;
long pos;
{
    struct environment *me;
    if(me=self->header.text.rootEnvironment) me=environment_GetEnclosing(me,pos);
    return me;
}

struct style *mtext__GetStyle(self,pos)
struct mtext *self;
long pos;
{
    struct environment *me;
    if(me=mtext_GetEnvironment(self,pos)) return me->data.style;
    return NULL;
}

void mtext__InsertNewStyle(self,pos,len,style,begflag,endflag)
struct mtext *self;
long pos,len;
struct style *style;
boolean begflag,endflag;
{
    struct environment *newenv;
    if (style!= NULL) {
	newenv=environment_InsertStyle(self->header.text.rootEnvironment, pos, style, 1);
	environment_SetLength(newenv, len);
	environment_SetStyle(newenv,begflag,endflag);
    }
}

/* assumes that pos points to 1 BEFORE ending delimiter */
long backwardSkipString(self,pos,delim)
struct mtext *self;
long pos;
char delim;
{
    while((pos>=0)&& mtext_GetChar(self,pos--)!=delim);
    return pos;
}
boolean InString(self,pos)
struct mtext *self;
long pos;
{
    long backwardSkipComment();
    char c,lastquote=0;
    boolean quotes=FALSE;
    while((pos>=0)&& (c=mtext_GetChar(self,pos--))!='\n') {
	if(c=='\'' || c=='\"') {
	    if(c==lastquote) {
		quotes=FALSE;
		lastquote=0;
	    } else if (!quotes) {
		quotes=TRUE;
		lastquote=c;
	    }
	}
    }
    return quotes;
}

long backwardcheckword(self,from,to)
struct mtext *self;
long from,to;
{
    NPDict *word;
    struct environment *me;
    char buff[1024];
    long j=from;
    if((!InString(self,from))&&isalnum(mtext_GetChar(self,from))&&(((me=mtext_GetEnvironment(self,from))==self->header.text.rootEnvironment)||(me->data.style==self->keyword_style))) {
	j=backwardcopyword(self,from,to,buff);
	if((word=NPlookup(words,buff))->stng!=NULL) {
	    int l;
	    l=strlen(buff);
	    mtext_ReplaceCharacters(self,j+1,l, word->stng,l);
	    if(environment_Remove(self->header.text.rootEnvironment,j+1,l,environment_Style,FALSE)) mtext_SetModified(self);
	    keyword(self,j+1,l);
	}
    }
    return j;
}

void mtext__BackwardCheckWord(self,from,to)
struct mtext *self;
long from,to;
{
    (void) backwardcheckword(self,from,to);
}

long checkword(self,i,end)
struct mtext *self;
long i,end;
{
    int j;
    NPDict *word;
    char buff[1024];
    i=skipwhitespace(self,i,end);
    j=copyword(self,i,end,buff);
    if((word=NPlookup(words,buff))->stng!=NULL) {
	int l=strlen(word->stng);
	mtext_ReplaceCharacters(self,i,l, word->stng,l);
	keyword(self,i,l);
    }
    return j;
}

void mtext__SetAttributes(self,atts)
struct mtext	*self;
struct attributes   *atts;
{
    super_SetAttributes(self,atts);

    while(atts!=NULL)
    {
	if(strncmp(atts->key,"mtext-",6)==0)
	{
	    char    *key=atts->key+6;

	    if(strcmp(key,"level-indent")==0)
		levelIndent=atoi(atts->value.string);
	    else if(strcmp(key,"cont-indent")==0)
		contIndent=atoi(atts->value.string);
	    else if(strcmp(key,"brace-indent")==0)
		braceIndent=atoi(atts->value.string);
	    else if(strcmp(key,"case-level-indent")==0)
		caseLevelIndent=atoi(atts->value.string);
	    else if(strcmp(key,"use-tabs")==0)
		useTabs=atoi(atts->value.string);
	}    

	atts=atts->next;
    }
}

static void SetupStyles(self)
struct mtext *self;
{
    self->comment_style = stylesheet_Find(self->header.text.styleSheet, "comment");
    self->keyword_style = stylesheet_Find(self->header.text.styleSheet, "keyword"); /* rjd */
    /*  fprintf (stderr,"Calling SetGlobalStyles\n");  rjd */

    mtext_SetGlobalStyle(self, stylesheet_Find(self->header.text.styleSheet, "global"));
}

boolean mtext__InitializeObject(classID, self)
struct classheader *classID;
struct mtext *self;
{
    struct attributes templateAttribute;

    templateAttribute.key = "template";
    templateAttribute.value.string = "mtext";
    templateAttribute.next = NULL;
    mtext_SetAttributes(self, &templateAttribute);
    mtext_SetExportEnvironments(self, FALSE);

    /*   fprintf (stderr,"Calling SetUpStyles\n");  rjd */
    SetupStyles(self);
    return TRUE;
}

void mtext__Clear(self)
struct mtext *self;
{

    super_Clear(self); /* This destroyes all styles in the stylesht. */

    SetupStyles(self);
}

long mtext__Read(self, file, id)
struct mtext *self;
FILE *file;
long id;
{
    long tmpRetValue;

    tmpRetValue = super_Read(self, file, id);
    if (tmpRetValue == dataobject_NOREADERROR) {
	SetupStyles(self);
	mtext_RedoStyles(self);
    }
    return tmpRetValue;
}


/****************************************************************
  *
  *  Style parsing code.
  *
  ****************************************************************/

#ifdef NOTUSED
static long skipstring(self,start)
struct mtext *self;
long start;
{
    long len=mtext_GetLength(self);
    char quote=mtext_GetChar(self,start);
    while((++start<=len)&&(mtext_GetChar(self,start)!=quote));
    return start;
}
#endif /* NOTUSED */

static long comment(self, start)
struct mtext *self;
long start;
{
    int prev, c = 0;
    long end, len = mtext_GetLength(self);
    int comments=1;
    for (end = start; end < len; end++) {
	prev = c;
	c = mtext_GetChar(self, end);
	switch(c) {
	    case '*':
		if(prev=='(') comments++;
		break;
	    case ')':
		if(prev=='*') comments--;
		break;
	    default: ;
	}
	if (comments<1) /* rjd */
	{
	    if (self->comment_style != NULL) mtext_InsertNewStyle(self,start,end-start,self->comment_style,TRUE,TRUE);
	    break;
	}
    }
    return end;
}

/* keyword inserts keyword looks. rjd */
void keyword(self, posn, len)
struct mtext *self;
long posn, len;
{
    if (self->keyword_style != NULL) {
	mtext_InsertNewStyle(self,posn,len,self->keyword_style,FALSE,FALSE);
    }
}
/* returns the length of a *constant* string */
#define cstrlen(str) (sizeof(str)-1)

/* pos should pointer to 2nd from last character-- last character won't be
* matched; strconst *must* be a constant string, so that sizeof works
*/
#define backwardMatch(self,pos,strConst) \
(pos>=cstrlen(strConst) && match(self,pos-(cstrlen(strConst)-1)+1,strConst,cstrlen(strConst)))

#define match(self,pos,str,len) ((pos==0 || !isalnum(mtext_GetChar(self,pos-1))) && !isalnum(mtext_GetChar(self,pos+len)) && domatch(self,pos,str,len))

domatch(self,pos,str,len)
struct mtext *self;
int pos;
char *str;
int len;
{
    while(len>0 && mtext_GetChar(self,pos++)==*str++)
	len--;

    return len==0;
}

static void DoFreeTree(self)
struct nestedmark *self;
{
    nestedmark_FreeTree(self);
}

void mtext__RedoStyles(self)
struct mtext *self;
{
    struct nestedmark *root = (struct nestedmark *)self->header.text.rootEnvironment;
    long posn, len = mtext_GetLength(self);
    int prev = 0, c = '\n', braces = 0, string = NONE;
    /* c is initialize to a newline so the start of the file looks like the start of line. */

    /* Remove the old styles, but leave the root environemt in place. */
    if (root->children) {
	tree23int_Apply(root->children, DoFreeTree);
	tree23int_Destroy(root->children);
	root->children = NULL;
    }
    for (posn = 0; posn < (len=mtext_GetLength(self)); posn++) {
	prev = c;
	posn=skipwhitespace(self,posn,len);
	c = mtext_GetChar(self, posn);
	    switch (c) {
		case '#':
		    /* Is this actually useful for Modula? */
#if 0
		    if (prev == '\n') {
			define = 1;
		    }
#endif /* 0 */
		    break;
		case '\n':
#if 0
		    define = 0;
#endif /* 0 */
		    break;
		case '*':
		    if (prev == '(' && string == NONE) /* rjd */
			/*last_comment = */ posn = comment(self, posn);
		    break;
		case '{':
		    if (string == NONE)
			braces++;
		    break;
		case '}':
		    if (string == NONE)
			braces--;
		    break;
		/* case '(':
		    if (!define && (string == NONE)) {
			    fn_name(self, posn, last_comment);}
		    break;
		case ')':
		    if (!define && string == NONE)
			braces--;
		    break; */
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
		    /* Parse Modula-2 keywords and set proper look rjd */
		default:
		    if(string==NONE) posn=checkword(self,posn,len)-1;
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
struct mtext *self;
long pos;
{
    /* returns true iff the character at pos is quoted (ie. "\"). Takes into account the slash being quoted. (ie "\\"). */

    boolean retval = FALSE;

    pos--;
    while (mtext_GetChar(self, pos) == '\\' && pos > 0) {
	retval = !retval;
	pos--;
    }

    return FALSE;
}

long mtext__ReverseBalance(self, pos)
struct mtext *self;
long pos;
{
    boolean found = FALSE, incomment = FALSE, instring = FALSE, doublestring = FALSE, atleastone = FALSE;
    int thischar, prechar;
    char *parentype;
    struct paren_node *parenstack = NULL/*, *tmp */;
    static char *opens = "({[", *closes = ")}]";


    while ((parenstack != NULL || !atleastone) && (pos > 0)) {
	thischar = mtext_GetChar(self, --pos);
	prechar = pos > 0 ? mtext_GetChar(self, pos - 1) : 0;

	if (incomment) {
	    if(thischar == '*' && prechar == '(') {
		incomment = FALSE;
		if (parenstack == NULL) found = TRUE;
		else pos= pos > 1 ? pos -= 2 : 0;
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
	    else if (thischar == ')' && prechar == '*') {
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

long mtext__Indent(self,mark)
struct mtext *self;
struct mark *mark;
{
    long end,pos=mark_GetPos(mark);
    int c;

    while(pos>=0 && mtext_GetChar(self,--pos)!='\n');

    pos++;

    do{
	pos=mtext_TabAndOptimizeWS(self,pos,indentation(self,pos-1));

	for(end=pos; (c=mtext_GetChar(self,end))==' ' || c=='\t'; end++ );
	if(end>pos)
	    mtext_DeleteCharacters(self,pos,end-pos);

	end=mark_GetEndPos(mark);
	while(pos<end && (c=mtext_GetChar(self,pos++))!=EOF && c!='\n');
    }while(pos<end && c!=EOF);

    return pos;
}

void mtext__ReindentLine(self,pos)
struct mtext *self;
long pos;
{
    long end;
    int c;

    while(pos>0 && mtext_GetChar(self,--pos)!='\n');

    pos++;

    pos=mtext_TabAndOptimizeWS(self,pos,indentation(self,pos-1));

    for(end=pos; (c=mtext_GetChar(self,end))==' ' || c=='\t'; end++);
    if(end>pos)
	mtext_DeleteCharacters(self,pos,end-pos);
}
long SkipComment(self,pos)
struct mtext *self;
long pos;
{
    int count=1;
    char ch=0,prev;
    if(pos>=0)
	while((count>0)&&(++pos>0)) {
	    prev=ch;
	    if(((ch=mtext_GetChar(self,pos))==')')&& (prev=='*')) count--;
	    else if ((ch=='*')&&(prev=='(')) count++;
	}
    return pos+1;
}
long backwardSkipComment(self,pos)
struct mtext *self;
long pos;
{
    int count=1;
    char ch=0,prev;
    if(pos>=0)
	while((count>0)&&(--pos>0)) {
	    prev=ch;
	    if(((ch=mtext_GetChar(self,pos))=='(')&& (prev=='*')) count--;
	    else if ((ch=='*')&&(prev==')')) count++;
	}
    return pos-1;
}

void mtext__StyleLine(self,pos)
struct mtext *self;
long pos;
{
    long i,start;
    int string=NONE;
    char ch=0,prev;
    start=i=pos;
    while((--start>=0)&&(ch=mtext_GetChar(self,start))!='\n');
    while((--i>=start)&&(mtext_GetChar(self,i))!='\n') {
	prev=ch;
	ch=mtext_GetChar(self,i);
	switch(ch) {
	    case '*':
		if (prev== ')' && (string==NONE)) i=backwardSkipComment(self,i)+1;
		break;
	    case '\'':
	    case '\"':
		if(string==NONE) string=ch;
		else if(string==ch) string=NONE;
	    break;
	default:
	    if(isalnum(ch)&& string==NONE) i=backwardcheckword(self,i,start)+1;
	}
    }
	/* if(i!=pos) {
	if(environment_Remove(self->header.text.rootEnvironment,i, pos-i,environment_Style,TRUE)) mtext_SetModified(self);
	while(i<pos) i=checkword(self,i,pos);
	mtext_RegionModified(self,i,pos-i);
    } */
    mtext__ReindentLine(self,pos);
}

/* assumes that  pos points to 1 BEFORE ending ')' */



/* beg and end must be constants (for switch) */
/* assumes ending delimiter AFTER pos */
long backwardSkipDelimited(self,pos,beg,end)
struct mtext *self;
long pos;
char beg, end;
{
    int level=1;
    while(pos>=0 && level>0){
	int c;
	switch(c=mtext_GetChar(self,pos--)){
	    case '\'':
	    case '"':
		pos=backwardSkipString(self,pos,c);
		break;
	    case ')':
		if ((mtext_GetChar(self,pos)) == '*')
		    pos=backwardSkipComment(self,pos);
		else if (c==end)
		    level++;
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
long skipjunk(self,pos)
struct mtext *self;
long pos;
{
    int commentlevel=0,stringtype=NONE;
    boolean done=FALSE;
    long len=mtext_GetLength(self);
    while((!done)&&(pos<len)) {
	if(pos<len) {
	    register c=mtext_GetChar(self,pos++);
	    register c2=mtext_GetChar(self,pos);
	    switch(c) {
		case '(':
		    if (c2=='*' && stringtype==NONE) commentlevel++;
		    break;
		case '*':
		    if (c2=')' && stringtype==NONE) commentlevel--;
		    break;
		case '\'':
		case '\"':
		    if(commentlevel==0) {
			if(stringtype==NONE) stringtype=c;
			else if(stringtype==c) stringtype=NONE;
		    }
		    break;
		case ' ':
		case '\t':
		case '\n':
		    break;
		default:
		    if (commentlevel==0 && stringtype==NONE) done=TRUE;
	    }
	}
    }
    return pos-1;
}
long backwardSkipJunk(self,i)
struct mtext *self;
long i;
{
    char ch=0;
    boolean done=FALSE;
    while(!done && (i>=0)) {
	ch=mtext_GetChar(self,i);
	switch(ch) {
	    case '\'':
	    case '\"':
		i=backwardSkipString(self,i-1,ch);
		break;
	    case ')':
		if(i<1 || mtext_GetChar(self,i-1)!='*') done=TRUE;
		else i=backwardSkipComment(self,i-1)+1;
	    case '\n':
	    case '\t':
	    case ' ':
		break;
	    default:
		done=TRUE;
	}
	ch=mtext_GetChar(self,i--);
    }
    return i+1;
}

int matchend(self,pos)
struct mtext *self;
long pos;
{
    int count=1;
    while((count>0)&&(--pos>=0)) {
	pos=backwardSkipJunk(self,pos);
	if(pos>=0) {
	    if(isalnum(mtext_GetChar(self,pos))) {
		NPDict *word2;
		char buff[256];
		pos=backwardcopyword(self,pos,0,buff)+1;
		word2=NPlookup(words,buff);
		if (word2->val&16) count++;
		else if(word2->val&1) count--;
	    }
	}
    }
    return currentIndent(self,pos);
}
int matchelse(self,pos)
struct mtext *self;
long pos;
{
    NPDict *word2 = NULL;
    char buff[256];
    int count=1;
    while((count>0)&&(--pos>=0)) {
	pos=backwardSkipJunk(self,pos);
	if(pos>=0) {
	    if(isalnum(mtext_GetChar(self,pos))) {
		pos=backwardcopyword(self,pos,0,buff)+1;
		word2=NPlookup(words,buff);
		if (word2->val&16) count++;
		else if(word2->val&1) count--;
	    }
	}
    }
    if(word2->val&64) return currentIndent(self,pos)+levelIndent;
    return currentIndent(self,pos);
}
long backline(self,pos)
struct mtext *self;
long pos;
{
    while((pos>=0)&&(mtext_GetChar(self,pos--)!='\n'));
    return pos;
}

int backLevel(self,pos)
struct mtext *self;
long pos;
{
    NPDict *word2;
    char buff[256];
    while(--pos>=0) {
	pos=backwardSkipJunk(self,pos);
	if(pos>=0) {
	    if(isalnum(mtext_GetChar(self,pos))) {
		pos=backwardcopyword(self,pos,0,buff)+1;
		word2=NPlookup(words,buff);
		if (word2->val&8) return currentIndent(self,pos);
	    }
	}
    }
    return 0;
}

int indentation(self,pos)
struct mtext *self;
long pos;
{
    char buff[256],c;
    NPDict *word;
    long i,j;
    int indent=0,oldpos;
    if(mtext_GetStyle(self,pos)==self->comment_style)
	return currentColumn(self,backwardSkipComment(self,pos)); 
    (void) copyword(self,pos+1,mtext_GetLength(self),buff);
    word=NPlookup(words,buff);
    if (word->val&32) return matchend(self,pos);
    if(word->val&8) return backLevel(self,pos);
    if(word->val&4) return matchelse(self,pos);
    j=backwardSkipJunk(self,pos);
    i=backline(self,j);
    switch((c=mtext_GetChar(self,j))) {
	case ';':
	    i=copyword(self,i+2,mtext_GetLength(self),buff);
	    word=NPlookup(words,buff);
	    if(word->val&128) {
		while(i<=pos && isalnum(mtext_GetChar(self,i++)));
		oldpos=i;
		while(i<=pos && (c=mtext_GetChar(self,i++))!=')' &&(c!='\n'));
		if(c=='\n') return currentColumn(self,oldpos);
	    }
	    return currentIndent(self,j);
	case '|':
	    return matchend(self,pos)+levelIndent;
	case ':':
	    return currentIndent(self,j)+levelIndent;
	default:
	    i=i+2;
	    while(i<j) {
		i=skipjunk(self,i);
		if(i<j) {
		    i=copyword(self,i, mtext_GetLength(self),buff);
		    word=NPlookup(words,buff);
		    if((word->val&1)||(word->val&4)) indent=indent+levelIndent;
		    if(word->val&2)
			indent=indent-levelIndent;
			    
		}
	    }
	    if (currentIndent(self,j)+indent<0) indent=0;
	    return currentIndent(self,j)+indent;
    }
}

int currentIndent(self,pos)
struct mtext *self;
long pos;
{
    int c,ind=0;

    while(pos>=0 && (c=mtext_GetChar(self,pos))!='\n')
	pos--;

    while(isspace(c=mtext_GetChar(self,pos++)))
	switch(c){
	    case ' ':
		ind++;
		break;
	    case '\t':
		ind=(ind+8)&~7;
		break;
	}
    /*    fprintf(stdout,"ind=%d\n",ind);
      fflush (stdout);
      */    return ind;
}


int currentColumn(self,pos)
struct mtext *self;
long pos;
{
    int ind=0,oldPos=pos;

    while(pos>=0 && mtext_GetChar(self,pos)!='\n')
	pos--;

    while(pos<oldPos)
	if(mtext_GetChar(self,pos++)=='\t')
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
mtext_DeleteCharacters(self,oldpos,pos-oldpos);\
pos=oldpos;\
if(useTabs){\
int nc;\
while(col>=(nc=(oldcol+8)&~7)){\
oldcol=nc;\
mtext_InsertCharacters(self,pos++,"\t",1);\
}\
}else{\
int count=(col-oldcol)/8;\
while(count--){\
mtext_InsertCharacters(self,pos,spaces,8);\
oldcol+=8;\
pos+=8;\
}\
}\
mtext_InsertCharacters(self,pos,spaces,col-oldcol);\
pos+=(col-oldcol);\
}

long mtext__TabAndOptimizeWS(self,pos,inc)
struct mtext    *self;
long	pos;
int	inc;
{
    int     home=0, oldPos, col=0, oldCol=0, target;

    while(--pos>0 && mtext_GetChar(self,pos)!='\n')
	home++;

    oldPos= ++pos;
    while(home--){
	switch(mtext_GetChar(self,pos)){
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
	char	c=mtext_GetChar(self,pos);

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
