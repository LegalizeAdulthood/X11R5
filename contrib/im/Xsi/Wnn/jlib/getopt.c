/*
 * $Id: getopt.c,v 1.2 1991/09/16 21:30:20 ohm Exp $
 */
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that all of the following conditions are satisfied:
 *
 * 1) The above copyright notices appear in all copies
 * 2) Both those copyright notices and this permission notice appear
 *    in supporting documentation
 * 3) The name of "Wnn" isn't changed unless substantial modifications
 *    are made, or
 * 3') Following words followed by the above copyright notices appear
 *    in all supporting documentation of software based on "Wnn":
 *
 *   "This software is based on the original version of Wnn developed by
 *    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
 *    OMRON Corporation and ASTEC Inc."
 *
 * 4) The names KURIMS, OMRON and ASTEC not be used in advertising or
 *    publicity pertaining to distribution of the software without
 *    specific, written prior permission
 *
 * KURIMS, OMRON and ASTEC make no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * KURIMS, OMRON, ASTEC AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KURIMS, OMRON, ASTEC OR
 * WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/*	Version 4.0
 */
/*LINTLIBRARY*/

/***********************************************************************
			getopt.c

	BSD用getopt関数。ライブラリファイルはlibgetopt.aです。
	システムに予めgetopt()が用意されていなかったら、これを
	リンクして下さい。そのためには、トップレベルの
	メークファイルのGETOPTLIBに、../jlib/libgetopt.aを
	加えて下さい。

***********************************************************************/

#define	NULL	0
#define	EOF	(-1)
#define	ERR(s, c)							\
	if(opterr){							\
		extern	int	strlen(), write();			\
		char	errbuf[2];					\
									\
		errbuf[0] = c;						\
		errbuf[1] = '\n';					\
		(void)write(2, argv[0], (unsigned)strlen(argv[0]));	\
		(void)write(2, s, (unsigned)strlen(s));			\
		(void)write(2, errbuf, 2);				\
	}

#ifdef	BSD42
#define	strchr	index
#endif

extern	int	strcmp();
extern	char	*strchr();

int	opterr = 1;
int	optind = 1;
int	optopt;
char	*optarg;

int	getopt(argc, argv, opts)
int	argc;
char	**argv, *opts;
{
	static	int	sp = 1;
	register int	c;
	register char	*cp;

	if(sp == 1){
		if(optind >= argc ||
		   argv[optind][0] != '-' || argv[optind][1] == '\0'){
			return(EOF);
		} else if(strcmp(argv[optind], "--") == NULL){
			optind++;
			return(EOF);
		}
	}
	optopt = c = argv[optind][sp];
	if(c == ':' || (cp=strchr(opts, c)) == NULL){
		ERR(": unknown option, -", c);
		if(argv[optind][++sp] == '\0'){
			optind++;
			sp = 1;
		}
		return('?');
	}
	if(*++cp == ':'){
		if(argv[optind][sp+1] != '\0'){
			optarg = &argv[optind++][sp+1];
		} else if(++optind >= argc){
			ERR(": argument missing for -", c);
			sp = 1;
			return('?');
		} else {
			optarg = argv[optind++];
		}
		sp = 1;
	} else {
		if(argv[optind][++sp] == '\0'){
			sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return(c);
}

/*
  Local Variables:
  kanji-flag: t
  End:
*/
