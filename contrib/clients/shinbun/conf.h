/* $Header: conf.h.dist,v 1.9 91/02/08 18:31:01 sob Exp $
 * Configuration information for use by NNTP server and support
 * programs.  Change these as appropriate for your system.
 */

/*
 * Compile time options.
 */
  

#undef	ALONE		/* True if we're running without inetd */

#ifdef ALONE
#undef	FASTFORK        /* True if we don't want to read active file on start*/
#endif

#define LOAD 5	        /* Loadav above which server refuses connections */
/*
 * If you want to experiment with the dynamic allocation of the article
 * array, define DYNAMIC_ART_ARRAY.
 * This code came to beta too late for through testing, but it is a problem
 * at some sites, so it is an option. Please report problems to "nntp@tmc.edu"
 */
#undef DYNAMIC_ART_ARRAY

#undef	BSD_42		/* 4.2 compatability code -- if this is defined, */
			/* DBM probably wants to be defined as well. */

#define BSD_43		/* Define if you are running on BSD 4.3 */

#undef CMU_MACH		/* Use CMU's MACH ioctl(FIOCFSPARAM) for dfree(). */

#undef	USG		/* System V support */

#undef TLI		/* Define this if you want to use TLI instead of */
			/* sockets */

#define NDBM		/* Use new-style (4.3) ndbm(3x) libraries */

#undef	DBM		/* True if we want to use the old dbm(3x) libraries */
			/* IF YOU DEFINE THIS, change CFLAGS in makefile to */
			/* be -ldbm */

#undef	DBZ		/* True if we want to use dbz libraries */
			/* IF YOU DEFINE THIS, change CFLAGS in makefile to */
			/* be /usr/lib/dbz.o and install dbz.h */

#undef	USGHIST		/* Use USG style history file (no DBM) */
			/* IF YOU DO NOT DEFINE NDBM or DBM, this is DEFAULT!*/

#undef CNEWS		/* define this if you are running C-NEWS */
#undef BATCHED_INPUT	/* define if you want to support C-NEWS style 
				batched input (not supported by B-NEWS,yet)  */

/* Vendor specific implementations */
#undef LAI_TCP		/* Lachman Streams TCP/IP support (Xenix) */
#undef EXCELAN		/* Excelan EXOS 205 support */
#undef WIN_TCP		/* WIN/TCP support [does not work yet] */

/* Things that vary in Unix variations */
#undef U_LONG		/* Define this if your <sys/types.h> is missing */
			/* typedefs for u_long */
#define SIGRET void	/* Newfangled signal() returns void, old returns int */

#define	GHNAME		/* Define if you have gethostname() */
#undef	UUNAME		/* Define to use /etc/uucpname */
			/* If neither of these are defined, */
			/* inews will use the contents of */
			/* /usr/include/whoami.h */

#undef MMAP		/* if your OS supports mmap() */

/*
 * If you DON'T have vfork, make this "#define vfork fork"
 * vfork will speed up article transfer nntpds by about 2.5 times.
 */

/* #define	vfork fork */

/* Controlling disk and inode consumption */
#define MINFREE 4000	/* NNTP will not allow an XFER if there is less */
			/* than this much diskspace (in blocks or kbytes) */
#define POSTBUFFER 1000 /* NNTP will allow local posting until */
			/* MINFREE-POSTBUFFER blocks or kbytes are left */
#undef MINFILES  MINFREE/4
			/* NNTP will not allow an XFER if there is less */
			/* than this many inodes on the SPOOLDIR filesystem */
#undef SETPROCTITLE	/* if you want status visable via ps */

#undef	IHAVE_DEBUG	/* Copious debugging output from ihave */

#define	XHDR		/* Optional XHDR command.  Defining this will */
			/* speed up '=' command in rn, but will load */
			/* the server more.  If your server is heavily */
			/* loaded already, defining this may be a bad idea */

/* Things that vary in network implementations */
#define	SUBNET		/* If you have 4.3 subnetting */
#undef	DAMAGED_NETMASK	/* If your subnet mask is not a multiple of */
			/* four bits (e.g., UCSD) */

#undef	NETMASK		/* If you don't have subnet ioctls, define */
			/* this to be a hex constant of your subnet */
			/* mask, e.g., #define NETMASK 0xffffff00 */
			/* Of course, you must define SUBNET above, too. */
