/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
 \* ********************************************************************** */

#include <stdio.h>
#include <ctype.h>
#include <setjmp.h>

#include <class.h>

#include <bind.ih>
#include <text.ih>
#include <textv.ih>
#include <mark.ih>
#include <search.ih>
#include <message.ih>
#include <im.ih>

#include <gsearch.eh>

#define MAX(a,b) (((a)>(b))?(a):(b))

#define DYNSTR_GROWSIZE (64)
#define STATESTACK_GROWSIZE (32)

#define CTRL_G (7)
#define CTRL_H (8)
#define CTRL_L (12)
#define CTRL_Q (17)
#define CTRL_R (18)
#define CTRL_S (19)
#define CTRL_W (23)
#define CTRL_Y (25)
#define ESC (27)
#define DEL (127)

struct dynstr {
    int used, allocated;
    char *text;
};

struct statestacknode {
    int patternlen, wrappedp, failurep;
    long position, length, searchfrom;
    int forwardp;
};

struct statestack {
    int used, allocated;
    struct statestacknode *nodes;
};

static char     rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/extensions/RCS/gsearch.c,v 1.7 1991/09/12 16:20:14 bobg Exp $";

extern char *malloc(), *realloc();

static jmp_buf jmpenv;
static struct dynstr LastPattern;
static struct statestacknode *StackTop;

static int statestack_init(s)
struct statestack *s;
{
    s->used = 0;
    s->allocated = STATESTACK_GROWSIZE;
    if (!(s->nodes =
	  (struct statestacknode *) malloc(STATESTACK_GROWSIZE *
					    (sizeof (struct statestacknode)))))
	return (1);
    return (0);
}

static void statestack_destroy(s)
struct statestack *s;
{
    free(s->nodes);
}

static void statestack_pop(s)
struct statestack *s;
{
    --(s->used);
    StackTop = s->nodes + s->used - 1;
}

static void statestack_push(s, pl, wp, fp, pos, len, sf, fwdp)
struct statestack *s;
int pl, wp, fp;
long pos, len, sf;
int fwdp;
{
    if (s->used == s->allocated) {
	int newsize = s->allocated + STATESTACK_GROWSIZE;

	if (!(s->nodes =
	     (struct statestacknode *)
	      realloc(s->nodes,
		      newsize * (sizeof (struct statestacknode)))))
	    longjmp(jmpenv, 1);
	s->allocated = newsize;
    }
    s->nodes[s->used].patternlen = pl;
    s->nodes[s->used].wrappedp = wp;
    s->nodes[s->used].failurep = fp;
    s->nodes[s->used].position = pos;
    s->nodes[s->used].length = len;
    s->nodes[s->used].searchfrom = sf;
    s->nodes[(s->used)++].forwardp = fwdp;
    StackTop = s->nodes + s->used - 1;
}

static int dynstr_init(d)
struct dynstr *d;
{
    d->used = 1;
    d->allocated = DYNSTR_GROWSIZE;
    if (!(d->text = malloc(DYNSTR_GROWSIZE)))
	return (1);
    d->text[0] = '\0';
    return (0);
}

static void dynstr_shortento(d, size)
struct dynstr *d;
int size;
{
    d->used = size;
    d->text[d->used - 1] = '\0';
}

static void dynstr_ensuresize(d, size)
struct dynstr *d;
int size;
{
    int newsize;

    if (d->allocated >= size)
	return;
    if (!(d->text = realloc(d->text,
			    newsize = MAX(DYNSTR_GROWSIZE,
					  size + (DYNSTR_GROWSIZE >> 1)))))
	longjmp(jmpenv, 1);
    d->allocated = newsize;
}

static void dynstr_put(d, str)
struct dynstr *d;
char *str;
{
    int need = 1 + strlen(str);

    dynstr_ensuresize(d, need);
    strcpy(d->text, str);
    d->used = need;
}

static void dynstr_append(d, str)
struct dynstr *d;
char *str;
{
    int need = d->used + strlen(str);

    dynstr_ensuresize(d, need);
    strcat(d->text, str);
    d->used = need;
}

