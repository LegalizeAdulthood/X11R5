/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

static char *ossup_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/ossup.c,v 1.2 1991/09/12 17:28:11 bobg Exp $";

#include <stdio.h>
#include <andrewos.h>

#ifdef M_UNIX
#include <sys/param.h>
#include <sys/times.h>

extern int link(), unlink();

#if defined(MEMMOVE_IS_BROKEN)
/* add bcopy() so we don't have to pull in -lX11 */

void bcopy (b1, b2, length)
register unsigned char *b1, *b2;
register length;
{
    if (b1 >= b2 || (b1 + length) < b2) {
	memcpy(b2, b1, length);	/* Note: memmove was much slower */
	return;
    }

    b2 += length;
    b1 += length;
    while (length--) {
	*--b2 = *--b1;
    }
}
#endif /* MEMMOVE_IS_BROKEN */


#if defined(RENAME_IS_BROKEN)
/* This is due to a bug in rename() that limits it to 13 character
filenames!
*/
int
sco_rename(old,new)
char *old,*new;
{
	int cc;
	unlink(new);
	cc = link(old,new);
	if (cc>=0) {
	  cc = unlink(old);
	  if (cc<0)
		unlink(new);
	}
	return cc;
}
#endif /* RENAME_IS_BROKEN */


#include <dirent.h>

scandir(dirname, namelist, sel, comp)
char *dirname;
struct dirent *(*namelist[]);
int (*sel)();
int (*comp)();
{
#define NENT 16
	int nfound=0,nent=NENT;
	DIR *dir;
	struct dirent *thisent;
	struct dirent **name_p,**base_p;
	int size;

	base_p = (struct dirent **)malloc(sizeof(struct dirent *) * nent);
	name_p = base_p;
	dir = opendir(dirname);
	if (dir == NULL) {
		free(base_p);
		return 0;
	}
	while ( (thisent = readdir(dir)) != NULL ) {
		if (sel && ((*sel)(thisent))==0)
			continue;
		if (++nfound > nent) {
			nent += NENT;
			base_p = (struct dirent **)realloc(base_p, sizeof(struct dirent *) * nent);
			name_p = base_p + nfound - 1;
		}
		size = thisent->d_reclen;
		*name_p = (struct dirent *)malloc(size);
		bcopy(thisent, *name_p, size);
		name_p++;
	}
	closedir(dir);

	if (nfound < nent) {
		base_p = (struct dirent **)realloc(base_p, sizeof(struct dirent *) * nfound);
	}
	if (comp)
		qsort(base_p,nfound,sizeof(struct dirent *),comp);

	*namelist = base_p;
	
	return nfound;
}

alphasort(dir1,dir2)
struct dirent **dir1, **dir2;
{
	return strcmp((*dir1)->d_name,(*dir2)->d_name);
}

#include <signal.h>

sigset_t
sigblock(new)
sigset_t new;
{
	sigset_t s_new, s_old;
	s_new = new;
	sigprocmask(SIG_BLOCK,&s_new,&s_old);
	return s_old;
}

sigsetmask(new)
sigset_t new;
{
	sigset_t s_new = new;

	sigprocmask(SIG_SETMASK,&s_new,0);
}

#include <utime.h>

utimes(file, tvp)
char *file;
struct timeval *tvp;
{
	struct utimbuf times;
	times.actime = tvp[0].tv_sec;
	times.modtime = tvp[1].tv_sec;
	return utime(file,&times);
}

sco_gettimeofday(tp, tzp)
struct timeval *tp;
struct timezone *tzp;
{
	if (tp == NULL)
		return -1;
	tp->tv_sec = time(0);
	tp->tv_usec = 0;
}

fsync(fd)
int fd;
{
	return 0;
}

#endif /* M_UNIX */


#ifdef NO_ITIMER_ENV
int
setitimer(type, new, old)
int type;
struct itimerval *new,*old;
{
	int prev = alarm(new->it_value.tv_sec);
	if (old)
		old->it_value.tv_sec = prev;
	return 0;
}
#endif /* NO_ITIMER_ENV */
