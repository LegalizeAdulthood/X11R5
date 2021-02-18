/*
 * xlogin - X login manager
 *
 * $Id: login.c,v 1.3 1991/10/04 17:35:16 stumpf Exp stumpf $
 *
 * Copyright 1989, 1990, 1991 Technische Universitaet Muenchen (TUM), Germany
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of TUM not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  TUM makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 *
 * Author:  Markus Stumpf, Technische Universitaet Muenchen
 *		<stumpf@Informatik.TU-Muenchen.DE>
 *
 * Filename: login.c
 * Date:     Thu 04 Jul 1991  13:08:17
 * Creator:  Markus Stumpf <Markus.Stumpf@Informatik.TU-Muenchen.DE>
 *
 * $Source: /usr/wiss/stumpf/R4/local/clients/xlogin-2.01/RCS/login.c,v $
 *
 */

#include <sys/param.h>
#ifdef QUOTA
#include <sys/quota.h>
#endif
#include <sys/stat.h>

#include <errno.h>
#ifdef LASTLOGIN
#include <lastlog.h>
#endif				/* LASTLOGIN */
#include <pwd.h>
#include <utmp.h>
/* EMPTY is defined in most <utmp.h> */
#ifndef EMPTY
#define EMPTY ""
#endif
#ifndef sequent
#include <ttyent.h>
#endif

#include "xlogin.h"

struct passwd  *pwd;
struct utmp     wtmp;
#ifdef LASTLOGIN
struct lastlog  rll;
#endif				/* LASTLOGIN */

char            XTTYname[BUFSIZ];
char            HushLog[] = ".hushlogin";
char            XLogFile[] = ".Xlogin";
char            HushLogin[BUFSIZ];
char            Lastlog[] = "/usr/adm/lastlog";
#ifndef UTMP_FILE
char            Utmp[] = "/etc/utmp";
#else
char            Utmp[] = UTMP_FILE;
#endif				/* UTMP_FILE */
#ifndef WTMP_FILE
char            Wtmp[] = "/usr/adm/wtmp";
#else
char            Wtmp[] = WTMP_FILE;
#endif				/* WTMP_FILE */
char            Motd[] = "/etc/motd";
char            NoLogin[] = "/etc/nologin";
char            hostname[BUFSIZ];
char            dpyname[BUFSIZ];
char            version[BUFSIZ];
char            Myname[BUFSIZ];
char            MyVersion[BUFSIZ];
char            MyRevision[] = "$Revision: 1.3 $";
char            DEVname[BUFSIZ];
char            XLogin[BUFSIZ];

char            ll_label[BUFSIZ];
int             do_login = TRUE;
int             do_logout;
int             info_eproclim = FALSE;
int             info_eusers = FALSE;
int             info_lastlog = FALSE;
int             is_hushlogin;
int             have_display = TRUE;


/*
 * external functions
 */
extern char    *getenv();
extern char    *strtok();
extern int      errno;
extern int      lseek(), exit();

extern void GenerateMotdList _P((void));
extern void LoadPreResources _P((int *, char **));
extern void InitXDisplay _P((int *, char **));
extern void DoXDisplay _P((void));
extern void     Debug();
extern void ExitMotd _P((Widget, XtPointer, XtPointer));
extern void CloseXDisplay _P(());


/*
 * forward definitions
 */
void main       _P((int, char **));
void Init       _P((void));
static void WriteWtmp _P((void));
static void CheckMotd _P((void));
static char    *getttys _P((int));
#ifdef sequent
static int xttyslot _P((void));
#endif				/* sequent */
#ifdef LASTLOGIN
static void DoLastLogin _P((void));
#endif				/* LASTLOGIN */
#ifdef QUOTA
static void CheckQuota _P((void));
#endif				/* QUOTA */

typedef struct {
    String          unchanged_label;
    String          changed_label;
}               xlogin_label_subresourceRec, *xlogin_label_subresourcePtr;

