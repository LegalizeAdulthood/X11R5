/*
  IROHA interface for NEmacs

  $Header: /proj/gnu/nemacs/src/RCS/iroha.c,v 1.8 90/08/16 10:49:27 kon Exp Locker: kon $

  Functions defined in this file are

  (iroha-key-to-string key)
 		key: single STRING
 		RETURNS: a list of translated strings with intermediate
 			 translation information seen in Japanese
			 kana-to-kanji translation.
 		DESCRIPTION:
			 translate a key input to a set of strings.  The
			 strings contain both well-formed string and a
			 intermediate result to show the translation
			 information to a user.
 */

#include <stdio.h>
#include <iroha/jrkanji.h>
extern char *jrKanjiError;

#include "config.h"
#undef NULL
#include "lisp.h"
#include "buffer.h"

#define KEYTOSTRSIZE 2048
static char buf[KEYTOSTRSIZE];
static char **warning;

/* Lisp functions definition */

DEFUN ("iroha-key-to-string", Firoha_key_to_string, Siroha_key_to_string, 
       1, 1, 0,
"translate a key input to a set of strings.  The strings contain both\n\
well-formed string and intermediate result to show the translation\n\
information to a user.")
	(ch)
Lisp_Object ch;
{
  jrKanjiStatus ks;
  Lisp_Object makeReturnStruct();
  int len;

  CHECK_NUMBER (ch, 0);
  /* 取り敢えず jrKanjiString を呼びます */
  len = jrKanjiString(0, XINT (ch), buf, KEYTOSTRSIZE, &ks);
  return makeReturnStruct(buf, len, &ks);
}

static
Lisp_Object
makeReturnStruct(buf, len, ks)
char *buf;
int len;
jrKanjiStatus *ks;
{
  Lisp_Object val = Qnil;

  if (len < 0) { /* エラーが検出された場合 */
    val = Fcons(make_string(jrKanjiError, strlen(jrKanjiError)), Qnil);
  }
  else {
    /* なによりも読みの情報... */
    if (!(ks->info & KanjiYomiInfo)) {
      val = Fcons(Qnil, val);
    }
    else {
      Lisp_Object tmp;
      char *p = buf + len + 1;
      int yomilen = strlen(p);

      if (len + yomilen + 1 < KEYTOSTRSIZE) {
	int yomilen2;

	tmp = make_string(p, yomilen); /* 読み */
	p += yomilen + 1;
	yomilen2 = strlen(p);
	if (len + yomilen + yomilen2 + 2 < KEYTOSTRSIZE) {
	  tmp = Fcons(tmp, make_string(p, yomilen2)); /* ローマ字 */
	}
	else {
	  tmp = Fcons(tmp, Qnil);
	}
	val = Fcons(tmp, val);
      }
      else {
	val = Fcons(Qnil, val);
      }
    }

    /* 一覧の情報 */
    if (!(ks->info & KanjiGLineInfo) || (ks->gline.length < 0)) {
      val = Fcons(Qnil, val);
    }
    else {
      Lisp_Object cand;

      cand = Qnil;
      cand = Fcons(make_number(ks->gline.length), cand);
      cand = Fcons(make_number(ks->gline.revLen), cand);
      cand = Fcons(make_number(ks->gline.revPos), cand);
      cand = Fcons(make_string(ks->gline.line, ks->gline.length), cand);
      val = Fcons(cand, val);
    }
    
    if (ks->info & KanjiModeInfo) { /* モードの情報 */
      val = Fcons (make_string(ks->mode, strlen(ks->mode)), val);
    }
    else { /* モードの情報がなければ nil を入れておきます */
      /* モードの情報が空文字列のときとは違うので注意！ */
      val = Fcons (Qnil, val);
    }

    if (ks->length < 0) { /* 候補表示の文字列です。*/
      val  = Fcons(Qnil, val);
    }
    else {
      Lisp_Object cand;

      cand = Qnil;
      cand = Fcons(make_number(ks->length), cand);
      cand = Fcons(make_number(ks->revLen), cand);
      cand = Fcons(make_number(ks->revPos), cand);
      cand = Fcons(make_string(ks->echoStr, ks->length), cand);
      val  = Fcons(cand, val);
    }

    val = Fcons (make_string(buf, len), val);
  }

  return val;
}


