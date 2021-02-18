\begindata{text,269903968}
\textdsversion{12}
\template{default}
\define{index
menu:[Title~3,Index~91]}
\define{indexi
menu:[Title~3,InvisibleIndex~92]
attr:[Flags Hidden Int Set]}
\define{concat
}
\flushright{12 October 1990}



\italic{\{\{Future: This version of the manual includes planned but 
non-existent features.  These are delimited by double curly brackets so they 
can be removed to form the manual itself.


Note that the invisible index entries in this document also have the style 
Hidden.  To make them visible, edit the style with a `lookz` inset.\}\}}




\majorheading{Ness Function Reference Manual}


\center{by Wilfred J. Hansen}


\indent{\flushright{\smaller{_________________________________


Information Technology Center,

Carnegie Mellon University


Copyright IBM Corporation 1988, 1989

__________________________________


}}
Ness is a programming language for the Andrew ToolKit.  With it, documents can 
be processed and can even contain active elements controlled by Ness scripts. 
 The language features an innovative substring algebra for string processing. 
 


This document describes the functions available to Ness scripts and programs. 
 For a description of all Ness documents, see the \bold{Ness User's Manual.}


   1. Marker Functions

   2. Other Data Types

   3. ATK Access

   4. Unix Access

   5. Non-Ness functions

   6. Ness library functions


\smaller{CAVEAT: This manual describes an early version of the Ness language 
and system.  You are invited to contribute suggestions for change to the 
author (\italic{wjh+@andrew.cmu.edu}) or the bboard (\italic{andy.ness}).  But 
please realize that your suggestions and those of others may evolve the 
language enough to force you to revise or abandon programs written to this 
document.

}}\bold{
\begindata{bp,270056336}
\enddata{bp,270056336}
\view{bpv,270056336,0,0,0}}

        Copyright IBM Corporation 1988, 1989 - All Rights Reserved        


\smaller{Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided that 
the above copyright notice appear in all copies and that both that copyright 
notice and this permission notice appear in supporting documentation, and that 
the name of IBM not be used in advertising or publicity pertaining to 
distribution of the software without specific, written prior permission.


\smaller{IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING 
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL IBM 
BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION 
OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

}
/* $Header: /afs/andrew.cmu.edu/usr13/wjh/h/atk/ness/doc/RCS/nessuser.d,v 1.1 
89/09/13 10:38:05 wjh Exp Locker: wjh $ */

/* $ACIS:$ */

/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/doc/RCS/nessfunc.d,v $ */

/* $Log: nessfunc.d,v $
Revision 1.11  1991/02/08  14:39:33  wjh
changed definitions of readfile and writefile
 Revision 1.10  91/01/28  11:40:18  wjh added 
documentation for fileconvert.n, ctype.n, and octalimage.n  Revision 1.9 
 90/12/04  14:56:07  wjh Improved the general description of the search 
commands.  Revision 1.8  90/10/12  20:48:36  wjh added documentation for 
regSearch and regSearchReverse  

Revision 1.7  90/09/16  21:18:27  wjh see atk/ness/objects/changes.sept.90  

Revision 1.6  90/07/29  21:40:35  wjh documented date.n  

Revision 1.5  90/07/28  17:53:48  wjh added functions searchforstyle, 
definestyle, and addstylebyname  Revision 1.4  90/07/15  15:37:34  wjh 
nessfunc.d: document the new proctable entries in framecmd.c nessauth.d: 
describe usage of the framecmd stuff. nesssauth.d: describe --$ness 
nessauth.d: give example of a long string nessfunc.d: illustrate 
neststylegroup and nextstylesegment */

\begindata{bp,270034036}
\enddata{bp,270034036}
\view{bpv,270034036,1,0,0}}


Ness provides a variety of functions and access to functions defined outside 
the Ness system.  The first four sections below document functions and 
predefined identifiers implemented by the Ness system itself.  The Fifth 
section shows how to access functions implemented externally to Ness and the 
last section documents functions written in Ness and available as a library.


In the descriptions, functions and predefined variables are shown left-flush 
with their description indented.  A function description will list the 
argument names in parentheses; a predefined variable will have no parentheses 
or arguments.  The names of functions and predefined identifiers are not 
reserved in the Ness system;  you may declare a function or variable with the 
same name.  However, any reference to the name is then a reference to the one 
you have defined and not to the Ness one.


Assignment to predefined variables is a compile-time error.



\section{1. \index{Marker Functions}}


A "marker" value is a reference to a substring of some underlying base string. 
 The base itself may have originally been a constant, the contents of a file, 
or constructed by concatenating the contents of marker values.  In a 
concatenation, the string utilized for each marker operand is the referenced 
substring of the underlying base.


Example:  Suppose we have a base string "John Phillip Sousa" and two marker 
variables a and b referring to "hill" and "usa" respectively.  Then the 
concatenation a ~ b returns a marker referencing the entirety of a new base 
string consisting of "hillusa".  However, markers retain their position in 
their base, so extent(a, b) would have as its value a reference to "hillip 
Sousa" as a substring of the same original base as before.


Marker functions have markers as arguments and return marker values.  In 
almost all cases, the result value refers to a substring of the same base 
string as at least one of the operands.



\subsection{1.1 \index{primitive marker functions}}



All marker operations can be defined in terms of six primitive functions.  See 
the paper:  Hansen, W. J., \bold{The Computational Power of an Algebra for 
Subsequences}, ITC, CMU, 1989.


\indexi{next()}\description{\italic{next}(x) - Returns a marker for the single 
character following the end of x.


\indexi{start()}\italic{start}(x) - Returns an empty marker for the position 
at the beginning of x.


\indexi{base()}\italic{base}(x) - Returns a marker for the entire string of 
which x is a substring.


\indexi{extent()}\italic{extent}(x, y) - Returns a marker for the substring 
extending from the beginning of x to the end of y.  If y ends before x begins, 
the returned marker is for the empty substring at the end of y, the second 
argument.  If x and y are on different bases, \italic{extent}() returns a 
marker for a constant empty string on an empty base.


\indexi{replace()}\italic{replace}(x, r) - Modifies the string 
\italic{base}(x) by substituting r for the substring marked by x.  Returns a 
marker for the newly installed substring.  Other markers on the same base as x 
are adjusted so they refer as best they can to the same as what they formerly 
did.  The exact change to markers that overlap x is not defined;  that is, 
your program is incorrect if it assumes a particular definition of what 
happens to overlapping markers. \{The current precise behavior of replace() is 
to insert a copy of r at finish(x) and then delete x.  If any other marker, 
say M, begins at finish(x), then the insertion is not included in M.  If any 
marker M ends at finish(x), M is extended with the insertion only when x is 
non-null.  Following these rules, x itself will be updated to include the new 
value only when x is non-null.\}


\indexi{newbase()}\italic{newbase}() - Returns a marker for a non-constant 
empty string.  This (or concatenation) is the way to get new string values 
which can be expanded by \italic{replace}().


\{\{Future: \indexi{readonly()}\italic{readonly}(x) - returns a marker for a 
copy of x which is flagged as a constant.\}\}}



\subsection{1.2 \index{Marker computations}}


The functions in this section are all defined in terms of the primitive marker 
functions in the previous section.  Each has a single argument marker and 
returns a substring of the base related in some way to the argument.  \{The 
definitions of these functions in terms of primitives are given in braces.\}


The functions front(), first(), last(), previous(), second(), and nextn() all 
usually return a single element.  Front(x) and first(x) are similar:  if x is 
a non-empty string both return its first element.  When x is empty, first(x) 
returns x, an empty string; however, in the same situation \italic{front}() 
returns the element following x.  In fact, front(x) returns an empty string 
only when x is the empty string at the end of the base.  


Further example:  Suppose t is an empty marker between the first two 
characters in "abc".  Then first(t) is the same as t--that is, t has no 
characters so the value of the first of them can only be the empty string. 
 However, front(t) is a reference to the b because it is the first character 
following atferr the start of t.

\description{
\indexi{front()}\italic{front}(x) - a single character starting where x 
starts.  \{next(start(x))\}


\indexi{finish()}\italic{finish}(x) - an empty marker at the end of x. 
 \{start(next(x))\}


\indexi{rest()}\italic{rest}(x) - all of x but the first character;  empty if 
x is empty.  \{extent(next(front(x)), x)\}


\indexi{first()}\italic{first}(x) - the first character of x;  empty if x is 
empty.  \{extent(x, start(rest(x)))\}


\indexi{last()}\italic{last}(x) - Return the last character of x.  If x is 
empty, return x.

\example{\smaller{\{function last(m)

\leftindent{if rest(m) = "" then return m; 

else return last(rest(m)); 

}end function\}}}


\indexi{allnext()}\italic{allnext}(x) - everything from the end of x to the 
end of the base.  \{extent(next(x),base(x))\}


\indexi{allprevious()}\italic{allprevious}(x) - everything from the start of 
the base to the start of x.  \{extent(base(x), start(x))\}


\indexi{previous()}\italic{previous}(x) - the character before x. 
  \{last(allprevious(x))\}


\indexi{second()}\italic{second}(x) - the second character of x;  empty if x 
is less than two characters.   \{first(rest(x))\}


\indexi{advance()}\italic{advance}(x) - a marker with the same length as x, 
but starting with finish(front(x)).   \{extent(finish(front(x)), 
finish(next(x)))\}


\indexi{length()}\italic{length}(x) - (an integer value) the number of 
characters in x.  \{n := 0;  while x /= "" do n := n + 1;  x := rest(x); end 
while;  return n;\}


\indexi{nextn()}\italic{nextn}(x, n) - goes forward or backward from marker x. 
 If n is positive, returns a marker for the n'th character following x;  if n 
is negative, returns the n'th character preceding x;  if n is zero, returns x 
unchanged.  \{while n > 0 do x := next(x); n := n - 1; end while; while n < 0 
do x := previous(x); n := n + 1; end while; return x;\}



\indexi{copy()}\italic{copy}(x) - a copy of the marked substring.  \{newbase() 
~ x\}


\indexi{toend()}\italic{toend}(x) - everything from the start of x to the end 
of its base.  \{extent(x, base(x))\} 


\{\{Future: \indexi{translate()}\italic{translate}(x, old, new) - strings old 
and new must have the same length.  The value returned is a copy of x with 
each place that formerly held a character among those in old replaced by the 
corresponding character in new.  For instance, translate("out out da*n spot", 
"otu", "urr") produces "urr urr da*m spur".\}\}


\{\{Future: \indexi{repeat()}\italic{repeat}(x, n) - where n is an integer. 
 Produces a string formed by concatenating n copies of x.\}\}

}
\subsection{
1.3 \index{Search functions}}


Search functions have two arguments, a subject and a pattern.  They scan 
through the subject looking for the first substring described by the pattern. 
 The return value is a marker referring to a suitable substring of the 
subject.


By convention, the first argument controls both how much of the subject is 
searched and what value is returned if the search fails.  If the subject is 
zero-length, the search is made over extent(subject, base(subject));  but if 
non-zero, the search is limitied to the subject substring itself.  When a 
search succeeds it matches a portion of the subject, and returns a marker for 
that portion;  when a search fails it returns the empty substring at 
finish(subject).


For example, consider 

	marker m := "abcdef"

Now search(m, "d") returns a marker for the fourth element of m and 
search(start(m), "d") returns the same value.  But search(m, "q") returns the 
empty string finish(m) while search(start(m), "q") returns the empty string 
start(m).


Two of the search functions, \italic{search}() and \italic{match}() treat 
their pattern argument as a string to be matched exactly in the subject (white 
space and the case of characters must all match exactly).  The other three 
functions treat their pattern as a set of characters.  For instance, in the 
expression


	anyof(toend(x), ".?!")


the set has three characters and the expression will return a marker for the 
first character in toend(x) which is a period, question mark or exclamation 
point.  If there is no such character,  the expression returns an empty marker 
at finish(toend(x)), which is also the end of base(x).


\indexi{search()}\description{\italic{search}(x, pat) - find the first 
instance of pat after start(x).


\indexi{match()}\italic{match}(x, pat) - see if x begins with pat.  This 
function succeeds only if all of pat is found.


\indexi{anyof()}\italic{anyof}(x, set) - find the first character after 
start(x) that is one of the characters in the set.


\indexi{span()}\italic{span}(x, set) - match start(x) and all contiguous 
subsequent characters which are in the set.


\indexi{token()}\italic{token}(x, set) - find the first substring after 
start(x) which is composed of characters from set.  This is equivalent to:

\leftindent{\leftindent{\italic{function} \bold{token}(x, set)

\leftindent{\italic{marker} m := anyof (x, set)

\italic{if} m = "" \italic{then return} m \italic{end if}

\italic{if} x = "" \italic{then} 

\leftindent{\italic{return} span(toend(m), set)}

\italic{else} 

\leftindent{\italic{return} span(extent(m, x), set)}

\italic{end if}}

\italic{end function}}}}


regSearch(x, pat) - searches x for the regular expression pat.  The regular 
expression is as for interactive searches in ez, as documented with the `help 
search` command ($ANDREWDIR/help/search.help).  In accordance with Ness 
conventions the search is delimited to x if it is non-empty or otherwise to 
extent(x, base(x)).  The plain search() function defined above is considerably 
faster in those cases where there are no regular expression features in pat.


regSearchReverse(x, pat) - searches backward from finish(x) for the regular 
expression pat.  The regular expression is as for interactive searches in ez, 
as documented with the `help search` command ($ANDREWDIR/help/search.help). 
 In imitation of the usual Ness search convention, the search is delimited to 
x if it is non-empty BUT otherwise to extent(base(x), x), that is everything 
\italic{before} x.


\{\{Future: various sets of characters are provided as predefined identifiers:

	alpha_characters - upper and lowercase

	lower_characters - lower case letters

	upper_characters - uppercase letters

	digit_characters - 0...9

	whitespace_characters - space, tab, newline, carriage return, form feed, 
vertical tab

	punctuation_characters - ! : ; , . ?

	parenthesis_characters - ( ) \{ \} [ ]

	identifer_characters - alpha_characters, digit_characters, underline

	filename_characters - identifier_characters, period, (? what else)

	pathname_characters - filename_characters, slash

	control_characters - any non-printable character

\}\}

\description{
\indexi{parseint()}\italic{parseint}(m)   (Returns integer)  -  Starting at 
start(m), parseint looks for the characters which constitute an integer, 
parses the value, and returns it.  The length of m is ignored (but eventually 
parseint will be revised to conform to the search conventions).  For an error 
the function returns -2^31.  Integer values may be written in the text as 
decimal, octal, hexadecimal, or floating values, as accepted by the compiler 
(see the \bold{Ness Language Reference Manual});  floating values are rounded 
to the nearest integer.   A subsequent call to WhereItWas() returns a marker 
for the text that was parsed to compute the integer value.  \{\{Future: Should 
delimit the search to the length of m.\}\}


\indexi{parsereal()}\italic{parsereal}(m)  (Returns real)  -  Starting at 
start(m), parsereal looks for the characters which constitute a real value, 
parses the value, and returns it.  The length of m is ignored (but eventually 
firstobject will be revised to conform to the search conventions).  For an 
error the function returns NaN.  Real values may be written as decimal, octal, 
hexadecimal, or floating values, as accepted by the compiler (see the 
\bold{Ness Language Reference Manual}); integer values are converted to float. 
  A subsequent call to WhereItWas() returns a marker for the text that was 
parsed to compute the real value.  \{\{Future: Should delimit the search to 
the length of m.\}\}



\indexi{firstobject()}\italic{firstobject}(m)  (Returns object)  -  Scans 
forward from start(m) looking for an object.  If one is found, it is returned; 
otherwise the value is NULL.  The length of m is ignored (but eventually 
firstobject will be revised to conform to the search conventions).  A 
subsequent call to WhereItWas() will return the location of the object. 
 \{\{Future: Should delimit the search to the length of m.\}\}


 

\indexi{whereitwas()}\italic{whereitwas}()  -  Returns a marker for the text 
location of the most recent parseint(), parsereal(), or firstobject(). 
 \{\{Future: For an error WhereItWas should return an empty marker at the end 
of the original argument, m.\}\}

}

\subsection{1.4 \index{Other marker functions and predefined variables}}


\indexi{isreadonly()}\description{\italic{isreadonly}(x) - (returns Boolean) 
True if the base of x is a read-only string.


\indexi{textimage()}\italic{textimage}(any-value) - this function is defined 
for any value;  it returns a string that represents the value.


\indexi{replacewithobject()}\italic{replacewithobject}(m, obj, viewname)  - 
 The contents of marker m are replaced with the object 'obj'.  Information is 
stored with the text so that when the object is displayed it will by via the 
named view class.  M and viewname are markers, obj is an object value;  the 
function returns the modified marker m, whose contents now indicate the 
portion of the text containing the object.  


\{\{Future: textimage should be discarded because implicit conversion obviate 
the need for it\}\}


\{\{Future:  \indexi{setformat()}\italic{setformat}(numericvar, format) - sets 
the formatting information for the named variable to be that given by the 
format string.  This format is used when the numericvariable is inserted in a 
string.  Format contents are (?) similar to format items in C.\}\}


\{\{Future: \indexi{edit()}\italic{edit}(real, mask) - The real value is 
editted according to the mask and a marker for the result is returned.  The 
edit mask will specify the number of decimal places, the treatment of negative 
values, zero supression, and inclusion of currency and other symbols.\}\}


}
\subsection{1.5 \index{Style Manipulation}}


Text in ATK can have styles like different fonts, various indentation, flush 
left or right.  Such styles can appear in the strings referenced by Ness 
markers.  The functions in this section provide elemental tools for dealing 
with these styles:  finding them, determining what they, and changing them.


\indexi{addstyles()}\description{\italic{addstyles}(m, mstyle)  -  All styles 
associated with the first character of mstyle are added to the styles wrapping 
marker m.  If mstyle is an empty substring, the styles used are those of the 
character following mstyle.  Returns the marker m.


\indexi{nextstylegroup()}\italic{nextstylegroup}(m)  -  The returned value is 
a marker for the next style group - either one that starts where m does and 
exceeds it in length or one that starts further along in the string.  If there 
is no succeeding style group, the function returns start(m).  A program can 
find all styles by successive application of NextStyleGroup().  See below.


\indexi{nextstylegroup()}\italic{nextstylesegment}(m)  -  The returned value 
is a marker for the segment of base(m) which extends from finish(m) to the 
next point where the text style changes.  If finish(m) is at finish(base(m), 
the function returns start(m).  A program can find all style segments by 
successive application of NextStyleGroup().  See below.


\indexi{enclosingstylegroup()}\italic{enclosingstylegroup}(m)  -  Marker m 
must be either an empty substring or a marker for an entire substring that has 
a style.  The returned value is a marker for the next larger style group that 
encloses the same substring plus additional text.  If the conditions are not 
met or there is no such larger style group, the function returns start(m).


\indexi{clearstyles()}\italic{clearstyles}(m)  -  This function is the same as 
selecting the text marked m and choosing Plainest from the ez menu.  All 
styles which begin or end within m are reduced to only their portion outside 
m.  If a stylegroup both starts and ends within m, it is removed entirely. 
 Returns m.


\indexi{hasstyles()}\italic{hasstyles}(m, p)  -  (returns Boolean)  This 
predicate is True if the text beginning at start(m) has all the styles of the 
text beginning at start(p).


\italic{searchforstyle}(m, mstyle) - Searches for an instance of text with (at 
least) all the styles of front(mstyle). Returns the entire group of letters 
that are ensted within that style, but if m is not the empty string the result 
will not extend beyond the end of m.  This is a search function and obeys the 
conventions described in section 1.3:  the search is limited to m unless m is 
empty, in which case the search is over allnext(m);  the result for failure is 
finish(m).  


\italic{definestyle}(name, mstyle) - A new style name is defined and has all 
the styles of front(mstyle).  The name and nmenu name for the style are 
derived from the first argument, which can be in the full syntax for a menu 
item as shown by `help initfiles`.  If there is no comma in the menu name, the 
style will be put on the Style menu card.  The actual name of the style is the 
sequence of letters and digits following the comma or start of m, with 
uppercase converted to lower.  This name is returned by the function and has 
as its style the newly defined style.  If mstyle has more than one style 
layer, they are combined together in one single style.


\italic{addstylebyname}(m, name) - The text delimited by m is given the style 
named by the second argument.  This name should be all lowercase and have no 
menu card name, commas, or tildes.  The style is sought among those associated 
with base(m);  if it has no template, the default template is added and the 
style sought from there.  Next the style is sought in the text associated with 
name, and finally a search is made of all styles defined with definestyle().



\{\{Future: \indexi{stylediff()}\italic{stylediff}(m, p)  -  returns a marker 
for a one character string with the styles needed to change the style of p 
into m.  (Is this the right order for the arguments?)\}\}}


If the subject text is


\leftindent{Th\bold{is \italic{is a}\underline{ t}e}st 
\typewriter{w\bold{\italic{it}h vari}\underline{ous ne}sting} of styles.}


Then a loop cycling through the text with nextstylegroup will find 
successively:

\leftindent{
is is a te

is a

 t

with various nesting

it

ith vari

ous ne

}
and a loop utilizing nextstylesegment will find:


\leftindent{Th

is 

is a

 t

e

st 

w

it

h vari

ous ne

sting

 of styles.

}
It is possible to locate various pieces of text and mark them to be retrieved 
after the file has been written and read in again.  The trick is to define a 
style with a given name and give it to the pieces of text, using addstyles or 
addstylebyname.  After rereading the text, the pieces can be refound with 
searchforstyle.  The following example finds all Ness function headers in a 
text t, marks them with the style "functionhead", writes the text, reads it 
back, and then prints each function header.


function markAndWrite(t, filename)

	marker FunctionHeadStyle := definestyle("functionhead", " ")

			-- no style attributes are given in second argument

			-- because the marking should be invisible

	marker f, p

	p := t

	while True do

		f := search(extent(p, t), "function")

		p := search(extent(f, t), ")")

		if p = "" then exit while end if

		addstyles(extent(f, p), FunctionHeadStyle)

	end while

	writefile(filename, base(t))

end function


function readAndPrint(filename)

	marker t := readfile(filename)

	marker FunctionHeadStyle := definestyle("functionhead", " ") 

			-- no styles are needed in the second argument

			-- because the match will be on style name alone

	t := searchforstyle(t, FunctionHeadStyle)

	while t /= "" do

		printline(t)

		t := searchforstyle(finish(t), FunctionHeadStyle)

	end while

end function




\section{2. \index{Other Data Types}}


\{\{Future: \indexi{markerobject()}\italic{markerobject}(x) -  Returns an 
object value referring to the marker x.  Now the marker can be passed to 
functions as an object or even inserted in a sequence.  From the marker object 
the program can also retrieve a pointer to the text of which the marker 
selects a portion.\}\}


\subsection{2.1 \index{Functions on Real Values}}


Real functions are as described in /usr/man/man3/math.3m.  Help is available 
for most of these functions with the command `help <name>`, where <name> is 
given in brackets at the end of each line below.   Some functions are not 
defined on all machines;  at present these give compilation errors on those 
machines where they are not defined:  * - not available on the HP;  # - not 
available on the Vax.


\indexi{acos()}\italic{acos}(x)		inverse trigonometric functions  [sin]

\indexi{acosh()}\italic{acosh}(x)		inverse hyperbolic functions  [asinh]  *

\indexi{asin()}\italic{asin}(x)		inverse trigonometric functions  [sin]

\indexi{asinh()}\italic{asinh}(x)		inverse hyperbolic functions   [asinh]  *

\indexi{atan()}\italic{atan}(x)		inverse trigonometric functions  [sin]

\indexi{atanh()}\italic{atanh}(x)		inverse hyperbolic functions   [asinh]  *

\indexi{atan2()}\italic{atan2}(x, y)	inverse trigonometric functions  [sin]

\indexi{cbrt()}\italic{cbrt}(x)		cube root   [sqrt]  *

\indexi{cos()}\italic{cos}(x)		trigonometric function  [sin]

\indexi{cosh()}\italic{cosh}(x)		hyperbolic function  [sinh]

\indexi{erf()}\italic{erf}(x)		error function  [erf]

\indexi{erfc()}\italic{erfc}(x)		complementary error function  [erf]

\indexi{exp()}\italic{exp}(x)		exponential  [exp]

\indexi{expm1()}\italic{expm1}(x)  		exp(x) - 1   [exp]  *

\indexi{fabs()}\italic{fabs}(x)		absolute value  [floor]

\indexi{hypot()}\italic{hypot}(x, y)	Euclidean distance  [hypot]

\indexi{j0()}\italic{j0}(x)		bessel function  [j0]

\indexi{j1()}\italic{j1}(x)		bessel function  [j0]

\indexi{jn()}\italic{jn}(n, x)		bessel function  [j0]

\indexi{lgamma()}\italic{lgamma}(x)	log gamma function  [lgamma]  *#

\indexi{log()}\italic{log}(x)		natural logarithm  [exp]

\indexi{log10()}\italic{log10(x)}		logarithm to base 10  [exp]

\indexi{log1p()}\italic{log1p}(x)		log(1+x)   [exp]  *

\indexi{pow()}\italic{pow}(x, y)  		exponential x**y  [exp]

\indexi{sin()}\italic{sin}(x)		trigonometric function  [sin]

\indexi{sinh()}\italic{sinh}(x)		hyperbolic functions  [sinh]

\indexi{sqrt()}\italic{sqrt}(x)		square root  [sqrt]

\indexi{tan()}\italic{tan}(x)		trigonometric function  [sin]

\indexi{tanh()}\italic{tanh}(x)		hyperbolic function  [sinh]

\indexi{y0()}\italic{y0(x)}		bessel function  [j0]

\indexi{y1()}\italic{y1(x)}		bessel function  [j0]

\indexi{yn()}\italic{yn}(n, x)		bessel function  [j0]



real to integer


\indexi{ceil()}\italic{ceil}(x)		integer no less than  [floor]

\indexi{floor()}\italic{floor}(x)		integer no greater than  [floor]

\indexi{round()}\italic{round}(x)		closest integer


real to boolean


\indexi{isnan()}\italic{isnan}(x)		TRUE if x is invalid  [isinf]  *#

\indexi{finite()}\italic{finite}(x)		TRUE if x is not infinite  [isinf]


integer to real


\indexi{float()}\italic{float}(i)		best real approximation



\subsection{2.2. \index{Arrays}

}
A marker value \italic{is} an array;  by convention an array name, say B, is a 
marker for the start of its base.  The i'th component is nextn(B, i); if it is 
an object, its value can be extracted with firstobject().  The i'th component 
can have a value assigned with replace(nextn(B,i), 
marker-referring-to-new-value).  \{\{Future:  The second argument can be 
easily generated with a subsequence expression containing one element. \}\}


This is a lot of work for a simple array of integers.  Intentionally.  After 
all, if a program uses heavily arrays of integers, it may not be best written 
in Ness.


Another alternative is to use the table object.



\section{3. \index{ATK Access}}


The functions in this section provide access to facilities provided by ATK.


\subsection{3.1 \index{References to the current environment}}


The current environment during execution may be within a window on screen or 
without a window via `nessrun`.  The functions and predefined identifiers in 
this section yield values which are one or the other objects which constitute 
that environment. 


The values of the four predefined variables depend on exactly how this 
particular invocation of Ness was initiated.  Here are the situations:


\description{\indent{-nessrun:  The execution was started with `nessrun`.

-message-line: The user typed ESC-ESC and entered a statement in the message 
line.

-do-main: Executed by selecting Do Main() from the menu.

-event: An event was intercepted via \bold{on menu}, \bold{on mouse}, \bold{on 
keys}, or \bold{on event}.}}


The description of each predefined identifier explains what its value is in 
each of these situations.


In many cases an inset is composed of a number of separate objects internally. 
 Ness attempts to hide this fact from you by "looking down" from whatever 
object is its argument for one which meets the type required by the call. 
 Normally an inset is represented to Ness by a pointer to the celview.  Among 
the places Ness looks for the object of the proper class is in the child of 
the celview, the data object of any view encountered, or the child of an 
embedded scroll, lset, arbiter, lpair, matte, im.  This process descends 
through the view tree until it finds the correct pointer, or there is no way 
to continue.


Be careful.  Some functions have objects as arguments or return values, while 
others have markers.  Each of the four predefined variables yield object 
values;  the functions take objects as arguments and return markers.  Given a 
text object, say tobj, the proper way to get the text it contains is 
base(currentselection(tobj)).

\description{
\italic{\index{defaulttext}} - an object value;  the object which is the 
"default" text object.  Ness does its best to find a reasonable interpretation 
for the text that this variable is supposed to refer to, although sometimes it 
can find none.    -message-line: The defaulttext is derived from the inset 
active when ESC-ESC was pressed:  if that inset is text, it becomes the 
defaulttext; otherwise, successively surrounding views are checked to see if 
they are text and the first (nearest enclosing) is taken as the defaulttext. 
  -nessrun: A special default text is created, which is initially empty.  The 
program may put text in it to manipulate the text with the textview_... 
proctable functions.   -do-main and event: If the arbiter has an object named 
"defaulttext", this variable will refer to it; otherwise the child of the 
arbiter and the parent views of the arbiter are check to see if one is text 
and the first found is the default.  If still none is found and the Ness view 
is nested--however deeply--in text, this variable will refer to that 
surrounding text.


\italic{\index{currentinset}} - an object value;  the object for which an 
event is currently being handled.   -event: \italic{currentinset} has the 
object pointer corresponding to the inset named in the \bold{extend} block 
surrounding the event-interception-specifier.   -message-line: whatever inset 
had the input focus when the procedure was initiated.   -do-main and nessrun: 
currentinset is NULL.


\italic{\index{currentinputfocus}} - an object value;  the object which 
currently has input focus.  The value NULL is returned if there is no window 
or it has no input focus; generally this can be taken to mean that the ness is 
not running interactively.   -event: If the event is a mouse hit, the input 
focus will not have moved to the object clicked on if the mouse hit has not 
been passed along to the inset with dohit().  Even if it is passed along, the 
inset may choose to not request the input focus.   -do-main: There will 
usually be some input focus.   -message-line: The input focus will not be the 
message line, it will be whatever was the input focus before the prompt 
occurred.   -nessrun: There is no input focus.


\italic{\index{currentwindow}} - an object value; an object representing the 
window in which this script and surrounding document or application appears. 
 The value NULL is returned if this execution is standalone or no window has 
yet been allocated.  Currentwindow provides an appropriate value to pass as 
first argument to the  frame_  functions.  


\indexi{currentselection()}\italic{currentselection}(object) - the object 
should be a textview;  the function returns a marker for the currently 
selected text in it.  This is not a marker for a copy of the text: it refers 
directly to the string maintained by the text data object for the textview. 
 Frequently the pre-defined variable \italic{defaulttext} is used as the 
object argument.  (Indeed, the compiler permits you to omit the argument and 
get defaulttext as the value.  This is not guaranteed to last forever.) 
  \{\{Future:  currentselection should accept either marker or object as the 
argument.\}\}


\indexi{currentmark()}\italic{currentmark}(object) - the object should be a 
textview;  the function returns a marker for the currently marked text in it. 
 (The "marked" text is whatever was the selection the last time 
textview_ctrl_at() was executed.  This routine is executed by the Insert key 
on an RT and displays the message "Mark set.")  The value returned is not a 
marker for a copy of the text: it refers directly to the string maintained by 
the text data object for the textview.  Frequently the pre-defined variable 
\italic{defaulttext} is used as the object argument.  (Indeed, the compiler 
permits you to omit the argument and get defaulttext as the value.  This is 
not guaranteed to last forever.)   \{\{Future:  currentmark should accept 
either marker or object as the argument.\}\}


\indexi{setcurrentselection()}\italic{setcurrentselection}(object, x) - object 
must be a textview and x must be a marker value referring to a substring of 
the text being viewed therein.  The current selection in the textview is set 
to be x.  No value is returned.


}
\subsection{3.2 \index{User communication functions}}

\description{
\indexi{inset()}\italic{inset}(name) - returns an object value for the inset 
whose name is given.  \italic{Name} is a marker expression.  The name must 
have been registered by the inset with the surrounding arbiter as described in 
Section 3.1 of the \bold{Ness Author's Reference Manual}.


\indexi{focus()}\italic{focus}\italic{(object)} - input focus is given to the 
inset whose object is the argument.

\description{
}\indexi{TellUser()}\italic{TellUser}(msg) - the text of marker msg is 
displayed in the message line.  If the message is to appear before the Ness 
completes its operation, it may be appropriate to call im_ForceUpdate, see 
below.


\indexi{AskUser()}\italic{AskUser}(prompt, default) - displays the prompt and 
the default value using the standard message_AskForString conventions. 
 Returns a marker for the user's response.  If the user cancels the message, 
the return value is  "CANCEL";  every call to AskUser should test for this 
value and abort the present operation if it is found.  (Yes, there is no way 
for the user to actually respond with "CANCEL" to a prompt.)


\indexi{im_ForceUpdate()}\italic{im_ForceUpdate}() - an update cycle is 
initiated so all changes are made visible on the screen.  This function has 
the same name as one defined in ATK, but is actually implemented directly in 
Ness.


\{\{Future: \indexi{lockscreen()}\italic{lockscreen}(boolean) - enables or 
disables screen updates.  Eventually there will be some mechanism so that all 
screen updating is discarded (not deferred) when this function has been called 
with the value True.\}\}


\{\{Future: \indexi{screensync()}\italic{screensync}() - execution halts until 
screen update is complete in the window server.\}\}


}

\subsection{3.3 \index{Event interceptor functions and predefined variables}}


The functions and predefined variable in this section deal with mouse hits, 
menu selections, and key presses.  The first three can simulate user actions 
on objects.  The next five are predefined variables which have values only 
during the appropriate event interceptor block, \bold{on keys}, \bold{on 
menu}, or \bold{on mouse}.  The last six are constants suitable for comparison 
against the values in mouseaction.


\description{\{\{Future: \indexi{passdown()}\italic{passdown}() - This 
function must be called during the handling of an mouse, menu, or key event. 
 The way event interception works is to wrap a transparent inset called a cel 
around each inset.  When an intercepted event occurs, the interception is done 
by the cel, so the inset has not yet seen it.  The pass() function passes the 
event along to the object.  (Note that the passage is in the opposite 
direction from the pass operation in HyperTalk, where pass sends the event up 
the object hierarchy.) \}\}


\indexi{dokeys()}\italic{dokeys}(object, m) - the marker value m is treated as 
a sequence of keys typed by the user to the inset object.  \{At present, Ness 
fails if the keys passed in m would cause a call on a Ness event 
interceptor.\}


\indexi{domenu()}\italic{domenu}(object, menuname) - looks up the menuname in 
the menus posted when the object has input focus and executes the operation 
corresponding to the match.


\indexi{dohit()}\italic{dohit}(object, action, x, y) - simulates the effect of 
a user mouse click with the indicated x, y, and action.  The action must have 
the value of one of the six predefined constants: 
mouse\{left|right\}\{down|move|up\}.  The usual call is dohit(currentinset, 
mouseaction, mousex, mousey).



\italic{\index{lastkeys}} - this pre-defined variable has as its value the key 
sequence which activated the \bold{on keys} event currently in progress.  If 
none is in progress, a runtime error occurs.


\italic{\index{lastmenu}} - this pre-defined variable has as its value the 
menu item which activated the \bold{on menu} event currently in progress.  If 
none is in progress, a run-time error occurs.}


\description{\italic{\index{mouseaction}} - the action for the mouse event 
currrently being responded to;  see the next six entries for possible values. 
 A run-time error occurs if no mouse event is in progress.

\description{
}\italic{\index{mousex}} - the x coordinate of the mouse event currrently 
being responded to.  A run-time error occurs if no mouse event is in progress.


\italic{\index{mousey}} - the y coordinate of the mouse event currrently being 
responded to.  A run-time error occurs if no mouse event is in progress.



\italic{\index{mouseleftdown}} - mouseaction value for a down press of the 
left button.


\italic{\index{mouseleftup}} - mouseaction value for a release of the left 
button.


\italic{\index{mouseleftmove}} - mouseaction value for coordinates generated 
by moving the mouse with the left button down.


\italic{\index{mouserightdown}} - mouseaction value for a down press of the 
right button.


\italic{\index{mouserightup}} - mouseaction value for a release of the right 
button.


\italic{\index{mouserightmove}} - mouseaction value for coordinates generated 
by moving the mouse with the right button down.}


\subsection{3.4 \index{Launching an application window}}


A Ness program can open a window and display a file or other interactive 
application.  The function called is


\leftindent{\italic{launchApplication}(m, title, programname, enablecommands)}


A new window is created and the text indicated by marker m is displayed in it. 
 The currentselection in the window will be the portion of the text selected 
by m.  The window title is that given by the second argument and the program 
name is given by the third.  The fourth argument is a Boolean value which, if 
true, specifies that the frame commands are provided on the menu; these 
include Switch File, Delete Window, Save As, Save All, and the printer 
options.


The function does not return until the user chooses the Quit option in the 
window.  However, if there are named objects in m and these have event 
handlers elsewhere in the Ness program, these handlers will be invoked when 
the specified events occur to those objects as they are displayed in the 
window.  In particular when the object becomes visible on the screen, the 
event handler for event "BecameVisible" will be invoked.


The return value will be a copy of the original base string as possibly 
modifed by the interaction.



\section{4. \index{Unix Access}}


\indexi{print()}\description{\italic{print}(x) - prints the string referenced 
by the marker x to stdout. (If the Ness is a script in an embedded inset, 
 \italic{print}() displays the value in some unsuspecting typescript.) 


\indexi{printline()}\italic{printline}(x) - the same as \italic{print}(), but 
printline appends "\\n" to the value, thus causing a newline after it.


\indexi{readfile()}\italic{readfile}(f) - interprets the marker value f as the 
name of a file, reads the file, and returns a marker for the entirety of a new 
string containing the contents.  Note: if the file is read-only, the resulting 
string will be read-only.  To convert a readonly string x to read-write, you 
can copy it with copy(x).  If the file cannot be read or is not in ATK format, 
a message is written to standard error and the program continues with whatever 
could be read from the file.


\indexi{writefile()}\italic{writefile}(f, x) - interprets f as a filename, 
opens it, and writes to it the string referenced by x.  Returns the value x. 
 If the file cannot written, the return value is start(x).   Subsequent 
\italic{writefile}() calls to the same f will overwrite the file.


\indexi{system()}\italic{system}(m) - the marker value m is presented to sh(1) 
to be interpreted as a shell command and executed.  The result output to 
stdout is made into a string and a marker for its entirety is returned.  Note: 
to start up a program and yet continue in the Ness code, the command should 
redirect stdin, stdout, and stderr.  To run ez, for example, the call might 
be:

	system("ez /tmp/t >/dev/null </dev/null 2>&1 &");


\{\{Future: \indexi{readline()}\italic{readline}() - reads from stdin and 
returns as many characters as are available, up to end of line.  This function 
is most useful in standalone applications.  It is probably meaningless in 
interactive execution.\}\}


\{\{Future: \indexi{readrawfile()}\italic{readrawfile}(f) - reads the file as 
ASCII text even if it is an ATK file.\}\}


\{\{Future: \indexi{standalone()}\italic{standalone}() - Boolean.  True iff 
this execution was started with nessrun.\}\}


\{\{Future: \indexi{starttimer()}\italic{starttimer}(object, event, 
milliseconds) - a timer is started which will expire at least the given number 
of milliseconds in the future.  When it expires, the named event is signalled 
for the named object.\}\}


\{\{Future: \indexi{signal()}\italic{signal}(object, event-name) - pretends 
the object has signalled the event and executes the Ness code associated with 
that event.  Both object and event-name may be string constants or marker 
expressions.\}\}


\{\{Future: \indexi{pause()}\italic{pause}(milliseconds) - execution halts for 
the specified number of milliseconds\}\}


\{\{Future: \indexi{filter()}\italic{filter(command, input)} - calls sh(1) on 
the command and passes the marker value of input as the stdin data for the 
command.  Waits for the command to complete and returns whatever the command 
wrote to stdout.\}\}


\{\{Future: \indexi{filterinput()}\italic{filterinput(command)} - calls sh(1) 
on the command and passes the Nesses stdin as stdin for the command.  Waits 
for the command to complete and returns whatever the command wrote to stdout. 
 This function is useful for making standalone Ness functions part of a pipe, 
except that it waits for the subprocess to complete before returning a 
value.\}\}


\{\{Future: \indexi{stderr()}\italic{stderr()} - returns stderr from most 
recent call to system(), filter(), or filterinput().\}\}


\{\{Future: \indexi{pwd()}\italic{pwd()} - Returns a marker value for the 
current directory of the process.\}\}


\{\{Future: \indexi{cd()}\italic{cd(m)} - Changes the directory for the 
current process to m.  This also changes the environment for system(), 
filter(), and filterinput().  No return value.\}\}


\{\{Future:\italic{ }\indexi{setenv()}\italic{setenv}(var, value) - Sets the 
environment variable var to the given value. Both arguments are marker values. 
 No return value.  The resulting environment will be used for subsequent calls 
to system(), filter(), and filterinput(), and will also affect any operation 
in the current process which later happens to interrogate the environment.\}\}


\{\{Future: \indexi{unsetenv()}\italic{unsetenv}(var) - Removes the 
environment variable var.  Similar in scope and consequences to setenv().\}\}


\{\{Future: \indexi{getenv()}\italic{getenv}(var) - Returns the value in the 
environment for variable var.\}\}


\{\{Future: \indexi{date()}\italic{date}() - returns a marker for the days 
date in form:  January 14, 1989 (Saturday).\}\}


\{\{Future: \indexi{time()}\italic{time}() - returns a marker for the time in 
form:  2:12 PM.\}\}


\{\{Future: \indexi{elapsed()}\italic{elapsed}() - returns an integer value 
giving the elapsed time in milliseconds since execution began.\}\}

}

\section{5. \index{Non-Ness Functions}}



\subsection{5.1 \index{Proctable functions}}


Ness scripts can refer to proctable functions, whose names are of the form


	<insetname>_<functionname>


If the function is not in the proctable when this call is encountered during 
compilation, an attempt is made to load the dynamic object <insetname>.do and 
then the function is sought again.  If it cannot be found, a compilation error 
is indicated.


Many proctable functions require that the first argument be an object of type 
<insetname>.  Ness attempts to check for this, but cannot always do so because 
there is no real indication of what types are required by proctable functions. 
 Additional arguments can be passed to proctable functions.  Again, their 
types are not checked by Ness.  If an incorrect argument is passed to a 
proctable function, unpredictable results will occur.  One quite likely 
outcome is a "bus error" or "segmentation fault", both of which mean that the 
program tried to use a meaningless address.  In this case Ness gives the 
execution time message


	! ! !   Disasterous Ness error in this function.   Quit soon.   !!!!!!!!!!


and continues.  If the user does not understand the cause of this error, it is 
probably best to save the files and exit from this instance of the editor.  In 
most cases, however, it is safe to continue editting; the access to an illegal 
address did nothing harmful because the access was disallowed.  


The proctable does have provision for specifying the return value from a 
function call.  If this is specified, the proctable function call can appear 
as an expression value in Ness.  If no return type is specified, whatever 
value is returned is treated by Ness as an integer.  \{\{Future: The proctable 
must be augmented with ways to describe the types of parameters.\}\}


Note that proctable calls and Ness library functions both have underline (_) 
as a separator between the file name and the actual function name.  This means 
there is a simple upgrade path to convert Ness library functions into C code 
and make them available as proctable entries.


\{\{Future: Along with Ness will come tools for building and maintaining a 
list of all proctable entries in the system.\}\}


Note: Many proctable entries prompt the user for further information.  For 
instance, the textview_search() function prompts for the string to be sought. 
 This is useless from within Ness functions.  In AMS, the Andrew Message 
System, many of the functions that prompt also check to see if their second 
argument is non-zero, and if so, use its value instead of the value that would 
come from the prompt in the message line.  Unfortunately, there is no general 
method of knowing which functions these are.  \{\{Future:  General conventions 
will be established to show which proctable functions can avoid the message 
line by supplying additional arguments.\}\}



\subsection{5.2 \index{Functions on Arbitrary Classes}}

\description{
\indexi{class()}\italic{class}(v) - Returns a pointer to a "quintessential" 
object of the class given by v, which may be an object or a marker giving the 
name of a class of objects.


\indexi{new()}\italic{new}(obj) - Returns a new object in the same class as 
the argument obj.


\{\{Future: \indexi{typename()}\italic{typename}(x) - For any value x returns 
a string giving the type.  If x is an object, its class name is returned. 
 Otherwise one of the strings "integer", "real", "boolean", or "marker".\}\}


}
In the ATK \italic{class} system, objects have instance variables and may 
serve as arguments to \index{methods} and \index{class procedures}.  Ness 
statements and expressions can access instance variables and call methods and 
class procedures.  To tell the Ness compiler that this feature is desired, the 
program must contain the comment


	--$\index{enable class access}


The primary reason for this is that a lengthy process is entered to initialize 
class access and this can be avoided if the program is not going to need it. 
 (Otherwise the lengthy process could be triggered by function calls that were 
misspelled or forward references.)


Here follows a short sketch of access to class variables and functions.  For 
more details, see the \bold{Ness Hacker's Manual}.  Let <var>, <class>, 
<method>, and <proc> be identifiers and \italic{obj} an expression with an 
object as its value.  Then:


To retrieve the value of \index{instance variable} <var> of object obj in 
class <class> write


	<class>_Get<var>(obj)


and to set the value write 


	<class>_Set<var>(obj, value)


To call  <method>  in  <class>  on object obj write


	<class>_<method>(obj, other parameters)


and to call the classprocedure <proc>  in  <class>  write


	<class>_<proc>(parameter list)




 \subsection{5.3 \index{Value cel access}}


If an inset is a "value", it can have three sorts of values (simultaneously 
!): numeric value, string value, and an array of string values.  The functions 
in this section provide access to these values.  The first argument to each 
function should be an object value referring to an inset which contains one of 
the various flavors of value objects: bargraphV, fourwayV, sliderV, thumbV, 
buttonV, onoffV, sliderstrV, thumbstrV, stringV, enterstrV, ... .


\description{\indexi{value_GetValue()}\italic{value_GetValue}(object) - 
returns the integer value.

\indexi{value_GetString()}\italic{value_GetString}(object) - returns the 
string value.

\indexi{value_GetArraySize()}\italic{value_GetArraySize}(object) - returns the 
number of elements in the string array.

\indexi{value_GetArrayElt()}\italic{value_GetArrayElt}(object,  index) - 
returns the index'th element in the string array.  It is a run-time error if 
the index is out of range.


\indexi{value_SetValue()}\italic{value_SetValue}(object, val) - sets the 
numeric value

\indexi{value_SetString()}v\italic{alue_SetString}(object, m) - sets the 
string value.  See warning below.

\indexi{value_SetArraySize()}\italic{value_SetArraySize}(object, nElements) - 
sets the number of elements.  Actually allocates space for the array; the old 
value is ignored.

\indexi{value_SetArrayElt()}\italic{value_SetArrayElt}(object, index, m) - 
stores the string given by marker m into the index'th element of the array. 
 It is a run-time error if the index is out of range.  See warning below.

\indexi{value_SetNotify()}\italic{value_SetNotify}(object, boolean) - 
Esoteric.  Setting the Notify flag to False will cause subsequent value_SetXxx 
operations to NOT tell their clients of the change.  The flag should soon be 
set back to True.  This function makes it possible to make several changes to 
values without having observer objects notified of each separate change. 
 After resetting the value to True, do one last value_SetXxxx() to ensure that 
observers do get notified of the change.}


The string setting functions replace pointers to string with pointers to other 
string values.  WARNING:  If the old value was a pointer to a buffer that 
other locations also point to then the system state will be inconsistent. 
 WARNING: Memory occupied by the old string will never be reused ; this 
constitutes a CORELEAK.


\subsection{5.4 Play}


Play is a "package" that provides an interface for playing notes and tones on 
an IBM/RT/PC keyboard.  On other hardware, it does nothing, gracefully.   Play 
can be utilized via its proctable entries as defined in the help file which 
can be accessed under the name \italic{playproc.}


The following proctable functions are defined.  For details see 
/usr/andrew/help/musproc.help.

\description{
\italic{play_notes}(code string) - The code string has the same format as the 
code string for BASICA on the IBM PC.  For example, here is Three Blind Mice:


		play_Notes("T240 L2 E D C  P4     E D C P4  \\

			G L4 F F L2 E P4    G L4 F F L2 E P4  \\

			L4 G >C   C < B A B  >C<  G G P4  \\

			G >C   C < B A B  >C<   G G P4  \\

			G L1 E D C")

\italic{play_tone}(freq, duration)  - Duration is the number of 128th's of a 
second.

\italic{play_volume}(vol)  - Scale of 0 to 9, although only three levels are 
implemented.

\italic{play_retard}(slowness)  -  Modify playing speed; zero is normal.  -30 
<= slowness <= 40

\italic{play_tuning}(letter)  -  The first letter of  EqualTemper, MeanTone, 
JustIntonation, or Pythagorean.

}

\{\{Future: \subsection{5.5 \index{Table access}}


Negotiations are underway for mechanisms by which Ness functions can access 
the cells of a table and tables can use Ness code to describe the values in 
cells.  These mechanisms will probably include functions for access to table 
elements and a new keyword after \bold{on}.\}\}



\section{6. \index{Ness library functions}}


Functions can be called from the Ness library just by referring to them.  By 
default the Ness library is the collection of files in /usr/andrew/lib/ness. 
 The names of exported functions in the library files are of the form 


	<filename>_<function>


as in enum_Digits.  If the name of a called function begins with "<filename>_" 
and there is a library file named <filename>.n, that file will be compiled and 
checked to see if it defines <filename>_<function>.  If it does, that 
definition is used; if not, an error is indicated.  In other words, if a 
function reference begins with the name of a file in the library, the function 
must be defined in that file; it cannot be defined by a proctable entry or an 
object class definition.


More than one library directory may be referenced.  Library files are found by 
looking in the directories named in the preference \index{NessPath}.  For 
example, my preferences file contains the line


	*.NessPath:/afs/andrew.cmu.edu/usr13/wjh/nesslib:/usr/andrew/lib/ness


The default path, if none is specified, is $\{ANDREWDIR\}/lib/ness.  No 
security applies to libraries;  any function can be called from any library 
listed in a nesspath without prompting the user.  Thus users should exercise 
caution in adding directories to their NessPath.


The library file is compiled the first time it is referred to by a function 
call.  If there is a syntax error, a special window entitled 
"\bold{NessLibrary}" is opened to display the library file and its error 
messages.  This window behaves otherwise like any other ez window, except that 
whenever a library function has an error this window will be usurped to 
display the error.  Whatever was in the window will still be available in a 
buffer and can be accessed with ctl-X-ctl-O.  (When an error is detected in a 
library function called by a main program, the error message shown for the 
function call in the main program is "undefined function".)


Erroneous library routines can be corrected in the NessLibrary window and 
recompiled.  \{WARNING:  At present there is no provision for deciding what 
needs to be recompiled.  Moreover, doing it in the wrong order will crash the 
editor.  The right order is to recompile the library file and then go back, 
change the principal script (to force a recompile), and then reexecute the 
principal script (to actually do the recompile).\}


The following files in /usr/andrew/lib/ness provide library routines that can 
be called from Ness programs and scripts.  In each description are three 
sections.  First are listed functions that can be called from ness scripts. 
 Secoond is the command line and results of running the file standalone with 
nessrun.  Third is a description of what menu entries and key bindings are 
made if the file is loaded via 'call ness_load <filename>' in a .XXXinit file.


\bold{\index{enum.n}}:  Section numbering.  A section number is at the 
beginning of a line and has <digits><dot> repeated as often as desired.  Or 
 <digits>  can be a single dash.


\leftindent{\indexi{enum_Digits()}\italic{enum_Digits}() - scans the current 
selection of defaulttext and revises section numbering.


Standalone:

	nessrun /usr/andrew/lib/ness/enum.n <filename>\leftindent{

}renames the file to the extension .BAK and makes a new copy under the old 
name with updated enumeration values.

}
\bold{\index{tokens.n}}: scans tokens from source files

\leftindent{
\indexi{tokens_getC()}\description{\italic{tokens_getC}(text)   	-  returns 
the next C token after finish(text)

\indexi{tokens_getNess()}\italic{tokens_getNess}(text)	-  returns the next 
Ness token after finish(text)

\indexi{tokens_PreSpace()}\italic{tokens_PreSpace}()	-  returns the whitespace 
and comments preceding the most recent token returned}}


\leftindent{Standalone:

	nessrun /usr/andrew/lib/ness/tokens.n <filename>

prints all the tokens in the file, one per line.  C mode is used if the file 
name ends with c, otherwise Ness mode.}


\bold{\index{format.n}}:  Converts text with hard newlines to ATK text with 
one line per paragraph.


\leftindent{\indexi{format_wrap()}\italic{format_wrap}()  -  Combines lines of 
the paragraphs within the current selection by replacing unneeded newlines 
with blanks.}


\leftindent{Standalone:

	nessrun /usr/andrew/lib/ness/format.n <filename>

the file is renamed to have the extension BAK and the new version of the file 
is written under the original name.


ness-load:

Adds the option "\index{Wrap lines}" to the Search/Spell menu for all text 
views.  This menu option wraps lines in the text of the current selection.}


\bold{\index{swap.n}}:   This is a demonstration package. 


\leftindent{\indexi{swap_Words()}\italic{swap_Words}() - moves the currently 
selected text from defaulttext to be after the word that formerly preceded it.}


\bold{\index{sort.n}}: sort the records of a data base.  See the comments in 
the file for the format of records.  See /usr/andrew/lib/ness/demos/Birth.db 
for an example.


\leftindent{\indexi{sort_records()}\italic{sort_records}(fieldname, text)  - 
 sorts the records of text in order by the contents of the named field.  If 
fieldname is "", records are sorted by their entire contents.


\indexi{sort_records_per_flags()}\italic{sort_records_per_flags}(fieldname, 
text, flags) sorts the records under conditions given by the flags, as defined 
for the Unix function sort(1).}


\leftindent{Standalone: 

	nessrun /usr/andrew/lib/ness/sort.n  [ -f<fieldname> ]  <filename>

If  -f<fieldname> is omitted, records are sorted by their whole text.  The 
output file is written to the same name as the input, but with the additional 
extension ".sorted".}


\bold{\index{unscribe.n}}:  convert Scribe styles to ATK styles.  There is a 
table in the program so users can copy the program and change the styles to 
suit their own needs. 


\leftindent{Standalone: 

	nessrun /usr/andrew/lib/ness/unscribe.n  <filename>

The converted file is written in the same place as its argument, but with the 
extension ".d".  If the original file has no extension or has the extension 
".d", a new ".d" is appended to the name.  The old version of the output file 
is saved as filename.d.BAK

}
\bold{\index{wc.n}}:  count words


\leftindent{\indexi{wc_countwords()}\italic{wc_countwords}(text) - counts the 
words in the text and returns the value as an integer.


\indexi{wc_showcount()}\italic{wc_showcount}() - counts the words in the 
current selection and displayus the value in the message line.  (Intended to 
be called from ESC-ESC.)


Standalone:

	nessrun /usr/andrew/lib/ness/wc.n <filename>

Prints a message giving the number of words in the file.


ness-load:

Adds the entry "\index{Count words}" to the Search/Spell menu.  This entry 
calls wc_showcount().}


\bold{\index{xwipe.n}}: some alternative keyboard operations


\leftindent{ness-load:

Binds ESC-CTL-w to a function which deletes everything between selection and 
the mark (which latter can be set with CTL-@).


Binds BACKSPACE so that if the selection is non-zero length, it will be cut 
instead of doing a backspace.}


\bold{date.n}: convert dates to and from a canonical form


\leftindent{The function date_canonical() converts many forms of date into a 
canonical representation.  Four other functions convert the canonical form to 
convenient forms.  


Data_canonical scans from the start of its argument ignoring extraneous 
characters until it finds a date, which it parses, canonicalizes, and returns. 
 The text between the parts of the date cannot contain numbers other than a 
time, which must have its parts separated with colons.

}
\leftindent{date limits:}

		This module only handles dates from 1/1/1901 - 12/31/2000.

\leftindent{
int function \italic{date_canonical}(marker m):

	Returns a canonical integer representation for the date in the marker m.

	(This canonical integer is the number of days from 1/1/1901.)

marker function \italic{date_text}(int n):

	Returns a text representation (eg:  July 17, 1989) for the canonical

	date given in the integer parameter n.

marker function \italic{date_slashed}(int n):

	Returns a slashed representation (eg:  7/17/1989) for the canonical date

	given in the integer parameter n.

marker function \italic{date_dashed}(int n):

	Returns a dashed representation (eg:  7-17-1989) for the canonical date

	given in the integer parameter n.

marker function \italic{date_julian}(int n):

	Returns a two-value representation (eg:  1989.198) for the canonical date

	given in the integer parameter n (note:  format is year.day).

}
\leftindent{styles accepted:}

	separator styles:

		slashes:	7/17/1989

		dashes:	7-17-1989

		periods:	1989.198

	month styles:

		digits:	7

		abbr:	Jul	(must have legal first 3 letters)

		full:	July

	year styles:

		digits:	89

		abbr:	'89	(a number with an apostrophe in front)

		full:	1989	(a number > 1900)

	value styles:

		3 values:	7/17/1989	(month, day, year)

		2 values:	1989.198	(year, number days from 1/1)

		1 value:	32340	(number of days from 1/1/1901)

	position styles:

		month day year:	July 17, 1989

		day month year:	17-July-1989	(month must be abbr or full)

		year day:		1989.198		(year must be abbr or full)


	examples of dates handled:

		July 17, 1989 -- text format

		7/17/1989 -- slashed format

		7-17-1989 -- dashed format

		1989.198 -- julian format

		7-17-89 -- last two digits of year only

		7-17-'89 -- apostrophed year

		Jul 17, 1989 -- abbreviated moth

		17 July 1989 -- month in middle

		Mon, 17 Jul 89 14:03:19 -0500 (EST) -- date command

		Tue Jul 17 17:34:09 EST 1989 -- ams date

		32340 -- canonical date integer


\bold{octalimage.n}: convert integer to octal string

\leftindent{\italic{
octalimage_cvt}(integer i) - returns string for i, padded on left with zeroes 
to at least three digits}


\bold{fileconvert.n}: skeleton fileconversion package


\leftindent{This file is a prototypical file converter.  To specialize it for 
any specific conversion, the porition identified with $$$ need to be replaced.


\italic{fileconvert_convert}() - converts currentselection(defaulttext)

}
\bold{ctype.n}: case translation functions


\leftindent{\italic{ctype_tolower}(m) - Modifies the text referenced by m, 
replacing all upper case letters with the corresponding lower case letters.


\italic{ctype_toupper}(m) - Modifies the text referenced by m, replacing all 
lower case letters with the corresponding upper case letters.}

\enddata{text,269903968}
