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

#include <stdio.h>
#include <iroha/jrkanji.h>

#define MAX_SIZE 1024

main()
{
  int c, nbytes;
  unsigned char workbuf[MAX_SIZE];
  jrKanjiStatus ks;

  jrKanjiControl(0, KC_INITIALIZE, 0);
  {
    jrKanjiStatusWithValue ksv;

    ksv.ks = &ks;
    ksv.buffer = workbuf;
    ksv.bytes_buffer = MAX_SIZE;
    ksv.val = IROHA_MODE_HenkanMode;
    jrKanjiControl(0, KC_CHANGEMODE, &ksv);
  }
  c = getchar();
  while(c != EOF) {
    nbytes = jrKanjiString(0, c, workbuf, MAX_SIZE, &ks);
    if (nbytes > 0) {
      workbuf[nbytes] = '\0';
      printf("%s\n", workbuf);
    }
    c = getchar();
  }
  jrKanjiControl(0, KC_FINALIZE, 0);
  exit(0);
}
