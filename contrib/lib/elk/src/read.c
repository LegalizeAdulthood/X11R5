#include <ctype.h>
#include <math.h>       /* atof */

#include "scheme.h"

#ifdef TERMIO
#  include <termio.h>
#else
#  include <sys/ioctl.h>
#endif

extern char *index();

Object Sym_Quote,
       Sym_Quasiquote,
       Sym_Unquote,
       Sym_Unquote_Splicing;

static FILE *Last_File;

#define GETC    (str ? String_Getc (port) : getc (f))
#define UNGETC  {if (str) String_Ungetc (port,c); else (void)ungetc (c,f);}

#define Tweak_Stream(f) {if (!str && (feof (f) || ferror (f))) clearerr (f);}

#define Octal(c) ((c) >= '0' && (c) <= '7')

Object General_Read(), Read_Sequence(), Read_Atom(), Read_Special();
Object Read_String(), Read_Sharp();

Init_Read () {
    Define_Symbol (&Sym_Quote, "quote");
    Define_Symbol (&Sym_Quasiquote, "quasiquote");
    Define_Symbol (&Sym_Unquote, "unquote");
    Define_Symbol (&Sym_Unquote_Splicing, "unquote-splicing");
}

String_Getc (port) Object port; {
    register struct S_Port *p;
    register struct S_String *s;

    p = PORT(port);
    if (p->flags & P_UNREAD) {
	p->flags &= ~P_UNREAD;
	return p->unread;
    }
    s = STRING(p->name);
    return p->ptr >= s->size ? EOF : s->data[p->ptr++];
}

String_Ungetc (port, c) Object port; register c; {
    PORT(port)->flags |= P_UNREAD;
    PORT(port)->unread = c;
}

Check_Input_Port (port) Object port; {
    Check_Type (port, T_Port);
    if (!(PORT(port)->flags & P_OPEN))
	Primitive_Error ("port has been closed: ~s", port);
    if (!(PORT(port)->flags & P_INPUT))
	Primitive_Error ("not an input port: ~s", port);
}

Object P_Clear_Input_Port (argc, argv) Object *argv; {
    Discard_Input (argc == 1 ? argv[0] : Curr_Input_Port);
    return Void;
}

Discard_Input (port) Object port; {
    register FILE *f;

    Check_Input_Port (port);
    if (PORT(port)->flags & P_STRING)
	return;
    f = PORT(port)->file;
    f->_cnt = 0;
    f->_ptr = f->_base;
}

/* NOTE: dumps core on ISI 4.2BSD when called on an input file port that
 * has not yet been read from.
 */
Object P_Unread_Char (argc, argv) Object *argv; {
    Object port, ch;
    register struct S_Port *p;

    ch = argv[0];
    Check_Type (ch, T_Character);
    port = argc == 2 ? argv[1] : Curr_Input_Port;
    Check_Input_Port (port);
    p = PORT(port);
    if (p->flags & P_STRING) {
	if (p->flags & P_UNREAD)
	    Primitive_Error ("cannot push back more than one char");
	String_Ungetc (port, CHAR(ch));	
    } else {
	if (ungetc (CHAR(ch), p->file) == EOF)
	    Primitive_Error ("failed to push back char");
    }
    return ch;
}

Temp_Intr_Handler () {
    Immediate_Mode (Last_File, 0);
    (void)signal (SIGINT, Intr_Handler);
    Intr_Handler ();
}

Object P_Read_Char (argc, argv) Object *argv; {
    Object port;
    register FILE *f;
    register c, str, flags;

    port = argc == 1 ? argv[0] : Curr_Input_Port;
    Check_Input_Port (port);
    f = PORT(port)->file;
    flags = PORT(port)->flags;
    str = flags & P_STRING;
    if (flags & P_TTY) {
	(void)signal (SIGINT, Temp_Intr_Handler);
	Last_File = f;
	Immediate_Mode (f, 1);
    }
    c = GETC;
    if (flags & P_TTY) {
	Immediate_Mode (f, 0);
	(void)signal (SIGINT, Intr_Handler);
    }
    Tweak_Stream (f);
    return c == EOF ? Eof : Make_Char (c);
}

