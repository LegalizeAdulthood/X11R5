/* $Header: /usr/local/src/x11r5.mvex/RCS/highlight.c,v 1.4 1990/10/23 11:09:33 toddb Exp $ */
#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>

typedef u_char	boolean;

#define True	1
#define False	0

#define StringIncrement	128
#define EntryIncrement	5
char	*Program;

typedef struct {
    boolean	shown;
    boolean	fontChange;
    char	*name;
    char	*indexEntry;
} Entry, *EntryPtr;

typedef struct {
    char	*stringSpace;
    int		stringNext;
    int		stringLen;		/* length of string space */
    EntryPtr	index;
    int		indexNext;
    int		indexLen;		/* length of indices */
} Index, *IndexPtr;

typedef struct {
    char	buf[ BUFSIZ ];
    char	*space;
    char	*prefix;
    char	*name;
    char	*suffix;
} Chunk, *ChunkPtr;

#define SkipSpace(p)	{ while (isspace(*p)) p++; }

#define isidentifier(c)	(isalnum(c) || (c) == '_')
void Msg();
void Error();
void Fatal();
void Usage();
void FilterStdin();
IndexPtr ReadIndex();
char *GetChunk();
char *Realloc();
char *realloc();
char *malloc();

main(argc, argv)
    int	    argc;
    char    **argv;
{
    int	    argcnt = 0;
    int	    i;
    IndexPtr index;

    Program = *argv;
    for (argv++, argc--; argc; argv++, argc--) {
	if (argv[0][0] == '-') {
	    argcnt++;
	    for (i=1; argv[0][i]; i++)
		switch(argv[0][i]) {
		case 'z':
		    break;
		default:
		    Usage();
		    break;
		}
	    if (i == 1)
		Usage();
	} else {
	    index = ReadIndex(argv[0]);
	    argcnt++;
	}
    }
    if (argcnt == 0)
	Usage();
    
    FilterStdin(index);
    exit(0);
}

char *Realloc(ptr, len)
    char *ptr;
    int len;
{
    if (ptr == NULL)
	return (malloc(len));
    return (realloc(ptr, len));
}

void PrintWord(entry, chunk, fontChangeOk)
    EntryPtr	entry;
    ChunkPtr	chunk;
    boolean fontChangeOk;
{
    char fontChange = (fontChangeOk && entry) ? entry->fontChange : 0;

    if (*chunk->space)
	fputs(chunk->space, stdout);
    if (*chunk->prefix)
	fputs(chunk->prefix, stdout);
    if (fontChange) {
	fputs("\\f", stdout);
	fputc(entry->fontChange, stdout);
	fputs("\\^", stdout);
    }
    if (*chunk->name)
	fputs(chunk->name, stdout);
    if (fontChange)
	fputs("\\^\\fP", stdout);
    if (*chunk->suffix)
	fputs(chunk->suffix, stdout);
}

/*
 * Look for an index word using a binary search
 */
EntryPtr FindWord(index, name)
    IndexPtr index;
    char *name;
{
    int i, top, bottom, compare, change;
    EntryPtr base = index->index;

    if (index->indexNext == 0)
	return (NULL); /* idiot-proof */

    i = index->indexNext >> 1; /* start in the middle */
    top = index->indexNext;
    bottom = -1;

    for (;;) {
	if ((compare = strcmp(name, base[ i ].name)) == 0)
	    return(base + i);
	if (compare < 0) {
	    top = i;
	    if ((change = (top - bottom) >> 1) == 0)
		change = 1;
	    if ((i -= change) <= bottom)
		return(NULL);
	} else {
	    bottom = i;
	    if ((change = (top - bottom) >> 1) == 0)
		change = 1;
	    if ((i += change) >= top)
		return(NULL);
	}
    }
	
#ifdef notdef
    int i = index->indexNext;
    EntryPtr entry = index->index;

    while (--i >= 0) {
	if (strcmp(name, entry->name) == 0) {
	    return (entry);
	}
	entry++;
    }
    return (NULL);
#endif
}

