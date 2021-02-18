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
 *
 *  Author : Osamu Hata, NEC Corporation  (hata@d1.bs2.mt.nec.co.jp)
 *
 */

/*
static	char	rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/cmd/icustom/RCS/icustom.h,v 1.10 91/09/06 13:06:16 hata Exp $";
*/

#define NHENKAN_MAX 9999

extern char *kanjidicname[], *userdicname[],  *bushudicname[], *localdicname[]; 
extern int  nkanjidics, nuserdics, nbushudics, nlocaldics;
extern char  *RomkanaTable, *RengoGakushu, *KatakanaGakushu;
extern int InitialMode, CursorWrap, SelectDirect, HexkeySelect, BunsetsuKugiri;
extern int IchitaroMove, ReverseOnlyCursor, Gakushu, QuitIchiranIfEnd;
extern int kakuteiIfEndOfBunsetsu, stayAfterValidate, BreakIntoRoman;
extern int kouho_threshold, gramaticalQuestion;
extern char *mode_mei[], null_mode[];

extern char *allKey[], *alphaKey[], *yomiganaiKey[];
extern char *yomiKey[], *jishuKey[], *tankouhoKey[];
extern char *ichiranKey[], *zenHiraKey[], *zenKataKey[];
extern char *zenAlphaKey[], *hanKataKey[], *hanAlphaKey[];
extern char *allFunc[], *alphaFunc[], *yomiganaiFunc[];
extern char *yomiFunc[], *jishuFunc[], *tankouhoFunc[];
extern char *ichiranFunc[], *zenHiraFunc[], *zenKataFunc[];
extern char *zenAlphaFunc[], *hanKataFunc[], *hanAlphaFunc[];
extern int NallKeyFunc, NalphaKeyFunc, NyomiganaiKeyFunc, NyomiKeyFunc;
extern int NjishuKeyFunc, NtankouhoKeyFunc,  NichiranKeyFunc;
extern int NzenHiraKeyFunc, NzenKataKeyFunc, NzenAlphaKeyFunc;
extern int NhanKataKeyFunc, NhanAlphaKeyFunc;
extern char returnKey[];
extern char *funcList[];

char **current_acts, **current_keys;
char err_mess[255];
char *initFileSpecified =(char *)NULL;

int junban;

char *mode_ichiran[] = {
  "����ե��٥åȥ⡼��",
  "[��] �Ѵ����ϥ⡼��",
  "[����] �������ɽ������",
  "��[��] ���ѤҤ餬�ʳ������ϥ⡼��",
  "��[��] ���ѥ������ʳ������ϥ⡼��",
  "��[��] Ⱦ�ѥ������ʳ������ϥ⡼��",
  "��[��] ���ѥ���ե��٥åȳ������ϥ⡼��",
  "��[a] Ⱦ�ѥ���ե��٥åȳ������ϥ⡼��",
  "[��] �ɤ����Ͼ���",
  "[����] ʸ�����Ѵ�����",
  "[����] ñ�����ɽ������",
  "[����] �������ɽ������",
  "[����] ñ����Ͽ����ʸɽ������",
  "[����] ñ����Ͽ����ʸɽ������",
  "[��] ���ѥ��������Ѵ����ϥ⡼��",
  "[��] Ⱦ�ѥ��������Ѵ����ϥ⡼��",
  "[��] ���ѥ���ե��٥å��Ѵ����ϥ⡼��",
  "[a] Ⱦ�ѥ���ե��٥å��Ѵ����ϥ⡼��",
  "[16��] �����ʥ��������Ͼ���",
  "[����] ������ɤ����Ͼ���",
  "[��ĥ] ��ĥ��ǽ�������",
  "[��] ����ʸ���������",
  "[��] ���ꥷ��ʸ���������",
  "[����] �����������",
  "[�ѹ�] �������ѹ�����",
  "[�Ѵ�] �Ѵ���������",
  "[���] ñ��������",
  "[��Ͽ] ñ����Ͽ�⡼��",
  "[�ʻ�] ñ����Ͽ�⡼�ɤ��ʻ��������",
  "[����] ñ����Ͽ�⡼�ɤμ����������",
  "[��] �������ϥ⡼��",
  "[�Խ�] �Խ��⡼��",
  "[����] ����Υޥ���ȡ�����ޥ���Ⱦ���",
  NULL
};

