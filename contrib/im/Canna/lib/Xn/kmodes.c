/***********************************************************
 *							   *	
 *            COPYRIGHT (C) 1988 NEC CORPORATION	   * 
 *      	     ALL RIGHTS RESERVED		   *
 *						           *	
 ***********************************************************/
/*****************************
 *
 *	8/16 bit String Manipulations.
 *
 *****************************/

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk/ui/RCS/kmodes.c,v 2.2 91/02/14 11:12:34 satoko Exp $";

#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <iroha/kanji.h>

/*
  以下の定義は kanji.h の中にある。

  typedef enum {single_byte_code, double_byte_code} code_len;
  
  typedef struct {
  code_len nbyte;
  int (*func)();
  } fs_mode_rec, *fs_mode;
  */

#define EUC_MASK	(char) 0x80
#define SS2		(char) 0x8e
  
static int pickupEUC8(),	pickupEUC16();
static int pickupSimpleJIS8(),	pickupSimpleJIS16();
static int pickupJIS8(),	pickupJIS16();
static int pickupJIS_EUC_8(),	pickupJIS_EUC_16();
static int pickupEUCg0(),	pickupEUCg1(),		pickupEUCg2();
static int pickupJIS_EUC_g0(),	pickupJIS_EUC_g1(),	pickupJIS_EUC_g2();

fs_mode_rec fs_euc_mode[] = {
  {single_byte_code, pickupEUC8},
  {double_byte_code, pickupEUC16}};
/* EUC set 
   Index == 0 means the string belongs to  ASCII (or JIS-ROMAN) set.
   Index == 1 means the string belongs to 2 Byte Code set.
   */

fs_mode_rec fs_euc3_mode[] = {
  {single_byte_code, pickupEUCg0},
  {double_byte_code, pickupEUCg1},
  {single_byte_code, pickupEUCg2}};
/* EUC set 
   Index == 0 means the string belongs to  ISO8859-1 set.
   Index == 1 means the string belongs to 2 Byte Code set.
   Index == 2 means the string belongs to 1 Byte Katakana Code set.
   */

fs_mode_rec fs_simple_jis_mode[] = {
  {single_byte_code, pickupSimpleJIS8},
  {double_byte_code, pickupSimpleJIS16}};
/* Simple JIS set
   Index == 0 : ASCII (or JIS-ROMAN) and KANA (JIS-8bit) set.
   (^[(B, ^[(J, ^[(H and ^[(I)
   Index == 1 : KANJI Code set. (^[$B, ^[$@)
   */

fs_mode_rec fs_jis_mode[] = {
  {single_byte_code, pickupJIS8},   /* ^[(B */
  {single_byte_code, pickupJIS8},   /* ^[(A */
  {single_byte_code, pickupJIS8},   /* ^[(J */
  {single_byte_code, pickupJIS8},   /* ^[(I */
  {single_byte_code, pickupJIS8},   /* ^[(H */
  {double_byte_code, pickupJIS16},  /* ^[$B */
  {double_byte_code, pickupJIS16}   /* ^[$@ */
};
/* JIS set
   Index == 0 : ASCII set.
   Index == 1 : UK set.
   Index == 2 : JIS-ROMAN set.
   Index == 3 : JIS-KANA set.
   Index == 4 : obsolete JIS-ROMAN set. 
   Index == 5 : JIS-KANJI set.
   index == 6 : obsolete JIS-KANJI set.
   */

fs_mode_rec fs_jis_euc_mode[] = {
  {single_byte_code, pickupJIS_EUC_8},
  {double_byte_code, pickupJIS_EUC_16}};
/* JIS and EUC set.
   Index == 0 : ASCII (or JIS-ROMAN) and KANA (JIS-8bit) set.
   Index == 1 : KANJI Code set.  (Allow both JIS & EUC)
   */

fs_mode_rec fs_jis_euc3_mode[] = {
  {single_byte_code, pickupJIS_EUC_g0},
  {double_byte_code, pickupJIS_EUC_g1},
  {single_byte_code, pickupJIS_EUC_g2}};