typedef struct {
    XFontStruct    *font;
}               xlogin_text_subresourceRec, *xlogin_text_subresourcePtr;

MotdWidgetClassRec mwc;
extern String   display;
extern xlogin_label_subresourceRec app_label_subresources;
extern xlogin_text_subresourceRec app_text_subresources;
extern xlogin_resourceRec app_resources;
extern xlogin_preresourceRec pre_app_resources;



/*
 * This procedure is called to initialize the passwd struct,
 * the paths to the various files and the utmp struct.
 */
void
#if NeedFunctionPrototypes
Init(
	void)
#else
Init()
#endif				/* NeedFunctionPrototypes */
{
    char           *user, *home;
    char           *number, *tmp;
    int             home_len;

    Debug("Init()");
    /*
     * get the user from the environment
     */
    if ((user = getenv("USER")) == NULL) {
	fprintf(stderr, "%s: cannot getenv(\"USER\") \n", Myname);
	exit(1);
    }
    Debug("Init(): user=%s ", user);
    /*
     * get the display from the environment,
     * if we hadn't it yet from the options
     */
    if (pre_app_resources.display == (String) NULL) {
	if ((display = getenv("DISPLAY")) == NULL) {
	    fprintf(stderr, "%s: cannot getenv(\"DISPLAY\") \n",
		    Myname);
	    exit(1);
	}
    }
    else
	display = pre_app_resources.display;
    Debug("Init(): display=%s ", display);
    /*
     * get the passwd entry of the user
     */
    if ((pwd = getpwnam(user)) == NULL) {
	endpwent();
	fprintf(stderr, "%s: cannot getpwnam(%s) \n", Myname, user);
	exit(1);
    }
    endpwent();
    /*
     * set the path to the users home directory
     */
    if ((home = getenv("HOME")) == NULL) {
	home = pwd->pw_dir;
    }
    Debug("Init(): home=%s ", home);
    /*
     * build up the variables pointing to the files we want
     * to use and check, whether we reserved enough space
     * otherwise exit
     */
    home_len = strlen(home) + 1;/* pathname + delimiter */
    if (home_len >= BUFSIZ) {
	fprintf(stderr, "%s: pathname length overflow \n", Myname);
	exit(1);
    }
    STRCPY(HushLogin, home);
    strcat(HushLogin, PATH_DELIMITER);
    STRCPY(XLogin, home);
    strcat(XLogin, PATH_DELIMITER);
    if (home_len + strlen(HushLog) > BUFSIZ) {
	fprintf(stderr, "%s: pathname length overflow \n", Myname);
	exit(1);
    }
    if (home_len + strlen(XLogFile) > BUFSIZ) {
	fprintf(stderr, "%s: pathname length overflow \n", Myname);
	exit(1);
    }
    strncat(HushLogin, HushLog, BUFSIZ - strlen(HushLogin));
    strncat(XLogin, XLogFile, BUFSIZ - strlen(XLogFile));

    /*
     * set up XTTYname from display information
     */
    STRCPY(dpyname, display);
    if (number = index(dpyname, ':')) {
	*number++ = '\0';
	if (tmp = index(dpyname, '.'))
	    *tmp = '\0';
	gethostname(hostname, sizeof(hostname));
	if (tmp = index(hostname, '.'))
	    *tmp = '\0';
	if (strcmp(dpyname, hostname) == 0 ||
	    strcmp(dpyname, "localhost") == 0 ||
	    strcmp(dpyname, "unix") == 0 ||
	    strcmp(dpyname, "") == 0) {
	    if (tmp = index(number, '.'))
		*tmp = '\0';
	    STRCPY(XTTYname, "X");
	    strncat(XTTYname, number,
		    sizeof(XTTYname) - strlen(XTTYname));
	}
	else
	    STRCPY(XTTYname, dpyname);
    }
    else
	STRCPY(XTTYname, "Xlogin");

    if (*display == ':') {
#ifdef HOSTNAME
	STRCPY(dpyname, hostname);
	strncat(dpyname, display, sizeof(dpyname) - strlen(dpyname));
#else
	*dpyname = '\0';
#endif				/* HOSTNAME */
    }
    else {
	STRCPY(dpyname, display);
    }

    Debug("Init(): XTTYName=%s, display=%s ", XTTYname, dpyname);
    /*
     * set up the utmp struct for the wtmp file
     */
    STRCPY(wtmp.ut_name, pwd->pw_name);
    STRCPY(wtmp.ut_host, dpyname);
    STRCPY(wtmp.ut_line, XTTYname);
    time((time_t *) & wtmp.ut_time);

    /*
     * check if $HOME/.hushlogin exists and set the
     * appropriate flag
     */
    is_hushlogin = (access(HushLogin, F_OK) == 0);
}