static void dynstr_destroy(d)
struct dynstr *d;
{
    free(d->text);
}

static int dynstr_empty(d)
struct dynstr *d;
{
    return ((d->used == 0) || (d->text[0] == '\0'));
}

static void dynstr_addchar(d, c)
struct dynstr *d;
int c;
{
    int need = d->used + 1;

    dynstr_ensuresize(d, need);
    d->text[d->used - 1] = c;
    d->text[(d->used)++] = '\0';
}

static void dynstr_copy(dest, src)
struct dynstr *dest, *src;
{
    dynstr_ensuresize(dest, src->used);
    strcpy(dest->text, src->text);
    dest->used = src->used;
}

static void dosearch(tv, forwardp)
struct textview *tv;
int forwardp;
{
    FILE *tmpfile;
    struct text *txt = (struct text *) textview_GetDataObject(tv);
    long origpos = textview_GetDotPosition(tv);
    long origlen = textview_GetDotLength(tv);
    long foundloc, newsearchfrom;
    struct dynstr pattern, prompt;
    int wasmeta = 0;
    int c, dodokey = 0, oldforwardp;
    char *compiled, *compileerr, *tmpbuf;
    struct statestack stack;

    if (dynstr_init(&pattern)) {
	message_DisplayString(tv, 0, "I-Search is out of memory; aborted.");
	im_ForceUpdate();
	return;
    }
    if (dynstr_init(&prompt)) {
	dynstr_destroy(&pattern);
	message_DisplayString(tv, 0, "I-Search is out of memory; aborted.");
	im_ForceUpdate();
	return;
    }
    if (statestack_init(&stack)) {
	dynstr_destroy(&pattern);
	dynstr_destroy(&prompt);
	message_DisplayString(tv, 0, "I-Search is out of memory; aborted.");
	im_ForceUpdate();
	return;
    }
    
    if (setjmp(jmpenv)) {
	dynstr_destroy(&pattern);
	dynstr_destroy(&prompt);
	statestack_destroy(&stack);
	textview_SetDotPosition(tv, origpos);
	textview_SetDotLength(tv, origlen);
	textview_FrameDot(tv, origpos);
	message_DisplayString(tv, 0, "I-Search is out of memory; aborted.");
	im_ForceUpdate();
	return;
    }

    statestack_push(&stack, 1, 0, 0, origpos, origlen,
		    textview_GetDotPosition(tv), forwardp);

  initialstate:
    
    dynstr_put(&prompt, "I-Search");
    if (!forwardp)
	dynstr_append(&prompt, " backward");
    if (!dynstr_empty(&LastPattern)) {
	dynstr_append(&prompt, " [");
	dynstr_append(&prompt, LastPattern.text);
	dynstr_append(&prompt, "]");
    }
    dynstr_append(&prompt, ": ");
    message_DisplayString(tv, 0, prompt.text);
    im_ForceUpdate();
    while (1) {
	c = im_GetCharacter(textview_GetIM(tv));
	switch (c) {
	  case CTRL_L:
	    im_RedrawWindow(textview_GetIM(tv));
	    break;
	  case CTRL_G:
	    dynstr_destroy(&prompt);
	    dynstr_destroy(&pattern);
	    statestack_destroy(&stack);
	    message_DisplayString(tv, 0, "Cancelled.");
	    im_ForceUpdate();
	    return;
	  case CTRL_H:
	  case DEL:
	    break;
	  case CTRL_Q:
	    while ((c = im_GetCharacter(textview_GetIM(tv))) == EOF)
		;
	    dynstr_addchar(&pattern, c);
	    goto compilestate;
	  case CTRL_R:
	  case CTRL_S:
	    forwardp = (c == CTRL_S);
	    if (!dynstr_empty(&LastPattern)) {
		dynstr_copy(&pattern, &LastPattern);
		goto compilestate;
	    }
	    goto initialstate;
	  case CTRL_W:
	    if (textview_GetDotLength(tv) > 0)
		goto appendselectionstate;
	    break;
	  case CTRL_Y:
	    goto appendkillheadstate;
	  case ESC:
	    wasmeta = im_WasMeta(textview_GetIM(tv));
	    /* Fall through */
	  case EOF:
	    goto exitstate;
	  default:
	    if (isascii(c) && (isprint(c) || isspace(c))) {
		dynstr_addchar(&pattern, c);
		goto compilestate;
	    }
	    else {
		dodokey = 1;
		goto exitstate;
	    }
	}
    }

  emptypatternstate:
    
    dynstr_put(&prompt, "I-Search");
    if (!forwardp)
	dynstr_append(&prompt, " backward");
    dynstr_append(&prompt, ": ");
    message_DisplayString(tv, 0, prompt.text);
    im_ForceUpdate();
    while (1) {
	c = im_GetCharacter(textview_GetIM(tv));
	switch (c) {
	  case CTRL_L:
	    im_RedrawWindow(textview_GetIM(tv));
	    break;
	  case CTRL_G:
	    dynstr_destroy(&prompt);
	    dynstr_destroy(&pattern);
	    statestack_destroy(&stack);
	    message_DisplayString(tv, 0, "Cancelled.");
	    im_ForceUpdate();
	    return;
	  case CTRL_H:
	  case DEL:
	    break;
	  case CTRL_Q:
	    while ((c = im_GetCharacter(textview_GetIM(tv))) == EOF)
		;
	    dynstr_addchar(&pattern, c);
	    goto compilestate;
	  case CTRL_R:
	  case CTRL_S:
	    forwardp = (c == CTRL_S);
	    goto initialstate;
	  case CTRL_W:
	    if (textview_GetDotLength(tv) > 0)
		goto appendselectionstate;
	    break;
	  case CTRL_Y:
	    goto appendkillheadstate;
	  case ESC:
	    wasmeta = im_WasMeta(textview_GetIM(tv));
	    /* Fall through */
	  case EOF:
	    goto exitstate;
	  default:
	    if (isascii(c) && (isprint(c) || isspace(c))) {
		dynstr_addchar(&pattern, c);
		goto compilestate;
	    }
	    else {
		dodokey = 1;
		goto exitstate;
	    }
	}
    }

  compilestate:

    compiled = NULL;
    if ((compileerr = search_CompilePattern(pattern.text,
					    &compiled)) == NULL)
	goto searchstate;
    statestack_push(&stack, pattern.used, StackTop->wrappedp,
		    StackTop->failurep, StackTop->position, StackTop->length,
		    StackTop->searchfrom, forwardp);
    goto partialstate;

  searchstate:

    if (forwardp)
	foundloc = search_MatchPattern(txt, StackTop->searchfrom, compiled);
    else
	foundloc = search_MatchPatternReverse(txt, StackTop->searchfrom,
					      compiled);
    if (foundloc >= 0) {
	statestack_push(&stack, pattern.used, StackTop->wrappedp,
			0, foundloc, search_GetMatchLength(),
			StackTop->searchfrom, forwardp);
	textview_SetDotPosition(tv, foundloc);
	textview_SetDotLength(tv, search_GetMatchLength());
	textview_FrameDot(tv, foundloc);
	goto successstate;
    }
    statestack_push(&stack, pattern.used, StackTop->wrappedp,
		    1, StackTop->position, StackTop->length,
		    StackTop->searchfrom, forwardp);
    if (im_IsPlaying()) {
	im_CancelMacro();	/* This section should emulate exitstate */
	
	if (!dynstr_empty(&pattern))
	    dynstr_copy(&LastPattern, &pattern);
	dynstr_destroy(&prompt);
	dynstr_destroy(&pattern);
	statestack_destroy(&stack);
	mark_SetPos(tv->atMarker, origpos);
	mark_SetLength(tv->atMarker, origlen);
	message_DisplayString(tv, 0,
			      "Search failed, macro aborted, mark set.");
	im_ForceUpdate();
	return;
    }
    goto failurestate;

  newsearchstate:

    if (forwardp)
	foundloc = search_MatchPattern(txt, newsearchfrom, compiled);
    else
	foundloc = search_MatchPatternReverse(txt, newsearchfrom,
					      compiled);
    if (foundloc >= 0) {
	statestack_push(&stack, pattern.used, StackTop->wrappedp,
			0, foundloc, search_GetMatchLength(),
			newsearchfrom, forwardp);
	textview_SetDotPosition(tv, foundloc);
	textview_SetDotLength(tv, search_GetMatchLength());
	textview_FrameDot(tv, foundloc);
	goto successstate;
    }
    statestack_push(&stack, pattern.used, StackTop->wrappedp,
		    1, StackTop->position, StackTop->length,
		    StackTop->searchfrom, forwardp);
    if (im_IsPlaying()) {
	im_CancelMacro();	/* This section should emulate exitstate */
	
	if (!dynstr_empty(&pattern))
	    dynstr_copy(&LastPattern, &pattern);
	dynstr_destroy(&prompt);
	dynstr_destroy(&pattern);
	statestack_destroy(&stack);
	mark_SetPos(tv->atMarker, origpos);
	mark_SetLength(tv->atMarker, origlen);
	message_DisplayString(tv, 0,
			      "Search failed, macro aborted, mark set.");
	im_ForceUpdate();
	return;
    }
    goto failurestate;

  wrapsearchstate:

    if (forwardp)
	foundloc = search_MatchPattern(txt, (long) 0, compiled);
    else
	foundloc = search_MatchPatternReverse(txt, text_GetLength(txt),
					      compiled);
    if (foundloc >= 0) {
	statestack_push(&stack, pattern.used, 1,
			0, foundloc, search_GetMatchLength(),
			(long) 0, forwardp);
	textview_SetDotPosition(tv, foundloc);
	textview_SetDotLength(tv, search_GetMatchLength());
	textview_FrameDot(tv, foundloc);
	goto successstate;
    }
    statestack_push(&stack, pattern.used, 1,
		    1, StackTop->position, StackTop->length,
		    StackTop->searchfrom, forwardp);
    if (im_IsPlaying()) {
	im_CancelMacro();	/* This section should emulate exitstate */
	
	if (!dynstr_empty(&pattern))
	    dynstr_copy(&LastPattern, &pattern);
	dynstr_destroy(&prompt);
	dynstr_destroy(&pattern);
	statestack_destroy(&stack);
	mark_SetPos(tv->atMarker, origpos);
	mark_SetLength(tv->atMarker, origlen);
	message_DisplayString(tv, 0,
			      "Search failed, macro aborted, mark set.");
	im_ForceUpdate();
	return;
    }
    goto failurestate;

  partialstate:

    dynstr_put(&prompt, "");
    if (StackTop->failurep)
	dynstr_append(&prompt, "Failing ");
    if (StackTop->wrappedp) {
	if (StackTop->failurep)
	    dynstr_addchar(&prompt, 'w');
	else
	    dynstr_addchar(&prompt, 'W');
	dynstr_append(&prompt, "rapped ");
    }
    dynstr_append(&prompt, "I-Search");
    if (!forwardp)
	dynstr_append(&prompt, " backward");
    dynstr_append(&prompt, ": ");
    dynstr_append(&prompt, pattern.text);
    dynstr_append(&prompt, "  [incomplete input");
    if (compileerr) {
	dynstr_append(&prompt, " - ");
	dynstr_append(&prompt, compileerr);
    }
    dynstr_append(&prompt, "]");
    message_DisplayString(tv, 0, prompt.text);
    im_ForceUpdate();
    while (1) {
	c = im_GetCharacter(textview_GetIM(tv));
	switch (c) {
	  case CTRL_L:
	    im_RedrawWindow(textview_GetIM(tv));
	    break;
	  case CTRL_G:
	    dynstr_destroy(&prompt);
	    dynstr_destroy(&pattern);
	    statestack_destroy(&stack);
	    message_DisplayString(tv, 0, "Cancelled.");
	    im_ForceUpdate();
	    return;
	  case CTRL_H:
	  case DEL:
	    goto popstate;
	  case CTRL_Q:
	    while ((c = im_GetCharacter(textview_GetIM(tv))) == EOF)
		;
	    dynstr_addchar(&pattern, c);
	    goto compilestate;
	  case CTRL_R:
	  case CTRL_S:
	    forwardp = (c == CTRL_S);
	    break;
	  case CTRL_W:
	    goto appendselectionstate;
	  case CTRL_Y:
	    goto appendkillheadstate;
	  case ESC:
	    wasmeta = im_WasMeta(textview_GetIM(tv));
	    /* Fall through */
	  case EOF:
	    goto exitstate;
	  default:
	    if (isascii(c) && (isprint(c) || isspace(c))) {
		dynstr_addchar(&pattern, c);
		goto compilestate;
	    }
	    else {
		dodokey = 1;
		goto exitstate;
	    }
	}
    }

  exitstate:

    if (!dynstr_empty(&pattern))
	dynstr_copy(&LastPattern, &pattern);
    dynstr_destroy(&prompt);
    dynstr_destroy(&pattern);
    statestack_destroy(&stack);
    mark_SetPos(tv->atMarker, origpos);
    mark_SetLength(tv->atMarker, origlen);
    message_DisplayString(tv, 0, "Mark set.");
    im_ForceUpdate();
    if (dodokey)
	im_DoKey(textview_GetIM(tv), c);
    else {
	if (wasmeta) {
	    im_DoKey(textview_GetIM(tv), ESC);
	    im_DoKey(textview_GetIM(tv), im_GetCharacter(textview_GetIM(tv)));
	}
    }
    return;
    
  popstate:

    statestack_pop(&stack);
    goto poppedstate;

  popbeforeerrorstate:

    while (StackTop->failurep) {
	statestack_pop(&stack);
    }
    goto poppedstate;

  poppedstate:

    textview_SetDotPosition(tv, StackTop->position);
    textview_SetDotLength(tv, StackTop->length);
    textview_FrameDot(tv, StackTop->position);
    dynstr_shortento(&pattern, StackTop->patternlen);
    forwardp = StackTop->forwardp;
    if (dynstr_empty(&pattern))
	goto emptypatternstate;
    compiled = NULL;
    if (compileerr = search_CompilePattern(pattern.text, &compiled))
	goto partialstate;
    if (StackTop->failurep)
	goto failurestate;
    goto successstate;

  appendselectionstate:

    if (!(tmpbuf = malloc(1 + textview_GetDotLength(tv)))) {
	longjmp(jmpenv, 1);
    }
    text_CopySubString(txt, textview_GetDotPosition(tv),
		       textview_GetDotLength(tv), tmpbuf, FALSE);
    dynstr_append(&pattern, tmpbuf);
    free(tmpbuf);
    goto compilestate;

  appendkillheadstate:

    tmpfile = im_FromCutBuffer(textview_GetIM(tv));
    while ((c = fgetc(tmpfile)) != EOF)
	dynstr_addchar(&pattern, c);
    im_CloseFromCutBuffer(textview_GetIM(tv), tmpfile);
    goto compilestate;

  successstate:

    dynstr_put(&prompt, "");
    if (StackTop->wrappedp)
	dynstr_append(&prompt, "Wrapped ");
    dynstr_append(&prompt, "I-Search");
    if (!forwardp)
	dynstr_append(&prompt, " backward");
    dynstr_append(&prompt, ": ");
    dynstr_append(&prompt, pattern.text);
    message_DisplayString(tv, 0, prompt.text);
    im_ForceUpdate();
    while (1) {
	c = im_GetCharacter(textview_GetIM(tv));
	switch (c) {
	  case CTRL_L:
	    im_RedrawWindow(textview_GetIM(tv));
	    break;
	  case CTRL_G:
	    dynstr_destroy(&prompt);
	    dynstr_destroy(&pattern);
	    statestack_destroy(&stack);
	    textview_SetDotPosition(tv, origpos);
	    textview_SetDotLength(tv, origlen);
	    textview_FrameDot(tv, origpos);
	    message_DisplayString(tv, 0, "Cancelled.");
	    im_ForceUpdate();
	    return;
	  case CTRL_H:
	  case DEL:
	    goto popstate;
	  case CTRL_Q:
	    while ((c = im_GetCharacter(textview_GetIM(tv))) == EOF)
		;
	    dynstr_addchar(&pattern, c);
	    goto compilestate;
	  case CTRL_R:
	  case CTRL_S:
	    oldforwardp = forwardp;
	    forwardp = (c == CTRL_S);
	    if (oldforwardp == forwardp) {
		if (forwardp)
		    newsearchfrom = StackTop->position + 1;
		else
		    newsearchfrom = StackTop->position - 1;
		goto newsearchstate;
	    }
	    goto successstate;
	  case CTRL_W:
	    if (textview_GetDotLength(tv) > 0)
		goto appendselectionstate;
	    break;
	  case CTRL_Y:
	    goto appendkillheadstate;
	  case ESC:
	    wasmeta = im_WasMeta(textview_GetIM(tv));
	    /* Fall through */
	  case EOF:
	    goto exitstate;
	  default:
	    if (isascii(c) && (isprint(c) || isspace(c))) {
		dynstr_addchar(&pattern, c);
		goto compilestate;
	    }
	    else {
		dodokey = 1;
		goto exitstate;
	    }
	}
    }

  failurestate:

    dynstr_put(&prompt, "Failing ");
    if (StackTop->wrappedp)
	dynstr_append(&prompt, "wrapped ");
    dynstr_append(&prompt, "I-search");
    if (!forwardp)
	dynstr_append(&prompt, " backward");
    dynstr_append(&prompt, ": ");
    dynstr_append(&prompt, pattern.text);
    message_DisplayString(tv, 0, prompt.text);
    im_ForceUpdate();
    while (1) {
	c = im_GetCharacter(textview_GetIM(tv));
	switch (c) {
	  case CTRL_L:
	    im_RedrawWindow(textview_GetIM(tv));
	    break;
	  case CTRL_G:
	    goto popbeforeerrorstate;
	  case CTRL_H:
	  case DEL:
	    goto popstate;
	  case CTRL_Q:
	    while ((c = im_GetCharacter(textview_GetIM(tv))) == EOF)
		;
	    dynstr_addchar(&pattern, c);
	    goto compilestate;
	  case CTRL_R:
	  case CTRL_S:
	    oldforwardp = forwardp;
	    forwardp = (c == CTRL_S);
	    if (oldforwardp == forwardp)
		goto wrapsearchstate;
	    goto searchstate;
	  case CTRL_W:
	    if (textview_GetDotLength(tv) > 0)
		goto appendselectionstate;
	    break;
	  case CTRL_Y:
	    goto appendkillheadstate;
	  case ESC:
	    wasmeta = im_WasMeta(textview_GetIM(tv));
	    /* Fall through */
	  case EOF:
	    goto exitstate;
	  default:
	    if (isascii(c) && (isprint(c) || isspace(c))) {
		dynstr_addchar(&pattern, c);
		goto compilestate;
	    }
	    else {
		dodokey = 1;
		goto exitstate;
	    }
	}
    }
}

static void     fsearch(tv, key)
struct textview *tv;
long            key;
{
    dosearch(tv, 1);
}

static void     rsearch(tv, key)
struct textview *tv;
long            key;
{
    dosearch(tv, 0);
}

boolean         gsearch__InitializeClass()
{
    static struct bind_Description fns[] = {
        {"gsearch-forward", NULL, 0, NULL, 0, 0, fsearch,
        "Search forward incrementally.", "gsearch"},
        {"gsearch-backward", NULL, 0, NULL, 0, 0, rsearch,
	     "Search backward incrementally.", "gsearch"},
        {NULL},
    };
    struct classinfo *textviewClassinfo;

    if (dynstr_init(&LastPattern))
	return FALSE;
    textviewClassinfo = class_Load("textview");
    if (textviewClassinfo != NULL) {
        bind_BindList(fns, NULL, NULL, textviewClassinfo);
        return TRUE;
    }
    else
        return FALSE;
}