/* JIS and EUC set.
   Index == 0 : ISO8859-1 set.
   Index == 1 : KANJI Code set.  (Allow both JIS & EUC)
   Index == 2 : KATAKANA Code set. (JIS-x0201 7bit katakana)
   */

static int pickupEUC8(from, to, len, next_return, remain_return,
		      nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  int i = 0, ret = 0;
  int odd = 0;
  
  while (i < len) {
    if (!(*from & EUC_MASK)) {
      *to++ = *from++; i++; ret++;
    }
    else if (*from == SS2) {
      i++; from++;
      if (i < len) {
	*to++ = *from++; i++; ret++;
      }
      else {
	odd = 1;
      }
    }
    else {
      *next_return = from;
      *remain_return = len - i;
      *nextIndex_return = 1;
      return ret;
    }
  }
  *next_return = from;
  *remain_return = len - i;
  *nextIndex_return = 1;
  return ret;
}

static int pickupEUC16(from, to, len, next_return, remain_return,
		       nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  int i = 0, ret = 0;
  
  while (i < len) {
    if (*from == SS2) {
      *next_return = from;
      *remain_return = len - i;
      *nextIndex_return = 0;
      return ret;
    }
    else if (*from & EUC_MASK) {
      *to++ = *from++ & ~EUC_MASK; i++;
      if (i < len) {
	*to++ = *from++ & ~EUC_MASK; i++; ret++;
      }
      else {
	to--;
      }
    }
    else {
      *next_return = from;
      *remain_return = len - i;
      *nextIndex_return = 0;
      return ret;
    }
  }
  *next_return = from;
  *remain_return = len - i;
  *nextIndex_return = 0;
  return ret;
}


static int pickupSimpleJIS8(from, to, len, next_return, remain_return, 
			    nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  int i = 0, ret = 0;
  
  while (i < len) {
    if (*from != '\033') {
      *to++ = *from++; i++; ret++;
    }
    else { /* *from == '\033' */
      i++; from++;			/* skip ESC */
      if ((i < len) && (*from == '$')) {
	i++; from++;		/* skip '$' */
	if ((i < len) &&
	    (*from == 'B' || *from == '@')) {
	  i++; from++;	/* skip 'B' or '@' */
	  *next_return = from;
	  *remain_return = len - i;
	  *nextIndex_return = 1;
	  return ret;
	}
	if (i < len) {
	  *to++ = '\033';  ret++;
	  *to++ = '$';     ret++;
	  *to++ = *from;	 ret++;
	  i++; from++;	/* skip the CHAR */
	}
	else {
	  *to++ = '\033'; ret++;
	  *to++ = '$';	ret++;
	}
      }
      else if ((i < len) && (*from == '(')) {
	i++; from++;		/* skip '(' */
	if ((i < len) && (*from == 'B' ||
			  *from == 'J' ||
			  *from == 'I' ||
			  *from == 'H')) {
	  i++; from++;	/* skip 'B' etc. */
	}
	else if (i < len) {
	  *to++ = '\033';  ret++;
	  *to++ = '(';     ret++;
	  *to++ = *from;	 ret++;
	  i++; from++;	/* skip the CHAR */
	}
	else {
	  *to++ = '\033';  ret++;
	  *to++ = '(';     ret++;
	}
      }
      else if (i < len) {
	*to++ = '\033';	ret++;
	*to++ = *from;	ret++;
	i++; from++;		/* skip the CHAR */
      }
      else {
	*to++ = '\033';	ret++;
      }
    }
  }
  *next_return = from;
  *remain_return = len - i;
  *nextIndex_return = 1;
  return ret;
}

