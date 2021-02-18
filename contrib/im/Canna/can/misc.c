/*
	Copyright
		Kyoto University Research Institute for Mathematical Sciences
		1987, 1988, 1989
	Copyright OMRON TATEISI ELECTRONICS CO. 1987, 1988, 1989
	Copyright ASTEC, Inc. 1987, 1988, 1989


    Permission to use, copy, modify, and distribute this software
    and its documentation for any purpose and without any fee is
    hereby granted, subject to the following restrictions:

    The above copyright notice and this permission notice must appear
    in all versions of this software;

    The name of "Wnn" may not be changed; 

    All documentations of software based on "Wnn" must contain the wording
    "This software is based on the original version of Wnn developed by
    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
    OMRON TATEISI ELECTRONICS CO. and
    ASTEC, Inc.", followed by the above copyright notice;

    The name of KURIMS, OMRON and ASTEC may not be used
    for any purpose related to the marketing or advertising
    of any product based on this software.

    KURIMS, OMRON and ASTEC make no representations about
    the suitability of this software for any purpose.
    It is provided "as is" without express or implied warranty.

    KURIMS, OMRON and ASTEC DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
    SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
    IN NO EVENT SHALL KURIMS, OMRON AND ASTEC BE LIABLE FOR ANY SPECIAL,
    INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
    LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
    OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.

    Author: Hideki Tsuiki	Kyoto University
				tsuiki%kaba.or.jp@uunet.uu.net

	    Hiroshi Kuribayashi	OMRON TATEISI ELECTRONICS CO.
				kuri@frf.omron.co.jp
				uunet!nff.ncl.omron.co.jp!kuri

	    Naouki Nide		Kyoto University
				nide%kaba.or.jp@uunet.uu.net

	    Shozo Takeoka	ASTEC, Inc.
				take%astec.co.jp@uunet.uu.net

	    Takashi Suzuki	Advanced Software Technology & Mechatronics
				Research Institute of KYOTO
				suzuki%astem.or.jp@uunet.uu.net

*/
/* Copyright 1992 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of NEC
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  NEC Corporation makes no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */
/*
 * This sorftware is based on a Wnn's command "uum", that is, this is made
 * by modifying source code of "uum".
 */

#include "define.h"
#include <stdio.h>
#ifdef	BSD
#include <ttyent.h>
#endif	BSD
#include "extern.h"

extern	char	**environ;
char		*malloc();

#ifdef BSD

char *ttyname();
char *rindex();

int
ttyfdslot(fd)
     int fd;
{
  register char	*fullnamp;
  register int	slotnum;
  register char	*ttynamp;
  register struct ttyent	*te ;

  if ((fullnamp = ttyname(fd)) == NULL)
    return(NULL);
  if ((ttynamp = rindex(fullnamp, '/')) == NULL) {
    ttynamp = fullnamp;
  } else {
    ttynamp++;
  }
  for (slotnum = 1; (te = getttyent()) != NULL; slotnum++) {
    if ( strcmp(te->ty_name, ttynamp) == 0 ) {
      endttyent() ;
      return	slotnum ;
    }
  }
  endttyent() ;
  return NULL;
}

#endif	BSD

int
setenv(var, value)
     char *var;
     char *value;
{
  char	**newenv;
  int	i, j;
  int	l = strlen(var);
  
  for (i = 0;
       environ[i] != NULL && (environ[i][l] != '=' || strncmp(var, environ[i], l) != 0);
       i++)
    ;
  if (environ[i] == NULL) {
    if ((newenv = (char **)malloc((sizeof (char *))*(i + 2))) == NULL)
      return(ERROR);
    for (j = 0; j < i; j++) {
      newenv[j] = environ[j];
    }
    newenv[i + 1] = NULL;
    environ = newenv;
  }
  if ((environ[i] = malloc(l + strlen(value) + 2)) == NULL)
    return(ERROR);
  sprintf(environ[i], "%s=%s", var, value);
  return(0);
}

