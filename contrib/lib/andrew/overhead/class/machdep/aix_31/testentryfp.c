/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

static char *testentryfp_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/class/machdep/aix_31/RCS/testentryfp.c,v 1.2 1991/09/12 16:59:53 bobg Exp $";

#ifdef __STDC__
#define ClassEntry(n) \
extern void ClassEntry ## n (); \
void RealFunc ## n (double arg1) \
{ \
    printf("RealFunc%d called with %lf.\n", n, arg1); \
}
#else
#define ClassEntry(n) \
extern void ClassEntry/**/n (); \
void RealFunc/**/n (arg1) \
    double arg1; \
{ \
   printf("RealFunc%d called with %lf.\n", n, arg1); \
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

    Funcs[index](2.0);
    exit(0);
}

void blow(double arg1)
{
}

fptr *class_Lookup(index, crud)
    int index;
    void *crud;
{
    blow(3.0);
    return RealFuncs;
}
