/*
** @(#) xmailregexp.h
**
** xmail - X window system interface to the mail program
**
** Copyright 1991 by National Semiconductor Corporation
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose is hereby granted without fee, provided that
** the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation, and that the name of National Semiconductor Corporation not
** be used in advertising or publicity pertaining to distribution of the
** software without specific, written prior permission.
**
** NATIONAL SEMICONDUCTOR CORPORATION MAKES NO REPRESENTATIONS ABOUT THE
** SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS"
** WITHOUT EXPRESS OR IMPLIED WARRANTY.  NATIONAL SEMICONDUCTOR CORPORATION
** DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO
** EVENT SHALL NATIONAL SEMICONDUCTOR CORPORATION BE LIABLE FOR ANY SPECIAL,
** INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
** LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
** OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
** PERFORMANCE OF THIS SOFTWARE.
**
** Author:  Michael C. Wagnitz - National Semiconductor Corporation
**
*/

#define	C_START		0
#define	C_EXEC		1
#define	C_FILE		2
#define	C_DELETE	3
#define	C_NEWMAIL	4

#ifndef PARSER
extern PatternRec command_pattern[];
#else
PatternRec command_pattern[] = {
    {"Start",						NULL }, /* 0 */
    { NULL,						  "" }, /* indx chg */
    {"[-n]\\n",						NULL }, /* 1 */
    {"[pPcCsSw] .*\\n",					NULL }, /* 1 */
    {"hold .*\\n",					NULL }, /* 1 */
    { NULL,						  "" },
    {"file [^%]*\\n",					NULL }, /* 2 */
    { NULL,						  "" },
    {"[du].*\\n",					NULL }, /* 3 */
    { NULL,						  "" },
    {"file %\\n",					NULL }, /* 4 */
    {"inc\\n",						NULL }, /* 4 */
    { NULL,						NULL },
   };
#endif

#define	O_BELL		0
#define	O_EXEC		1
#define	O_PRINT		2
#define	O_FILE		3

#ifndef PARSER
extern PatternRec output_pattern[];
#else
PatternRec output_pattern[] = {
    { "At ",						NULL }, /* 0 */
    { "No ",						NULL }, /* 0 */
    { "Unknown ",					NULL }, /* 0 */
    { "Referencing ",					NULL }, /* 0 */
    { "Cannot ",					NULL }, /* 0 */
    { "Not ",						NULL }, /* 0 */
    { "May ",						NULL }, /* 0 */
    { "\"[^\"]*\" \\[Appended\\] ",			NULL }, /* 0 */
    { "\"[^\"]*\" \\[New file\\] ",			NULL }, /* 0 */
    { "\"[^\"]*\" No such file or directory\\n",	NULL }, /* 0 */
    { "\"[^\"]*\" Invalid argument\\n",			NULL }, /* 0 */
    { "\"[^\"]*\" Permission denied\\n",		NULL }, /* 0 */
    { "\"[^\"]*\" Not a directory\\n",			NULL }, /* 0 */
    { "[0-9][0-9]*: Inappropriate message\\n",		NULL }, /* 0 */
    { "[0-9][0-9]*: Invalid message number\\n",		NULL }, /* 0 */
    { "[^:]*: No such file or directory\\n",		NULL }, /* 0 */
    { "[^:]*: Not a directory\\n",			NULL }, /* 0 */
    { "[^:]*: not a regular file\\n",			NULL }, /* 0 */
    { "[^:]*: empty file\\n",				NULL }, /* 0 */
    { "[^ ]* unreadable",				NULL }, /* 0 */
    { "[^ ]* not found",				NULL }, /* 0 */
    { "[^:]*: [^:]*: Permission denied",		NULL }, /* 0 */
    { "[^:]*: Permission denied\\n",			NULL }, /* 0 */
    { NULL,						  "" },
    { "From ",						NULL }, /* 1 */
    { "Message ",					NULL }, /* 1 */
    { NULL,						  "" },
    { "Pipe to: \"[^\"]*\"\\n",				NULL }, /* 2 */
    { NULL,						  "" },
    { "\"[^\"]*\": [0-9][0-9]* message.*\\n",		NULL }, /* 3 */
    { "[^:]*: .*\\n",					NULL }, /* 3 */
    { NULL,						NULL },
};
#endif
