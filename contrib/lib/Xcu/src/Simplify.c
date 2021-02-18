/*
 * Copyright 1991 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          580 Theory Center, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@graphics.cornell.edu
 */


/*
 * This routine does a standard reduction of all Xcu resources
 * for String conversion.  It massages the input string in three
 * ways:
 *	1) convert all upper to lower case
 *	2) squeeze out all underscores
 *	3) remove any prefixes
 * It then returns the quark corresponding to the transformed string.
 * Thus, all of the following are equivalent when used as strings to
 * represent the XcuCellSequence resource, XcuRANDOM_COLOR:
 * "XcuRANDOM_COLOR", "random_color", "RandomColor", etc.
 * They are all reduced to "randomcolor".
 */

#include <X11/Intrinsic.h>
#include <ctype.h>

void XcuReduceArg () ;

XrmQuark
XcuSimplifyArg (arg, prefix)
    String arg ;
    String prefix ;
{
String	lower_name ;
String	name_ptr ;
XrmQuark q ;

XcuReduceArg (arg, prefix, &lower_name, &name_ptr) ;
q = XrmStringToQuark(name_ptr) ;
XtFree (lower_name) ;
return q ;
}

void
XcuReduceArg (arg, prefix, lowerName, name_ptr)
    String arg ;
    String prefix ;
    String *lowerName ;
    String *name_ptr ;
{
int	i, j ;
int	arg_length = strlen(arg) ;
int	prefix_length = strlen(prefix) ;
String lower_name = (String) XtMalloc (arg_length+1) ;

for (i=0,j=0; i<=arg_length; i++)
    {
    char c = arg[i] ;
    lower_name[j] = isupper(c) ? (char) tolower(c) : c ;
    if (lower_name[j] != '_') /* squeeze out underscores */
	j++ ;
    }
while (lower_name[j] == '_') /* remove trailing underscores */
    lower_name[j--] = '\0' ;

*name_ptr = lower_name ; /* remove prefixes */
if (strncmp (lower_name, prefix, prefix_length) == 0)
    *name_ptr += prefix_length ;

*lowerName = lower_name ;
return ;
}

