/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */

#ifndef lint
static char rcs[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/RK/RCS/RKfile.c,v 1.8 91/08/09 17:55:30 mako Exp $";
#endif
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <ctype.h>
#include	"RK.h"
#include	"RKintern.h"
#include        "RKdic.h"
#ifdef DEBUG
#include <errno.h>
#endif

extern int mkdir();
extern int fputs();
extern int fclose();
extern int fprintf();
extern int rename();

extern errno;

int
RemoveDicFile( dm )
  struct DM *dm;
{
    struct DF	*df=dm->dm_file;
    struct DD	*dd=df->df_direct;
    FILE	*fp_src, *fp_obj;
    int		Oldumsk;
    register char *s;
    char	line[RECSZ*4], buff[RECSZ*4];
    char	file_path[RECSZ];
    char	dics_path_org[RECSZ], dics_path_bk[RECSZ];
    char	link[RECSZ], member[RECSZ], nickname[RECSZ];
    int		cfdes;

    if (!dm ||
	!df ||
	!dd ||
	!(dd->dd_path) ||
	!(dm->dm_nickname)) {
	return -1;
    }
    (void)sprintf(file_path,"%s/%s", dd->dd_path, df->df_link);
    (void)sprintf(dics_path_org, "%s/dics.dir", dd->dd_path);
    (void)sprintf(dics_path_bk, "%s/#dics_dir", dd->dd_path);

    Oldumsk = umask(002);

    if (access(dd->dd_path, F_OK) == -1)
	return -1;
    if (access(dics_path_org, F_OK) == -1) {
	if ((cfdes = creat(dics_path_org, 0664)) < 0) {
	    return -1;
	}
	close(cfdes);
    }
    fp_src = fopen(dics_path_org, "r");
    fp_obj = fopen(dics_path_bk, "w");
    if ((fp_src == (FILE *)NULL) ||
	(fp_obj== (FILE *)NULL)) {
	(void)umask(Oldumsk);
	(void)unlink(dics_path_bk);
	(void)fclose(fp_src);
	(void)fclose(fp_obj);
	return -1;
    }
    while (fgets(s = line, sizeof(line), fp_src)) {
	if (CheckLine(s, link, member, nickname) == -1) {
	    if(line[0] == '#') {
		(void)fputs (line, fp_obj);
	    }
	    continue;
	}
	(void)sprintf(buff,"%s/%s", dd->dd_path, link);
	if ( access(buff, F_OK) ) {
#ifdef DEBUG
	    Debug("RemoveDicFile: cannot access %s", buff);
#endif
	    continue;
	}
	if (strncmp(nickname, dm->dm_nickname, RECSZ) == 0) {
#ifdef DEBUG
	    Debug("RemoveDicFile: skipping %s", nickname);
#endif
	    continue;
	}
	(void)fputs(line, fp_obj);
    }
    (void)fflush(fp_obj);
    (void)fclose(fp_obj);
    (void)fclose(fp_src);
    if (rename(dics_path_bk, dics_path_org) == 0) {
#ifdef DEBUG
	Debug ("RenameDicFile: cannot rename from %s to %s by %d\n",
		 dics_path_bk, dics_path_org, errno);
#endif
	(void)unlink(file_path);
	(void)umask(Oldumsk);
	return 0;
    }
    (void)unlink(dics_path_bk);
    (void)umask(Oldumsk);
    return -1;
}
int
MakeDicFile(dm, dicname, mode, filename)
  struct DM	*dm;
  char		*dicname;
  int		mode;
  char		*filename;
{
    struct DF	*df=dm->dm_file;
    struct DD	*dd=df->df_direct;
    FILE	*fp_src,*fp_obj;
    int		Oldumsk, ret = 0;
    register char	*s;
    char line[RECSZ*4], buff[RECSZ*4];
    char file_path[RECSZ], dics_path_org[RECSZ], dics_path_bk[RECSZ];
    char link[RECSZ], member[RECSZ], nickname[RECSZ];
    int		cfdes;

    if (!dm ||
	!df ||
	!dd ||
	!(dd->dd_path) ||
	!(dm->dm_nickname)) {
	return -1;
    }

    (void)sprintf(file_path,"%s/%s", dd->dd_path, filename);
    (void)sprintf(dics_path_org,"%s/dics.dir", dd->dd_path);
    (void)sprintf(dics_path_bk,"%s/#dics.dir",dd->dd_path);

    Oldumsk = umask(002);
    if (access(dd->dd_path, F_OK) == -1)
	return -1;
    if (access(dics_path_org, F_OK) == -1) {
      if ((cfdes = creat(dics_path_org, 0664)) < 0) {
	return -1;
      }
      close(cfdes);
    }

    fp_src = fopen(dics_path_org, "r");
    fp_obj = fopen(dics_path_bk, "w");
    if ((fp_src == (FILE *)NULL) ||
	(fp_obj == (FILE *)NULL)) {
	(void)unlink(dics_path_bk);
	(void)umask(Oldumsk);
	(void)fclose(fp_src);
	(void)fclose(fp_obj);
#ifdef DEBUG
	Debug("MakeDicFile: cannot open %s or %s",
	      dics_path_org,
	      dics_path_bk);
#endif
	return -1;
    }
    while (fgets(s = line, sizeof(line), fp_src)) {
	if (CheckLine(s, link, member, nickname) == -1) {
	    if(line[0] == '#') {
		(void)fputs(line, fp_obj);
	    }
	    continue;
	}
	(void)sprintf(buff,"%s/%s", dd->dd_path, link);
	if ( access(buff, F_OK) ) {
#ifdef DEBUG
	    Debug("MakeDicFile: cannot access %s", buff);
#endif
	    continue;
	}
	if (strncmp(nickname, dicname, RECSZ) == 0) {
#ifdef DEBUG
	    Debug("MakeDicFile: dicname is %s, skip", nickname);
#endif
	    continue;
	}
	(void)fputs(line, fp_obj);
    }
#ifdef DEBUG
    Debug("MakeDicFile: filename is %s", filename);
#endif
    switch (mode) {
      case Rk_MWD:
	(void)fprintf(fp_obj,"%s(.mwd)\t-%s-\n", filename, dicname);
	break;
      case Rk_SWD:
	(void)fprintf(fp_obj,"%s(.swd)\t-%s-\n", filename, dicname);
	break;
/*
 * not apply now.
 *     case Rk_PRE:
 *     case Rk_SUC:
 */
      default:
#ifdef DEBUG
	Debug("MakeDicFile: irregal mode %d", mode);
#endif
	ret = -1;
	break;
    }
    (void)fflush(fp_obj);
    (void)fclose(fp_src);
    (void)fclose(fp_obj);
    cfdes = -1;
    if ((ret == 0) &&
	((cfdes = creat(file_path, 0664)) >= 0) &&
	(rename(dics_path_bk, dics_path_org) == 0 )) {
	(void)umask(Oldumsk);
	close(cfdes);
	return 0;
    }
    if (cfdes >= 0) {
      close(cfdes);
    }
    (void)unlink(dics_path_bk);
    (void)unlink(file_path);
    (void)umask(Oldumsk);
#ifdef DEBUG
    Debug ("MakeDicFile: cannot create/rename %s / %s",
	   file_path, dics_path_bk);
#endif
    return -1;
}
int
RenameDicFile( dm, dicname )
  struct DM *dm;
  char *dicname;
{
    struct DF	*df=dm->dm_file;
    struct DD	*dd=df->df_direct;
    FILE	*fp_src, *fp_obj, *fopen();
    register char *s;
    char	line[RECSZ*4], buff[RECSZ*4];
    char	dics_path_org[RECSZ], dics_path_bk[RECSZ];
    char	link[RECSZ], member[RECSZ], nickname[RECSZ];
    int		Oldumsk;
    int		cfdes;

    if (!dm ||
	!df ||
	!dd ||
	!(dd->dd_path) ||
	!(dm->dm_nickname) ||
	!(df->df_link)) {
	return -1;
    }

    (void)sprintf(dics_path_org, "%s/%s", dd->dd_path, "dics.dir");
    (void)sprintf(dics_path_bk, "%s/%s", dd->dd_path, "#dics_dir");

    Oldumsk = umask(002);
    if (access(dd->dd_path, F_OK) == -1)
	return -1;
    if (access(dics_path_org, F_OK) == -1) {
	if ((cfdes = creat(dics_path_org, 0664)) < 0) {
	    return -1;
	}
	close(cfdes);
    }
    fp_src = fopen(dics_path_org, "r");
    fp_obj = fopen(dics_path_bk, "w");
    if ((fp_src == (FILE *)NULL) ||
	(fp_obj == (FILE *)NULL)) {
	(void)umask(Oldumsk);
	(void)unlink(dics_path_bk);
	(void)fclose(fp_src);
	(void)fclose(fp_obj);
	return -1;
    }
    while (fgets(s = line, sizeof(line), fp_src)) {
	if (CheckLine(s, link, member, nickname) == -1) {
	    if(line[0] == '#') {
		(void)fputs(line, fp_obj);
	    }
	    continue;
	}
	(void)sprintf(buff,"%s/%s", dd->dd_path, link);
	if ( access(buff, F_OK) ) {
#ifdef DEBUG
	    Debug("cannot access %s", buff);
#endif
	    continue;
	}
	if ((strncmp(link, df->df_link, RECSZ) == 0) &&
	    (strncmp(nickname, dm->dm_nickname, RECSZ) == 0)) {
	    continue;
	}
	(void)fputs(line, fp_obj);
    };
    if (dm->dm_class == ND_MWD) {
	(void)fprintf(fp_obj, "%s(.mwd)\t-%s-\n",
		      df->df_link, dicname);
    } else if (dm->dm_class == ND_SWD) {
	(void)fprintf(fp_obj, "%s(.swd)\t-%s-\n",
		      df->df_link, dicname);
	/*
	 * Not Apply
	 * } else if (dm->dm_class == ND_SUC) {
	 * } else if (dm->dm_class == ND_PRE) {
	 */
    };
    (void)fflush(fp_obj);
    (void)fclose(fp_obj);
    (void)fclose(fp_src);
    (void)umask(Oldumsk);
    if ((rename(dics_path_bk, dics_path_org) != -1)) {
	return 0;
    }
    (void)unlink(dics_path_bk);
    return -1;
}

unsigned char *
CheckDicFile(dm)
  struct DM	*dm;
{
    struct DF	*df=dm->dm_file;
    struct DD	*dd=df->df_direct;
    FILE	*fp;
    register char	*s;
    char line[RECSZ*4], dics_path[RECSZ*4];
    char link[RECSZ], member[RECSZ], nickname[RECSZ];
    if (access(dd->dd_path, F_OK) == -1) {
#ifdef DEBUG
	Debug("CheckDicFile: cannot access %s", dd->dd_path);
#endif
	return (unsigned char *)NULL;
    }
    (void)sprintf(dics_path,"%s/dics.dir", dd->dd_path);
    fp = fopen(dics_path,"r");
    if (fp == (FILE *)NULL) {
#ifdef DEBUG
	Debug("CheckDicFile: cannot open %s", dics_path);
#endif
	return (unsigned char *)NULL;
    }
    while (fgets(s = line, sizeof(line), fp)) {
#ifdef DEBUG
	Debug("CheckDicFile: %s",line);
#endif
	if (CheckLine(s, link, member, nickname) == -1) {
	    continue;
	}
	if ( !(s = (char *)strrchr(member, '.' )) ) {
	    continue;
	}
	s++;
	if ((strncmp (s, "mwd", RECSZ) != 0) &&
	    (strncmp (s, "swd", RECSZ) != 0))
	  {
#ifdef DEBUG
	    Debug("CheckDicFile: irregal member %s",member);
#endif
	    continue;
	}
	if ((strncmp(nickname, (char *)dm->dm_nickname, RECSZ) == 0) &&
	    (strncmp(link, (char *)df->df_link, RECSZ) == 0)) {
#ifdef DEBUG
	    Debug("CheckDicFile: find dictionary %s, file %s mode %s",
		  nickname, link, member);
#endif
	    fclose(fp);
	    return (unsigned char *)link;
	}
    }
    fclose(fp);
    return (unsigned char *)NULL;
}

/*
 * parse line
 *	link(mode) -nickname-
 */
int
CheckLine(line, link, member, nickname)
  char *line;
  char *link;
  char *member;
  char *nickname;
{
    register char *d, *s = line, par;	/* local ptr of line */
    register char *t;
    if ( !isalpha(line[0]) ) {
#ifdef DEBUG
	Debug("Checkline: skip %s", line);
#endif
	return -1;
    }
    for ( d = link; *s && *s != '('; ) *d++ = *s++;
    *d = '\0';
    if ( !*s++ ) return -1;
    if ( !(t = (char *)strrchr(link, '.' )) ) {
#ifdef DEBUG
	Debug("Checkline: skip %s", line);
#endif
	return -1;
    }
    t++;
    if(strncmp(t,"d", RECSZ) &&
       strncmp(t,"t", RECSZ))
	return -1;
    /*
     * #ifdef DEBUG
     * Debug( "Checkline: link is %s", link);
     * #endif
     */
    for ( d = member; *s && *s != ')'; ) *d++ = *s++;
    *d = '\0';
    if ( !*s++ ) return -1;
    if ( !(t = (char *)strrchr(member, '.' )) ) {
#ifdef DEBUG
	Debug("Checkline: skip %s", line);
#endif
	return -1;
    }
    t++;
    /*
     * #ifdef DEBUG
     * Debug( ": member is %s", member);
     * #endif
     */
    if (strncmp(t, "mwd", RECSZ) == 0
     && strncmp(t, "swd", RECSZ) == 0
/*   && strncmp(t, "pre", RECSZ) == 0 &&
 * 	strncmp(t, "suc", RECSZ) == 0
 */
	)
	return -1;
    while ( *s && isspace(*s) ) s++;
    if ( !(par = *s++) )
	return -1;
    if ((! *s) || (*s == par))
	return -1;
    d = nickname;
    *d++ = *s++;
    while ( *s && (*s != par)) *d++ = *s++;
    *d = '\0';
#ifdef DEBUG
    Debug("CheckLine: line is %s(%s)\t-%s-", link, member, nickname);
#endif
    return 0;
}

void
Debug(fmt, a, b, c)
  char *fmt, *a, *b, *c;
{
    char msg[512];
    (void)sprintf(msg, fmt, a, b, c);
    (void)fprintf(stderr, "%s\n", msg);
    (void)fflush(stderr);
}

unsigned char *
GetDicFilename(dd)
  struct DD	*dd;
{
    char *f_p_name[RECSZ], dics_dir[RECSZ];
    static char *f_name[RECSZ];
    register int i = 0, Oldumsk;
    int cfdes;

    /*
     * checks directory(/usr/lib/iroha/dic/???)
     */

    if (!dd || !(dd->dd_path)) {
	return (unsigned char *)NULL;
    }

    Oldumsk = umask(0);
    if (access(dd->dd_path, F_OK) == -1) {
	if(mkdir(dd->dd_path, 0775) == -1) {
#ifdef DEBUG
	    Debug ("GetDicFilename: cannot mkdir %s", dd->dd_path);
#endif
	    (void)umask(Oldumsk);
	    return (unsigned char *)NULL;
	}
    }
    /*
     * checks file(dics.dir)
     */
    (void)sprintf(dics_dir,"%s/dics.dir", (char *)dd->dd_path);
    if (access(dics_dir, F_OK) == -1) {
	if ((cfdes = creat((char *)dics_dir, 0644))<0) {
#ifdef DEBUG
	    Debug ("GetDicFilename: cannot create %s", dics_dir);
#endif
	    (void)umask(Oldumsk);
	    return (unsigned char *)NULL;
	}
	close(cfdes);
    }
    while (++i) {
	(void)sprintf((char *)f_name,"user%d.t\0", i);
	(void)sprintf((char *)f_p_name,"%s/%s\0", dd->dd_path, f_name);
	if (access((char *)f_p_name, F_OK) == -1) {
#ifdef DEBUG
	    Debug ("GetDicFilename: found %s, and return %s.",
		   f_p_name, f_name);
#endif
	    (void)umask(Oldumsk);
	    return (unsigned char *)f_name;
	}
    }
#ifndef lint
#ifdef DEBUG
    Debug ("GetDicFilename: irregal error, return NULL.");
#endif
#endif
#ifdef lint
    (void)umask(Oldumsk);
    return (unsigned char *)NULL;
#endif
}