static int pickupSimpleJIS16(from, to, len, next_return, remain_return,
			     nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  int i = 0, ret = 0;
  
  while (i < len) {
    if (*from != '\033') {
      if ((i + 1) < len) {
	*to++ = *from++; i++;
	*to++ = *from++; i++; ret++;
      }
      else {
	*next_return = from;
	*remain_return = len - i;
	*nextIndex_return = 0;
	return ret;
      }
    }
    else { /* *from == '\033' */
      i++; from++;			/* skip ESC */
      if ((i < len) && (*from == '$')) {
	i++; from++;		/* skip '$' */
	if ((i < len) &&
	    (*from == 'B' || *from == '@')) {
	  i++; from++;	/* skip 'B' or '@' */
	  /* no problem */
	}
	else {
	  i -= 2; from -= 2;
	  /* back to point ESC */
	  *next_return = from;
	  *remain_return = len - i;
	  *nextIndex_return = 0;
	  return ret;
	}
      }
      else {
	i--; from--;	/* back to point ESC */
	*next_return = from;
	*remain_return = len - i;
	*nextIndex_return = 0;
	return ret;
      }
    }
  }
  *next_return = from;
  *remain_return = len - i;
  *nextIndex_return = 0;
  return ret;
}

static int pickupJIS_EUC_8(from, to, len, next_return, remain_return,
			   nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  int i = 0, ret = 0;
  
  while (i < len) {
    if (!(*from & EUC_MASK)) {
      if (*from != '\033') {
	*to++ = *from++; i++; ret++;
      }
      else { /* *from == '\033' */
	i++; from++;			/* skip ESC */
	if ((i < len) && (*from == '$')) {
	  i++; from++;		/* skip '$' */
	  if ((i < len) &&
	      (*from == 'B' || *from == '@')) {
	    i++; from++;	/* skip 'B'etc*/
	    *next_return = from;
	    *remain_return = len-i;
	    *nextIndex_return = 1;
	    return ret;
	  }
	  if (i < len) {
	    *to++ = '\033';  ret++;
	    *to++ = '$';     ret++;
	    *to++ = *from;	 ret++;
	    i++; from++;	/* skip CHAR */
	  }
	  else {
	    *to++ = '\033'; ret++;
	    *to++ = '$';	ret++;
	  }
	}
	else if ((i < len) && (*from == '(')) {
	  i++; from++;		/* skip '(' */
	  if ((i < len) && (*from == 'B' ||
			    *from == 'J' ||
			    *from == 'I' ||
			    *from == 'H')) {
	    i++; from++;	/* skip 'B'etc*/
	  }
	  else if (i < len) {
	    *to++ = '\033';  ret++;
	    *to++ = '(';     ret++;
	    *to++ = *from;	 ret++;
	    i++; from++;	/* skip CHAR */
	  }
	  else {
	    *to++ = '\033';  ret++;
	    *to++ = '(';     ret++;
	  }
	}
	else if (i < len) {
	  *to++ = '\033';	ret++;
	  *to++ = *from;	ret++;
	  i++; from++;		/* skip CHAR */
	}
	else {
	  *to++ = '\033';	ret++;
	}
      }
    }
    else if (*from == SS2) {
      i++; from++;
      if (i < len) {
	*to++ = *from++; i++; ret++;
      }
    }
    else {
      *next_return = from;
      *remain_return = len - i;
      *nextIndex_return = 1;
      return ret;
    }
  }
  *next_return = from;
  *remain_return = len - i;
  *nextIndex_return = 1;
  return ret;
}

static int pickupJIS_EUC_16(from, to, len, next_return, remain_return,
			    nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  if (len > 0) {
    if (*from & EUC_MASK) {
      return pickupEUC16(from, to, len,
			 next_return, remain_return,
			 nextIndex_return);
    }
    else {
      return pickupSimpleJIS16(from, to, len,
			       next_return, remain_return,
			       nextIndex_return);
    }
  }
  return 0;
}