char *mode_ichiran2[] = {
  "alphaMode",
  "henkanNyuryokuMode",
  "[����] �������ɽ������",
  "zenHiraKakuteiMode",
  "zenKataKakuteiMode",
  "hanKataKakuteiMode",
  "zenAlphaKakuteiMode",
  "hanAlphaKakuteiMode",
  "[��] �ɤ����Ͼ���",
  "[����] ʸ�����Ѵ�����",
  "[����] ñ�����ɽ������",
  "[����] �������ɽ������",
  "[����] ñ����Ͽ����ʸɽ������",
  "[����] ñ����Ͽ����ʸɽ������",
  "zenKataHenkanMode",
  "hanKataHenkanMode",
  "zenAlphaHenkanMode",
  "hanAlphaHenkanMode",
  "[16��] �����ʥ��������Ͼ���",
  "[����] ������ɤ����Ͼ���",
  "[��ĥ] ��ĥ��ǽ�������",
  "[��] ����ʸ���������",
  "[��] ���ꥷ��ʸ���������",
  "[����] �����������",
  "[�ѹ�] �������ѹ�����",
  "[�Ѵ�] �Ѵ���������",
  "[���] ñ��������",
  "[��Ͽ] ñ����Ͽ�⡼��",
  "[�ʻ�] ñ����Ͽ�⡼�ɤ��ʻ��������",
  "[����] ñ����Ͽ�⡼�ɤμ����������",
  "[��] �������ϥ⡼��",
  "[�Խ�] �Խ��⡼��",
  "[����] ����Υޥ���ȡ�����ޥ���Ⱦ���",
  NULL
};

char *mode_ichiran3[] = {
  "Alpha",
  "HenkanNyuryoku",
  "Kigo",
  "ZenHiraKakutei",
  "ZenKataKakutei",
  "HanKataKakutei",
  "ZenAlphaKakutei",
  "HanAlphaKakutei",
  "Yomi",
  "Mojishu",
  "Tankouho",
  "Ichiran",
  "YesNo",
  "YesNo",
  "ZenKataHenkan",
  "HanKataHenkan",
  "ZenAlphaHenkan",
  "HanAlphaHenkan",
  "Hex",
  "Bushu",
  "Extend",
  "Russian",
  "Greek",
  "Line",
  "ChangingServer",
  "HenkanMethod",
  "DeleteDic",
  "Touroku",
  "TourokuHinshi",
  "TourokuDic",
  "QuotedInsert",
  "BubunMuhenkan",
  "MountDic",
  NULL
};

char mode_set[] = { 0, 1, 3, 4, 6, 5, 7, 1, 14, 16, 15, 17, -1};

char *base_menu[] = {
  "�������ޥ����ե�������ɤ߹���",
  "�������ޥ����ե�����ؤ���¸",
  "���Ѥ��뼭�������",
  "�����Υ������ޥ���",
  "�⡼�ɤ�ɽ��ʸ����Υ������ޥ���",
  "����¾�Υ������ޥ���",
  "��λ",
  NULL
};

char *d_mess[] = {
  "����μ����������ذ�ư�ĥ��ڡ�������,�ţӣå���",
  "��Ͽ����ɽ�����ذ�ư�ĥ��ڡ�������",
  "������Ͽ�ļ���μ���������֤ǥ꥿���󥭡�",
  "�������ļ������Ͽ����ɽ�����֤ǣĥ���",
  "���Ѥ��뼭��������λ�ļ���μ���������֤ǣţӣå���",
  NULL
};

char *dic_menu[] = {
  "���޻������Ѵ��ơ��֥�",
  "�����ƥ༭��̾",
  "�����Ѵ�����̾",
  "�桼������̾",
/*
  "Ϣ���Ѵ�����̾",
*/
  NULL
};

char *key_menu[] = {
  "���ƤΥ⡼��",
  "����ե��٥åȥ⡼��",
  "�Ѵ����ϥ⡼��(���ɤ����ϻ�)",
  "�Ѵ����ϥ⡼��(�ɤ����ϻ�)",
  "ʸ�����Ѵ�����",
  "ñ�����ɽ������",
  "�������ɽ������",
  "���ѤҤ餬�ʳ������ϥ⡼��",
  "���ѥ������ʳ������ϥ⡼��",
  "���ѥ���ե��٥åȳ������ϥ⡼��",
  "Ⱦ�ѥ������ʳ������ϥ⡼��",
  "Ⱦ�ѥ���ե��٥åȳ������ϥ⡼��",
  NULL
};