Object P_Read_String (argc, argv) Object *argv; {
    Object port;
    register FILE *f;
    register c, str;
    register char *p;
    char buf[MAX_STRING_LEN];

    port = argc == 1 ? argv[0] : Curr_Input_Port;
    Check_Input_Port (port);
    f = PORT(port)->file;
    str = PORT(port)->flags & P_STRING;
    p = buf;
    while (1) {
	c = GETC;
	if (c == EOF || c == '\n')
	    break;
	if (p == buf+MAX_STRING_LEN)
	    break;
	*p++ = c;
    }
    Tweak_Stream (f);
    return c == EOF ? Eof : Make_String (buf, p-buf);
}

Object P_Read (argc, argv) Object *argv; {
    return General_Read (argc == 1 ? argv[0] : Curr_Input_Port, 0);
}

Object General_Read (port, konst) Object port; {
    register FILE *f;
    register c, str;
    Object ret;

    Check_Input_Port (port);
    Flush_Output (Curr_Output_Port);
    f = PORT(port)->file;
    str = PORT(port)->flags & P_STRING;
    while (1) {
	c = GETC;
	if (c == EOF) {
	    ret = Eof;
	    break;
	}
	if (Whitespace (c))
	    continue;
	if (c == ';') {
	    if (Skip_Comment (port) == EOF) {
		ret = Eof;
		break;
	    }
	    continue;
	}
	if (c == '(') {
	    ret = Read_Sequence (port, 0, konst);
	} else {
	    UNGETC;
	    ret = Read_Atom (port, konst);
	}
	break;
    }
    Tweak_Stream (f);
    return ret;
}

Skip_Comment (port) Object port; {
    register FILE *f;
    register c, str;

    f = PORT(port)->file;
    str = PORT(port)->flags & P_STRING;
    do {
	c = GETC;
    } while (c != '\n' && c != EOF);
    return c;
}

Object Read_Atom (port, konst) Object port; {
    Object ret;

    ret = Read_Special (port, konst);
    if (TYPE(ret) == T_Special)
	Primitive_Error ("syntax error");
    return ret;
}

Object Read_Special (port, konst) Object port; {
    Object ret;
    register c, str;
    register FILE *f;
    char buf[MAX_SYMBOL_LEN+1];
    register char *p = buf;

#define READ_QUOTE(sym) \
    ( ret = Read_Atom (port, konst),\
      konst ? (ret = Const_Cons (ret, Null), Const_Cons (sym, ret))\
	   : (ret = Cons (ret, Null), Cons (sym, ret)))

    f = PORT(port)->file;
    str = PORT(port)->flags & P_STRING;
again:
    c = GETC;
    switch (c) {
    case EOF:
eof:
	Tweak_Stream (f);
	Primitive_Error ("premature end of file");
    case ';':
	if (Skip_Comment (port) == EOF)
	    goto eof;
	goto again;
    case ')':
	SET(ret, T_Special, c);
	return ret;
    case '(':
	return Read_Sequence (port, 0, konst);
    case '\'':
	return READ_QUOTE(Sym_Quote);
    case '`':
	return READ_QUOTE(Sym_Quasiquote);
    case ',':
	c = GETC;
	if (c == EOF)
	    goto eof;
	if (c == '@') {
	    return READ_QUOTE(Sym_Unquote_Splicing);
	} else {
	    UNGETC;
	    return READ_QUOTE(Sym_Unquote);
	}
    case '"':
	return Read_String (port, konst);
    case '#':
	ret = Read_Sharp (port, konst);
	if (TYPE(ret) == T_Special)
	    goto again;
	return ret;
    default:
	if (Whitespace (c))
	    goto again;
	if (c == '.') {
	    c = GETC;
	    if (c == EOF)
		goto eof;
	    if (Whitespace (c)) {
		SET(ret, T_Special, '.');
		return ret;
	    }
	    *p++ = '.';
	}
	while (!Whitespace (c) && !Delimiter (c) && c != EOF) {
	    if (p == buf+MAX_SYMBOL_LEN)
		Primitive_Error ("symbol too long");
	    if (c == '\\') {
		c = GETC;
		if (c == EOF)
		    break;
	    }
#ifdef CASE_INSENSITIVE
	    if (isupper (c))
		c = tolower (c);
#endif
	    *p++ = c;
	    c = GETC;
	}
	*p = '\0';
	if (c != EOF)
	    UNGETC;
	ret = Parse_Number (buf, 10);
	if (Nullp (ret))
	    ret = Intern (buf);
	return ret;
    }
    /*NOTREACHED*/
}

