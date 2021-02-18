/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

static char *testentry_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/class/machdep/aix_31/RCS/testentry.c,v 1.2 1991/09/12 16:59:50 bobg Exp $";

#ifdef __STDC__
#define ClassEntry(n) \
extern void ClassEntry ## n (); \
void RealFunc ## n () \
{ \
    printf("RealFunc%d called.\n", n); \
}
#else
#define ClassEntry(n) \
extern void ClassEntry/**/n (); \
void RealFunc/**/n () \
{ \
   printf("RealFunc%d called.\n", n); \
}
#endif /* __STDC__ */

#include <../common/entrydefs.h>

#undef ClassEntry

#ifdef __STDC__
#define ClassEntry(n) \
    RealFunc ## n ,
#else
#define ClassEntry(n) \
    RealFunc/**/n ,
#endif /* __STDC__ */

typedef void (*fptr)();

fptr RealFuncs[] = {
#include <../common/entrydefs.h>
};

#undef ClassEntry

#ifdef __STDC__
#define ClassEntry(n) \
    ClassEntry ## n ,
#else
#define ClassEntry(n) \
    ClassEntry/**/n ,
#endif /* __STDC__ */

fptr Funcs[] = {
#include <../common/entrydefs.h>
};

Usage()
{
    puts("testentry <index>");
    exit(1);
}

double global;

main(argc, argv)
    int argc;
    char *argv[];
{

    int index;

    if (argc != 2 || (index = atoi(argv[1])) == 0)
        Usage();

#if 1
    global = 2.0 * index;
#endif

    Funcs[index]();
    exit(0);
}

fptr *class_Lookup(index, crud)
    int index;
    void *crud;
{
    return RealFuncs;
}
