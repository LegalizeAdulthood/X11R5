/*#include "copyright.h"*/

/* $XConsortium: XKeyBind.c,v 11.58 89/12/11 19:09:38 rws Exp $ */
/* Copyright 1985, 1987, Massachusetts Institute of Technology */

/* Beware, here be monsters (still under construction... - JG */

#define NEED_EVENTS
#include "Xlib.h"
#include "Xlibint.h"
#include "Xutil.h"
#define XK_MISCELLANY
#define XK_LATIN1
#define XK_LATIN2
#define XK_LATIN3
#define XK_LATIN4
#include <X11/keysymdef.h>
#include <stdio.h>
#include <X11/Sunkeysym.h>

#define AllMods (ShiftMask|LockMask|ControlMask| \
		 Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask)

#define DYNAMIC_MODS_MASK	0xFFF8	 /* mask out Shift, Caps, Ctrl */

static ComputeMaskFromKeytrans();
static int Initialize();
static void XConvertCase();

struct XKeytrans {
	struct XKeytrans *next;/* next on list */
	char *string;		/* string to return when the time comes */
	int len;		/* length of string (since NULL is legit)*/
	KeySym key;		/* keysym rebound */
	unsigned int state;	/* modifier state */
	KeySym *modifiers;	/* modifier keysyms you want */
	int mlen;		/* length of modifier list */
};

typedef struct {
    KeySym	*compose_ptr;
    int		 chars_matched;
}   TempXComposeStatus;

static KeySym
KeyCodetoKeySym(dpy, keycode, col)
    register Display *dpy;
    KeyCode keycode;
    int col;
{
    register int per = dpy->keysyms_per_keycode;
    register KeySym *syms;
    KeySym lsym, usym;

    if ((col < 0) || ((col >= per) && (col > 3)) ||
	(keycode < dpy->min_keycode) || (keycode > dpy->max_keycode))
      return NoSymbol;

    syms = &dpy->keysyms[(keycode - dpy->min_keycode) * per];
    if (col < 4) {
	if (col > 1) {
	    while ((per > 2) && (syms[per - 1] == NoSymbol))
		per--;
	    if (per < 3)
		col -= 2;
	}
	if ((per <= (col|1)) || (syms[col|1] == NoSymbol)) {
	    XConvertCase(dpy, syms[col&~1], &lsym, &usym);
	    if (!(col & 1))
		return lsym;
	    else if (usym == lsym)
		return NoSymbol;
	    else
		return usym;
	}
    }
    return syms[col];
}

#if NeedFunctionPrototypes
KeySym
XKeycodeToKeysym(Display *dpy,
#if NeedWidePrototypes
		 unsigned int kc,
#else
		 KeyCode kc,
#endif
		 int col)
#else
KeySym
XKeycodeToKeysym(dpy, kc, col)
    Display *dpy;
    KeyCode kc;
    int col;
#endif
{
    if ((! dpy->keysyms) && (! Initialize(dpy)))
	return NoSymbol;
    return KeyCodetoKeySym(dpy, kc, col);
}

KeyCode
XKeysymToKeycode(dpy, ks)
    Display *dpy;
    KeySym ks;
{
    register int i, j;

    if ((! dpy->keysyms) && (! Initialize(dpy)))
	return (KeyCode) 0;
    for (i = dpy->min_keycode; i <= dpy->max_keycode; i++) {
	for (j = 0; j < dpy->keysyms_per_keycode; j++) {
	    if (KeyCodetoKeySym(dpy, (KeyCode) i, j) == ks)
		return i;
	}
    }
    return 0;
}

KeySym
XLookupKeysym(event, col)
    register XKeyEvent *event;
    int col;
{
    if ((! event->display->keysyms) && (! Initialize(event->display)))
	return NoSymbol;
    return KeyCodetoKeySym(event->display, event->keycode, col);
}