static int pickupJIS8(from, to, len, next_return, remain_return,
		      nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  int i = 0, ret = 0;
  
  while (i < len) {
    if (*from != '\033') {
      *to++ = *from++; i++; ret++;
    }
    else { /* *from == '\033' */
      i++; from++;			/* skip ESC */
      if ((i < len) && (*from == '$')) {
	i++; from++;		/* skip '$' */
	if ((i < len) && *from == 'B') {
	  i++; from++;	/* skip 'B' */
	  *next_return = from;
	  *remain_return = len - i;
	  *nextIndex_return = 5;
	  return ret;
	}
	if ((i < len) && *from == '@') {
	  i++; from++;	/* skip '@' */
	  *next_return = from;
	  *remain_return = len - i;
	  *nextIndex_return = 6;
	  return ret;
	}
	if (i < len) {
	  *to++ = '\033';  ret++;
	  *to++ = '$';     ret++;
	  *to++ = *from;	 ret++;
	  i++; from++;	/* skip the CHAR */
	}
	else {
	  *to++ = '\033'; ret++;
	  *to++ = '$';	ret++;
	}
      }
      else if ((i < len) && (*from == '(')) {
	i++; from++;		/* skip '(' */
	if ((i < len) && *from == 'B') {
	  i++; from++;	/* skip 'B' */
	  *next_return = from;
	  *remain_return = len - i;
	  *nextIndex_return = 0;
	  return ret;
	}
	if ((i < len) && *from == 'A') {
	  i++; from++;	/* skip 'A' */
	  *next_return = from;
	  *remain_return = len - i;
	  *nextIndex_return = 1;
	  return ret;
	}
	if ((i < len) && *from == 'J') {
	  i++; from++;	/* skip 'J' */
	  *next_return = from;
	  *remain_return = len - i;
	  *nextIndex_return = 2;
	  return ret;
	}
	if ((i < len) && *from == 'I') {
	  i++; from++;	/* skip 'I' */
	  *next_return = from;
	  *remain_return = len - i;
	  *nextIndex_return = 3;
	  return ret;
	}
	if ((i < len) && *from == 'H') {
	  i++; from++;	/* skip 'H' */
	  *next_return = from;
	  *remain_return = len - i;
	  *nextIndex_return = 4;
	  return ret;
	}
	if (i < len) {
	  *to++ = '\033';  ret++;
	  *to++ = '(';     ret++;
	  *to++ = *from;	 ret++;
	  i++; from++;	/* skip the CHAR */
	}
	else {
	  *to++ = '\033';  ret++;
	  *to++ = '(';     ret++;
	}
      }
      else if (i < len) {
	*to++ = '\033';	ret++;
	*to++ = *from;	ret++;
	i++; from++;		/* skip the CHAR */
      }
      else {
	*to++ = '\033';	ret++;
      }
    }
  }
  *next_return = from;
  *remain_return = len - i;
  *nextIndex_return = 0;
  return ret;
}

static int pickupJIS16(from, to, len, next_return, remain_return,
		       nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  int i = 0, ret = 0;
  
  while (i < len) {
    if (*from != '\033') {
      if ((i + 1) < len) {
	*to++ = *from++; i++;
	*to++ = *from++; i++; ret++;
      }
      else {
	*next_return = from;
	*remain_return = len - i;
	*nextIndex_return = 0;
	return ret;
      }
    }
    else { /* *from == '\033' */
      i++; from++;			/* skip ESC */
      if ((i < len) && (*from == '$')) {
	i++; from++;		/* skip '$' */
	if ((i < len) && *from == 'B') {
	  i++; from++;	/* skip 'B' */
	  *next_return = from;
	  *remain_return = len - i;
	  return ret;
	}
	if ((i < len) && *from == '@') {
	  i++; from++;	/* skip '@' */
	  *next_return = from;
	  *remain_return = len - i;
	  *nextIndex_return = 6;
	  return ret;
	}
	i -= 2; from -= 2;
	/* back to point ESC */
	*next_return = from;
	*remain_return = len - i;
	*nextIndex_return = 0;
	return ret;
      }
      else {
	i--; from--;	/* back to point ESC */
	*next_return = from;
	*remain_return = len - i;
	*nextIndex_return = 0;
	return ret;
      }
    }
  }
  *next_return = from;
  *remain_return = len - i;
  *nextIndex_return = 0;
  return ret;
}


static int pickupEUCg0(from, to, len, next_return, remain_return,
		       nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  int i = 0, ret = 0;
  
  while (i < len) {
    if (!(*from & EUC_MASK)) {
      *to++ = *from++; i++; ret++;
    }
    else {
      *next_return = from;
      *remain_return = len - i;
      *nextIndex_return = (*from == SS2) ? 2 : 1;
      return ret;
    }
  }
  *next_return = from;
  *remain_return = len - i;
  *nextIndex_return = 0;
  return ret;
}

