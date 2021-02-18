/* ********************************************************************** *\
  *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
  *        For full copyright information see:'andrew/config/COPYRITE'     *
  \* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/rmtext/RCS/rmtext.c,v 1.7 1991/09/12 16:54:02 bobg Exp $ */
/* $ACIS:rmtext.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/rmtext/RCS/rmtext.c,v $ */

/* rmtext, a Modula-2 mode for BE2. */

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
#include <compresv.ih>    /*RSKadd*/
#include <rmtext.eh>

static char rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/rmtext/RCS/rmtext.c,v 1.7 1991/09/12 16:54:02 bobg Exp $";
static char CopyRight[] = " Copyright (c) IBM Corp. 1989";

/*
  * Parsing (NPDict, NPLookup) by Chris Newman, 1988
  */

#define	NPTOUPPER(c) (islower(c) ? toupper(c) : (c))

typedef struct Dict {
    char    *stng;
    short   val;
    struct Dict *next;
} Dict;

# define TABLESIZE 113
struct Dict *words[TABLESIZE];

#define new(type) (type *) malloc(sizeof(type))

#define NONE 0
#define SINGLE '\''
#define DOUBLE '\"'

/*
  * Varables used to control the indenting style.
  */

boolean useTabs = FALSE;
int levelIndent=4,
contIndent=2;


void keyword();
void predefined();/*RSKadd*/
void function();/*RSKadd*/

boolean is_whitespace(ch)
char ch;
{
    return((ch==' ')||(ch=='\t'));
}

long skipwhitespace(ct,pos,end)
struct rmtext *ct;
long pos,end;
{
    while((pos<end)&&is_whitespace(rmtext_GetChar(ct,pos))) pos++;
    return pos;
}

boolean is_alnum(ch)    /*RSKadd*/
char ch;
{
    return isalnum(ch)||ch=='_'||ch=='|';
}

boolean rmtext__IsAlnum(classID,ch)    /*RSKadd*/
struct classheader *classID;
char ch;
{
    return is_alnum(ch);
}

void makeupper(str)
char *str;
{
    while(*str!=0) {
	if(islower(*str)) *str=toupper(*str);
	str++;
    }
}

void makelower(str)    /*RSKadd*/
char *str;
{
    while(*str!=0) {
	if(isupper(*str)) *str=tolower(*str);
	str++;
    }
}

long copyword(ct,pos,end,buffer)
struct rmtext *ct;
long pos,end;
char buffer[];
{
    char ch;
    int count=0;
    buffer[0]=0;
    pos=skipwhitespace(ct,pos,end);
    while((count<=255)&&(pos<end)&&is_alnum(ch=rmtext_GetChar(ct,pos++))) 
	buffer[count++]=(ch);/*RSKmod: remove NPTOUPPER*/
    buffer[count]=0;
    return pos;
}

long backwardcopyword(ct,from,to,buffer)
struct rmtext *ct;
long from,to;
char buffer[];
{
    int count=0,i=0;
    long j;
    char ch;    
    boolean lowerfound=FALSE, upperfound=FALSE;
    buffer[0]=0;
    while((from-i>=to)&&is_alnum(ch=rmtext_GetChar(ct,from-i)))
    {
	i++;
	if (islower(ch)) lowerfound = TRUE;
	else if (isupper(ch)) upperfound = TRUE;
    }

    j=from-i;
    if(from-i>=to-1) for(;i>0 && count<255;i--,count++)
	buffer[count]=(rmtext_GetChar(ct,from-i+1));/*RSKmod*/
    if (lowerfound && upperfound) count = 0;
    buffer[count]=0;
    if (rmtext_GetForceUpper(ct)) makeupper(buffer);/*RSKmod*/
    return j;
}

unsigned hash( word )    /*DHGadd*/
char *word;
{
    unsigned hashval;
    int len = strlen(word);
    hashval = *word + word[len-1] * len;
    return hashval % TABLESIZE;
}

void insert( hashTable, word )    /*DHGadd*/
Dict *hashTable[];
Dict *word;
{
    Dict *bucket;
    int hashval;
    if ( word != NULL )
    {
	hashval = hash( word->stng );
	bucket = ( Dict * ) malloc ( sizeof( Dict ) );
	bucket->stng = ( char * ) malloc ( strlen( word->stng ) +1 );
	strcpy( bucket->stng, word->stng );
	bucket->val = word->val;
	bucket->next = hashTable[ hashval ];
	hashTable[ hashval ] = bucket;
    }
}