#define VEC_KAKUTEI 0
#define VEC_ECHOSTR 1
#define VEC_EREVPOS 2
#define VEC_EREVLEN 3
#define VEC_ECHOLEN 4
#define VEC_MODESTR 5
#define VEC_GLINSTR 6
#define VEC_GREVPOS 7
#define VEC_GREVLEN 8
#define VEC_GLINLEN 9
#define VEC_YOMIINF 10
#define VEC_ROMEINF 11

static
Lisp_Object
storeKanjiStatus(vec, buf, len, ks)
Lisp_Object vec;
char *buf;
int len;
jrKanjiStatus *ks;
{
  Lisp_Object val = Qnil;

  CHECK_VECTOR(vec, 0);
  if (XVECTOR(vec)->size < 12)
    return(Qnil);

  if (len < 0) { /* エラーが検出された場合 */
    XVECTOR(vec)->contents[VEC_KAKUTEI] = Qnil;
    XVECTOR(vec)->contents[VEC_ECHOSTR] = 
      make_string(jrKanjiError, strlen(jrKanjiError));
  }
  else {
    /* まず確定した時の読みの情報... */
    XVECTOR(vec)->contents[VEC_YOMIINF] = Qnil;
    XVECTOR(vec)->contents[VEC_ROMEINF] = Qnil;
    if (ks->info & KanjiYomiInfo) {
      Lisp_Object tmp;
      char *p = buf + len + 1;
      int yomilen = strlen(p);

      if (len + yomilen + 1 < KEYTOSTRSIZE) {
	int yomilen2;

	XVECTOR(vec)->contents[VEC_YOMIINF] =
	  make_string(p, yomilen); /* 読み */
	p += yomilen + 1;
	yomilen2 = strlen(p);
	if (len + yomilen + yomilen2 + 2 < KEYTOSTRSIZE) {
	  XVECTOR(vec)->contents[VEC_ROMEINF] =
	    make_string(p, yomilen2); /* ローマ字 */
	}
      }
    }

    /* 一覧の情報 */
    if ((ks->info & KanjiGLineInfo) && !(ks->gline.length < 0)) {
      XVECTOR(vec)->contents[VEC_GLINLEN] = make_number(ks->gline.length);
      XVECTOR(vec)->contents[VEC_GREVLEN] = make_number(ks->gline.revLen);
      XVECTOR(vec)->contents[VEC_GREVPOS] = make_number(ks->gline.revPos);
      XVECTOR(vec)->contents[VEC_GLINSTR] = 
	make_string(ks->gline.line, ks->gline.length);
    }
    else {
      XVECTOR(vec)->contents[VEC_GLINSTR] = Qnil;
    }

    if (ks->info & KanjiModeInfo) { /* モードの情報 */
      XVECTOR(vec)->contents[VEC_MODESTR] =
	make_string(ks->mode, strlen(ks->mode));
    }
    else { /* モードの情報がなければ nil を入れておきます */
      /* モードの情報が空文字列のときとは違うので注意！ */
      XVECTOR(vec)->contents[VEC_MODESTR] = Qnil;
    }

    if (ks->length < 0) { /* 候補表示の文字列です。*/
      XVECTOR(vec)->contents[VEC_ECHOSTR] = Qnil;
    }
    else {
      XVECTOR(vec)->contents[VEC_ECHOLEN] = make_number(ks->length);
      XVECTOR(vec)->contents[VEC_EREVLEN] = make_number(ks->revLen);
      XVECTOR(vec)->contents[VEC_EREVPOS] = make_number(ks->revPos);
      XVECTOR(vec)->contents[VEC_ECHOSTR] = 
	make_string(ks->echoStr, ks->length);
    }

    XVECTOR(vec)->contents[VEC_KAKUTEI] = make_string(buf, len);
  }

  return vec;
}