#ifdef QUOTA
/*
 * deny login because of bad quota ?
 */
#if NeedFunctionPrototypes
static void
CheckQuota(
	      void)
#else
static void
CheckQuota()
#endif				/* NeedFunctionPrototypes */
{
    int             retcode;

    Debug("CheckQuota()");
    Debug("CheckQuota(): userid=%d ", pwd->pw_uid);
    errno = 0;
    retcode = quota(Q_SETUID, 0, 0, (caddr_t) 0);
    Debug("CheckQuota(): quota returned %d, errno=%d", retcode, errno);
    if ((retcode = quota(Q_SETUID, pwd->pw_uid, 0, (caddr_t) 0) < 0)
	&& (errno != EINVAL)) {
	if (errno == EUSERS) {
	    do_login = FALSE;
	    info_eusers = TRUE;
	}
	else {
	    if (errno = EPROCLIM) {
		do_login = FALSE;
		info_eproclim = TRUE;
	    }
	}
    }
    Debug("CheckQuota(): quota returned %d, errno=%d", retcode, errno);
}

#endif				/* QUOTA */

/*
 * check if motd file exists and whether it is newer than
 * the lastlogin-time or if that is 0, if it is newer than
 * a possibly existing file $HOME/.Xlogin
 */
#if NeedFunctionPrototypes
static void
CheckMotd(
	     void)
#else
static void
CheckMotd()
#endif				/* NeedFunctionPrototypes */
{
    struct stat     buf;

    Debug("CheckMotd()");

    mwc.ll_time = (long) 0;

    if (access(NoLogin, F_OK) == 0) {
	Debug("    nologin-file %s found", NoLogin);
	mwc.motdFileString = NoLogin;
	if (pwd->pw_uid == 0) {
#ifdef LASTLOGIN
	    DoLastLogin();
	    if (rll.ll_time != 0) {
		Debug("    lastlogin time found");
		mwc.ll_time = rll.ll_time;
	    }
#endif				/* LASTLOGIN */
	    Debug("    ROOT may pass");
	}
	else {
	    do_login = False;
	    STRCPY(ll_label, app_resources.nologin_label);
	}
    }
    else {
#ifdef LASTLOGIN
	DoLastLogin();
	if (rll.ll_time != 0) {
	    Debug("    lastlogin time found");
	    mwc.ll_time = MAX(rll.ll_time, mwc.ll_time);
	}
	else
#endif				/* LASTLOGIN */
	{
	    if (stat(XLogin, &buf) == 0) {
		Debug("    file %s found", XLogin);
		utime(XLogin, NULL);
		mwc.ll_time = MAX(buf.st_mtime,
				  mwc.ll_time);
	    }
	}
    }
    GenerateMotdList();
    Debug("CheckMotd: leaving ...");
}



/*
 * Return the number of the slot in the utmp file
 * Definition is the line number in the /etc/ttys file.
 */
#ifndef sequent
#if NeedFunctionPrototypes
static int
xttyslot(
	    void)
#else
static int
xttyslot()
#endif				/* NeedFunctionPrototypes */
{
    register struct ttyent *ty;
    register int    s;

    setttyent();
    s = 0;
    while ((ty = getttyent()) != NULL) {
	s++;
	if (strcmp(ty->ty_name, XTTYname) == 0) {
	    endttyent();
	    return (s);
	}
    }
    endttyent();
    return (0);
}