void buildTable( hashTable )    /*DHGadd*/
Dict *hashTable[];
{
    static Dict words[]={
	/* NOTE: The following list of Modula-2 Reserved words and Predefined Procedures were taken from "Programming in Modula-2" by Niklaus Wirth, 2nd edition plus min and max from the 3rd edition.  The Reserved words are in uppercase and the Predefined Procedures are in lowercase. RLQmod */

	{"abs",0},
	{"adr",0},
	{"AND",0},
	{"ARRAY",0},
	{"BEGIN",9},
	{"bitset",0},
	{"boolean",0},
	{"BY",0},
	{"cap",0},
	{"cardinal",0},
	{"CASE",1},/*RSKmod: change 65 to 1*/
	{"char",0},
	{"chr",0},
	{"CONST",9},
	{"dec",0},
	{"DEFINITION",0},
	{"dispose",0},
	{"DIV",0},/*RSKadd*/
	{"DO",0},
	{"ELSE",4},
	{"ELSIF",36},
	{"END",50},
	{"excl",0},
	{"EXIT",0},/*RSKadd*/
	{"EXPORT",0},
	{"false",0},
	{"float",0},
	{"FOR",1},
	{"FROM",0},
	{"halt",0},
	{"high",0},
	{"IF",1},
	{"IMPLEMENTATION",0},
	{"IMPORT",0},
	{"IN",0},/*RSKadd*/
	{"inc",0},
	{"incl",0},
	{"integer",0},
	{"INTERFACE",0},
	{"longint",0},/*RSKadd*/
	{"longreal",0},/*RSKadd*/
	{"LOOP",1},/*RSKadd*/
	{"max",0},/*RSKadd*/
	{"min",0},/*RSKadd*/
	{"MOD",0},
	{"MODULE",8},
	{"new",0},
	{"nil",0},/*RSKadd*/
	{"NOT",0},
	{"odd",0},
	{"OF",0},
	{"OR",0},
	{"ord",0},
	{"POINTER",0},
	{"proc",0},
	{"PROCEDURE",136},
	{"QUALIFIED",0},
	{"real",0},
	{"REF",0},/*RLQ addition per Orgass & Seurer */
	{"RECORD",1},
	{"REPEAT",1},
	{"RETURN",0},
	{"SET",0},
	{"size",0},
	{"TO",0},
	{"THEN",0},
	{"true",0},
	{"trunc",0},
	{"tsize",0},
	{"TYPE",9},
	{"UNTIL",50},
	{"val",0},
	{"VAR",9},
	{"WHILE",1},
	{"WITH",1},
	{"|",4},/*this is to align |'s with case statements; note that [Tab] key should be pressed after [|] so that cases line up properly*/
	{NULL,0}
    }, *wordPtr;
    int i;
    wordPtr = words;
    for ( i = 0; i < TABLESIZE; i++ )
	hashTable[ i ] = NULL;
    while ( wordPtr->stng != NULL ){
	insert( hashTable, wordPtr++ );
    }
}

Dict *lookup(hashTable, word)    /*DHGadd*/
Dict *hashTable[];
char *word;
{
    Dict *bucket;
    static Dict miss = {NULL,0};
    if ( hashTable != NULL ) 
    {
	bucket = hashTable[ hash( word ) ];

	while ( ( bucket != NULL ) && ( (strcmp( bucket->stng, word ) ) != 0 ) )
	    bucket = bucket->next;

	if ( bucket != NULL ) 
	    return bucket;
	else 
	    return &miss; 
    }
    else
	return &miss;
}

Dict *ULlookup(hashTable,word)    /*RSKadd*/
Dict *hashTable[];
char *word;
{
    Dict *bucket;
    static Dict miss = {NULL,0};
    char wordup[256],wordlo[256];
    strncpy(wordup, word, 256); strncpy(wordlo, word, 256);
    makeupper(wordup); makelower(wordlo);
    if (strcmp(word, wordup)!=0)
	return &miss;
    else
	if ((bucket=lookup(hashTable,wordup))->stng!=NULL)
	    return bucket;
	else
	    return lookup(hashTable,wordlo);
}

struct environment *rmtext__GetEnvironment(self,pos)
struct rmtext *self;
long pos;
{
    struct environment *me;
    if(me=self->header.text.rootEnvironment) me=environment_GetEnclosing(me,pos);
    return me;
}