static int pickupEUCg1(from, to, len, next_return, remain_return,
		       nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  int i = 0, ret = 0;
  
  while (i < len) {
    if (*from == SS2) {
      *next_return = from;
      *remain_return = len - i;
      *nextIndex_return = 2;
      return ret;
    }
    else if (*from & EUC_MASK) {
      *to++ = *from++ & ~EUC_MASK; i++;
      *to++ = *from++ & ~EUC_MASK; i++; ret++;
    }
    else {
      *next_return = from;
      *remain_return = len - i;
      *nextIndex_return = 0;
      return ret;
    }
  }
  *next_return = from;
  *remain_return = len - i;
  *nextIndex_return = 0;
  return ret;
}

static int pickupEUCg2(from, to, len, next_return, remain_return,
		       nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  int i = 0, ret = 0;
  
  while (i < len) {
    if (!(*from & EUC_MASK)) {
      *next_return = from;
      *remain_return = len - i;
      *nextIndex_return = 0;
      return ret;
    }
    else if (*from == SS2) {
      i++; from++;
      if (i < len) {
	*to++ = *from++ & ~EUC_MASK; i++; ret++;
      }
    }
    else {
      *next_return = from;
      *remain_return = len - i;
      *nextIndex_return = 1;
      return ret;
    }
  }
  *next_return = from;
  *remain_return = len - i;
  *nextIndex_return = 0;
  return ret;
}

static int pickupJISi(from, to, len, next_return, remain_return, 
		      nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  int i = 0, ret = 0;
  
  while (i < len) {
    if (*from != '\033') {
      *to++ = *from++; i++; ret++;
    }
    else { /* *from == '\033' */
      i++; from++;			/* skip ESC */
      if ((i < len) && (*from == '$')) {
	i++; from++;		/* skip '$' */
	if ((i < len) &&
	    (*from == 'B' || *from == '@')) {
	  i++; from++;	/* skip 'B' or '@' */
	  *next_return = from;
	  *remain_return = len - i;
	  *nextIndex_return = 1;
	  return ret;
	}
	if (i < len) {
	  *to++ = '\033';  ret++;
	  *to++ = '$';     ret++;
	  *to++ = *from;	 ret++;
	  i++; from++;	/* skip the CHAR */
	}
	else {
	  *to++ = '\033'; ret++;
	  *to++ = '$';	ret++;
	}
      }
      else if ((i < len) && (*from == '(')) {
	i++; from++;		/* skip '(' */
	if ((i < len) && (*from == 'B' ||
			  *from == 'J' ||
			  *from == 'H')) {
	  i++; from++;	/* skip 'B' etc. */
	}
	else if (i < len && *from == 'I') {
	  i++; from++; /* skip 'I' */
	  *next_return = from;
	  *remain_return = len - i;
	  *nextIndex_return = 2;
	  return ret;
	}
	else if (i < len) {
	  *to++ = '\033';  ret++;
	  *to++ = '(';     ret++;
	  *to++ = *from;	 ret++;
	  i++; from++;	/* skip the CHAR */
	}
	else {
	  *to++ = '\033';  ret++;
	  *to++ = '(';     ret++;
	}
      }
      else if (i < len) {
	*to++ = '\033';	ret++;
	*to++ = *from;	ret++;
	i++; from++;		/* skip the CHAR */
      }
      else {
	*to++ = '\033';	ret++;
      }
    }
  }
  *next_return = from;
  *remain_return = len - i;
  *nextIndex_return = 1;
  return ret;
}