DEFUN ("iroha-key-proc", Firoha_key_proc, Siroha_key_proc, 
       2, 2, 0,
"translate a key input to a set of strings.  The strings contain both\n\
well-formed string and intermediate result to show the translation\n\
information to a user.\n\
This function is similar to iroha-key-to-string except this function\n\
does not consume cons cells, but storing the result into vector which\n\
is given as a first argument.")
	(vec, ch)
Lisp_Object vec, ch;
{
  jrKanjiStatus ks;
  Lisp_Object storeKanjiStatus();
  int len;

  CHECK_VECTOR (vec, 0);
  CHECK_NUMBER (ch, 0);
  /* 取り敢えず jrKanjiString を呼びます */
  len = jrKanjiString(0, XINT (ch), buf, KEYTOSTRSIZE, &ks);
  return storeKanjiStatus(vec, buf, len, &ks);
}


DEFUN ("iroha-initialize", Firoha_initialize, Siroha_initialize, 0, 3, 0,
"Initialize ``iroha'', which is a kana-to-kanji converter for GNU Emacs.\n\
The first arg specifies if inserting space character between BUNSETSU when\n\
candidates are displayed.\n\
The second arg specifies server.\n\
The third arg specifies a file which will be used as a customization\n\
description.\n\
If nil is specified for each arg, the default value will be used.")
(num, server, rcfile)
Lisp_Object num, server;
{
  Lisp_Object val;
  int res;
  char **p;

  int kugiri; /* 文節区切りをするか？ */

  if (NULL(num)) {
    kugiri = 1;
  }
  else {
    CHECK_NUMBER(num, 0);
    kugiri = XINT(num);
    kugiri = (kugiri == 1) ? 1 : 0;
  }

  if (NULL(server)) {
    jrKanjiControl(0, KC_SETSERVERNAME, 0);
  }
  else {
    char servername[256];

    CHECK_STRING(server, 0);
    strncpy(servername, XSTRING(server)->data, XSTRING(server)->size);
    servername[XSTRING(server)->size] = '\0';
    jrKanjiControl(0, KC_SETSERVERNAME, servername);
  }

#ifdef KC_SETINITFILENAME
  if (NULL(rcfile)) {
    jrKanjiControl(0, KC_SETINITFILENAME, (char *)0);
  }
  else {
    char rcname[256];

    CHECK_STRING(rcfile, 0);
    strncpy(rcname, XSTRING(rcfile)->data, XSTRING(rcfile)->size);
    rcname[XSTRING(rcfile)->size] = '\0';
    jrKanjiControl(0, KC_SETINITFILENAME, rcname);
  }
#endif /* KC_SETINITFILENAME */

  warning = 0;
  res = jrKanjiControl(0, KC_INITIALIZE, &warning);

  val = Qnil;
  if (warning) {
    for (p = warning ; *p ; p++) {
      val = Fcons(make_string(*p, strlen(*p)), val);
    }
  }
  val = Fcons(val, Qnil);

  if (res == -1) {
    val = Fcons(make_string(jrKanjiError, strlen(jrKanjiError)), val);
    /* イニシャライズで失敗した場合。 */
    return Fcons(Qnil, val);
  }
  else {
    extern (*jrBeepFunc)();
    Lisp_Object Fding(), IROHA_mode_keys();

    jrBeepFunc = Fding;

    jrKanjiControl(0, KC_SETBUNSETSUKUGIRI, kugiri);
    jrKanjiControl(0, KC_SETWIDTH, 78);
    jrKanjiControl(0, KC_INHIBITHANKAKUKANA, 1);
    jrKanjiControl(0, KC_YOMIINFO, 2); /* ※２: ローマ字まで返す */
    val = Fcons(Qnil, val);
    return Fcons(IROHA_mode_keys(), val);
  }
}