static int
InitModMap(dpy)
    Display *dpy;
{
    register XModifierKeymap *map;
    register int i, j, n;
    KeySym sym;
    register struct XKeytrans *p;

    if (! (dpy->modifiermap = map = XGetModifierMapping(dpy)))
	return 0;
    if ((! dpy->keysyms) && (! Initialize(dpy)))
	return 0;
    LockDisplay(dpy);
    /* If any Lock key contains Caps_Lock, then interpret as Caps_Lock,
     * else if any contains Shift_Lock, then interpret as Shift_Lock,
     * else ignore Lock altogether.
     */
    dpy->lock_meaning = NoSymbol;
    /* Lock modifiers are in the second row of the matrix */
    n = 2 * map->max_keypermod;
    for (i = map->max_keypermod; i < n; i++) {
	for (j = 0; j < dpy->keysyms_per_keycode; j++) {
	    sym = KeyCodetoKeySym(dpy, map->modifiermap[i], j);
	    if (sym == XK_Caps_Lock) {
		dpy->lock_meaning = XK_Caps_Lock;
		break;
	    } else if (sym == XK_Shift_Lock) {
		dpy->lock_meaning = XK_Shift_Lock;
	    }
	}
    }
    /* Now find any Mod<n> modifier acting as the Group modifier */
    dpy->mode_switch = 0;
    n *= 4;
    for (i = 3*map->max_keypermod; i < n; i++) {
	for (j = 0; j < dpy->keysyms_per_keycode; j++) {
	    sym = KeyCodetoKeySym(dpy, map->modifiermap[i], j);
	    if (sym == XK_Mode_switch)
		dpy->mode_switch |= 1 << (i / map->max_keypermod);
	}
    }
    for (p = dpy->key_bindings; p; p = p->next)
	ComputeMaskFromKeytrans(dpy, p);
    UnlockDisplay(dpy);
    return 1;
}

XRefreshKeyboardMapping(event)
    register XMappingEvent *event;
{

    if(event->request == MappingKeyboard) {
	/* XXX should really only refresh what is necessary
	 * for now, make initialize test fail
	 */
	LockDisplay(event->display);
	if (event->display->keysyms) {
	     Xfree ((char *)event->display->keysyms);
	     event->display->keysyms = NULL;
	}
	UnlockDisplay(event->display);
    }
    if(event->request == MappingModifier) {
	LockDisplay(event->display);
	if (event->display->modifiermap) {
	    XFreeModifiermap(event->display->modifiermap);
	    event->display->modifiermap = NULL;
	}
	UnlockDisplay(event->display);
	/* go ahead and get it now, since initialize test may not fail */
	(void) InitModMap(event->display);
    }
}

static int
Initialize(dpy)
    Display *dpy;
{
    int per, n;
    KeySym *keysyms;

    /* 
     * lets go get the keysyms from the server.
     */
    if (!dpy->keysyms) {
	n = dpy->max_keycode - dpy->min_keycode + 1;
	keysyms = XGetKeyboardMapping (dpy, (KeyCode) dpy->min_keycode,
				       n, &per);
	/* keysyms may be NULL */
	if (! keysyms) return 0;

	LockDisplay(dpy);
	dpy->keysyms = keysyms;
	dpy->keysyms_per_keycode = per;
	UnlockDisplay(dpy);
    }
    if (!dpy->modifiermap)
        return InitModMap(dpy);
    return 1;
}

/*ARGSUSED*/
static void
XConvertCase(dpy, sym, lower, upper)
    Display *dpy;
    register KeySym sym;
    KeySym *lower;
    KeySym *upper;
{
    *lower = sym;
    *upper = sym;
    switch(sym >> 8) {
    case 0:
	if ((sym >= XK_A) && (sym <= XK_Z))
	    *lower += (XK_a - XK_A);
	else if ((sym >= XK_a) && (sym <= XK_z))
	    *upper -= (XK_a - XK_A);
	else if ((sym >= XK_Agrave) && (sym <= XK_Odiaeresis))
	    *lower += (XK_agrave - XK_Agrave);
	else if ((sym >= XK_agrave) && (sym <= XK_odiaeresis))
	    *upper -= (XK_agrave - XK_Agrave);
	else if ((sym >= XK_Ooblique) && (sym <= XK_Thorn))
	    *lower += (XK_oslash - XK_Ooblique);
	else if ((sym >= XK_oslash) && (sym <= XK_thorn))
	    *upper -= (XK_oslash - XK_Ooblique);
	break;
    default:
	/* XXX do all other sets */
	break;
    }
}


static int
XIsKeypadSym(sym)
    KeySym sym;
{
    /*
     * This is dependent upon XK_KP_xxxxxx definitions in keysymdef.h
     * We could make this a macro...
     */

    if (sym >= XK_KP_Space && sym <= XK_KP_Equal)
	return(1);
    else
	return(0);
}