#else
static char     ttys[] = "/etc/ttys";
static char    *_b, *_p;
static int      _c;
#if NeedFunctionPrototypes
static int
xttyslot(
	    void)
#else
static int
xttyslot()
#endif				/* NeedFunctionPrototypes */
{
    register int    s, tf;
    register char  *tp;
    char            b[1024];

    if ((tf = open(ttys, 0)) < 0)
	return (0);
    _p = _b = b;
    _c = 0;
    s = 0;
    while (tp = getttys(tf)) {
	s++;
	if (strcmp(XTTYname, tp) == 0) {
	    close(tf);
	    return (s);
	}
    }
    close(tf);
    return (0);
}

#if NeedFunctionPrototypes
static char    *
getttys(
	   int f)
#else
static char    *
getttys(f)
    int             f;
#endif				/* NeedFunctionPrototypes */
{

    static char     line[32];
    register char  *lp;

    lp = line;
    for (;;) {
	if (--_c < 0)
	    if ((_c = read(f, _p = _b, 1024)) <= 0)
		return (NULL);
	    else
		--_c;
	*lp = *_p++;
	if (*lp == '\n') {
	    *lp = '\0';
	    return (line + 2);
	}
	if (lp >= &line[sizeof line])
	    return (line + 2);
	lp++;
    }
}

#endif				/* sequent */



/*
 * this procedure writes the wtmp entry
 */
#if NeedFunctionPrototypes
static void
WriteWtmp(
	     void)
#else
static void
WriteWtmp()
#endif				/* NeedFunctionPrototypes */
{

    struct stat     buf;
    int             wtmpfd, utmpfd;
    int             xtty;

    Debug("WriteWtmp()");
    xtty = xttyslot();
    Debug("WriteWtmp(): xttyslot=%d", xtty);
    if (xtty > 0 && (utmpfd = open(Utmp, O_WRONLY)) >= 0) {
	Debug("WriteWtmp(): reading utmp file");
	lseek(utmpfd, (long) (xtty * sizeof(wtmp)), 0);
	write(utmpfd, (char *) &wtmp, sizeof(wtmp));
	close(utmpfd);
	if (strcmp(wtmp.ut_name, EMPTY) != 0) {
	    STRCPY(DEVname, "/dev/");
	    strncat(DEVname, XTTYname,
		    sizeof(DEVname) - strlen(DEVname));
	    if (stat(DEVname, &buf) == 0) {
		Debug("WriteWtmp(): uptiming %s", DEVname);
		utime(DEVname, NULL);
	    }
	}
    }
    /*
     * write only if file exists, don't create it
     */
    if ((wtmpfd = open(Wtmp, O_WRONLY | O_APPEND)) >= 0) {
	Debug("WriteWtmp(): writing wtmp file");
	write(wtmpfd, (char *) &wtmp, sizeof(wtmp));
	close(wtmpfd);
    }
    Debug("WriteWtmp(): leaving ...");
}



#ifdef LASTLOGIN
/*
 * this procedure reads from and writes to the
 * lastlogin file. If none exists, it will not be created.
 */
#if NeedFunctionPrototypes
static void
DoLastLogin(
	       void)