#undef	DECNET		/* If you want decnet support */

/*
 * If you want CNEWS batch files created that are not world writable,
 * remove the comments from the UMASK line below. This does not apply
 * if you are running BNEWS. At least, not yet. :-)
 */

/* #define UMASK 022 */

/*
 * If you have the syslog library routine, define SYSLOG to
 * be the syslog facility name under which stats should be
 * logged.  Newer 4.3 systems might choose LOG_NEWS;
 * LOG_LOCAL7 is an acceptable substitute.
 *
 * If you don't have support for syslog, but want a facsimile,
 * define FAKESYSLOG to be the name of a file to which to log stuff,
 * then define SYSLOG and LOG, too.  e.g.,
 *
 *	#define	FAKESYSLOG	"/usr/lib/news/nntplog"
 *
 * If your host supports the BSD fdopen() function and the O_APPEND flag
 * to open(), you should define FAKEAPPEND with FAKESYSLOG so that
 * multiple copies of nntpd don't trash the log with buffered fprintf's.
 *
 * If you don't want any syslog-type activity, #undef SYSLOG.
 * Obviously, this means that you can't define LOG, either.
 */

#undef	FAKESYSLOG	"/usr/lib/news/nntplog"
#undef	FAKEAPPEND

#define	SYSLOG	LOG_NEWS

#ifdef SYSLOG		/* Define LOG if you want copious logging info */
#undef	 LOG		/* undef it if you don't */
#endif			/* but you can only have LOG if you have SYSLOG */

/*
 * How long you want nntp servers to hang out without receiving
 * commands before they close the connection with an error message.
 *
 * If you don't want any timeout, #undef it, i.e.,
 *
 *	#undef	TIMEOUT
 *
 * TIMEOUT should be at least two hours, which allows users some time
 * away from their terminal (e.g., at lunch) while reading news.
 */

#define	TIMEOUT	(2 * 3600)


/*
 * How long you want nntp servers to wait without receiving data
 * during article transfers.  You CANNOT have XFER_TIMEOUT while
 * running in standalone (ALONE) mode.
 *
 * If you don't want any transfer timeouts, #undef it, as above.
 */

#ifndef ALONE
#   define	XFER_TIMEOUT	(30 * 60)
#endif ALONE

/*
 * Your domain.  This is for the inews generated From: line,
 * assuming that it doesn't find one in the article's head.
 * Suggestions are .UUCP if you don't belong to the Internet.
 * If your hostname returns the fully-qualified domain name
 * as some 4.3 BSD systems do, simply undefine DOMAIN.
 * If you want your network to appear to be one host, define
 * HIDDENNET.
 *
 * e.g.  #define	DOMAIN		"berkeley.edu"
 */

#define	DOMAIN	"uucp"
#undef HIDDENNET

/*
 * Means do a gethostbyname() to get the canonical host name. 
 * Define this and HIDDENNET will have no effect.
 */

#undef REALDOMAIN

/*
 * Define DO_DOTDIR if you want the mini-inews to look for 
 * this directory if defined in the environment.  This is like rn.
 * Undefine it and it will only look in the user's home directory. 
 */
#define DO_DOTDIR

/* Things that relate to authentication and access */
/* Define AUTH to use the proposed NNTP Version 2 authentication protocol. */
#define	AUTH	
/*
 * A file containing the name of the host which is running
 * the news server.  This will have to match what rrn thinks,
 * too.
 */

#define	SERVER_FILE	"/usr/local/lib/rn/server"

/*
 * Person (user name) to post news as.
 */

#define	POSTER		"usenet"

/*
 * Define DOMAINMATCH if you want to use domain specifications in the 
 * access list instead of just hostnames. See README for more information 
 */
#define DOMAINMATCH

#ifdef AUTH
/* 
 * the file where the nntpxmit site/userid/passwords are kept
 * think (and PROTECT!) this file like L.sys, i.e., mode 600
 */
# define	PASSFILE	"/etc/nntp.sys"
#endif
/*
 * This file is where access information is stored
 */
#define ACCESS_FILE	"/usr/lib/news/nntp_access"
/*
 * These files are generated by the support programs, and are needed
 * by the NNTP server.  Make sure that whatever directory you
 * decide these files should go is writable by whatever uid you
 * have the sypport programs run under.
 */
