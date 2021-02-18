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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/ulkigo.c,v 3.3 91/09/17 17:53:22 kon Exp $";

#include	<errno.h>
#include "iroha.h"

extern int makeGlineStatus();
extern int uiUtilIchiranTooSmall();

#define	UURD_SZ	66

static
char *russia_data[] = { "§¡", "§¢", "§£", "§¤", "§¥", "§¦", "§§", "§¨",
			"§©", "§ª", "§«", "§¬", "§­", "§®", "§¯", "§°", 
			"§±", "§²", "§³", "§´", "§µ", "§¶", "§·", "§¸",
			"§¹", "§º", "§»", "§¼", "§½", "§¾", "§¿", "§À", 
			"§Á", "§Ñ", "§Ò", "§Ó", "§Ô", "§Õ", "§Ö", "§×",
			"§Ø", "§Ù", "§Ú", "§Û", "§Ü", "§Ý", "§Þ", "§ß",
			"§à", "§á", "§â", "§ã", "§ä", "§å", "§æ", "§ç",
			"§è", "§é", "§ê", "§ë", "§ì", "§í", "§î", "§ï",
			"§ð", "§ñ", 
			 0
		      };

#define	UUGD_SZ	48

static
char *greek_data[] =  { "¦¡", "¦¢", "¦£", "¦¤", "¦¥", "¦¦", "¦§", "¦¨",
                        "¦©", "¦ª", "¦«", "¦¬", "¦­", "¦®", "¦¯", "¦°",
			"¦±", "¦²", "¦³", "¦´", "¦µ", "¦¶", "¦·", "¦¸",
		        "¦Á", "¦Â", "¦Ã", "¦Ä", "¦Å", "¦Æ", "¦Ç", "¦È",
		        "¦É", "¦Ê", "¦Ë", "¦Ì", "¦Í", "¦Î", "¦Ï", "¦Ð",
			"¦Ñ", "¦Ò", "¦Ó", "¦Ô", "¦Õ", "¦Ö", "¦×", "¦Ø",
			 0
		      };

#ifdef pcux_r32
#define UUKD_SZ 76

static
char *keisen_data[] =  { "¬¤", "¬¥", "¬¦", "¬§", "¬¨", "¬©", "¬ª", "¬«",
			 "¬¬", "¬­", "¬®", "¬¯", "¬°", "¬±", "¬²", "¬³",
			 "¬´", "¬µ", "¬¶", "¬·", "¬¸", "¬¹", "¬º", "¬»",
			 "¬¼", "¬½", "¬¾", "¬¿", "¬À", "¬Á", "¬Â", "¬Ã",
			 "¬Ä", "¬Å", "¬Æ", "¬Ç", "¬È", "¬É", "¬Ê", "¬Ë",
			 "¬Ì", "¬Í", "¬Î", "¬Ï", "¬Ð", "¬Ñ", "¬Ò", "¬Ó",
			 "¬Ô", "¬Õ", "¬Ö", "¬×", "¬Ø", "¬Ù", "¬Ú", "¬Û",
			 "¬Ü", "¬Ý", "¬Þ", "¬ß", "¬à", "¬á", "¬â", "¬ã",
			 "¬ä", "¬å", "¬æ", "¬ç", "¬è", "¬é", "¬ê", "¬ë",
			 "¬ì", "¬í", "¬î", "¬ï",
			  0
		       };
#else /* EWS-UX/V */
#define	UUKD_SZ	32

static
char *keisen_data[] =  { "¨¡", "¨¢", "¨£", "¨¤", "¨¥", "¨¦", "¨§", "¨¨",
			 "¨©", "¨ª", "¨«", "¨¬", "¨­", "¨®", "¨¯", "¨°",
			 "¨±", "¨²", "¨³", "¨´", "¨µ", "¨¶", "¨·", "¨¸",
			 "¨¹", "¨º", "¨»", "¨¼", "¨½", "¨¾", "¨¿", "¨À",
			  0
 		       };
#endif

static
uuKigoExitDo(d, retval)
uiContext d;
int retval;
{
  popForIchiranMode(d);
  popCallback(d);
  retval = YomiExit(d, retval);
  currentModeInfo(d);

  return(retval);
}