char *GetChunk(pbuf, chunk)
    char *pbuf;
    ChunkPtr chunk;
{
    char *pfrom, *pto, *lastSpace, *p, c;
    boolean inWord = False;

    pfrom = pbuf;
    lastSpace = chunk->buf;
    pto = chunk->buf + 1; /* leave room for a null character */
    c = *pfrom;
    while (! isidentifier(c)) {
	if (! c) {
	    if (pfrom == pbuf)
		return(NULL);

	    chunk->space = chunk->buf + 1;
	    *pto = '\0';
	    chunk->prefix = pto;
	    chunk->name = pto;
	    chunk->suffix = pto;
	    return (pfrom);
	}
	if (isspace(c))
	    lastSpace = pto;
	*pto++ = c;
	pfrom++;
	c = *pfrom;
    }
    *pto++ = '\0';

    /*
     * Copy the spaces at the beginning back one character to
     * make room for a null character.
     */
    for (p = chunk->buf; p < lastSpace; p++)
	p[0] = p[1];
    *p = '\0';
    chunk->space = chunk->buf;
    chunk->prefix = lastSpace + 1;

getAlphaWord:

    chunk->name = pto;
    while (isidentifier(*pfrom))
	*pto++ = *pfrom++;

    /*
     * Include () pairs that immediately follow a word, because it
     * is probably a function declaration.
     */
    if (pfrom[0] == '(' && pfrom[1] == ')') {
	*pto++ = *pfrom++;
	*pto++ = *pfrom++;
    }
    *pto++ = '\0';

    chunk->suffix = pto;
    while (! isidentifier(*pfrom) && ! isspace(*pfrom))
	*pto++ = *pfrom++;
    *pto++ = '\0';

    return (pfrom);
}

void FilterStdin(index)
    IndexPtr index;
{
    FILE *fp = stdin;
    char buf[ BUFSIZ ], *p;
    Chunk	chunk;
    EntryPtr	entry;
    EntryPtr	todo[ 100 ];
    int todoCount, i, line = 0;
    int inDisplay = 0;

    while (fgets(p = buf, BUFSIZ, fp)) {
	todoCount = 0;
	line++;

	/*
	 * If it is a troff directive, don't bother.
	 */
	if (*p == '.' || *p == '\'') {
	    /*
	     * Catch .DS and .DE and suppress the addition of \fC and \fB
	     */
	    if (p[1] == 'D') {
		if (p[2] == 'S') {
		    if (inDisplay)
			Msg("Nested .DS at input line %d and %d\n",
			    inDisplay, line);
		    inDisplay = line;
		} else if (p[2] == 'E') {
		    if (! inDisplay)
			Msg("Unmatched .DE at input line %d\n", line);
		    inDisplay = 0;
		}
	    }
	    fputs(buf, stdout);
	    continue;
	}

	/* printf(">> %s", buf); */
	while (p = GetChunk(p, &chunk)) {
	    entry = FindWord(index, chunk.name, inDisplay);
	    PrintWord(entry, &chunk, inDisplay == 0);
	    if (entry)
		todo[ todoCount++ ] = entry;
	}

	for (i = 0; i < todoCount; i++) {
	    if (! todo[ i ]->shown) {
		printf(".IN \"%s\" \"%s\" \"\"\n", 
		    todo[ i ]->indexEntry, todo[ i ]->name);
		printf(".IN \"%s\" \"\" \"\"\n", todo[ i ]->name);
		todo[ i ]->shown = True;
	    }
	}
	for (i = 0; i < todoCount; i++)
	    todo[ i ]->shown = False;
    }

    if (inDisplay)
	Msg("Unmatched .DE at input line %d\n", inDisplay);
}