# define STAT_FILE	"/usr/lib/news/mgdstats"
# define NGDATE_FILE	"/usr/lib/news/groupdates"
/*
 * But if you have C News, you can define ACTIVE_TIMES_FILE instead of
 * STAT_FILE and NGDATE_FILE, and you won't have to run "mkgrdates".
 */

# define ACTIVE_TIMES_FILE	"/usr/lib/news/active.times"

/*
 * Some commonly used programs and files.
 */

#define	ACTIVE_FILE	"/usr/lib/news/active"
#define DISTRIBUTIONS_FILE	"/usr/lib/news/distributions"
#define NEWSGROUPS_FILE	"/usr/lib/news/newsgroups"
#define HISTORY_FILE	"/usr/lib/news/history"
#define	SPOOLDIR	"/usr/spool/news"
#define INEWS		"/usr/lib/news/inews"
#define RNEWS		"/usr/bin/rnews"		/* Link to inews? */

/*
 * Support for C-News style batching 
 */
#undef  NONEWSRUN		/* define this if you are using the daemon */
				/* version of relaynews */
#ifdef NONEWSRUN
#define TOOBIG 1L		
#define TOOMANY 1
#else
#define TOOBIG 300000L		/* batch > TOOBIG bytes, kick rnews */
#define TOOMANY 1024		/* batch > TOOMANY articles, kick rnews */
#define NEWSRUN			"/usr/lib/newsbin/input/newsrun"
#endif
#define TOOOLD (5*60)		/* batch > TOOOLD seconds old, kick rnews */
#define COPYSIZE 8192		/* bytes to copy at one time */
#define MAXDIGITS 25		/* lg(maxlongint) + epsilon */
#define MAXSTR 1024
#define INDIR			"/usr/spool/news/in.coming"
/* You may wish to delete the pathname from the front of BATCH_FILE */
#define BATCH_FILE		"/usr/spool/news/in.coming/nntp.XXXXXX"

/************************************************************************/
/* We don't recommend that you make changes in anything after this line */
/************************************************************************/

#ifdef DBZ		/* If you use DBZ, then you need DBM as well. */
#ifndef DBM
#define DBM
#endif /* DBM */
#endif /* DBZ */

#ifdef BSD_42		/* This is a logical, warranted assumption */
#   ifndef DBM		/* which will probably get me in trouble. */
#	define DBM	/* Kill it if you have 4.2 *and* ndbm.  */
#   endif not DBM
#   ifndef sun		/* not a sun */
#       ifndef ultrix   /* not ultrix */
#           ifndef CMU_MACH /* not CMU's Mach */
#		ifndef NeXT /* not a NeXT */
#                  ifndef READ_SUPER
#	                define READ_SUPER /* read super block for space() */
#		    endif
#               endif
#           endif
#       endif
#   endif
#endif BSD_42

#ifndef USG
#    ifndef BSD_42
#        ifndef CMU_MACH
#            ifndef BSD_43
#                define BSD_43
#            endif
#        endif
#    endif
#endif

#ifdef BSD_43		/* And now more assumptions! */
#   ifndef sun
#	ifndef ultrix	/* Ultrix 4.0 or greater */
#		ifndef READ_SUPER
#			define READ_SUPER
#		endif
#	endif
#   endif
#	ifndef DBZ
#		ifndef DBM
#			ifndef NDBM
#				define NDBM
#			endif
#		endif
#	endif
#endif

/*
 * System V compatability
 */

#ifdef USG
# define	FCNTL			/* If O_etc is defined in <fcntl.h> */
#ifdef dgux
#define		FTRUNCATE
#else
# define	NDIR			/* If you  need ndir library support */
#ifdef hpux
#define		DIRSIZ_MACRO
#endif
#endif
# define	index	strchr
# define	rindex	strrchr
# ifdef U_LONG
   typedef	unsigned long	u_long;
   typedef	unsigned short	u_short;
# endif U_LONG
# define	IPPORT_NNTP	119
#endif USG


/*
 * Some miscellaneous stuff you probably don't want to change.
 */

#define	MAX_ARTICLES	4096		/* Maximum number of articles/group */
#define READINTVL	60 * 10		/* 10 minutes b/n chking active file */


