/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* These routines are in an include file instead of a libary because they
 * depend on the internal structure of the doload_environment which may or
 * may not be the same between various machine types. In short, it would
 * have required rewriting more code than I have time to in order to do
 * this right. This solution gets source code sharing if nothing else...
 * Also this way, the safe routines are not exposedas externals in doload.
 */

static void doload_punt(e, message)
struct doload_environment *e;
char *message;
{
    fprintf(stderr, "doload:  %s\n", message);
    fflush(stderr);
    longjmp(e->errorJump, 1);
    /*NOTREACHED*/
}

static char *safe_malloc(e, size)
struct doload_environment *e;
long size;
{
    char *result;
    
    result = (char *)malloc((unsigned)size);
    if (result == NULL)
	doload_punt(e, "insufficient memory");
    return result;
}

static char *safe_realloc(e, ptr, size)
struct doload_environment *e;
char *ptr;
long size;
{
    char *result;

    if (ptr == NULL)
	result = (char *)malloc((unsigned)size);
    else
	result = (char *)realloc(ptr, (unsigned)size);
    if (result == NULL)
	doload_punt(e, "insufficient memory");
    return result;
}

/* Not static since this is used by dofix. */
static void safe_free(thing)
char *thing;
{
    if (thing != NULL)
	free(thing);
    return ;
}

static safe_read(e, thing, size)
struct doload_environment *e;
char *thing;
long size;
{	
    if ((unsigned)read(e->fd, thing, (int)size) < size)
	doload_punt(e, "incomplete load file");
}

/* Not static since this is used by dofix. */
static void safe_write(e, fd, thing, size)
struct doload_environment *e;
int fd;
char *thing;
long size;
{
    int n;

    n = write(fd, thing, (int)size);
    if (n < size) {
	perror("dofix");
	doload_punt(e, "problems writing .do file");
    }
    return;
}

static safe_lseek(e, offset, how)
struct doload_environment *e;
long offset;
int how;
{	
    if (lseek(e->fd, offset, how) == -1)
	doload_punt(e, "seek failed");
}