static int
XTranslateKey(dpy, keycode, modifiers, modifiers_return, keysym_return)
    register Display *dpy;
    KeyCode keycode;
    register unsigned int modifiers;
    unsigned int *modifiers_return;
    KeySym *keysym_return;
{
    int per, i, j, n, k;
    register KeySym *syms;
    KeySym modsym, sym, lsym, usym;
    XModifierKeymap *map;
    int NumMask = 0;

    if ((! dpy->keysyms) && (! Initialize(dpy)))
	return 0;

    /*
     * REMIND: what is this? what about mod1 thru mod5?
     */
    *modifiers_return = (ShiftMask|LockMask) | dpy->mode_switch;

    if ((keycode < dpy->min_keycode) || (keycode > dpy->max_keycode))  {
	*keysym_return = NoSymbol;
	return 1;
    }
    per = dpy->keysyms_per_keycode;
    syms = &dpy->keysyms[(keycode - dpy->min_keycode) * per];
    while ((per > 2) && (syms[per - 1] == NoSymbol))
	per--;
    
    if ((per > 2) && (modifiers & dpy->mode_switch)) {
	syms += 2;
	per -= 2;
    }

    if (modifiers & DYNAMIC_MODS_MASK && !(modifiers & dpy->mode_switch)) {
	/*
	 * Some dynamic modifier is in effect (mod1 - mod5).
	 * Find out which keysym is attached to each modifier in effect
	 * and build a mask for each (only NumLock supported now).
	 */
	map = dpy->modifiermap;
	n = map->max_keypermod * 8;

	for (i=3; i < 8; i++) {		/* foreach dynamic modifier */
	    if (! (modifiers & (1 << i)))
		continue;
	    k = i * map->max_keypermod;
	    for (j = 0; j < map->max_keypermod; j++) {
		if (map->modifiermap[k+j] == NoSymbol)
		    break;

		/* determine keysym of this modifier map entry */
		modsym = dpy->keysyms[(map->modifiermap[k+j] -
			 dpy->min_keycode) * dpy->keysyms_per_keycode];

		if (modsym == XK_Num_Lock)
		    NumMask = (1 << i);

		/* Add tests for other dynamic modifiers here */
	    }	    
	}	    
    }

    /*
     * REMIND: This needs to be rewritten, no?
     */
    if (!(modifiers & ShiftMask) &&
	(!(modifiers & LockMask) || (dpy->lock_meaning == NoSymbol))) {
	/* no Shift and no CapsLock on but other modifiers may be */
	if ((modifiers & NumMask) && (XIsKeypadSym(syms[1])))
	    *keysym_return = syms[1];
	else if ((per == 1) || (syms[1] == NoSymbol))
	    XConvertCase(dpy, syms[0], keysym_return, &usym);
	else
	    *keysym_return = syms[0];
    } else if (!(modifiers & LockMask) || (dpy->lock_meaning != XK_Caps_Lock)) {
	/* no CapsLock on but lock and other modifiers may be in effect */
	if ((modifiers & NumMask) && (XIsKeypadSym(syms[1]))) {
	    if (modifiers & ShiftMask)
	        *keysym_return = syms[0];
	    else
	        *keysym_return = syms[1];
	}
	else {
	    if ((per == 1) || ((usym = syms[1]) == NoSymbol))
	        XConvertCase(dpy, syms[0], &lsym, &usym);
	    *keysym_return = usym;
	}
    } else {
	/* CapsLock on, possibly other modifiers, too */
        if ((modifiers & NumMask) && XIsKeypadSym(syms[1])) {
	    if (modifiers & ShiftMask)
	        *keysym_return = syms[0];
	    else
	        *keysym_return = syms[1];
	} else {
	    if ((per == 1) || ((sym = syms[1]) == NoSymbol))
	        sym = syms[0];
	    XConvertCase(dpy, sym, &lsym, &usym);
	    if (!(modifiers & ShiftMask) && (sym != syms[0]) &&
	            ((sym != usym) || (lsym == usym)))
	        XConvertCase(dpy, syms[0], &lsym, &usym);
	    *keysym_return = usym;
	}
    }

    if (*keysym_return == XK_VoidSymbol)
	*keysym_return = NoSymbol;
    return 1;
}

