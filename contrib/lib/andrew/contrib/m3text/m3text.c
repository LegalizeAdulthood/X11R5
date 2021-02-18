/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/m3text/RCS/m3text.c,v 1.6 1991/09/12 16:50:36 bobg Exp $
*/

/* M3text, a Modula-3 mode for BE2. */

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
#include <m3text.eh>

static char rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/m3text/RCS/m3text.c,v 1.6 1991/09/12 16:50:36 bobg Exp $";
static char CopyRight[] = " Copyright (c) IBM Corp. 1989";

#define	NPTOUPPER(c) (islower(c) ? toupper(c) : (c))

typedef struct Dict {
    char    *stng;
    short   val;
    struct Dict *next;
} Dict;

# define TABLESIZE 157
struct Dict *words[TABLESIZE];

#define new(type) (type *) malloc(sizeof(type))

#define NONE 0
#define SINGLE '\''
#define DOUBLE '\"'

#define PROCEDURE_VAL 200 /*RSKadd*/

/*
 * Varables used to control the indenting style.
 */

boolean useTabs = FALSE;
int levelIndent=4,
contIndent=2;

void keyword();
void ident(); /*RSKadd*/
void function(); /*RSKadd*/

boolean is_whitespace(ch)
char ch;
{
    return((ch==' ')||(ch=='\t'));
}

long skipwhitespace(ct,pos,end)
struct m3text *ct;
long pos,end;
{
    while((pos<end)&&is_whitespace(m3text_GetChar(ct,pos))) pos++;
    return pos;
}

boolean is_alnum(ch)    /*RSKadd*/
    char ch;
    {
    return isalnum(ch)||ch=='_'||ch=='|';
    }

boolean m3text__IsAlnum(classID,ch)    /*RSKadd*/
    struct classheader *classID;
    char ch;
    {
    return is_alnum(ch);
    }
    
long copyword(ct,pos,end,buffer)
struct m3text *ct;
long pos,end;
char buffer[];
{
    char ch;
    int count=0;
    buffer[0]=0;
    pos=skipwhitespace(ct,pos,end);
    while((count<=255)&&(pos<end)&&is_alnum(ch=m3text_GetChar(ct,pos++)))
	buffer[count++]=(ch);
    buffer[count]=0;
    return pos;
}