char *kh_mess[] = {
  "�����������ޥ�������������ϡֿ��������פ�����塢�꥿���󥭡��򲡤�",
  "��������������������ǽ����ϡֿ�����ǽ�פ�����塢�꥿���󥭡��򲡤�",
  "��",
  "���������ޥ��������Խ����ѹ�������ʬ������塢",
  "��������������������������������ĥ����򲡤�",
  "�������������������������Ժ�����˥����򲡤�",
  "���������������������������������ɥ����򲡤�",
  "�������������������������ɲá����������򲡤�",
  "���Υ⡼�ɤ�ȴ���롧�ţӣå������ѥ����򲡤�",
  NULL
};

struct dk_block {
  char *str;
  int len;
  int gyousu;
  int keysu;
  int actsu;
} dk_blocks[1024];

struct U_dk_block {
  char str[256];
  int len;
  int gyousu;
  int keysu;
  int actsu;
} U_dk_blocks;

struct wdisp {
  int top;
  int line;
  int kora;
  int ban;
  int current;
} WDisp, U_WDisp;

char *cc_keys[1], *cc_acts[1];
char ccc_keys[256], ccc_acts[256];
char undo_keys[256], undo_acts[256];
int  cc_keys_len = 0, cc_acts_len = 0;
int touroku_start = -1;
int undo_status = -1, undo_ts;

char *key_iroha[] = {
  "all",
  "alpha",
  "yomiganai",
  "yomi",
  "mojishu",
  "tankouho",
  "ichiran",
  "zenHiraKakutei",
  "zenKataKakutei",
  "zenAlphaKakutei",
  "hanKataKakutei",
  "hanAlphaKakutei",
  NULL
  };

char *keyLine[] = {
  "C-@  C-a  C-b  C-c  C-d  C-e  C-f  C-g  C-h  C-i  C-j  C-k",
  "C-l  C-m  C-n  C-o  C-p  C-q  C-r  C-s  C-t  C-u  C-v  C-w",
  "C-x  C-y  C-z  C-[  C-\\  C-]  C-^  C-_  space  !  \"  #  $  %",
  "&  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9",
  ":  ;  <  =  >  ?  @  A  B  C  D  E  F  G  H  I  J  K  L  M",
  "N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \\  ]  ^  _  `  a",
  "b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u",
  "v  w  x  y  z  {  |  }  ~  DEL  Nfer  Xfer  Up  Left  Right",
  "Down  Insert  Rollup  Rolldown  Clear  HELP  S-nfer  S-xfer",
  "S-up  S-left  S-right  S-down  C-nfer  C-xfer  C-up  C-left",
  "C-right  C-down  F1  F2  F3  F4  F5  F6  F7  F8  F9  F10",
  "PF1  PF2  PF3  PF4  undefine",
  NULL
};

char *actLine[] = {
  "SelfInsert:��ʸ����������",
  "QuotedInsert:���ΰ�ʸ����̵�������Ϥ���",
  "JapaneseMode:���ܸ�⡼�ɤؤΰܹ�",
  "AlphaMode:����ե��٥åȥ⡼�ɤ˰ܹ�",
  "HenkanNyuryokuMode:�Ѵ����ϥ⡼�ɤ˰ܹ�",
  "ZenHiraKakuteiMode:���ѤҤ餬�ʳ���⡼�ɤ˰ܹ�",
  "ZenKataKakuteiMode:���ѥ������ʳ���⡼�ɤ˰ܹ�",
  "HanKataKakuteiMode:Ⱦ�ѥ������ʳ���⡼�ɤ˰ܹ�",
  "ZenAlphaKakuteiMode:���ѥ���ե��٥åȳ���⡼�ɤ˰ܹ�",
  "HanAlphaKakuteiMode:Ⱦ�ѥ���ե��٥åȳ���⡼�ɤ˰ܹ�",
  "HexMode:�����ʥ��������ϥ⡼�ɤ˰ܹ�",
  "BushuMode:�������ϥ⡼�ɤ˰ܹ�",
  "KigouMode:�������ϥ⡼�ɤ˰ܹ�",
  "Forward:����ư(����)",
  "Backward:����ư(����)",
  "Next:������ʸ�����Ѵ�(�ɤ����ϤΤȤ�)",
  "Previous:������ʸ�����Ѵ�(�ɤ����ϤΤȤ�)",
  "BeginningOfLine:��Ƭ(��ü)����",
  "EndOfLine:����(��ü)����",
  "DeleteNext:����������ʬ��ʸ�����",
  "DeletePrevious:��������κ���ʸ�����",
  "KillToEndOfLine:���������걦¦��ʸ��������ޤǺ��",
  "Henkan:�Ѵ�",
  "Kakutei:����",
  "Extend:�ΰ迭�Ф�������ʸ�����ڤ��ؤ���(�ɤ����ϤΤȤ�)",
  "Shrink:�ΰ�̤ᡢ����ʸ�����ڤ��ؤ���(�ɤ����ϤΤȤ�)",
  "Quit:�����",
  "Touroku:�桼�ƥ���ƥ�(��ĥ)�⡼�ɤذܹ�",
  "ConvertAsHex:�ɤߤ�16�ʥ����ɤȤߤʤ����Ѵ�",
  "ConvertAsBushu:�ɤߤ�����̾�Ȥߤʤ����Ѵ�",
  "KouhoIchiran:�������ɽ��",
  "BubunMuhenkan:��ʬ̵�Ѵ�",
  "Zenkaku:���Ѥ��Ѵ�",
  "Hankaku:Ⱦ�Ѥ��Ѵ�",
  "ToUpper:��ʸ�����Ѵ�",
  "Capitalize:��Ƭʸ���Τߤ���ʸ���ˤ���",
  "ToLower:��ʸ�����Ѵ�",
  "Hiragana:�Ҥ餬�ʤ��Ѵ�",
  "Katakana:�������ʤ��Ѵ�",
  "Romaji:���޻����Ѵ�",
  "NextKouho:������",
  "PrevKouho:������",
  NULL
};