static
uuKigoRExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  forichiranContext fc;

  popCallback(d); /* °ìÍ÷¤ò pop */

  fc = (forichiranContext)d->modec;
  d->currussia = fc->curIkouho;

  return(uuKigoExitDo(d, retval));
}

static
uuKigoGExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  forichiranContext fc;

  popCallback(d); /* °ìÍ÷¤ò pop */

  fc = (forichiranContext)d->modec;
  d->curgreek = fc->curIkouho;

  return(uuKigoExitDo(d, retval));
}

static
uuKigoKExitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  forichiranContext fc;

  popCallback(d); /* °ìÍ÷¤ò pop */

  fc = (forichiranContext)d->modec;
  d->curkeisen = fc->curIkouho;

  return(uuKigoExitDo(d, retval));
}

static
uuKigoQuitCatch(d, retval, env)
uiContext d;
int retval;
mode_context env;
{
  popCallback(d); /* °ìÍ÷¤ò pop */

  popForIchiranMode(d);
  popCallback(d);
  currentModeInfo(d);

  return(uiUtil2Mode(d, 0)); /* 0 ¤Ïµ­¹æÆþÎÏ */
}

static
uuKigoMake(d, allkouho, size, cur, mode, exitfunc)
uiContext d;
unsigned char **allkouho;
int size;
char cur, mode;
int (*exitfunc)();
{
  forichiranContext fc;
  ichiranContext ic;
  unsigned char inhibit = 0;
  int retval = 0;

  d->status = 0;

  if((retval = getForIchiranContext(d)) == NG) {
    return(GLineNGReturn(d));
  }
  fc = (forichiranContext)d->modec;

  /* selectOne ¤ò¸Æ¤Ö¤¿¤á¤Î½àÈ÷ */
  fc->allkouho = allkouho;
  fc->curIkouho = 0;
  inhibit |= (unsigned char)NUMBERING;

  if((retval = selectOne(d, fc->allkouho, &fc->curIkouho, size,
		 KIGOBANGOMAX, inhibit, 0, WITH_LIST_CALLBACK,
		 0, exitfunc,
		 uuKigoQuitCatch, uiUtilIchiranTooSmall)) == NG) {
    return(GLineNGReturnFI(d));
  }

  ic = (ichiranContext)d->modec;
  ic->minorMode = mode;
  currentModeInfo(d);

  *(ic->curIkouho) = (int)cur;

  /* ¸õÊä°ìÍ÷¹Ô¤¬¶¹¤¯¤Æ¸õÊä°ìÍ÷¤¬½Ð¤»¤Ê¤¤ */
  if(ic->tooSmall) {
    d->status = AUX_CALLBACK;
    return(retval);
  }

  if ( !(ic->flags & ICHIRAN_ALLOW_CALLBACK) ) {
    makeGlineStatus(d);
  }

  /* d->status = ICHIRAN_EVERYTIME; */

  return(retval);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * µ­¹æ°ìÍ÷                                                                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

kigoZenpan(d)
uiContext	d;
{
  if(makeKigoIchiran(d, 0) == NG) /* 0 ¤Ï³ÈÄ¥¤Îµ­¹æ°ìÍ÷ */
    return(GLineNGReturn(d));
  else
    return(0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ¥í¥·¥¢Ê¸»ú¤ÎÆþÎÏ                                                          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

kigoRussia(d)
uiContext d;
{
  return(uuKigoMake(d, (unsigned char **)russia_data, UURD_SZ,
		    d->currussia, IROHA_MODE_RussianMode, uuKigoRExitCatch));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ¥®¥ê¥·¥ãÊ¸»ú¤ÎÆþÎÏ                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

kigoGreek(d)
uiContext d;
{
  return(uuKigoMake(d, (unsigned char **)greek_data, UUGD_SZ,
		    d->curgreek, IROHA_MODE_GreekMode, uuKigoGExitCatch));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * ·ÓÀþ¤ÎÆþÎÏ                                                                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

kigoKeisen(d)
uiContext d;
{
  return(uuKigoMake(d, (unsigned char **)keisen_data, UUKD_SZ,
		    d->curkeisen, IROHA_MODE_LineMode, uuKigoKExitCatch));
}