DEFUN ("iroha-finalize", Firoha_finalize, Siroha_finalize, 0, 0, 0,
"finalize ``iroha'', which is a kana-to-kanji converter for GNU Emacs.\n\
This cause to write miscellaneous informations to kana-to-kanji dictionary.")
()
{
  Lisp_Object val;
  char **p;

  jrKanjiControl(0, KC_FINALIZE, &warning);

  val = Qnil;
  if (warning) {
    for (p = warning ; *p ; p++) {
      val = Fcons(make_string(*p, strlen(*p)), val);
    }
  }
  val = Fcons(val, Qnil);
  return val;
}

DEFUN ("iroha-touroku-string", Firoha_touroku_string, 
       Siroha_touroku_string, 1, 1, 0,
"Register Kanji words into kana-to-kanji conversion dictionary.")
	(str)
Lisp_Object str;
{
  jrKanjiStatusWithValue ksv;
  jrKanjiStatus ks;
  int len;
  Lisp_Object val;

  CHECK_STRING(str, 0);
  ksv.buffer = (unsigned char *)buf;
  ksv.bytes_buffer = KEYTOSTRSIZE;
  ks.echoStr = XSTRING (str)->data;
  ks.length = XSTRING (str)->size;
  ksv.ks = &ks;
  len = jrKanjiControl(0, KC_DEFINEKANJI, &ksv);
  val = makeReturnStruct(buf, ksv.val, ksv.ks);
  return val;
}

DEFUN ("iroha-set-width", Firoha_set_width,
       Siroha_set_width, 1, 1, 0,
"Set status-line width information, which is used to display \n\
kanji candidates.")
	(num)
Lisp_Object num;
{
  jrKanjiStatusWithValue ksv;
  jrKanjiStatus ks;
  int len;
  Lisp_Object val;

  CHECK_NUMBER(num, 0);

  jrKanjiControl(0, KC_SETWIDTH,  XINT (num));
  return Qnil;
}

DEFUN ("iroha-change-mode", Firoha_change_mode,
       Siroha_change_mode, 1, 1, 0,
"Change Japanese pre-edit mode.")
	(num)
Lisp_Object num;
{
  jrKanjiStatusWithValue ksv;
  jrKanjiStatus ks;
  Lisp_Object val;

  CHECK_NUMBER(num, 0);

  ksv.buffer = (unsigned char *)buf;
  ksv.bytes_buffer = KEYTOSTRSIZE;
  ksv.ks = &ks;
  ksv.val = XINT (num);
  jrKanjiControl(0, KC_CHANGEMODE,  &ksv);
  val = makeReturnStruct(buf, ksv.val, ksv.ks);
  return val;
}

Lisp_Object
IROHA_mode_keys()
{
#define IROHAWORKBUFSIZE 32
  char xxx[IROHAWORKBUFSIZE];
  Lisp_Object val;
  int i, n;

  n = jrKanjiControl(0, KC_MODEKEYS, xxx);
  val = Qnil;
  for (i = n ; i > 0 ;) {
    --i;
    val = Fcons(make_number((int)(0xFF & (unsigned char)xxx[i])), val);
  }
  return val;
}

DEFUN ("iroha-store-yomi", Firoha_store_yomi, Siroha_store_yomi, 
       1, 2, 0,
"Store yomi characters as a YOMI of kana-to-kanji conversion.")
	(yomi, roma)