long backwardcopyword(ct,from,to,buffer)
struct m3text *ct;
long from,to;
char buffer[];
{
    long count=0,i=0,j;
    buffer[0]=0;
    while((from-i>=to)&&is_alnum(m3text_GetChar(ct,from-i))) 
	i++;
    j=from-i;
    if(from-i>=to-1) for(;i>0 && count<255;i--,count++) {
	buffer[count]=(m3text_GetChar(ct,from-i+1));
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

void makelower(str)    /*RSKadd*/
char *str;
{
    while(*str!=0) {
	if(isupper(*str)) *str=tolower(*str);
	str++;
    }
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
/*RSKmod documentation: meanings of bit values:
  bit#0   (1) indent lines and match with an END (e.g.BEGIN,IF,etc)
  bit#1   (2) outdent this line (e.g.END)
  bit#2   (4) used to match ELSE and ELSIF with IF
  bit#3   (8) used to align CONST,TYPE,VAR,PROCEDURE,and BEGIN
  bit#4  (16) match END with BEGIN,IF,etc
  bit#5  (32) completely outdents MODULE and INTERFACE
  bit#6  (64) indent lines, but don't match with an END (e.g.PROCEDURE)
  bit#7 (128) used for lining up PROCEDURE arguments or something
*/
/* Keywords are all uppercase *//*RSKmod*/
    {"AND",0},
    {"ARRAY",0},
    {"BEGIN",9},
    {"BITS",0},/*added*/
    {"BY",0},
    {"CASE",1},/*change 65 to 1; free up bit#6 for future use*/
    {"CONST",72},/*change 9 to 72*/
    {"DIV",0},/*added*/
    {"DO",1},/*change 0 to 1 so FOR can be used with BITS without indenting*/
    {"ELSE",4},
    {"ELSIF",4},/*change 36 to 4; use bit#5 for INTERFACE/MODULE*/
    {"END",18},/*change 50 to 18; see ELSIF above*/
    {"EVAL",0},/*added*/
    {"EXCEPT",4},/*added*/
    {"EXCEPTION",0},/*added*/
    {"EXIT",0},/*added*/
    {"EXPORTS",0},/*added the "S"*/
    {"FINALLY",4},/*added*/
    {"FOR",0},/*changed 1 to 0, indent DO instead*/
    {"FOREIGN",32},/*Modula-2++; not standard Modula-3*//*RSKmod 32??*/
    {"FROM",0},
    {"IF",1},
    {"IMPORT",0},
    {"IN",0},/*added*/
    {"INLINE",136},/*added*/
    {"INTERFACE",33},/*changed 0 to 33*/
    {"LOCK",0},/*added, note that indentation is taken care of by DO*/
    {"LOOP",1},/*added, but it's not in 9.1.Keywords on p.46 of digital's "Modula-3 Report"*/
    {"METHODS",4},/*added*/
    {"MOD",0},
    {"MODULE",96},/*changed 8 to 96*/
    {"NOT",0},
    {"OBJECT",1},/*added*/
    {"OF",0},
    {"OR",0},
    {"PROCEDURE",PROCEDURE_VAL},/*change 136 to PROCEDURE_VAL(=200)*/
    {"RAISES",0},/*added*/
    {"READONLY",0},/*added*/
    {"RECORD",1},
    {"REF",0},/*added*/
    {"REPEAT",1},
    {"RETURN",0},
    {"SET",0},
    {"THEN",0},
    {"TO",0},
    {"TRY",1},/*added*/
    {"TYPE",72},/*change 9 to 72*/
    {"TYPECASE",1},/*added*/
    {"UNSAFE",0},/*added*/
    {"UNTIL",18},/*change 50 to 18, see ELSIF*/
    {"UNTRACED",0},/*added*/
    {"VALUE",0},/*added*/
    {"VAR",72},/*change 9 to 72*/
    {"WHILE",0},/*change 1 to 0, let DO take care of indentation*/
    {"WITH",0},/*same as WHILE*/
    {"|",4},/*this is to align |'s with case statements; note that [Tab] key should be pressed after [|] so that cases line up properly*/
/* Reserved Identifiers are all lowercase *//*RSKadds*/
    {"abs",0},
    {"address",0},
    {"adr",0},
    {"adrsize",0},
    {"bitsize",0},
    {"boolean",0},
    {"bytesize",0},
    {"cardinal",0},
    {"char",0},
    {"ceiling",0},
    {"dec",0},
    {"dispose",0},
    {"excl",0},
    {"false",0},
    {"first",0},
    {"float",0},
    {"floor",0},
    {"inc",0},
    {"incl",0},
    {"integer",0},
    {"last",0},
    {"longfloat",0},
    {"longreal",0},
    {"loophole",0},
    {"max",0},
    {"narrow",0},
    {"new",0},
    {"nil",0},
    {"null",0},
    {"number",0},
    {"ord",0},
    {"raise",0},
    {"real",0},
    {"refany",0},
    {"round",0},
    {"subarray",0},
    {"true",0},
    {"trunc",0},
    {"typecode",0},
    {"val",0},
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

Dict *lookup(hashTable, word)
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
    if (strcmp(word, wordup)!=0 && strcmp(word, wordlo)!=0)
	return &miss;
    else
	if ((bucket=lookup(hashTable,wordup))->stng!=NULL)
	    return bucket;
	else
	    return lookup(hashTable,wordlo);
    }

struct environment *m3text__GetEnvironment(self,pos)
struct m3text *self;
long pos;
{
    struct environment *me;
    if(me=self->header.text.rootEnvironment) me=environment_GetEnclosing(me,pos);
    return me;
}

struct style *m3text__GetStyle(self,pos)
struct m3text *self;
long pos;
{
    struct environment *me;
    if(me=m3text_GetEnvironment(self,pos)) return me->data.style;
    return NULL;
}

void m3text__InsertNewStyle(self,pos,len,style,begflag,endflag)
struct m3text *self;
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
struct m3text *self;
long pos;
char delim;
{
    while((pos>=0)&& m3text_GetChar(self,pos--)!=delim);
    return pos;
}
boolean InString(self,pos)
struct m3text *self;
long pos;
{
    long backwardSkipComment();
    char c,lastquote=0;
    boolean quotes=FALSE;
    while((pos>=0)&& (c=m3text_GetChar(self,pos--))!='\n') {
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
struct m3text *self;
long from,to;
{
    Dict *word;
    struct environment *me;
    char buff[1024];
    long j=from;
    long checkword();/*RSKadd*/
    if((!InString(self,from))&& (is_alnum(m3text_GetChar(self,from)))&& (((me=m3text_GetEnvironment(self,from))==self->header.text.rootEnvironment)|| (me->data.style==self->keyword_style))) {/*RSKmod*/
	j=backwardcopyword(self,from,to,buff);
	if((word=ULlookup(words,buff))->stng!=NULL) {
	    int l;
	    l=strlen(buff);
	    /*m3text_ReplaceCharacters(self,j+1,l, buff,l);*//*RSKremove*/
	    if(environment_Remove(self->header.text.rootEnvironment, j+1,l, environment_Style,FALSE)) m3text_SetModified(self);
	    if (islower(*(word->stng))) ident(self,j+1,l);  else /*RSKmod*/
		keyword(self,j+1,l);
	    if (word->val==PROCEDURE_VAL) checkword(self,j+1,m3text_GetLength(self)); /*RSKadd*/
	}
    }
    return j;
}

void m3text__BackwardCheckWord(self,from,to)
struct m3text *self;
long from,to;
{
    (void) backwardcheckword(self,from,to);
}

long checkword(self,i,end)
struct m3text *self;
long i,end;
{
    long j;
    Dict *word;
    char buff[1024];
    i=skipwhitespace(self,i,end);
    j=copyword(self,i,end,buff);
    if((word=ULlookup(words,buff))->stng!=NULL) {
	int l=strlen(word->stng);
	/*m3text_ReplaceCharacters(self,i,l, buff,l);*//*RSKremove*/
	if (islower(*(word->stng))) ident(self,i,l);  else /*RSKmod*/
	    keyword(self,i,l);
	/*RSKadd:*/
	if (word->val==PROCEDURE_VAL) {
	    i=skipwhitespace(self,j-1,end);
	    j=copyword(self,i,end,buff);/*get procedure name*/
	    if ((word=ULlookup(words,buff))->stng==NULL) { /*legit proc name*/
		l=strlen(buff);
		if (l>0)
		    function(self,i,l);
	    }
	    else { /*not a proc name!! make appropriate style*/
		l=strlen(word->stng);
		/*m3text_ReplaceCharacters(self,i,l, buff,l);*/
		if (islower(*(word->stng))) ident(self,i,l);  else
		    keyword(self,i,l);
	    }
	}
    }
    return j;
}

void m3text__SetAttributes(self,atts)
struct m3text	*self;
struct attributes   *atts;
{
    super_SetAttributes(self,atts);

    while(atts!=NULL)
    {
	if(strncmp(atts->key,"m3text-",7)==0)
	{
	    char    *key=atts->key+7;

	    if(strcmp(key,"level-indent")==0)
		levelIndent=atoi(atts->value.string);
	    else if(strcmp(key,"cont-indent")==0)
		contIndent=atoi(atts->value.string);
	    else if(strcmp(key,"use-tabs")==0)
		useTabs=atoi(atts->value.string);
	}    
	atts=atts->next;
    }
}

static void SetupStyles(self)
struct m3text *self;
{
    self->comment_style = stylesheet_Find(self->header.text.styleSheet, "comment");
    self->keyword_style = stylesheet_Find(self->header.text.styleSheet, "keyword"); /* rjd */
    self->ident_style = stylesheet_Find(self->header.text.styleSheet, "identifier"); /*RSKadd*/
    self->function_style = stylesheet_Find(self->header.text.styleSheet, "function"); /*RSKadd*/
    /*  fprintf (stderr,"Calling SetGlobalStyles\n");  rjd */

    m3text_SetGlobalStyle(self, stylesheet_Find(self->header.text.styleSheet, "global"));
    buildTable(words);/*DHGadd*/
}

boolean m3text__InitializeObject(classID, self)    /*RSKmod*/
struct classheader *classID;
struct m3text *self;
{
    struct attributes templateAttribute;
    templateAttribute.key = "template";
    templateAttribute.value.string = "m3text";
    templateAttribute.next = NULL;
    m3text_SetAttributes(self, &templateAttribute);
    m3text_SetExportEnvironments(self, FALSE);

    /*   fprintf (stderr,"Calling SetUpStyles\n");  rjd */
    SetupStyles(self);
    return TRUE;
}

void m3text__Clear(self)
struct m3text *self;
{

    super_Clear(self); /* This destroyes all styles in the stylesht. */

    SetupStyles(self);
}

long m3text__Read(self, file, id)
struct m3text *self;
FILE *file;
long id;
{
    long tmpRetValue;
    tmpRetValue = super_Read(self, file, id);
    if (tmpRetValue == dataobject_NOREADERROR) {
	SetupStyles(self);
	m3text_RedoStyles(self);
    }
    return tmpRetValue;
}

long m3text__Write(self, file, writeID, level)    /*RSKadd*/
    struct m3text *self;
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
struct m3text *self;
long start;
{
    long len=m3text_GetLength(self);
    char quote=m3text_GetChar(self,start);
    while((++start<=len)&&(m3text_GetChar(self,start)!=quote));
    return start;
}
#endif /* NOTUSED */

static long comment(self, start)
struct m3text *self;
long start;
{
    int prev, c = 0;
    long end, len = m3text_GetLength(self);
    int comments=1;
    for (end = start; end < len; end++) {
	prev = c;
	c = m3text_GetChar(self, end);
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
	    if (self->comment_style != NULL) m3text_InsertNewStyle(self,start,end-start,self->comment_style,TRUE,TRUE);
	    break;
	}
    }
    return end;
}

/* keyword inserts keyword looks. rjd */
void keyword(self, posn, len)
struct m3text *self;
long posn, len;
{
    if (self->keyword_style != NULL) {
	m3text_InsertNewStyle(self,posn,len,self->keyword_style,FALSE,FALSE);
    }
}
/*RSKadd: "ident" does same thing as "keyword", but italic*/
void ident(self, posn, len)
struct m3text *self;
long posn, len;
{
    if (self->ident_style != NULL) {
	m3text_InsertNewStyle(self,posn,len,self->ident_style,FALSE,FALSE);
    }
}
/*RSKadd: "function" does same thing as "keyword", but 2 points larger*/
void function(self, posn, len)
struct m3text *self;
long posn, len;
{
    if (self->function_style != NULL) {
	m3text_InsertNewStyle(self,posn,len,self->function_style,FALSE,FALSE);
    }
}

/* returns the length of a *constant* string */
#define cstrlen(str) (sizeof(str)-1)

/* pos should pointer to 2nd from last character-- last character won't be
* matched; strconst *must* be a constant string, so that sizeof works
*/
#define backwardMatch(self,pos,strConst) \
(pos>=cstrlen(strConst) && match(self,pos-(cstrlen(strConst)-1)+1,strConst,cstrlen(strConst)))

#define match(self,pos,str,len) ((pos==0 || !(is_alnum(m3text_GetChar(self,pos-1)))) && !(is_alnum(m3text_GetChar(self,pos+len))) && domatch(self,pos,str,len))

boolean domatch(self,pos,str,len)
struct m3text *self;
int pos;
char *str;
int len;
{
    while(len>0 && m3text_GetChar(self,pos++)==*str++)
	len--;

    return len==0;
}

static void DoFreeTree(self)
struct nestedmark *self;
{
    nestedmark_FreeTree(self);
}

void m3text__RedoStyles(self)
struct m3text *self;
{
    struct nestedmark *root = (struct nestedmark *)self->header.text.rootEnvironment;
    long posn, origpos, len = m3text_GetLength(self);
    int prev = 0, c = '\n', string = NONE;
    /* c is initialized to a newline so the start of the file looks like the start of line. */

    /* Remove the old styles, but leave the root environemt in place. */
    if (root->children) {
	tree23int_Apply(root->children, DoFreeTree);
	tree23int_Destroy(root->children);
	root->children = NULL;
    }
    for (posn = 0; posn < (len/*=m3text_GetLength(self)*/); posn++) /*RSKmod*/
    {
	prev = c;
	posn=skipwhitespace(self,posn,len);
	c = m3text_GetChar(self, posn);
	switch (c) {
	    case '\n':
		break;
	    case '*':
		if (prev == '(' && string == NONE) /* rjd */
		    posn = comment(self, posn);
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
		/* Parse Modula-3 keywords and set proper look rjd */
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
struct m3text *self;
long pos;
{
    /* returns true iff the character at pos is quoted (ie. "\"). Takes into account the slash being quoted. (ie "\\"). */

    boolean retval = FALSE;

    pos--;
    while (m3text_GetChar(self, pos) == '\\' && pos > 0) {
	retval = !retval;
	pos--;
    }

    return FALSE;
}

long m3text__ReverseBalance(self, pos)
struct m3text *self;
long pos;
{
    boolean found = FALSE, /*RSKmod remove "incomment = FALSE,"*/ instring = FALSE, doublestring = FALSE, atleastone = FALSE;
    int incomment=0;/*RSKadd*/
    int thischar, prechar;
    char *parentype;
    struct paren_node *parenstack = NULL/*, *tmp */;
    static char *opens = "({[", *closes = ")}]";

    while ((parenstack != NULL || !atleastone) && (pos > 0)) {
	thischar = m3text_GetChar(self, --pos);
	prechar = pos > 0 ? m3text_GetChar(self, pos - 1) : 0;

	if (incomment) {
	    if(thischar == '*' && prechar == '(') {
		incomment--;/*RSKmod*/
		if (parenstack == NULL) found = TRUE;
		else pos= pos > 1 ? pos -= 2 : 0;
	    }
	    else if (thischar==')' && prechar=='*')
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

long m3text__Indent(self,mark)
struct m3text *self;
struct mark *mark;
{
    long end,pos=mark_GetPos(mark);
    int c;

    while(pos>=0 && m3text_GetChar(self,--pos)!='\n');

    pos++;

    do{
	pos=m3text_TabAndOptimizeWS(self,pos,indentation(self,pos-1));

	for(end=pos; (c=m3text_GetChar(self,end))==' ' || c=='\t'; end++ );
	if(end>pos)
	    m3text_DeleteCharacters(self,pos,end-pos);

	end=mark_GetEndPos(mark);
	while(pos<end && (c=m3text_GetChar(self,pos++))!=EOF && c!='\n');
    }while(pos<end && c!=EOF);

    return pos;
}

void m3text__ReindentLine(self,pos)
struct m3text *self;
long pos;
{
    long end;
    int c;

    while(pos>0 && m3text_GetChar(self,--pos)!='\n');

    pos++;

    pos=m3text_TabAndOptimizeWS(self,pos,indentation(self,pos-1));

    for(end=pos; (c=m3text_GetChar(self,end))==' ' || c=='\t'; end++);

    if(end>pos)
	m3text_DeleteCharacters(self,pos,end-pos);
}
long SkipComment(self,pos)
struct m3text *self;
long pos;
{
    int count=1;
    char ch=0,prev;
    long len=m3text_GetLength(self);/*RSKadd*/
    if(pos>=0)
	while((count>0)&&(++pos<len)) {/*RSKmod changed >0 to <len*/
	    prev=ch;
	    if(((ch=m3text_GetChar(self,pos))==')')&& (prev=='*')) count--;
	    else if ((ch=='*')&&(prev=='(')) count++;
	}
    return pos+1;
}
long backwardSkipComment(self,pos)
struct m3text *self;
long pos;
{
    int count=1;
    char ch=0,prev;
    if(pos>=0)
	while((count>0)&&(--pos>0)) {
	    prev=ch;
	    if(((ch=m3text_GetChar(self,pos))=='(')&& (prev=='*')) count--;
	    else if ((ch=='*')&&(prev==')')) count++;
	}
    return pos-1;
}

void m3text__StyleLine(self,pos)
struct m3text *self;
long pos;
{
    long i,start;
    int string=NONE;
    char ch=0,prev;
    start=i=pos;
    while((--start>=0)&&(ch=m3text_GetChar(self,start))!='\n');
    while((--i>=start)&&(m3text_GetChar(self,i))!='\n') {
	prev=ch;
	ch=m3text_GetChar(self,i);
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
	    if(is_alnum(ch)&& string==NONE) i=backwardcheckword(self,i,start)+1;/*RSKmod*/
	}
    }
	/* if(i!=pos) {
	if(environment_Remove(self->header.text.rootEnvironment,i, pos-i,environment_Style,TRUE)) m3text_SetModified(self);
	while(i<pos) i=checkword(self,i,pos);
	m3text_RegionModified(self,i,pos-i);
    } */
    m3text__ReindentLine(self,pos);
}

/* assumes that  pos points to 1 BEFORE ending ')' */



/* beg and end must be constants (for switch) */
/* assumes ending delimiter AFTER pos */
long backwardSkipDelimited(self,pos,beg,end)
struct m3text *self;
long pos;
char beg, end;
{
    int level=1;
    while(pos>=0 && level>0){
	int c;
	switch(c=m3text_GetChar(self,pos--)){
	    case '\'':
	    case '"':
		pos=backwardSkipString(self,pos,c);
		break;
	    case ')':
		if ((m3text_GetChar(self,pos)) == '*')
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
struct m3text *self;
long pos;
{
    int commentlevel=0,stringtype=NONE;
    boolean done=FALSE;
    long len=m3text_GetLength(self);
    while((!done)&&(pos<len)) {
	if(pos<len) {
	    register c=m3text_GetChar(self,pos++);
	    register c2=m3text_GetChar(self,pos);
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
struct m3text *self;
long i;
{
    char ch=0;
    boolean done=FALSE;
    while(!done && (i>=0)) {
	ch=m3text_GetChar(self,i);
	switch(ch) {
	    case '\'':
	    case '\"':
		i=backwardSkipString(self,i-1,ch);
		break;
	    case ')':
		if(i<1 || m3text_GetChar(self,i-1)!='*') done=TRUE;
		else i=backwardSkipComment(self,i-1)+1;
	    case '\n':
	    case '\t':
	    case ' ':
		break;
	    default:
		done=TRUE;
	}
	ch=m3text_GetChar(self,i--);
    }
    return i+1;
}
long matchend(self,pos)
struct m3text *self;
long pos;
{
    int count=1;
    while((count>0)&&(--pos>=0)) {
	pos=backwardSkipJunk(self,pos);
	if(pos>=0) {
	    if(is_alnum(m3text_GetChar(self,pos))) {/*RSKmod*/
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

long backline(self,pos)
struct m3text *self;
long pos;
{
    while((pos>=0)&&(m3text_GetChar(self,pos--)!='\n'));
    return pos;
}

long backLevel(self,pos,word)   /*RSKmod*/
struct m3text *self;
long pos;
Dict *word;/*RSKadd*/
{
    Dict *word2;
    char buff[256];
    boolean isbegin=(word->val&8)&&(word->val&1),
    isproc=word->val&128;
    while(--pos>=0) {
	/*RSKmod:*/
	pos=backline(self,pos);
	if (pos>=0) {
	    (void)copyword(self,pos+2,m3text_GetLength(self),buff);
	    word2=ULlookup(words,buff);
	    if (word2->val&32) {
		if (isbegin) return 0;
		else return levelIndent;
	    }
	    if ((word2->val&8)&&(word2->val&1))
		if (isbegin) return 0;
	    if (word2->val&128) {
		if (isproc) return currentIndent(self,pos+1);
		return currentIndent(self,pos+1)+levelIndent;
	    }
	    if ((word2->val&8)&&(!isproc)) return currentIndent(self,pos+1);
	}
    }
    return 0;
}

#if 0 /*RSKremove*/
long lineupComment(self,pos)    /*RSKadd*/
struct m3text *self;
long pos;
{
    pos+=3;
    while (is_whitespace(m3text_GetChar(self,pos))) pos++;
    return pos-1;
}
#endif /*RSKremove*/

char backwardFirstnonwhitespace(self,pos)    /*RSKadd*/
struct m3text *self;
long pos;
{
    char ch;
    while (pos>0)
	if (!is_whitespace(ch=m3text_GetChar(self,pos--)))
	    return ch;
}

/* indentation returns the desired indentation of the line following pos.  pos is assumed to point to the <cr> that precedes the line */
int indentation(self,pos)
struct m3text *self;
long pos;
{
    char buff[256];
    Dict *word;
    long i,j;
    int indent=0;
    long oldj,temppos;/*RSKadd*/
    if(m3text_GetStyle(self,pos)==self->comment_style)
	return currentIndent(self,pos);/*RSKmod*/
    temppos=skipwhitespace(self,pos+1,m3text_GetLength(self));/*RSKadd*/
    if (m3text_GetChar(self, temppos)=='(' && m3text_GetChar(self, temppos+1)=='*')
	return currentIndent(self,pos);
    (void) copyword(self,pos+1,m3text_GetLength(self),buff);
    word=ULlookup(words,buff);
    if (word->val&32) return 0;/*RSKadd*/
    if (word->val&16) return matchend(self,pos);/*RSKmod: change 32 to 16*/
    if (word->val&8) return backLevel(self,pos,word);/*RSKmod: add word*/
    if (word->val&4) return matchend(self,pos);/*RSKmod: change "matchelse" to "matchend"*/
    j=backwardSkipJunk(self,pos);
    i=backline(self,j);
    if (m3text_GetChar(self,j)=='|')
	return matchend(self,pos)+levelIndent; /*RSKmod*/
    i=i+((i>0)?2:-i);/*RSKmod*/
    /*RSKmod:*/
    oldj=j;
    while (j>i) {
	j=backwardcopyword(self,j,i,buff);
	word=ULlookup(words,buff);
	if((word->val&1)||(word->val&4)||(word->val&64 && backwardFirstnonwhitespace(self,j)=='\n'))
	    indent=indent+levelIndent;
	if(word->val&2)
	    return matchend(self,j)+indent;
	j=backwardSkipJunk(self,j-1);
    }
    if ((i=currentIndent(self,oldj))+indent<0) indent= -i; /*RSKmod*/
    return i+indent;
}
int currentIndent(self,pos)
struct m3text *self;
long pos;
{
    int c,ind=0;

    while(pos>=0 && (c=m3text_GetChar(self,pos))!='\n')
	pos--;

    while(is_whitespace(c=m3text_GetChar(self,++pos))) /*RSKmod*/
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
int m3text__CurrentIndent(self,pos)    /*RSKadd*/
struct m3text *self;
long pos;
{
    return currentIndent(self,pos);
}

int currentColumn(self,pos)
struct m3text *self;
long pos;
{
    int ind=0;
    long oldPos=pos;

    while(pos>=0 && m3text_GetChar(self,pos)!='\n')
	pos--;

    while(pos<oldPos)
	if(m3text_GetChar(self,pos++)=='\t')
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
m3text_DeleteCharacters(self,oldpos,pos-oldpos);\
pos=oldpos;\
if(useTabs){\
int nc;\
while(col>=(nc=(oldcol+8)&~7)){\
oldcol=nc;\
m3text_InsertCharacters(self,pos++,"\t",1);\
}\
}else{\
int count=(col-oldcol)/8;\
while(count--){\
m3text_InsertCharacters(self,pos,spaces,8);\
oldcol+=8;\
pos+=8;\
}\
}\
m3text_InsertCharacters(self,pos,spaces,col-oldcol);\
pos+=(col-oldcol);\
}

long m3text__TabAndOptimizeWS(self,pos,inc)
struct m3text    *self;
long	pos;
int	inc;
{
    long     home=0, oldPos, col=0, oldCol=0, target;

    while(--pos>0 && m3text_GetChar(self,pos)!='\n')
	home++;

    oldPos= ++pos;
    while(home--){
	switch(m3text_GetChar(self,pos)){
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
	char	c=m3text_GetChar(self,pos);

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
