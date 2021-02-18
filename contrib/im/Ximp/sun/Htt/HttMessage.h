/*
 *        %Z%%M% %I% %E%  
 * 
 * Derived from  @(#)Message.h	2.1 91/07/03 11:28:51 FUJITSU LIMITED. */
/******************************************************************

              Copyright 1991, by FUJITSU LIMITED
              Copyright 1991, by Sun Microsystems, Inc.	      

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of FUJITSU LIMITED and Sun
Microsystems, Inc. not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.  FUJITSU LIMITED and Sun Microsystems, Inc. makes no
representations about the suitability of this software for any
purpose.  It is provided "as is" without express or implied warranty.

FUJITSU LIMITED AND SUN MICROSYSTEMS, INC. DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL FUJITSU LIMITED AND SUN
MICROSYSTEMS, INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

  Author: Takashi Fujiwara     FUJITSU LIMITED 
                               fujiwara@a80.tech.yk.fujitsu.co.jp
          Kiyoaki oya          FUJITSU LIMITED
	  Hideki  Hiura           (hhiura@Sun.COM) Sun Microsystems, Inc.

******************************************************************/

#ifndef lint
#ifdef sccs
static char     sccsid[] = "%Z%%M% %I% %E% Sun Microsystems Inc.";
#endif
#endif

Message english_message[] = {
{   1,	"Can't open display ! (display name: %s)" },
{   2,	"Frontend already running." },
{   8,	"Frontend terminated by SIGTERM." },
{   9,	"Frontend terminated by server shutdown or KillClient." },
{  10,	"Bad color name  '%s', use default color." },
{  11,	"Bad color name, use default color." },

{  20,	"Can't open a file (%s), using %s instead." },
{  21,	"Can't open a file (%s), using built-in defaults instead." },
{  22,	"Errors found in fekeybind %s." },
{  23,	"Unknown keysym name (%s)." },
{  25,	"Unknown command argument (%s)." },

{  30,	"BadName (font dose not exist)\n (%s)" },
{  32,	"Undefined ascii/kana font ?\n" },
{  34,	"Undefined kanji font ?\n" },

{  70,	"Can't malloc : size = %d." },

#ifdef FUJITSU_SERVER_EXTENSION
{  80,	"Can't used : Control Modifiers Extenstion." },
#endif /*FUJITSU_SERVER_EXTENSION*/

/* no : 100 - 199 reserved for XIMP */
{ 100,	"Undefined protocol ?" },
{ 101,	"XIMP_CREATE : Can't open." },
{ 102,	"XIMP_CREATE : Duplicate open." },
{ 103,	"XIMP_CREATE : Can't open ; too many user !" },
{ 108,	"No Property (%s)." },
{ 109,	"No Property type (%s) size=%d." },
{ 111,	"XIMP_DESTROY : Not connected." },
{ 121,	"XIMP_BEGINE : Not connected." },
{ 131,	"XIMP_END : Not connected." },
{ 141,	"XIMP_SETFOCUS : Not connected." },
{ 151,	"XIMP_UNSETFOCUS : Not connected." },
{ 161,	"XIMP_MOVE : Not connected." },
{ 181,	"XIMP_RESET : Not connected." },
{ 185,	"XIMP_KEYPRESS : Not connected." },
{ 186,	"XIMP_SETVALUE : Not connected." },
{ 187,	"XIMP_GETVALUE : Not connected." },
{ 190,	"XIMP_EXTENSION : Not connected." },
{ 191,	"not open client." },

/* no : 200 - 299 reserved for XJP */
{ 200,	"Not support XJP protocol !" },
#ifdef XJP
{ 201,	"XJP_OPEN : Can't open." },
{ 202,	"XJP_OPEN : Duplicate open." },
{ 203,	"XJP_OPEN : Can't open ; too many user !" },
{ 211,	"XJP_CLOSE : Not connected." },
{ 221,	"XJP_BEGIN : Not connected." },
{ 231,	"XJP_END : Not connected." },
{ 241,	"XJP_VISIBLE : Not connected." },
{ 251,	"XJP_INVISIBLE : Not connected." },
{ 261,	"XJP_MOVE : Not connected." },
{ 271,	"XJP_CHANGE : Not connected." },
#endif XJP

};

int	max_message_no = sizeof(english_message) / sizeof(Message);
Message *message = english_message;
