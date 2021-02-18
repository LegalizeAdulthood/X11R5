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
static	char	rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/cmd/icustom/RCS/set.h,v 1.10 91/09/06 13:08:18 hata Exp $";
*/

#define MAX_DICS 16

char *kanjidicname[MAX_DICS];
int  nkanjidics = 0;

char *userdicname[MAX_DICS];
int  nuserdics = 0;

char *bushudicname[MAX_DICS];
int  nbushudics = 0;

char *localdicname[MAX_DICS];
int  nlocaldics = 0;

char *RomkanaTable;
int InitialMode =0;
int CursorWrap = 1;
int SelectDirect = -1;
int HexkeySelect = 1;
int BunsetsuKugiri = -1;

char *RengoGakushu;
char *KatakanaGakushu;
int IchitaroMove = 1;
int ReverseOnlyCursor = -1;
int Gakushu = 1;
int QuitIchiranIfEnd = -1;
int kakuteiIfEndOfBunsetsu = -1;
int stayAfterValidate = 1;
int BreakIntoRoman = -1;
int kouho_threshold = 2;
int gramaticalQuestion = 1;

char *mode_mei[40];
char null_mode[40];

char *allKey[1024];
char *allFunc[1024];
char *alphaKey[256];
char *alphaFunc[256];
char *yomiganaiKey[512];
char *yomiganaiFunc[512];
char *yomiKey[1024];
char *yomiFunc[1024];
char *jishuKey[512];
char *jishuFunc[512];
char *tankouhoKey[512];
char *tankouhoFunc[512];
char *ichiranKey[512];
char *ichiranFunc[512];
char *zenHiraKey[256];
char *zenHiraFunc[256];
char *zenKataKey[256];
char *zenKataFunc[256];
char *zenAlphaKey[256];
char *zenAlphaFunc[256];
char *hanKataKey[256];
char *hanKataFunc[256];
char *hanAlphaKey[256];
char *hanAlphaFunc[256];

int NallKeyFunc = 0;
int NalphaKeyFunc = 0;
int NyomiganaiKeyFunc = 0;
int NyomiKeyFunc = 0;
int NjishuKeyFunc = 0;
int NtankouhoKeyFunc = 0;
int NichiranKeyFunc = 0;
int NzenHiraKeyFunc = 0;
int NzenKataKeyFunc = 0;
int NzenAlphaKeyFunc = 0;
int NhanKataKeyFunc = 0;
int NhanAlphaKeyFunc = 0;

char *keyCharMap[] = {               
  "Space",    "Delete",      "Nfer",     "Xfer",     "Up",
  "Left",     "Right",    "Down",     "Insert",   "Rollup",
  "Rolldown", "Clear",    "Help",     "KeyPad",   "S-nfer",
  "S-xfer",   "S-up",     "S-left",   "S-right",  "S-down",
  "C-nfer",   "C-xfer",   "C-up",     "C-left",   "C-right",
  "C-down",   "F1",       "F2",       "F3",       "F4",
  "F5",       "F6",       "F7",       "F8",       "F9",
  "F10",      "PF1",      "PF2",      "PF3",      "PF4",
  "PF5",      "PF6",      "PF7",      "PF8",      "PF9",
  "PF10",
};

char returnKey[16];

char *funcList[] = {
  "Undefine",
  "SelfInsert",
  "SelfInsert",
  "QuotedInsert",
  "JapaneseMode",
  "AlphaMode",
  "HenkanNyuryokuMode",
  "ZenHiraKakuteiMode",
  "ZenKataKakuteiMode",
  "HanKataKakuteiMode",
  "ZenAlphaKakuteiMode",
  "HanAlphaKakuteiMode",
  "HexMode",
  "BushuMode",
  "KigouMode",
  "Forward",
  "Backward",
  "Next",
  "Previous",
  "BeginningOfLine",
  "EndOfLine",
  "DeleteNext",
  "DeletePrevious",
  "KillToEndOfLine",
  "Henkan",
  "Kakutei",
  "Extend",
  "Shrink",
  "Quit",
  "Touroku",
  "ConvertAsHex",
  "ConvertAsBushu",
  "KouhoIchiran",
  "BubunMuhenkan",
  "Zenkaku",
  "Hankaku",
  "ToUpper",
  "Capitalize",
  "ToLower",
  "Hiragana",
  "Katakana",
  "Romaji",
  "FuncSequence",
  "UseOtherKeymap",
  "NextKouho",
  "PrevKouho",
  "NULL"
};