static int
XTranslateKeySym(dpy, keysym, modifiers, buffer, nbytes, event, status)
    Display *dpy;
    KeySym *keysym;
    unsigned int modifiers;
    char *buffer;
    int nbytes;
    XKeyEvent *event;
    TempXComposeStatus *status;
{
    register struct XKeytrans *p; 
    int length;
    unsigned long hiBytes;
    register unsigned char c;
    register KeySym  symbol;
    static int	HandleComposeSequence();
    int		return_val;

    if (!keysym)
	return 0;
    else
        symbol = *keysym;
    /* see if symbol rebound, if so, return that string. */
    for (p = dpy->key_bindings; p; p = p->next) {
	if (((modifiers & AllMods) == p->state) && (symbol == p->key)) {
	    length = p->len;
	    if (length > nbytes) length = nbytes;
	    bcopy (p->string, buffer, length);
	    return length;
	}
    }
    /* try to convert to Latin-1, handling control */
    hiBytes = symbol >> 8;
    /*
     * The check for hiBytes == 0x10FF is Sun-specific.
     * We need the the check so that Sun specific keysyms will pass 
     * through this check and onto to be handled by HandleComposeSequence
     */
    if (!(nbytes && 
	  ((hiBytes == 0) || (hiBytes == 0x1000FF) ||
	   ((hiBytes == 0xFF) &&
	    (((symbol >= XK_BackSpace) && (symbol <= XK_Clear)) ||
	     (symbol == XK_Multi_key) ||
	     (symbol == XK_Return) ||
	     (symbol == XK_Escape) ||
	     (symbol == XK_KP_Space) ||
	     (symbol == XK_KP_Tab) ||
	     (symbol == XK_KP_Enter) ||
	     ((symbol >= XK_KP_Multiply) && (symbol <= XK_KP_9)) ||
	     (symbol == XK_KP_Equal) ||
	     (symbol == XK_Delete))))))
	return 0;

    /*
     * if this is a compose sequence, then HandleComposeSequence already
     * deals w/ this. This is Sun-specific.
     */
    if (HandleComposeSequence(symbol, buffer, keysym, status, &return_val, event)) {
	return return_val;
    }
    /* if X keysym, convert to ascii by grabbing low 7 bits */
    if (symbol == XK_KP_Space)
	c = XK_space & 0x7F; /* patch encoding botch */
    else if (symbol == XK_hyphen)
	c = XK_minus & 0xFF; /* map to equiv character */
    else if (hiBytes == 0xFF)
	c = symbol & 0x7F;
    else
	c = symbol & 0xFF;
    /* only apply Control key if it makes sense, else ignore it */
    if (modifiers & ControlMask) {
	if ((c >= '@' && c < '\177') || c == ' ') c &= 0x1F;
	else if (c == '2') c = '\000';
	else if (c >= '3' && c <= '7') c -= ('3' - '\033');
	else if (c == '8') c = '\177';
	else if (c == '/') c = '_' & 0x1F;
    }
    buffer[0] = c;
    return 1;
}
  
int
XSunLookupString (event, buffer, nbytes, keysym, client_status)
    register XKeyEvent *event;
    char *buffer;	/* buffer */
    int nbytes;	/* space in buffer for characters */
    KeySym *keysym;
    XComposeStatus *client_status;	/* not implemented */
{
    return XLookupString (event, buffer, nbytes, keysym, client_status) ;
}
/*ARGSUSED*/
int
XLookupString (event, buffer, nbytes, keysym, client_status)
    register XKeyEvent *event;
    char *buffer;	/* buffer */
    int nbytes;	/* space in buffer for characters */
    KeySym *keysym;
    XComposeStatus *client_status;	/* not implemented */
{
    unsigned int modifiers;
    KeySym symbol;
    TempXComposeStatus	*status;
    extern char _XComposeStr[];

    if (! XTranslateKey(event->display, event->keycode, event->state,
		  &modifiers, &symbol))
	return 0;

    if (client_status) {
	client_status->compose_ptr = _XComposeStr;
    }
    status = (TempXComposeStatus *)client_status;
    if (keysym) {
	*keysym = symbol;
        /*
         * In Generic R4, XTranslateKeySym doesn't take the event argument
         * We need it here. Also in generic R4, symbol is passed, instead
         * of keysym.  We need to pass keysym because we might change
         * it if we're in the middle of a compose sequence.
         */
        /* arguable whether to use (event->state & ~modifiers) here */
        return XTranslateKeySym(event->display, keysym, event->state,
			        buffer, nbytes, event, status);
    } else {
	/* Here keysym came in NULL, so we will pass in &symbol to avoid
	 * dereferencing a null pointer.
	 */
        /* arguable whether to use (event->state & ~modifiers) here */
        return XTranslateKeySym(event->display, &symbol, event->state,
			        buffer, nbytes, event, status);
    }
}

#if NeedFunctionPrototypes
XRebindKeysym (
    Display *dpy,
    KeySym keysym,
    KeySym *mlist,
    int nm,		/* number of modifiers in mlist */
    const unsigned char *str,
    int nbytes)
#else
XRebindKeysym (dpy, keysym, mlist, nm, str, nbytes)
    Display *dpy;
    KeySym keysym;
    KeySym *mlist;
    int nm;		/* number of modifiers in mlist */
    unsigned char *str;
    int nbytes;
#endif
{
    register struct XKeytrans *tmp, *p;
    int nb;

    if ((! dpy->keysyms) && (! Initialize(dpy)))
	return;
    LockDisplay(dpy);
    tmp = dpy->key_bindings;
    nb = sizeof(KeySym) * nm;

    if ((! (p = (struct XKeytrans *) Xmalloc( sizeof(struct XKeytrans)))) ||
	((! (p->string = (char *) Xmalloc( (unsigned) nbytes))) && 
	 (nbytes > 0)) ||
	((! (p->modifiers = (KeySym *) Xmalloc( (unsigned) nb))) &&
	 (nb > 0))) {
	if (p) {
	    if (p->string) Xfree(p->string);
	    if (p->modifiers) Xfree((char *) p->modifiers);
	    Xfree((char *) p);
	}
	UnlockDisplay(dpy);
	return;
    }

    dpy->key_bindings = p;
    p->next = tmp;	/* chain onto list */
    bcopy ((char *) str, p->string, nbytes);
    p->len = nbytes;
    bcopy ((char *) mlist, (char *) p->modifiers, nb);
    p->key = keysym;
    p->mlen = nm;
    ComputeMaskFromKeytrans(dpy, p);
    UnlockDisplay(dpy);
    return;
}

