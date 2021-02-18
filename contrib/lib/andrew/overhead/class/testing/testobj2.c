/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *testobj_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/class/testing/RCS/testobj2.c,v 1.3 1991/09/12 17:08:54 bobg Exp $";

/* test object for dynamic loader */

#include "class.h"
#include "testobj2.eh"

boolean testobject2__InitializeClass()
{
    printf("Testobj2 InitializeClass entered.\n");
    return TRUE;
}

boolean testobject2__InitializeObject()
{
    printf("Testobj2 InitializeObject entered.\n");
    return TRUE;
}

void testobject2__Diddle(X, n)
struct testobject2 *X;
int n;
{
    printf("Testobj2 Diddle entered, n=%d\n", n);
}