void AddName(index, name, entry, fontChange)
    IndexPtr index;
    char *name;
    char *entry;
    boolean fontChange;
{
    int nameLen = strlen(name) + 1;
    int entryLen;
    int i;
    char *newBuf;
    int newOffset;
    EntryPtr newEntry;

    SkipSpace(entry);
    entryLen = strlen(entry);
    entry[ entryLen - 1 ] = 0; /* remove newline */

    /*
     * Allocate new string space
     */
    if (index->stringNext + nameLen + entryLen > index->stringLen) {
	if (nameLen + entryLen < StringIncrement)
	    index->stringLen += StringIncrement;
	else
	    index->stringLen += nameLen + entryLen;
	newBuf = Realloc(index->stringSpace, index->stringLen);

	/*
	 * move the string pointers
	 */
	if (newBuf != index->stringSpace) {
	    newOffset = newBuf - index->stringSpace;
	    for (i=0; i<index->indexNext; i++) {
		index->index[ i ].name += newOffset;
		index->index[ i ].indexEntry += newOffset;
	    }
	    index->stringSpace = newBuf;
	}
    }

    /*
     * allocate new entries
     */
    if (index->indexNext >= index->indexLen) {
	index->indexLen += EntryIncrement;
	index->index = (EntryPtr) Realloc(index->index,
					  sizeof(Entry) * index->indexLen);
    }
    newEntry = index->index + index->indexNext;
    index->indexNext++;

    newEntry->shown = False;
    newEntry->fontChange = fontChange;
    newEntry->name = index->stringSpace + index->stringNext;
    index->stringNext += nameLen;
    strcpy(newEntry->name, name);

    newEntry->indexEntry = index->stringSpace + index->stringNext;
    index->stringNext += entryLen;
    strcpy(newEntry->indexEntry, entry);
}

int Compare(elem1, elem2)
    EntryPtr elem1;
    EntryPtr elem2;
{
    return(strcmp(elem1->name, elem2->name));
}

IndexPtr ReadIndex(file)
    char *file;
{
    static Index index;
    FILE *fp;
    char buf[ BUFSIZ ], *p;
    boolean fontChange;
    Chunk	chunk;

    if ((fp = fopen(file, "r")) == NULL)
	Error("Can't open %s\n", file);

    while (fgets(buf, BUFSIZ, fp)) {
	if (buf[0] == '\n')
	    continue; /* skip blank lines */

	fontChange = False;
	p = GetChunk(buf, &chunk);
	if (strcmp(chunk.name, "bold") == 0) {
	    fontChange = 'B';
	    p = GetChunk(p, &chunk);
	}
	else if (strcmp(chunk.name, "constant") == 0) {
	    fontChange = 'C';
	    p = GetChunk(p, &chunk);
	}
	AddName(&index, chunk.name, p, fontChange);
    }
    fclose(fp);

    qsort(index.index, index.indexNext, sizeof(Entry), Compare);

    return (&index);
}

void Msg(fmt, x0,x1,x2,x3,x4,x5,x6,x7,x8,x9)
    char    *fmt;
{
    fprintf(stderr, fmt, x0,x1,x2,x3,x4,x5,x6,x7,x8,x9);
}

void Error(fmt, x0,x1,x2,x3,x4,x5,x6,x7,x8,x9)
    char    *fmt;
{
    extern char	    *sys_errlist[];
    extern int	    errno;

    Msg("%s: ", Program);
    if (errno)
	Msg("%s: ", sys_errlist[ errno ]);
    Msg(fmt, x0,x1,x2,x3,x4,x5,x6,x7,x8,x9);
}

void Fatal(fmt, x0,x1,x2,x3,x4,x5,x6,x7,x8,x9)
    char    *fmt;
{
    Error(fmt, x0,x1,x2,x3,x4,x5,x6,x7,x8,x9);
    exit(1);
}

void Usage()
{
    Msg("Usage: cat troff-files | %s index | troff\n", Program);
    Msg("\twhere `index' is a file with lines of the form\n");
    Msg("\n");
    Msg("\t\t[bold|constant] <string> <index entry>\n");
    Msg("\n");
    exit (1);
}