_XFreeKeyBindings (dpy)
    Display *dpy;
{
    register struct XKeytrans *p, *np;

    for (p = dpy->key_bindings; p; p = np) {
	np = p->next;
	Xfree(p->string);
	Xfree((char *)p->modifiers);
	Xfree((char *)p);
    }   
}

/*
 * given a KeySym, returns the first keycode containing it, if any.
 */
static CARD8
FindKeyCode(dpy, code)
    register Display *dpy;
    register KeySym code;
{

    register KeySym *kmax = dpy->keysyms + 
	(dpy->max_keycode - dpy->min_keycode + 1) * dpy->keysyms_per_keycode;
    register KeySym *k = dpy->keysyms;
    while (k < kmax) {
	if (*k == code)
	    return (((k - dpy->keysyms) / dpy->keysyms_per_keycode) +
		    dpy->min_keycode);
	k += 1;
	}
    return 0;
}

	
/*
 * given a list of modifiers, computes the mask necessary for later matching.
 * This routine must lookup the key in the Keymap and then search to see
 * what modifier it is bound to, if any.  Sets the AnyModifier bit if it
 * can't map some keysym to a modifier.
 */
static
ComputeMaskFromKeytrans(dpy, p)
    Display *dpy;
    register struct XKeytrans *p;
{
    register int i;
    register CARD8 code;
    register XModifierKeymap *m = dpy->modifiermap;

    p->state = AnyModifier;
    for (i = 0; i < p->mlen; i++) {
	/* if not found, then not on current keyboard */
	if ((code = FindKeyCode(dpy, p->modifiers[i])) == 0)
		return;
	/* code is now the keycode for the modifier you want */
	{
	    register int j = m->max_keypermod<<3;

	    while ((--j >= 0) && (code != m->modifiermap[j]))
		;
	    if (j < 0)
		return;
	    p->state |= (1<<(j/m->max_keypermod));
	}
    }
    p->state &= AllMods;
}

static void
Do_Compose(key, keysym, new_keysym, c)
KeySym		 key;
KeySym	 	 keysym;
KeySym  	*new_keysym;
unsigned char	*c;
{
    int status;
    int valid_first;

    /* 
     * keysyms may be in either order, because one can do
     * 	1. compose-key.  2. a	3. circumflex
     * and
     *	2. compose-key.  2. circumflex  3. a
     * Both should return XK_acircumflex.
     */
    if ((status = find_string(key, keysym, new_keysym, c)) == 1)
	/* found the string */
	return;
    else {
	if (status == -1)
	    valid_first = 0;
	else
	    valid_first = 1;

	if (find_string(keysym, key, new_keysym, c) == 1)
	    return;

	if (valid_first == 1)
	    *new_keysym = *c = NoSymbol;
	else if (keysym != XK_Dead_Grave
	         && keysym != XK_Dead_Tilde
	         && keysym != XK_Dead_Circum)
	    *c = *new_keysym;
	else
	    *new_keysym = *c = NoSymbol;
    }
/*
 * REMIND: Redesign!
 *
 * Code needs to properly handle three cases:
 * 1. Compose Valid_First_Char Valid_Second_Char ==> NewKeySym
 * 2. Compose Valid_First_Char Invalid_Second_Char ==> NoSymbol
 * 3. Compose Invalid_First_Char ==> Termination of Compose
 *
 * Currently handles four cases like this:
 * 1. Compose Valid_First_Char Valid_Second_Char ==> NewKeySym
 * 2. Compose Valid_First_Char Invalid_Second_Char ==> NoSymbol or Second_Char
 * 3. Compose Invalid_First_Char Non-DeadKey ==> Second_Char or TranslatedChar
 * 4. Compose Invalid_First_Char DeadKey ==> NoSymbol
 *
 * Example problem case: Compose Z DeadKey
 * Example problem case: Compose - Z
 * Example problem case: Compose a Z
 * Example problem case: Compose f '
 */
}


struct KeySym_map{
        KeySym    key;
	KeySym    result;
};

