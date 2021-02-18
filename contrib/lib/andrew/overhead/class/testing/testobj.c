/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *testobj_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/class/testing/RCS/testobj.c,v 2.7 1991/09/12 17:08:51 bobg Exp $";

/* test object for dynamic loader */

#include "class.h"
#include "testobj.eh"

boolean testobject__InitializeClass()
{
    printf("Testobj InitializeClass entered.\n");
    return TRUE;
}

boolean testobject__InitializeObject()
{
    printf("Testobj InitializeObject entered.\n");
    return TRUE;
}

void testobject__Diddle(X, n)
struct testobject *X;
int n;
{
    printf("Testobj Diddle entered, n=%d\n", n);
}