Lisp_Object yomi, roma;
{
  jrKanjiStatusWithValue ksv;
  jrKanjiStatus ks;

  CHECK_STRING(yomi, 0);
  strncpy(buf, XSTRING(yomi)->data, XSTRING(yomi)->size);
  buf[XSTRING(yomi)->size] = '\0';

  if (NULL(roma)) {
    ks.mode = 0;
  }
  else {
    CHECK_STRING(roma, 0);

    strncpy(buf + XSTRING(yomi)->size + 1, XSTRING(roma)->data,
	    XSTRING(roma)->size);
    buf[XSTRING(yomi)->size + 1 + XSTRING(roma)->size] = '\0';
    ks.mode = (unsigned char *)(buf + XSTRING(yomi)->size + 1);
  }

  ks.echoStr = (unsigned char *)buf;
  ks.length = XSTRING (yomi)->size;
  ksv.buffer = (unsigned char *)buf; /* 返値用 */
  ksv.bytes_buffer = KEYTOSTRSIZE;
  ksv.ks = &ks;

  jrKanjiControl(0, KC_STOREYOMI, &ksv);

  return makeReturnStruct(buf, ksv.val, ksv.ks);
}

#ifdef KC_DO
DEFUN ("iroha-do-function", Firoha_do_function, Siroha_do_function, 
       1, 2, 0,
"Do specified function at current mode.")
	(num, ch)
Lisp_Object num, ch;
{
  jrKanjiStatusWithValue ksv;
  jrKanjiStatus ks;
  Lisp_Object val;

  CHECK_NUMBER(num, 0);

  if (NULL(ch)) {
    *buf = '@';
  }
  else {
    CHECK_NUMBER(ch, 0);
    *buf = XINT (ch);
  }

  ksv.buffer = (unsigned char *)buf;
  ksv.bytes_buffer = KEYTOSTRSIZE;
  ksv.ks = &ks;
  ksv.val = XINT (num);
  jrKanjiControl(0, KC_DO,  &ksv);
  val = makeReturnStruct(buf, ksv.val, ksv.ks);
  return val;
}
#endif /* KC_DO */

DEFUN ("iroha-parse", Firoha_parse, Siroha_parse, 
       1, 1, 0,
"Parse customize string.")
	(str)
Lisp_Object str;
{
  jrKanjiStatusWithValue ksv;
  jrKanjiStatus ks;
  Lisp_Object val;
  char *p;
  int n;

  CHECK_STRING(str, 0);

  strncpy(buf, XSTRING(str)->data, XSTRING(str)->size);
  buf[XSTRING(str)->size] = '\0';
  p = buf;
  n = jrKanjiControl(0, KC_PARSE,  &p);
  val = Qnil;
  while (n > 0) {
    n--;
    val = Fcons(make_string(p[n], strlen(p[n])), val);
  }
  return val;
}

#ifdef EWS4800
#include <sys/types.h>
#include <sys/times.h>

DEFUN ("iroha-current-tick", Firoha_current_tick, Siroha_current_tick,
       0, 0, 0,
"Return current times which elapsed real\
time, in 60ths (100ths) of a second, since an arbitrary\
point in the past (e.g., system start-up time)..")
	()
{
  Lisp_Object val;
  struct tms buffer;
  long l;
  
  l = times(&buffer);
  val = make_number(l);
  return val;
}
#endif /* EWS4800 */

/*
 * Functions following this line is for KKCP interface compatible
 * library.  These functions may be used by MILK system.
 */

#ifdef KC_GETCONTEXT

#include <iroha/RK.h>

#define RKBUFSIZE 1024

static int IRCP_context = -1;

DEFUN ("IRCP:henkan-begin", FIRCP_henkan_begin, SIRCP_henkan_begin,
       1, 1, 0,
"かな漢字変換した結果を返還する。文節切りがしてある。")
	(yomi)
{
  int nbun;
  Lisp_Object res, kanjiYomiList();

  CHECK_STRING(yomi, 0);
  if (IRCP_context < 0) {
    int context;

    if ((context = jrKanjiControl(0, KC_GETCONTEXT, 0)) == -1) {
      return Qnil;
    }
    IRCP_context = context;
  }
  nbun = RkBgnBun(IRCP_context, XSTRING(yomi)->data, XSTRING(yomi)->size,
		  (RK_XFER << RK_XFERBITS) | RK_KFER);

  return kanjiYomiList(IRCP_context, nbun);
}