static int
find_string(first, second, new_keysym, c)
KeySym		first, second;
KeySym  	*new_keysym;
unsigned char	*c;
{
    int	result = 0;
    static int find_match();

    static struct KeySym_map   tilde_chain[4] = { 
    				XK_A, XK_Atilde,
				XK_O, XK_Otilde,
				XK_N, XK_Ntilde,
				0, 0
    };
    static struct KeySym_map   grave_chain[6] = {
    				XK_A, XK_Agrave,
				XK_E, XK_Egrave,
				XK_I, XK_Igrave,
				XK_O, XK_Ograve,
				XK_U, XK_Ugrave,
				0, 0
    };
    static struct KeySym_map   acute_chain[7] = {
    				XK_A, XK_Aacute,
				XK_E, XK_Eacute,
				XK_I, XK_Iacute,
				XK_O, XK_Oacute,
				XK_U, XK_Uacute,
				XK_Y, XK_Yacute,
				0, 0
    };
    static struct KeySym_map   diaeresis_chain[8] = {
    				XK_A, XK_Adiaeresis,
				XK_E, XK_Ediaeresis,
				XK_I, XK_Idiaeresis,
				XK_O, XK_Odiaeresis,
				XK_U, XK_Udiaeresis,
				XK_y, XK_ydiaeresis,
				XK_diaeresis, XK_diaeresis,
				0, 0
    };
    static struct KeySym_map   circum_chain[13] = {
    				XK_A, XK_Acircumflex,
				XK_E, XK_Ecircumflex,
				XK_I, XK_Icircumflex,
				XK_O, XK_Ocircumflex,
				XK_U, XK_Ucircumflex,
				XK_asterisk, XK_degree,
				XK_0, XK_degree,
				XK_1, XK_onesuperior,
				XK_2, XK_twosuperior,
				XK_3, XK_threesuperior,
				XK_period, XK_periodcentered,
				XK_minus, XK_macron,
				0, 0
    };
    static struct KeySym_map	minus_chain[14] = {
				/* no upper case equivalents here */
				XK_L, XK_sterling,
				XK_l, XK_sterling,
				XK_Y, XK_yen,
				XK_y, XK_yen,
				XK_A, XK_ordfeminine,
				XK_a, XK_ordfeminine,

				XK_comma, XK_notsign,
				XK_bar, XK_notsign,
				XK_minus, XK_hyphen,
				XK_asciicircum, XK_macron,
				XK_plus, XK_plusminus,
				XK_D, XK_ETH,
				XK_colon, XK_division,
				0, 0
    };
    
    
    switch (first) {
      case XK_Dead_Tilde:
          result = find_match(tilde_chain, second, new_keysym, c);
	  break;
      case XK_Dead_Grave:
          result = find_match(grave_chain, second, new_keysym, c);
	  break;
      case XK_Dead_Circum:
          result = find_match(circum_chain, second, new_keysym, c);
	  break;
      case XK_diaeresis:
          result = find_match(diaeresis_chain, second, new_keysym, c);
	  break;
      case XK_acute:
          result = find_match(acute_chain, second, new_keysym, c);
	  break;
      case XK_minus:
          result = find_match(minus_chain, second, new_keysym, c);
	  break;

      case XK_space:
          if (second == XK_space) {
	      *new_keysym = *c = XK_nobreakspace;
	      result = 1;
	  }
      case XK_exclam:
          if (second == XK_exclam) {
	      *new_keysym = *c = XK_exclamdown;
	      result = 1;
	  }
	  break;
      case XK_question:
          if (second == XK_question) {
	      *new_keysym = *c = XK_questiondown;
	      result = 1;
	  }
	  break;
      case XK_slash:
          if (second == XK_O) {
	      *new_keysym = *c = XK_Ooblique;
	      result = 1;
	  } else if (second == XK_o) {
	      *new_keysym = *c = XK_oslash;
	      result = 1;
	  } else if (second == XK_u) {
	      *new_keysym = *c = XK_mu;
	      result = 1;
	  }
	  break;
      case XK_underscore:
          if (second == XK_O || second == XK_o) {
	      *new_keysym = *c = XK_masculine;
	      result = 1;
	  }
	  break;
      case XK_bar:
          if (second == XK_bar) {
	      *new_keysym = *c = XK_brokenbar;
	      result = 1;
	  }
	  break;
      case XK_backslash:
          if (second == XK_backslash) {
	      *new_keysym = *c = XK_acute;
	      result = 1;
	  }
	  break;
      case XK_cedilla:
          if (second == XK_C){
	      *new_keysym = *c = XK_Ccedilla;
	      result = 1;
	  } else if (second == XK_c) {
	      *new_keysym = *c = XK_ccedilla;
	      result = 1;
	  }
	  break;
      case XK_comma:
          if (second == XK_comma){
	      *new_keysym = *c = XK_cedilla;
	      result = 1;
	  } else if (second == XK_minus) {
	      *new_keysym = *c = XK_notsign;
	      result = 1;
	  }
	  break;
      case XK_less:
          if (second == XK_less) {
	      *new_keysym = XK_guillemotleft;
	      *c = XK_guillemotleft;
	      result = 1;
	  }
	  break;
      case XK_greater:
          if (second == XK_greater) {
	      *new_keysym = *c = XK_guillemotright;
	      result = 1;
	  }
	  break;
      case XK_A:
          if (second == XK_E) {
	      *new_keysym = *c = XK_AE;
	      result = 1;
	  } else if (second == XK_asterisk) {
	      *new_keysym = *c = XK_Aring;
	      result = 1;
	  }
	  break;
      case XK_1:
          if (second == XK_4) {
	      *new_keysym = XK_onequarter;
	      *c = XK_onequarter;
	      result = 1;
	  } else if (second == XK_2) {
	      *new_keysym = XK_onehalf;
	      *c = XK_onehalf;
	      result = 1;
	  }
	  break;
      case XK_3:
          if (second == XK_4) {
	      *new_keysym = XK_threequarters;
	      *c = XK_threequarters;
	      result = 1;
	  }
	  break;
      case XK_a:
          if (second == XK_e) {
	      *new_keysym = *c = XK_ae;
	      result = 1;
	  } else if (second == XK_asterisk) {
	      *new_keysym = *c = XK_aring;
	      result = 1;
	  }
	  break;
      case XK_c:
          if (second == XK_slash) {
	      *new_keysym = *c = XK_cent;
	      result = 1;
	  } else if (second == XK_o) {
	      *new_keysym = *c = XK_copyright;
	      result = 1;
	  }
	  else if (second == XK_comma) {
	      *new_keysym = *c = XK_ccedilla;
	      result = 1;
	  }
	  break;
      case XK_C:
          if (second == XK_slash) {
	      *new_keysym = *c = XK_cent;
	      result = 1;
	  } else if (second == XK_O) {
	      *new_keysym = *c = XK_copyright;
	      result = 1;
	  }
	  else if (second == XK_comma) {
	      *new_keysym = *c = XK_Ccedilla;
	      result = 1;
	  }
	  break;
      case XK_p:
          if (second == XK_bar) {
	      *new_keysym = *c = XK_thorn;	/* lower */
	      result = 1;
	  } else if (second == XK_exclam) {
	      *new_keysym = *c = XK_paragraph;
	      result = 1;
	  }
	  break;
      case XK_P:
          if (second == XK_bar) {
	      *new_keysym = *c = XK_Thorn;	/* upper */
	      result = 1;
	  } else if (second == XK_exclam) {
	      *new_keysym = *c = XK_paragraph;
	      result = 1;
	  }
	  break;
      case XK_r:
          if (second == XK_o) {
	      *new_keysym = *c = XK_registered;
	      result = 1;
	  }
	  break;
      case XK_R:
          if (second == XK_O) {
	      *new_keysym = *c = XK_registered;
	      result = 1;
	  }
	  break;
      case XK_s:
          if (second == XK_s) {
	      *new_keysym = *c = XK_ssharp;
	      result = 1;
	  } else if (second == XK_o) {
	      *new_keysym = *c = XK_section;
	      result = 1;
	  }
	  break;
      case XK_S:
          if (second == XK_O) {
	      *new_keysym = *c = XK_section;
	      result = 1;
	  }
	  break;
      case XK_T:
          if (second == XK_H) {
	      *new_keysym = *c = XK_Thorn;
	      result = 1;
	  }
	  break;
      case XK_t:
          if (second == XK_h) {
	      *new_keysym = *c = XK_thorn;
	      result = 1;
	  }
	  break;
      case XK_x:
          if (second == XK_o || second == XK_0) {   /* o or Zero */
	      *new_keysym = *c = XK_currency;
	      result = 1;
          } else if (second == XK_x) {
	      *new_keysym = *c = XK_multiply;
	      result = 1;
	  }
      case XK_X:
          if (second == XK_O || second == XK_0) {   /* O or Zero */
	      *new_keysym = *c = XK_currency;
	      result = 1;
          }
	  break;
      default:
	  *new_keysym = *c = NoSymbol;
	  result = -1;
	  break;
    }
    return result;
}