Object Read_Sequence (port, vec, konst) Object port; {
    Object ret, e, tail, t;
    GC_Node3;

    ret = tail = Null;
    GC_Link3 (ret, tail, port);
    while (1) {
	e = Read_Special (port, konst);
	if (TYPE(e) == T_Special) {
	    if (CHAR(e) == ')') {
		GC_Unlink;
		return ret;
	    }
	    if (vec)
		Primitive_Error ("wrong syntax in vector");
	    if (CHAR(e) == '.') {
		if (Nullp (tail)) {
		    ret = Read_Atom (port, konst);
		} else {
		    e = Read_Atom (port, konst);
		    /*
		     * Possibly modifying pure cons.  Must be fixed!
		     */
		    Cdr (tail) = e;
		}
		e = Read_Special (port, konst);
		if (TYPE(e) == T_Special && CHAR(e) == ')') {
		    GC_Unlink;
		    return ret;
		}
		Primitive_Error ("dot in wrong context");
	    }
	    Primitive_Error ("syntax error");
	}
	if (konst) t = Const_Cons (e, Null); else t = Cons (e, Null);
	if (!Nullp (tail))
	    /*
	     * Possibly modifying pure cons.  Must be fixed!
	     */
	    Cdr (tail) = t;
	else
	    ret = t;
	tail = t;
    }
    /*NOTREACHED*/
}

Object Read_String (port, konst) Object port; {
    char buf[MAX_STRING_LEN];
    register char *p = buf;
    register FILE *f;
    register n, c, oc, str;

    f = PORT(port)->file;
    str = PORT(port)->flags & P_STRING;
    while (1) {
	c = GETC;
	if (c == EOF) {
eof:
	    Tweak_Stream (f);
	    Primitive_Error ("end of file in string");
	}
	if (c == '\\') {
	    c = GETC;
	    switch (c) {
	    case EOF: goto eof;
	    case 'b': c = '\b'; break;
	    case 't': c = '\t'; break;
	    case 'r': c = '\r'; break;
	    case 'n': c = '\n'; break;
	    case '0': case '1': case '2': case '3':
	    case '4': case '5': case '6': case '7':
		oc = n = 0;
		do {
		    oc <<= 3; oc += c - '0';
		    c = GETC;
		    if (c == EOF) goto eof;
		} while (Octal (c) && ++n <= 2);
		UNGETC;
		c = oc;
	    }
	} else if (c == '"')
	    break;
	if (p == buf+MAX_STRING_LEN)
	    Primitive_Error ("string too long");
	*p++ = c;
    }
    return General_Make_String (buf, p-buf, konst);
}

Object Read_Sharp (port, konst) Object port; {
    register c, str;
    register FILE *f;
    register char *p;
    char buf[MAX_SYMBOL_LEN+3];
    Object ret;

    f = PORT(port)->file;
    str = PORT(port)->flags & P_STRING;
    c = GETC;
    if (c == EOF) {
eof:
	Tweak_Stream (f);
	Primitive_Error ("end of file after `#'");
    }
    switch (c) {
    case '(':
	return List_To_Vector (Read_Sequence (port, 1, konst), konst);
    case 'b': case 'o': case 'd': case 'x':
    case 'B': case 'O': case 'D': case 'X':
	p = buf; *p++ = '#'; *p++ = c;
	while (1) {
	    c = GETC;
	    if (c == EOF)
		goto eof;
	    if (p == buf+MAX_SYMBOL_LEN+2)
		Primitive_Error ("number too long");
	    if (Whitespace (c) || Delimiter (c))
		break;
	    *p++ = c;
	}
	UNGETC;
	*p = '\0';
	ret = Parse_Number (buf, 10);
	if (Nullp (ret))
	    Primitive_Error ("radix not followed by a valid number");
	return ret;
    case '\\':
	p = buf;
	c = GETC;
	if (c == EOF)
	    goto eof;
	*p++ = c;
	while (1) {
	    c = GETC;
	    if (c == EOF)
		goto eof;
	    if (Whitespace (c) || Delimiter (c))
		break;
	    if (p == buf+9)
		goto bad;
	    *p++ = c;
	}
	UNGETC;
	*p = '\0';
	if (p == buf+1)
	    return Make_Char (*buf);
	if (p == buf+3) {
	    for (c = 0, p = buf; p < buf+3 && Octal (*p); p++)
		c = (c << 3) | (*p - '0');
	    if (p == buf+3)
		return Make_Char (c);
	}
	for (p = buf; *p; p++)
	    if (isupper (*p))
		*p = tolower (*p);
	if (strcmp (buf, "space") == 0)
	    return Make_Char (' ');
	if (strcmp (buf, "newline") == 0)
	    return Make_Char ('\n');
	if (strcmp (buf, "return") == 0)
	    return Make_Char ('\r');
	if (strcmp (buf, "tab") == 0)
	    return Make_Char ('\t');
	if (strcmp (buf, "formfeed") == 0)
	    return Make_Char ('\f');
	if (strcmp (buf, "backspace") == 0)
	    return Make_Char ('\b');
	goto bad;
    case 'f': case 'F':
	return False;
    case 't': case 'T':
	return True;
    case 'v': case 'V':
	return Void;
    case '!':                         /* Kludge for interpreter files */
	if (Skip_Comment (port) == EOF)
	    return Eof;
	return Special;
    default:
bad:
	Primitive_Error ("syntax error after `#'");
    }
    /*NOTREACHED*/
}