#else
static void
DoLastLogin()
#endif				/* NeedFunctionPrototypes */
{
    struct lastlog  wll;
    int             llfd;	/* Lastlog file descriptor */

    Debug("DoLastLogin()");
    rll.ll_time = 0;
    STRCPY(ll_label, "Last Login: ");
    if ((llfd = open(Lastlog, O_RDONLY)) >= 0) {
	/*
	 *  get time of last login, if any
	 */
	Debug("DoLastLogin(): reading lastlog file");
	lseek(llfd, (long) pwd->pw_uid * sizeof(struct lastlog), 0);
	if (read(llfd, (char *) &rll, sizeof(rll)) == sizeof(rll)
	    && (rll.ll_time != 0)) {
	    strncat(ll_label, ctime((long *) &rll.ll_time), 24);
	    if (strcmp(rll.ll_host, "") != 0) {
		strcat(ll_label, " from ");
		strncat(ll_label, rll.ll_host, 16);
	    }
	    info_lastlog = !is_hushlogin;
	}
	close(llfd);
	/*
	 * write new last login time
	 */
	if ((llfd = open(Lastlog, O_WRONLY)) >= 0) {
	    Debug("DoLastLogin(): writing lastlog file");
	    lseek(llfd, (long) pwd->pw_uid * sizeof(struct lastlog), 0);
	    wll.ll_time = wtmp.ut_time;
	    STRCPY(wll.ll_line, XTTYname);
	    STRCPY(wll.ll_host, wtmp.ut_host);
	    write(llfd, (char *) &wll, sizeof(wll));
	    close(llfd);
	}
    }
    Debug("DoLastLogin(): leaving ...");
}

#endif				/* LASTLOGIN */


#if NeedFunctionPrototypes
void
main(
	int argc,
	char *argv[])
#else
void
main(argc, argv)
    int             argc;
    char           *argv[];
#endif				/* NeedFunctionPrototypes */
{
    int             i;

    STRCPY(Myname, argv[0]);
    MyRevision[0] = ' ';
    MyRevision[strlen(MyRevision) - 1] = '\0';

    sprintf(MyVersion, "%s.%s\0", VERSION, PATCHLEVEL);
    sprintf(version, "version %s.%s -%s\0",
	    VERSION, PATCHLEVEL, MyRevision);

    LoadPreResources(&argc, argv);
    if (pre_app_resources.version_only) {
	fprintf(stderr, "%s: %s \n", Myname, version);
	exit(1);
    }

    do_logout = pre_app_resources.logout;

    if ((argc > 1) && do_logout) {
	for (i = 2; i <= argc; i++) {
	    fprintf(stderr, "%s: ignoring unknown option %s \n", Myname,
		    argv[i - 1]);
	}
    }

    Init();
    Debug("main(): after Init()");

    if (do_logout) {
	Debug("main(): logging out ...");
	STRCPY(wtmp.ut_name, EMPTY);
	WriteWtmp();
	exit(0);
    }

    InitXDisplay(&argc, argv);
    Debug("main(): after InitXDisplay()");

    if ((argc > 1) && have_display) {
	for (i = 2; i <= argc; i++) {
	    fprintf(stderr, "%s: ignoring unknown option %s \n", Myname,
		    argv[i - 1]);
	}
    }

    do_login = !do_logout;

    if (have_display) {
	mwc.motd_always = (XmuCompareISOLatin1(
				app_resources.show_MOTD, "always") == 0);

	mwc.motdFileString = (char *) malloc(
				    strlen(pre_app_resources.sys_files) +
				   strlen(app_resources.user_files) + 2);
	if (mwc.motdFileString == (char *) NULL) {
	    fprintf(stderr, "%s: main() cannot perform malloc\n",
		    Myname);
	    fprintf(stderr, "%s:        will try to handle sysFiles\n",
		    Myname);
	    mwc.motdFileString = pre_app_resources.sys_files;
	}
	else
	    sprintf(mwc.motdFileString, "%s:%s",
		    pre_app_resources.sys_files,
		    app_resources.user_files);
    }

    else {
	mwc.motd_always = False;
	mwc.motdFileString = pre_app_resources.sys_files;
    }

    Debug("    motdFileString=%s ", mwc.motdFileString);

#ifdef QUOTA
    CheckQuota();
#endif				/* QUOTA */

    if (do_login)
	CheckMotd();
    if (do_login)
	WriteWtmp();

    /*
     * display motd only if no .hushlogin exists
     * and  we have a DISPLAY open!
     */
    Debug("    %s.hushlogin found", is_hushlogin ? "" : "no ");
    if (have_display && !is_hushlogin) {
	DoXDisplay();
    }

    exit(!do_login);
}