static int
find_match(map, key, new_keysym, string)
struct KeySym_map  	*map;
KeySym			 key, *new_keysym;
unsigned char		*string;
{
    *string = *new_keysym = NoSymbol;
    while (map->key) {
	if (map->key == key) {
	    *new_keysym = map->result;
	    *string = *new_keysym;
	    return 1;
	} else if (XK_A <= map->key && map->key <= XK_Z) {
	    /* oh, this stinks so bad! */
	    if (map->result != XK_sterling
		&& map->result != XK_yen
		&& map->result != XK_ordfeminine
		&& map->key + (XK_a - XK_A) == key) {
	        *new_keysym = map->result + (XK_aacute - XK_Aacute);
	        *string = *new_keysym;
	        return 1;
	    }
	    else
	        map++;
	} else
	    map++;
    }
    return 0;
}

#define LOOKUP_1CHAR	0x1000FF10
#define LOOKUP_2CHAR	0x1000FF11
#define HAVE_SEEN_UP	0x1000FF12
#define HAVE_SEEN_DOWN	0x1000FF13
/*
 * Called from XTranslateKeySym
 * Look at compose sequence and dead key sequence and try to get a 
 * Latin-1 character out of it.  Return 1 if it is able to resolve
 * the lookup based on the info it has, return 0 if it can't, then
 * XTranslateKeySym needs to do further processing.
 */