char *mode1_menu[] = {
  "         ����ե��٥åȥ⡼��",
  "  [��]   �Ѵ����ϥ⡼��",
  "��[��]   ���ѤҤ餬�ʳ������ϥ⡼��",
  "��[��]   ���ѥ������ʳ������ϥ⡼��",
  "��[��]    Ⱦ�ѥ������ʳ������ϥ⡼��",
  "��[��]   ���ѥ���ե��٥åȳ������ϥ⡼��",
  "��[a]    Ⱦ�ѥ���ե��٥åȳ������ϥ⡼��",
  "  [����] ʸ�����Ѵ�����",
  "  [����] ñ�����ɽ������",
  "  [����] �������ɽ������",
  "  [��]   ���ѥ��������Ѵ����ϥ⡼��",
  "  [��]    Ⱦ�ѥ��������Ѵ����ϥ⡼��",
  "  [��]   ���ѥ���ե��٥å��Ѵ����ϥ⡼��",
  "  [a]    Ⱦ�ѥ���ե��٥å��Ѵ����ϥ⡼��",
  NULL
};

char *mode2_menu[] = {
  "  [16��] �����ʥ��������Ͼ���",
  "  [����] ������ɤ����Ͼ���",
  "  [����] �������ɽ������",
  "  [��ĥ] ��ĥ��ǽ�������",
  "  [��]   ����ʸ���������",
  "  [��]   ���ꥷ��ʸ���������",
  "  [����] �����������",
  "  [�ѹ�] �������ѹ�����",
  "  [���] ñ��������",
  "  [��Ͽ] ñ����Ͽ�⡼��",
  "  [�ʻ�] ñ����Ͽ�⡼�ɤ��ʻ��������",
  "  [����] ñ����Ͽ�⡼�ɤμ����������",
  "  [��]   �������ϥ⡼��",
  "  [�Խ�] �Խ��⡼��",
  "  [����] ����Υޥ���ȡ�����ޥ���Ⱦ���",
  NULL
};

char mm1_set[] = { 0, 1, 3, 4, 5, 6, 7, 9, 10, 11, 14, 15, 16, 17 };

char mm2_set[] = { 18, 19, 2, 20, 21, 22, 23, 24, 26, 27, 28, 29, 30, 31, 32 };

char *etc_menu[] = {
  "initialMode",
  "cursorWrap",
  "numericalKeySelect",
  "selectDirect",
  "bunsetsuKugiri",
  "characterBasedMove",
  "reverseWidely",
  "quitIfEndOfIchiran",
  "breakIntoRoman",
  "gakushu",
  "stayAfterValidate",
  "kakuteiIfEndOfBunsetsu",
  "gramaticalQuestion",
  "nHenkanForIchiran",
  NULL
};
  
#define ON   1
#define OFF -1
#define MID  2

char etc_ctm[20];