static int pickupJISk(from, to, len, next_return, remain_return,
		      nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  int i = 0, ret = 0;
  
  while (i < len) {
    if (*from != '\033') {
      if ((i + 1) < len) {
	*to++ = *from++; i++;
	*to++ = *from++; i++; ret++;
      }
      else {
	*next_return = from;
	*remain_return = len - i;
	*nextIndex_return = 0;
	return ret;
      }
    }
    else { /* *from == '\033' */
      i++; from++;			/* skip ESC */
      if ((i < len) && (*from == '$')) {
	i++; from++;		/* skip '$' */
	if ((i < len) &&
	    (*from == 'B' || *from == '@')) {
	  i++; from++;	/* skip 'B' or '@' */
	  /* no problem */
	}
	else {
	  i -= 2; from -= 2;
	  /* back to point ESC */
	  *next_return = from;
	  *remain_return = len - i;
	  *nextIndex_return = 0;
	  return ret;
	}
      }
      else {
	i--; from--;	/* back to point ESC */
	*next_return = from;
	*remain_return = len - i;
	*nextIndex_return = 0;
	return ret;
      }
    }
  }
  *next_return = from;
  *remain_return = len - i;
  *nextIndex_return = 0;
  return ret;
}

static int pickupJISkana(from, to, len, next_return, remain_return,
			 nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  int i = 0, ret = 0;
  
  while (i < len) {
    if (*from != '\033') {
      if (i < len) {
	*to++ = *from++; i++; ret++;
      }
      else {
	*next_return = from;
	*remain_return = len - i;
	*nextIndex_return = 0;
	return ret;
      }
    }
    else { /* *from == '\033' */
      i++; from++;			/* skip ESC */
      if ((i < len) && (*from == '(')) {
	i++; from++;		/* skip '(' */
	if ((i < len) &&
	    (*from == 'I')) {
	  i++; from++;	/* skip 'B' or '@' */
	  /* no problem */
	}
	else {
	  i -= 2; from -= 2;
	  /* back to point ESC */
	  *next_return = from;
	  *remain_return = len - i;
	  *nextIndex_return = 0;
	  return ret;
	}
      }
      else {
	i--; from--;	/* back to point ESC */
	*next_return = from;
	*remain_return = len - i;
	*nextIndex_return = 0;
	return ret;
      }
    }
  }
  *next_return = from;
  *remain_return = len - i;
  *nextIndex_return = 0;
  return ret;
}

static int pickupJIS_EUC_g0(from, to, len, next_return, remain_return,
			    nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  int i = 0, ret = 0;
  
  while (i < len) {
    if (!(*from & EUC_MASK)) {
      if (*from != '\033') {
	*to++ = *from++; i++; ret++;
      }
      else { /* *from == '\033' */
	i++; from++;			/* skip ESC */
	if ((i < len) && (*from == '$')) {
	  i++; from++;		/* skip '$' */
	  if ((i < len) &&
	      (*from == 'B' || *from == '@')) {
	    i++; from++;	/* skip 'B'etc*/
	    *next_return = from;
	    *remain_return = len-i;
	    *nextIndex_return = 1;
	    return ret;
	  }
	  if (i < len) {
	    *to++ = '\033';  ret++;
	    *to++ = '$';     ret++;
	    *to++ = *from;	 ret++;
	    i++; from++;	/* skip CHAR */
	  }
	  else {
	    *to++ = '\033'; ret++;
	    *to++ = '$';	ret++;
	  }
	}
	else if ((i < len) && (*from == '(')) {
	  i++; from++;		/* skip '(' */
	  if ((i < len) && (*from == 'B' ||
			    *from == 'J' ||
			    *from == 'H')) {
	    i++; from++;	/* skip 'B'etc*/
	  }
	  else if (i < len && *from == 'I') {
	    i++; from++; /* skip 'I' */
	    *next_return = from;
	    *remain_return = len - i;
	    *nextIndex_return = 2;
	    return ret;
	  }
	  else if (i < len) {
	    *to++ = '\033';  ret++;
	    *to++ = '(';     ret++;
	    *to++ = *from;	 ret++;
	    i++; from++;	/* skip CHAR */
	  }
	  else {
	    *to++ = '\033';  ret++;
	    *to++ = '(';     ret++;
	  }
	}
	else if (i < len) {
	  *to++ = '\033';	ret++;
	  *to++ = *from;	ret++;
	  i++; from++;		/* skip CHAR */
	}
	else {
	  *to++ = '\033';	ret++;
	}
      }
    }
    else if (*from == SS2) {
      *next_return = from;
      *remain_return = len - i;
      *nextIndex_return = 2;
      return ret;
    }
    else {
      *next_return = from;
      *remain_return = len - i;
      *nextIndex_return = 1;
      return ret;
    }
  }
  *next_return = from;
  *remain_return = len - i;
  *nextIndex_return = 1;
  return ret;
}

