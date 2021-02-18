#ifndef lint
static char *rcsid = "$Header: /../dude/usr3/emu/term/parser/RCS/error.c,v 2.0 90/04/25 13:42:36 terry Exp Locker: terry $";
#endif


/*
 * $Log:	error.c,v $
 * Revision 2.0  90/04/25  13:42:36  terry
 * Stable for first real test with canvas.
 * 
 * Revision 1.1  90/04/16  22:47:05  terry
 * Initial revision
 * 
 *
 */


#include <stdio.h>
#define ERROR_HDR "PARSER"

/* VARARGS1 */

void
parse_error(u, v, w, x, y, z)
char *u;
int v;
int w;
int x;
int y;
int z;
{
    extern void perror();
    extern void exit();
    
    fprintf(stderr, "%s : ", ERROR_HDR);
    fprintf(stderr, u, v, w, x, y, z);
    if (fputc('\n', stderr) == EOF){
        perror("fputc");
        exit(2);
    }
    if (fflush(stderr) == EOF){
        perror("fflush");
        exit(2);
    }

    exit(1);
}