char *etc_manual[] = {
  "  ��ư���Υ⡼�ɤ��������(��,��)�ǻ��ꤷ�ޤ����ǥե���Ȥϥ���ե��٥å�\
�⡼�ɤǤ���",
  "  �ɤߤ����Ϥ��Ƥ�����֤ǥ���������ư������˱�ü���鱦\
�ذ�ư�������򤷤����亸ü���麸�ذ�ư�������򤷤�����\
ȿ��¦�Υ������뤬��ư���뤳�Ȥ���ꤷ�ޤ���on�ǰ�ư����off\
�ǰ�ư���ޤ��󡣥ǥե���Ȥ�on�Ǥ���",
  "��������ǡ������������Ѥ��Ƹ��������Ǥ��뤫�ɤ��������\
���ޤ����ǥե���Ȥ�on�Ǥ���off����ꤹ��ȡ������Υ�����\
������������򤵤�Ƥ�����䤬���ꤷ���������������ϼ����ɤߤȤ���\
��갷���ޤ���",
  "numericalKeySelect��on�Ǥ���Ȥ��ˡ����������Ǹ�������򤷤��Ȥ�\
�˸������ɽ���Τޤޤ������Ǥʤ�������ꤷ�ޤ���on�ξ��ϸ��������\
��λ���ޤ����ǥե���Ȥ�off�Ǥ���",
  "�����ɽ�����Ƥ���Ȥ���ʸ�ᤴ�Ȥ˶���Ƕ��ڤ뤫�ɤ��������\
���ޤ���on�Ƕ��ڤ�ޤ����ǥե���Ȥ�off�Ǥ���",
  "�ɤߤ����Ϥ��Ƥ���Ȥ��˥��������ư��Ԥ����ˡ�ʸ��ñ�̤ǰ�ư��\
�Ԥ����ɤ�������ꤷ�ޤ����ǥե���Ȥ�on�Ǥ���off�ˤ���ȡ����޻�\
�����Ѵ��γ����ñ�̤��ˤ��ƥ���������ư���ޤ���ʸ�������Ʊ�ͤ�\
�Ԥ��ޤ���",
  "on����ꤹ����ɤߤ����Ϥ��Ƥ������ʸ�����ȿž�ϰϤ������ʤ�ޤ���\
�ǥե���Ȥ�off�Ǥ���",
  "��������Ǻǽ������ɽ�����Ƥ�����֤Ǽ���������Ԥ��ȸ��������\
��λ�����ɤߤ��Τ�Τ����Ȥ���ɽ������褦�ˤʤ�ޤ��������ܤ�\
���ڡ���������������ɽ���˥������ޥ������Ƥ�����ʤɤ�on�ˤ��Ƥ���\
�������Ǥ����ǥե���Ȥ�off�Ǥ���",
  "�Хå����ڡ����������Ǥä��Ȥ���ľ���Υ��޻������Ѵ����줿ʸ����\
���޻�����뤫�ɤ�������ꤷ�ޤ����ǥե���Ȥ�off�Ǥ���",
  "���ʴ����Ѵ����ؽ���Ԥ����ɤ�������ꤷ�ޤ����ǥե���Ȥ�on�Ǥ���",
  "����������֤Ǹ�������򤷤�ñ�����ɽ�����֤���ä��Ȥ��ˡ�������\
ʸ��򼡤�ʸ��˰�ư�����뤫�ɤ�������ꤷ�ޤ���off�ǥ�����ʸ��˰�ư\
���ޤ���on�Ǥϥ�����ʸ����Ѥ��ޤ��󡣥ǥե���Ȥ�on�Ǥ���",
  "�Ǳ�ʸ��Ǽ�ʸ��ذ�ư���褦�Ȥ����Ȥ��ˡ����ꤹ�뤫�ݤ�����ꤷ�ޤ���\
off����ꤹ��ȺǺ�ʸ�᤬������ʸ��ˤʤ�ޤ����ǥե���Ȥ�off�Ǥ���",
  "ñ����Ͽ���ʻ����ꤷ���塢�ܺ٤��ʻ�ʬ��Τ���μ����Ԥ����ݤ���\
���ꤷ�ޤ���on�Ǽ����Ԥ���off�Ǥϼ���򤷤ޤ��󡣥ǥե���Ȥ�on�Ǥ���",
  "�Ѵ������򲿲󤫲����ȸ������ɽ�����Ԥ���褦�ˤ����\
���Ǥ��ޤ��������ǤϤ��β�����������(��,��)�ǻ��ꤷ�ޤ���\
0��ˤ���Ȳ����Ѵ�������\
�����Ƥ����ɽ�����Ԥ��ʤ��ʤ�ޤ����ǥե���Ȥ�2��ǡ�2���Ѵ�������\
�����Ȱ���ɽ���ˤʤ�ޤ���",
  NULL
};