Object Parse_Number (buf, radix) char *buf; {
    register char *p;
    register c, mdigit = 0, edigit = 0, expo = 0, neg = 0, point = 0;
    register i;

    if (buf[0] == '#') {
	switch (buf[1]) {
	case 'b': case 'B':
	    radix = 2; break;
	case 'o': case 'O':
	    radix = 8; break;
	case 'd': case 'D':
	    radix = 10; break;
	case 'x': case 'X':
	    radix = 16; break;
	default:
	    return Null;
	}
	buf += 2;
    }
    p = buf;
    if (*p == '+' || (neg = *p == '-'))
	p++;
    for ( ; c = *p; p++) {
	if (c == '.') {
	    if (expo || point++)
		return Null;
	} else if (radix != 16 && (c == 'e' || c == 'E')) {
	    if (expo++)
		return Null;
	    if (p[1] == '+' || p[1] == '-')
		p++;
	} else if (radix == 16 && !index ("0123456789abcdefABCDEF", c)) {
	    return Null;
	} else if (radix < 16 && (c < '0' || c > '0' + radix-1)) {
	    return Null;
	} else {
	    if (expo) edigit++; else mdigit++;
	}
    }
    if (!mdigit || (expo && !edigit))
	return Null;
    if (point || expo) {
	if (radix != 10)
	    Primitive_Error ("reals must be given in decimal");
	return Make_Reduced_Flonum (atof (buf));
    }
    for (i = 0, p = buf; c = *p; p++) {
	if (c == '-' || c == '+') {
	    buf++;
	    continue;
	}
	if (radix == 16) {
	    if (isupper (c))
		c = tolower (c);
	    if (c >= 'a')
		c = '9' + c - 'a' + 1;
	}
	i = radix * i + c - '0';
	if (!FIXNUM_FITS(neg ? -i : i))
	    return Make_Bignum (buf, neg, radix);
    }
    if (neg)
	i = -i;
    return Make_Fixnum (i);
}

#ifdef TERMIO

Immediate_Mode (f, on) FILE *f; {
    static struct termio b;
    static oldlflag, oldeof;

    if (on) {
	(void)ioctl (fileno (f), TCGETA, &b);
	oldlflag = b.c_lflag;
	oldeof = b.c_cc[VEOF];
	b.c_lflag &= ~ICANON;
	b.c_cc[VEOF] = 1;
    } else {
	b.c_lflag = oldlflag;
	b.c_cc[VEOF] = oldeof;
    }
    (void)ioctl (fileno (f), TCSETA, &b);
}

#else

Immediate_Mode (f, on) FILE *f; {
    static struct sgttyb b;
    static oldflags;

    if (on) {
	(void)ioctl (fileno (f), TIOCGETP, &b);
	oldflags = b.sg_flags;
	b.sg_flags |= CBREAK;
    } else {
	b.sg_flags = oldflags;
    }
    (void)ioctl (fileno (f), TIOCSETP, &b);
}

#endif