static Lisp_Object kanjiYomiList(context, nbun)
int context, nbun;
{
  Lisp_Object val, res = Qnil;
  unsigned char RkBuf[RKBUFSIZE];
  int len, i;

  for (i = nbun ; i > 0 ; ) {
    i--;
    RkGoTo(context, i);
    len = RkGetKanji(context, RkBuf, RKBUFSIZE);
    val = make_string(RkBuf, len);
    len = RkGetYomi(context, RkBuf, RKBUFSIZE);
    res = Fcons(Fcons(val, make_string(RkBuf, len)), res);
  }
  return res;
}

DEFUN ("IRCP:henkan-next", FIRCP_henkan_next, SIRCP_henkan_next,
       1, 1, 0,
"候補一覧を求める。")
	(bunsetsu)
{
  int i, nbun, slen, len;
  unsigned char *p, RkBuf[RKBUFSIZE];
  Lisp_Object res = Qnil, endp;

  CHECK_NUMBER(bunsetsu, 0);
  if (IRCP_context < 0) {
    int context;

    if ((context = jrKanjiControl(0, KC_GETCONTEXT, 0)) == -1) {
      return Qnil;
    }
    IRCP_context = context;
  }
  RkGoTo(IRCP_context, XINT (bunsetsu));
  len = RkGetKanjiList(IRCP_context, RkBuf, RKBUFSIZE);
  p = RkBuf;
  for (i = 0 ; i < len ; i++) {
    slen = strlen(p);
    if (res == Qnil) {
      endp = res = Fcons(make_string(p, slen), Qnil);
    }
    else {
      endp = XCONS (endp)->cdr = Fcons(make_string(p, slen), Qnil);
    }
    p += slen + 1;
  }
  return res;
}

DEFUN ("IRCP:bunsetu-henkou", FIRCP_bunsetu_henkou, SIRCP_bunsetu_henkou,
       2, 2, 0,
"文節の長さを指定する。")
	(bunsetsu, bunlen)
{
  int nbun;

  CHECK_NUMBER(bunsetsu, 0);
  CHECK_NUMBER(bunlen, 0);
  
  nbun = XINT (bunsetsu);
  if (IRCP_context < 0) {
    int context;

    if ((context = jrKanjiControl(0, KC_GETCONTEXT, 0)) == -1) {
      return Qnil;
    }
    IRCP_context = context;
  }
  RkGoTo(IRCP_context, nbun);
  return kanjiYomiList(IRCP_context, RkResize(IRCP_context, XINT(bunlen)));
}

DEFUN ("IRCP:henkan-end", FIRCP_henkan_end, SIRCP_henkan_end,
       0, 0, 0,
"変換終了。")
	()
{
  if (IRCP_context < 0) {
    int context;

    if ((context = jrKanjiControl(0, KC_GETCONTEXT, 0)) == -1) {
      return Qnil;
    }
    IRCP_context = context;
  }
  RkEndBun(IRCP_context, 1); /* 学習はいつでも行って良いものなのか？ */
  return Qt;
}

#endif /* KC_GETCONTEXT */

syms_of_iroha ()
{
  defsubr (&Siroha_key_to_string);
  defsubr (&Siroha_initialize);
  defsubr (&Siroha_finalize);
  defsubr (&Siroha_touroku_string);
  defsubr (&Siroha_set_width);
  defsubr (&Siroha_change_mode);
  defsubr (&Siroha_store_yomi);
#ifdef KC_DO
  defsubr (&Siroha_do_function);
#endif /* KC_DO */
  defsubr (&Siroha_parse);
#ifdef EWS4800
  defsubr (&Siroha_current_tick);
#endif /* EWS4800 */
  defsubr (&Siroha_key_proc);
#ifdef KC_GETCONTEXT
  defsubr (&SIRCP_henkan_begin);
  defsubr (&SIRCP_henkan_next);
  defsubr (&SIRCP_bunsetu_henkou);
  defsubr (&SIRCP_henkan_end);
#endif /* KC_GETCONTEXT */
}