static int
HandleComposeSequence(symbol, buffer, keysym, status, return_val, event)
KeySym	symbol, *keysym;
char 	*buffer;
int	*return_val;
XKeyEvent	*event;
TempXComposeStatus	*status;
{
    unsigned char c = 0;
    
    if (status && symbol == XK_Multi_key) {
	/* if the user hits COMPOSE key, start compose sequence */
	status->compose_ptr[0] = XK_Multi_key;
	*keysym = NoSymbol;
    } else if (status  && status->compose_ptr[0] != XK_Multi_key &&
		  (symbol == XK_Dead_Grave || symbol == XK_Dead_Tilde || 
		   symbol == XK_Dead_Circum || symbol == XK_cedilla ||
		   symbol == XK_acute || symbol == XK_diaeresis)) {
	    /*
	     * if it's a dead key, then fake it as if the user has typed
	     * COMPOSE, some-accent-mark
	     */
	    status->compose_ptr[0] = XK_Multi_key;
	    if (event->type == KeyPress) {
		status->compose_ptr[1] = HAVE_SEEN_DOWN;
	    } else if (event->type == KeyRelease) {
		status->compose_ptr[1] = HAVE_SEEN_UP;
	    }
	    status->compose_ptr[2] = LOOKUP_1CHAR;
	    status->compose_ptr[3] = symbol;
	    *keysym = NoSymbol;
    } else if (status && status->compose_ptr[0] == XK_Multi_key) {
	if (symbol == XK_grave) {
	    symbol = XK_Dead_Grave; 
	} else if (symbol == XK_asciitilde) {
	    symbol = XK_Dead_Tilde;
	} else if (symbol == XK_asciicircum) {
	    symbol = XK_Dead_Circum;
	} else if (symbol == XK_quoteright) {
	    symbol = XK_acute;
	} else if (symbol == XK_quotedbl) {
	    symbol = XK_diaeresis;
	}
	if (status->compose_ptr[2] == LOOKUP_1CHAR) {
	    /*
	     * At this point, we have seen the 1st of the 2 characters
	     * after the Compose key. The question is whether the current
	     * character is the up of the 1st or is it actually
	     * the second character
	     */
	    if ((status->compose_ptr[1] == HAVE_SEEN_DOWN &&
		 event->type == KeyPress) || 
		(status->compose_ptr[1] == HAVE_SEEN_UP &&
		 event->type == KeyRelease)) {
		Do_Compose(status->compose_ptr[3], symbol, keysym, &c);
		status->compose_ptr[0] = status->compose_ptr[1] =
		status->compose_ptr[2] = status->compose_ptr[3] =
		status->compose_ptr[4] = 0;
	    } else if (status->compose_ptr[1] == HAVE_SEEN_UP &&
		event->type == KeyPress) {
		Do_Compose(status->compose_ptr[3], symbol, keysym, &c);
	    } else if (status->compose_ptr[1] == HAVE_SEEN_DOWN && 
		event->type == KeyRelease) {
		status->compose_ptr[1] = HAVE_SEEN_UP;
		/* this is the up of the 1st char after Compose */
		*keysym = NoSymbol;
	    }
	} else {
	    /* 
	     * this is the first of the 2 keystrokes following a compose
	     * key
	     */
	    if (event->type == KeyPress) {
		status->compose_ptr[1] = HAVE_SEEN_DOWN;
	    } else if (event->type == KeyRelease) {
		status->compose_ptr[1] = HAVE_SEEN_UP;
	    }
	    status->compose_ptr[2] = LOOKUP_1CHAR;
	    status->compose_ptr[3] = symbol;
	    *keysym = NoSymbol;
	}
    } else {
	return 0;
    }
    buffer[0] = c;
    if (*keysym == NoSymbol || c == 0) {
	*return_val = 0;
    } else {
	*return_val = 1;
    }
    return 1;
}