static int pickupJIS_EUC_g1(from, to, len, next_return, remain_return,
			    nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  if (len > 0) {
    if (*from & EUC_MASK) {
      return pickupEUCg1(from, to, len,
			 next_return, remain_return,
			 nextIndex_return);
    }
    else {
      return pickupJISk(from, to, len,
			next_return, remain_return,
			nextIndex_return);
    }
  }
  return 0;
}

static int pickupJIS_EUC_g2(from, to, len, next_return, remain_return,
			    nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  if (len > 0) {
    if (*from & EUC_MASK) {
      return pickupEUCg2(from, to, len,
			 next_return, remain_return,
			 nextIndex_return);
    }
    else {
      return pickupJISkana(from, to, len,
			   next_return, remain_return,
			   nextIndex_return);
    }
  }
  return 0;
}

/*
 * 以下はＮＥＣ標準外字フォントの出力のためのルーチンである。標準外字
 * は９区から１５区までである。ただし、このうち、９〜１１区は半角文字、
 * １４〜１５区は１／４角文字である。 
 *
 * 以下の関数では、それらの領域(0x2900〜0x2fff)に対してはＮＥＣ標準外
 * 字フォントを用いて表示しましょうねということをしている。
 */

static int pickupNONEC16(from, to, len, next_return, remain_return,
		       nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  int i = 0, ret = 0;
  
  while (i < len) {
    if (*from == SS2) {
      *next_return = from;
      *remain_return = len - i;
      *nextIndex_return = 0;
      return ret;
    }
    else if (*from & EUC_MASK) {
      if (0x28 < (*from & ~EUC_MASK) && (*from & ~EUC_MASK) < 0x30) {
	*next_return = from;
	*remain_return = len - i;
	*nextIndex_return = 2;
	return ret;
      }
      else {
	*to++ = *from++ & ~EUC_MASK; i++;
	*to++ = *from++ & ~EUC_MASK; i++; ret++;
      }
    }
    else {
      *next_return = from;
      *remain_return = len - i;
      *nextIndex_return = 0;
      return ret;
    }
  }
  *next_return = from;
  *remain_return = len - i;
  *nextIndex_return = 0;
  return ret;
}

static int pickupNEC16(from, to, len, next_return, remain_return,
		       nextIndex_return)
     char *from, *to, **next_return;
     int len, *remain_return, *nextIndex_return;
{
  int i = 0, ret = 0;
  
  while (i < len) {
    if (*from == SS2) {
      *next_return = from;
      *remain_return = len - i;
      *nextIndex_return = 0;
      return ret;
    }
    else if (*from & EUC_MASK) {
      if (0x28 < (*from & ~EUC_MASK) && (*from & ~EUC_MASK) < 0x30) {
	*to++ = *from++ & ~EUC_MASK; i++;
	*to++ = *from++ & ~EUC_MASK; i++; ret++;
      }
      else {
	*next_return = from;
	*remain_return = len - i;
	*nextIndex_return = 1;
	return ret;
      }
    }
    else {
      *next_return = from;
      *remain_return = len - i;
      *nextIndex_return = 0;
      return ret;
    }
  }
  *next_return = from;
  *remain_return = len - i;
  *nextIndex_return = 0;
  return ret;
}

fs_mode_rec fs_nec_euc_mode[] = {
  {single_byte_code, pickupEUC8},
  {double_byte_code, pickupNONEC16},
  {double_byte_code, pickupNEC16}
};
/* EUC set 
   Index == 0 means the string belongs to  ASCII (or JIS-ROMAN) set.
   Index == 1 means the string belongs to 2 Byte Code set. (標準JIS)
   Index == 2 は、ＮＥＣ標準外字フォントを表す。
   */
