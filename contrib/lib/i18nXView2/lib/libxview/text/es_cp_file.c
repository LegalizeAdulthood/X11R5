#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)es_cp_file.c 50.6 90/11/30";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Routines to copy a file.  Stolen from cp.c, then modified.
 */
#include <stdio.h>
#ifdef OW_I18N
#include <stdlib.h>
#include <sys/types.h>  /* For size_t */
#include <sys/file.h>
#include <xview/xv_i18n.h>
#endif
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <xview/pkg.h>
#include <xview/attrol.h>

#ifndef BSIZE
#define BSIZE   8192
#endif BSIZE

#ifndef SVR4
#define strrchr rindex
#endif SVR4

#ifndef OW_I18N
#ifndef SVR4
char           *rindex(), *sprintf();
#else SVR4
char    *strrchr();
#endif SVR4
#endif

es_copy_file(from, to)
    CHAR            *from, *to;
{
#ifdef OW_I18N
    int             from_fd, result;
    char	    *from_mb;
#else
    int             from_fd = open(from, 0), result;
#endif

#ifdef OW_I18N
    from_mb  = wcstombsdup(from_mb);
    from_fd = open(from_mb, 0);
    free(from_mb);
#endif
    if (from_fd < 0)
	return 1;
    result = es_copy_fd(from, to, from_fd);
    (void) close(from_fd);
    return (result);
}

#define FSTAT_FAILED	-1
#define WILL_OVERWRITE	 1
Pkg_private int
es_copy_status(to, fold, from_mode)
    char            *to;
    int             fold;
    int            *from_mode;
{
    struct stat     stfrom, stto;

    if (fstat(fold, &stfrom) < 0)
	return (FSTAT_FAILED);
    if (stat(to, &stto) >= 0) {
	if (stfrom.st_dev == stto.st_dev &&
	    stfrom.st_ino == stto.st_ino) {
	    return (WILL_OVERWRITE);
	}
    }
    *from_mode = (int) stfrom.st_mode;
    return (0);
}

#define	es_Perror(s)
#ifdef OW_I18N
Pkg_private int
es_copy_fd(from_wc, to_wc, fold)
    CHAR            *from_wc, *to_wc;
    int             fold;
#else
Pkg_private int
es_copy_fd(from, to, fold)
    char            *from, *to;
    int             fold;
#endif
{

    int             fnew, fnew_mode, n;
    		    /* OW_I18N: Does not make sense for destname to
    		     * be size of BSIZE.  MAXNAMLEN is good enough.
    		     * nng
    		     */
    char            *last, destname[MAXNAMLEN], buf[BSIZE];
    struct stat     stto;
#ifdef OW_I18N
    char            from[MAXNAMLEN], to[MAXNAMLEN];
    
    wcstombs(from, from_wc, MAXNAMLEN);
    wcstombs(to, to_wc, MAXNAMLEN);
#endif    

    if (stat(to, &stto) >= 0 &&
	(stto.st_mode & S_IFMT) == S_IFDIR) {
	last = strrchr(from, '/');
	if (last)
	    last++;
	else
	    last = from;
	if (strlen(to) + strlen(last) >= MAXNAMLEN - 1) {
	    return (1);
	}
	(void) sprintf(destname, "%s/%s", to, last);
#ifdef OW_I18N
	strcpy(to, destname);
#else	
	to = destname;
#endif	
    }
    switch (es_copy_status(to, fold, &fnew_mode)) {
      case FSTAT_FAILED:
	es_Perror(from);
	return (1);
      case WILL_OVERWRITE:
	return (1);
      default:
	break;
    }
    fnew = creat(to, fnew_mode);
    if (fnew < 0) {
	es_Perror(to);
	return (1);
    }
    for (;;) {
	n = read(fold, buf, BSIZE);
	if (n == 0)
	    break;
	if (n < 0) {
	    es_Perror(from);
	    (void) close(fnew);
	    return (1);
	}
	if (write(fnew, buf, n) != n) {
	    es_Perror(to);
	    (void) close(fnew);
	    return (1);
	}
    }
    (void) close(fnew);
    return (0);
}


#ifdef OW_I18N
size_t
my_mbstowcs(pwcs, s, n)
wchar_t *pwcs;
char *s;
size_t 		*n;
{
        register short i, j;
        register char	*old;
 
        j = 0;
        old = s;
        while (*s) {    /* Until we encounter the NUL. */
                if (j >= (short )*n)
                        break;  /* We has used up the array. */
                           
                i = mbtowc(pwcs + j, s, MB_CUR_MAX);
                if (i == -1) {
                     pwcs[j] = 0;
                     *n = s - old;
                     return (size_t)j;
                }       
                s += i;
                ++j;
        }
        if (j < (short)*n) {    /* Has enough room for the terminator. */
                pwcs[j]=0;
        }
        return (size_t)j;
}        

int
es_mb_to_wc_fd(from, to, fold)
    char            *from, *to;
    int             fold;
{
    int             fnew_mode, n, temp, len_in_wc;
    char            *last, destname[MAXNAMLEN], buf[BSIZE + 1];
    CHAR	    buf_ws[BSIZE + 10];
    int		    fnew;
    struct stat     stto;
    int		    old_pos, new_pos;

    if (stat(to, &stto) >= 0 &&
	(stto.st_mode & S_IFMT) == S_IFDIR) {
	last = rindex(from, '/');
	if (last)
	    last++;
	else
	    last = from;
	if (strlen(to) + strlen(last) >= MAXNAMLEN - 1) {
	    return (1);
	}
	(void) sprintf(destname, "%s/%s", to, last);
	to = destname;
    }
    switch (es_copy_status(to, fold, &fnew_mode)) {
      case FSTAT_FAILED:
	es_Perror(from);
	return (1);
      case WILL_OVERWRITE:
	return (1);
      default:
	break;
    }
    fnew = creat(to, fnew_mode);
    if (fnew < 0) {
	es_Perror(to);
	return (1);
    }
    old_pos = new_pos = 0;
    for (;;) {
	n = read(fold, buf, BSIZE);
	if (n == 0)
	    break;
	if (n < 0) {
	    es_Perror(from);
	    (void) close(fnew);
	    return (1);
	}
	temp = n;
	buf[n] = NULL;
	
	len_in_wc = my_mbstowcs(buf_ws, buf, &temp);
	
	if (temp != n) {
	   /* re-read the incomplete mb character */
	    new_pos = lseek(fold, temp - n, L_INCR);
	    if (new_pos == old_pos) {
	       /* Invalid char, so advance to next byte */
	        old_pos = lseek(fold, 1L, L_INCR);   
	    } else
	        old_pos = new_pos;
	}
	if (WRITE(fnew, (char *)buf_ws, len_in_wc) != len_in_wc) {
	    es_Perror(to);
	    (void) close(fnew);
	    return (1);
	}
    }
    close(fnew);
    return (0);
}
#endif