struct style *rmtext__GetStyle(self,pos)
struct rmtext *self;
long pos;
{
    struct environment *me;
    if(me=rmtext_GetEnvironment(self,pos)) return me->data.style;
    return NULL;
}

void rmtext__InsertNewStyle(self,pos,len,style,begflag,endflag)
struct rmtext *self;
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
struct rmtext *self;
long pos;
char delim;
{
    while((pos>=0)&& rmtext_GetChar(self,pos--)!=delim);
    return pos;
}
boolean InString(self,pos)
struct rmtext *self;
long pos;
{
    long backwardSkipComment();
    char c,lastquote=0;
    boolean quotes=FALSE;
    while((pos>=0)&& (c=rmtext_GetChar(self,pos--))!='\n') {
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
struct rmtext *self;
long from,to;
{
    Dict *word;
    struct environment *me;
    char buff[1024];
    long j=from;
    if((!InString(self,from))&&is_alnum(rmtext_GetChar(self,from))&&(((me=rmtext_GetEnvironment(self,from))==self->header.text.rootEnvironment)||(me->data.style==self->keyword_style))) {
	j=backwardcopyword(self,from,to,buff);
	if((word=ULlookup(words,buff))->stng!=NULL) {
	    int l;
	    l=strlen(buff);
	    rmtext_ReplaceCharacters(self,j+1,l, buff,l);
	    if(environment_Remove(self->header.text.rootEnvironment, j+1,l, environment_Style,FALSE)) rmtext_SetModified(self);
	    if (islower(*(word->stng))) predefined(self,j+1,l);  else /*RSKmod*/
		keyword(self,j+1,l);
	}
    }
    return j;
}

void rmtext__BackwardCheckWord(self,from,to)
struct rmtext *self;
long from,to;
{
    (void) backwardcheckword(self,from,to);
}

long checkword(self,i,end)
struct rmtext *self;
long i,end;
{
    long j;
    Dict *word;
    char buff[1024];
    i=skipwhitespace(self,i,end);
    j=copyword(self,i,end,buff);
    if((word=ULlookup(words,buff))->stng!=NULL) {
	int l=strlen(word->stng);
	/*rmtext_ReplaceCharacters(self,i,l, buff,l);*//*RSKremove*/
	if (islower(*(word->stng))) predefined(self,i,l);  else /*RSKmod*/
	    keyword(self,i,l);
	/*RSKadd:*/
	if (word->val&128) {
	    i=skipwhitespace(self,j-1,end);
	    j=copyword(self,i,end,buff);/*get procedure name*/
	    if ((word=ULlookup(words,buff))->stng==NULL) { /*legit proc name*/
		l=strlen(buff);
		if (l>0)
		    function(self,i,l);
	    }
	    else { /*not a proc name!! make appropriate style*/
		l=strlen(word->stng);
		/*rmtext_ReplaceCharacters(self,i,l, buff,l);*/
		if (islower(*(word->stng))) predefined(self,i,l);  else
		    keyword(self,i,l);
	    }
	}
    }
    return j;
}

void rmtext__SetAttributes(self,atts)
struct rmtext	*self;
struct attributes   *atts;
{
    static int init=0;/*DHG*/
    super_SetAttributes(self,atts);

    if (!init) rmtext_SetForceUpper( self, FALSE );/*DHG addition: initialize Force Upper option Off */
    while(atts!=NULL)
    {
	if(strncmp(atts->key,"rmtext-",6)==0)
	{
	    char    *key=atts->key+6;

	    if(strcmp(key,"level-indent")==0)
		levelIndent=atoi(atts->value.string);
	    else if(strcmp(key,"cont-indent")==0)
		contIndent=atoi(atts->value.string);
	    else if(strcmp(key,"use-tabs")==0)
		useTabs=atoi(atts->value.string);
	    else if(strcmp(key,"force-upper")==0&&init<1)    /*D43K addition*/
	    {
		rmtext_SetForceUpper(self,atoi(atts->value.string));/*DHG addition*/
		init=1;
	    }
	}    
	atts=atts->next;
    }
    if (!init) init = -1;
}

static void SetupStyles(self)
struct rmtext *self;
{
    self->comment_style = stylesheet_Find(self->header.text.styleSheet, "comment");
    self->keyword_style = stylesheet_Find(self->header.text.styleSheet, "keyword"); /* rjd */
    self->predefined_style = stylesheet_Find(self->header.text.styleSheet, "predefined"); /*RSKadd*/
    self->function_style = stylesheet_Find(self->header.text.styleSheet, "function"); /*RSKadd*/
    /*  fprintf (stderr,"Calling SetGlobalStyles\n");  rjd */

    rmtext_SetGlobalStyle(self, stylesheet_Find(self->header.text.styleSheet, "global"));
    buildTable(words);/*DHGadd*/
}

boolean rmtext__InitializeObject(classID, self)
struct classheader *classID;
struct rmtext *self;
{
    struct attributes templateAttribute;

    templateAttribute.key = "template";
    templateAttribute.value.string = "rmtext";
    templateAttribute.next = NULL;
    rmtext_SetAttributes(self, &templateAttribute);
    rmtext_SetExportEnvironments(self, FALSE);

    /*   fprintf (stderr,"Calling SetUpStyles\n");  rjd */
    SetupStyles(self);
    return TRUE;
}

void rmtext__Clear(self)
struct rmtext *self;
{

    super_Clear(self); /* This destroyes all styles in the stylesht. */

    SetupStyles(self);
}

long rmtext__Read(self, file, id)
struct rmtext *self;
FILE *file;
long id;
{
    long tmpRetValue;

    tmpRetValue = super_Read(self, file, id);
    if (tmpRetValue == dataobject_NOREADERROR) {
	SetupStyles(self);
	rmtext_RedoStyles(self);
    }
    return tmpRetValue;
}

long rmtext__Write(self, file, writeID, level)    /*RSKadd*/
struct rmtext *self;
FILE *file;
long writeID;
int level;
{
    long temp;
    compressv_HideBoxes();
    temp=super_Write(self,file,writeID,level);
    compressv_UnhideBoxes();
    return temp;
}


/****************************************************************
  *
  *  Style parsing code.
  *
  ****************************************************************/

#ifdef NOTUSED
static long skipstring(self,start)
struct rmtext *self;
long start;
{
    long len=rmtext_GetLength(self);
    char quote=rmtext_GetChar(self,start);
    while((++start<=len)&&(rmtext_GetChar(self,start)!=quote));
    return start;
}
#endif /* NOTUSED */

static long comment(self, start)
struct rmtext *self;
long start;
{
    int prev, c = 0;
    long end, len = rmtext_GetLength(self);
    int comments=1;
    for (end = start; end < len; end++) {
	prev = c;
	c = rmtext_GetChar(self, end);
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
	    if (self->comment_style != NULL) rmtext_InsertNewStyle(self,start,end-start,self->comment_style,TRUE,TRUE);
	    break;
	}
    }
    return end;
}

/* keyword inserts keyword looks. rjd */
void keyword(self, posn, len)
struct rmtext *self;
long posn, len;
{
    if (self->keyword_style != NULL) {
	rmtext_InsertNewStyle(self,posn,len,self->keyword_style,FALSE,FALSE);
    }
}
/*RSKmod: predefined does same thing as keyword, but italic, too*/
void predefined(self, posn, len)
struct rmtext *self;
long posn, len;
{
    if (self->predefined_style != NULL) {
	rmtext_InsertNewStyle(self,posn,len,self->predefined_style,FALSE,FALSE);
    }
}
/*RSKmod: function wraps a function style around the procedure name*/
void function(self, posn, len)
struct rmtext *self;
long posn, len;
{
    if (self->function_style != NULL) {
	rmtext_InsertNewStyle(self,posn,len,self->function_style,FALSE,FALSE);
    }
}
/* returns the length of a *constant* string */
#define cstrlen(str) (sizeof(str)-1)

/* pos should pointer to 2nd from last character-- last character won't be
* matched; strconst *must* be a constant string, so that sizeof works
*/
#define backwardMatch(self,pos,strConst) \
(pos>=cstrlen(strConst) && match(self,pos-(cstrlen(strConst)-1)+1,strConst,cstrlen(strConst)))

#define match(self,pos,str,len) ((pos==0 || !is_alnum(rmtext_GetChar(self,pos-1))) && !is_alnum(rmtext_GetChar(self,pos+len)) && domatch(self,pos,str,len))

domatch(self,pos,str,len)
struct rmtext *self;
long pos;
char *str;
int len;
{
    while(len>0 && rmtext_GetChar(self,pos++)==*str++)
	len--;

    return len==0;
}

static void DoFreeTree(self)
struct nestedmark *self;
{
    nestedmark_FreeTree(self);
}

void rmtext__RedoStyles(self)
struct rmtext *self;
{
    struct nestedmark *root = (struct nestedmark *)self->header.text.rootEnvironment;
    long posn, origpos, len = rmtext_GetLength(self);
    int prev = 0, c = '\n', braces = 0, string = NONE;
    /* c is initialize to a newline so the start of the file looks like the start of line. */
    /* Remove the old styles, but leave the root environemt in place. */
    if (root->children) {
	tree23int_Apply(root->children, DoFreeTree);
	tree23int_Destroy(root->children);
	root->children = NULL;
    }
    for (posn = 0; posn < (len/*=rmtext_GetLength(self)*/); posn++) /*RSKmod*/
    {
	prev = c;
	posn=skipwhitespace(self,posn,len);
	c = rmtext_GetChar(self, posn);
	switch (c) {
	    case '\n':
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
		if(string==NONE)
		{
		    origpos=posn; /*RSKadd*/
		    posn = checkword( self, posn, len )-1;
		    if (posn<origpos+2) /*RSKadd*/
			posn=origpos;
		    else
			posn--;
		}
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
struct rmtext *self;
long pos;
{
    /* returns true iff the character at pos is quoted (ie. "\"). Takes into account the slash being quoted. (ie "\\"). */

    boolean retval = FALSE;

    pos--;
    while (rmtext_GetChar(self, pos) == '\\' && pos > 0) {
	retval = !retval;
	pos--;
    }

    return FALSE;
}

long rmtext__ReverseBalance(self, pos)
struct rmtext *self;
long pos;
{
    boolean found = FALSE, /*RSKmod remove "incomment = FALSE,"*/ instring = FALSE, doublestring = FALSE, atleastone = FALSE;
    int incomment=0;/*RSKadd*/
    int thischar, prechar;
    char *parentype;
    struct paren_node *parenstack = NULL/*, *tmp */;
    static char *opens = "({[", *closes = ")}]";


    while ((parenstack != NULL || !atleastone) && (pos > 0)) {
	thischar = rmtext_GetChar(self, --pos);
	prechar = pos > 0 ? rmtext_GetChar(self, pos - 1) : 0;

	if (incomment) {
	    if(thischar == '*' && prechar == '(') {
		incomment--;/*RSKmod*/
		if (parenstack == NULL) found = TRUE;
		else pos= pos > 1 ? pos -= 2 : 0;
	    }
	    else if (thischar==')' && prechar=='*') /*RSKmod*/
		incomment++;/*RSKmod*/
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
		incomment++;/*RSKmod*/
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

long rmtext__Indent(self,mark)
struct rmtext *self;
struct mark *mark;
{
    long end,pos=mark_GetPos(mark);
    int c;

    while(pos>=0 && rmtext_GetChar(self,--pos)!='\n');

    pos++;

    do{
	pos=rmtext_TabAndOptimizeWS(self,pos,indentation(self,pos-1));

	for(end=pos; (c=rmtext_GetChar(self,end))==' ' || c=='\t'; end++ );
	if(end>pos)
	    rmtext_DeleteCharacters(self,pos,end-pos);

	end=mark_GetEndPos(mark);
	while(pos<end && (c=rmtext_GetChar(self,pos++))!=EOF && c!='\n');
    }while(pos<end && c!=EOF);

    return pos;
}

void rmtext__ReindentLine(self,pos)
struct rmtext *self;
long pos;
{
    long end;
    int c;

    while(pos>0 && rmtext_GetChar(self,--pos)!='\n');

    pos++;

    pos=rmtext_TabAndOptimizeWS(self,pos,indentation(self,pos-1));

    for(end=pos; (c=rmtext_GetChar(self,end))==' ' || c=='\t'; end++);
    if(end>pos)
	rmtext_DeleteCharacters(self,pos,end-pos);
}
long SkipComment(self,pos)
struct rmtext *self;
long pos;
{
    int count=1;
    char ch=0,prev;
    long len=rmtext_GetLength(self);/*RSKadd*/
    if(pos>=0)
	while((count>0)&&(++pos<len)) {/*RSKmod changed >0 to <len*/
	    prev=ch;
	    if(((ch=rmtext_GetChar(self,pos))==')')&& (prev=='*')) count--;
	    else if ((ch=='*')&&(prev=='(')) count++;
	}
    return pos+1;
}
long backwardSkipComment(self,pos)
struct rmtext *self;
long pos;
{
    int count=1;
    char ch=0,prev;
    if(pos>=0)
	while((count>0)&&(--pos>0)) {
	    prev=ch;
	    if(((ch=rmtext_GetChar(self,pos))=='(')&& (prev=='*')) count--;
	    else if ((ch=='*')&&(prev==')')) count++;
	}
    return pos-1;
}

void rmtext__StyleLine(self,pos)
struct rmtext *self;
long pos;
{
    long i,start;
    int string=NONE;
    char ch=0,prev;
    start=i=pos;
    while((--start>=0)&&(ch=rmtext_GetChar(self,start))!='\n');
    while((--i>=start)&&(rmtext_GetChar(self,i))!='\n') {
	prev=ch;
	ch=rmtext_GetChar(self,i);
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
		if(is_alnum(ch)&& string==NONE) i=backwardcheckword(self,i,start)+1;
	}
    }
    /* if(i!=pos) {
	if(environment_Remove(self->header.text.rootEnvironment,i, pos-i,environment_Style,TRUE)) rmtext_SetModified(self);
	while(i<pos) i=checkword(self,i,pos);
	rmtext_RegionModified(self,i,pos-i);
    } */
    rmtext__ReindentLine(self,pos);
}

/* assumes that  pos points to 1 BEFORE ending ')' */



/* beg and end must be constants (for switch) */
/* assumes ending delimiter AFTER pos */
long backwardSkipDelimited(self,pos,beg,end)
struct rmtext *self;
long pos;
char beg, end;
{
    int level=1;
    while(pos>=0 && level>0){
	int c;
	switch(c=rmtext_GetChar(self,pos--)){
	    case '\'':
	    case '"':
		pos=backwardSkipString(self,pos,c);
		break;
	    case ')':
		if ((rmtext_GetChar(self,pos)) == '*')
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
struct rmtext *self;
long pos;
{
    int commentlevel=0,stringtype=NONE;
    boolean done=FALSE;
    long len=rmtext_GetLength(self);
    while((!done)&&(pos<len)) {
	if(pos<len) {
	    register c=rmtext_GetChar(self,pos++);
	    register c2=rmtext_GetChar(self,pos);
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
struct rmtext *self;
long i;
{
    char ch=0;
    boolean done=FALSE;
    while(!done && (i>=0)) {
	ch=rmtext_GetChar(self,i);
	switch(ch) {
	    case '\'':
	    case '\"':
		i=backwardSkipString(self,i-1,ch);
		break;
	    case ')':
		if(i<1 || rmtext_GetChar(self,i-1)!='*') done=TRUE;
		else i=backwardSkipComment(self,i-1)+1;
	    case '\n':
	    case '\t':
	    case ' ':
		break;
	    default:
		done=TRUE;
	}
	ch=rmtext_GetChar(self,i--);
    }
    return i+1;
}

long matchend(self,pos)
struct rmtext *self;
long pos;
{
    int count=1;
    while((count>0)&&(--pos>=0)) {
	pos=backwardSkipJunk(self,pos);
	if(pos>=0) {
	    if(is_alnum(rmtext_GetChar(self,pos))) {
		Dict *word2;
		char buff[256];
		pos=backwardcopyword(self,pos,0,buff)+1;
		word2=ULlookup(words,buff);
		if (word2->val&16) count++;
		else if(word2->val&1) count--;
	    }
	}
    }
    return currentIndent(self,pos);
}
long matchelse(self,pos)
struct rmtext *self;
long pos;
{
    Dict *word2 = NULL;
    char buff[256];
    int count=1;
    while((count>0)&&(--pos>=0)) {
	pos=backwardSkipJunk(self,pos);
	if(pos>=0) {
	    if(is_alnum(rmtext_GetChar(self,pos))) {
		pos=backwardcopyword(self,pos,0,buff)+1;
		word2=ULlookup(words,buff);
		if (word2->val&16) count++;
		else if(word2->val&1) count--;
	    }
	}
    }
    if(word2->val&64) return currentIndent(self,pos)+levelIndent;
    return currentIndent(self,pos);
}

long backline(self,pos)
struct rmtext *self;
long pos;
{
    while((pos>=0)&&(rmtext_GetChar(self,pos--)!='\n'));
    return pos;
}

long backLevel(self,pos)
struct rmtext *self;
long pos;
{
    Dict *word2;
    char buff[256];
    while(--pos>=0) {
	pos=backwardSkipJunk(self,pos);
	if(pos>=0) {
	    if(is_alnum(rmtext_GetChar(self,pos))) {
		pos=backwardcopyword(self,pos,0,buff)+1;
		word2=ULlookup(words,buff);
		if (word2->val&8) return currentIndent(self,pos);
	    }
	}
    }
    return 0;
}

char backwardFirstnonwhitespace(self,pos)   /*RSKadd*/
struct rmtext *self;
long pos;
{
    char ch;
    while (pos>0)
	if (!is_whitespace(ch=rmtext_GetChar(self,pos--)))
	    return ch;
}

/* indentation returns the desired indentation of the line following pos.  pos is assumed to point to the <cr> that precedes the line */
int indentation(self,pos)
struct rmtext *self;
long pos;
{
    char buff[256];
    Dict *word;
    long i,j;
    int indent=0;
    long oldj,temppos;/*RSKadd*/
    if(rmtext_GetStyle(self,pos)==self->comment_style)
	return currentIndent(self,pos); /*RSKmod*/
    temppos=skipwhitespace(self,pos+1,rmtext_GetLength(self));
    if (rmtext_GetChar(self, temppos)=='(' && rmtext_GetChar(self, temppos+1)=='*')
	return currentIndent(self,pos);
    (void) copyword(self,pos+1,rmtext_GetLength(self),buff);
    word=ULlookup(words,buff);
    if (word->val&32) return matchend(self,pos);
    if(word->val&8) return backLevel(self,pos);
    if(word->val&4) return matchelse(self,pos);
    j=backwardSkipJunk(self,pos);
    i=backline(self,j);
    if (rmtext_GetChar(self,j)=='|')/*RSKmod*/
	return matchend(self,pos)+levelIndent;
    i=i+((i>0)?2:-i);/*RSKmod*/
    /*RSKmod:*/
    oldj=j;
    while (j>i) {
	j=backwardcopyword(self,j,i,buff);/*RSKmod: j-1?*/
	word=ULlookup(words,buff);
	if((word->val&1)||(word->val&4)||(word->val&64 && backwardFirstnonwhitespace(self,j)=='\n'))
	    indent=indent+levelIndent;
	if(word->val&2)
	    return matchend(self,j)+indent;/*RSKmod: j-1?*/
	j=backwardSkipJunk(self,j-1);
    }
    if (indent>levelIndent) indent=levelIndent;/*RSKadd*/
    if ((i=currentIndent(self,oldj))+indent<0) indent = -i;
    return i+indent;
}

int currentIndent(self,pos)
struct rmtext *self;
long pos;
{
    int c,ind=0;

    while(pos>=0 && (c=rmtext_GetChar(self,pos))!='\n')
	pos--;

    while(is_whitespace(c=rmtext_GetChar(self,++pos))) /*RSKmod*/
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
struct rmtext *self;
long pos;
{
    int ind=0;
    long oldPos=pos;

    while(pos>=0 && rmtext_GetChar(self,pos)!='\n')
	pos--;

    while(pos<oldPos)
	if(rmtext_GetChar(self,pos++)=='\t')
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
rmtext_DeleteCharacters(self,oldpos,pos-oldpos);\
pos=oldpos;\
if(useTabs){\
int nc;\
while(col>=(nc=(oldcol+8)&~7)){\
oldcol=nc;\
rmtext_InsertCharacters(self,pos++,"\t",1);\
}\
}else{\
int count=(col-oldcol)/8;\
while(count--){\
rmtext_InsertCharacters(self,pos,spaces,8);\
oldcol+=8;\
pos+=8;\
}\
}\
rmtext_InsertCharacters(self,pos,spaces,col-oldcol);\
pos+=(col-oldcol);\
}

long rmtext__TabAndOptimizeWS(self,pos,inc)
struct rmtext    *self;
long	pos;
int	inc;
{
    long     home=0, oldPos, col=0, oldCol=0, target;

    while(--pos>0 && rmtext_GetChar(self,pos)!='\n')
	home++;

    oldPos= ++pos;
    while(home--){
	switch(rmtext_GetChar(self,pos)){
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
	char	c=rmtext_GetChar(self,pos);

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
