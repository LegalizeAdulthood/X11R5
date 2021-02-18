#ifndef lint
     static char *rcsid = "$Header: /../dude/usr3/emu/term/parser/RCS/try.c,v 2.3 90/07/26 02:12:40 jkh Exp Locker: terry $";
#endif


/*
 * This file is part of the PCS emu system.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 * Stand-alone test code for the trie parser.
 *
 * Author: Terry Jones and Jordan K. Hubbard
 * Date: March 20th, 1990.
 * Description: This code links with the trie section of the term widget
 *		to generate a stand-alone parser tester.
 *
 * Revision History:
 *
 * $Log:	try.c,v $
 * Revision 1.1  90/03/14  18:18:17  terry
 * Initial revision
 * 
 */

#include "TermP.h"

Local char *myname;
Local struct _termRec fakeWidget;	/* highly kludge, but it works */
Import void print_regs();
Import int atoi();

Export int main(argc, argv)
int argc;
char **argv;
{
     Local void prompt(), help();
     Import void parse_init(), exit();
     Import int open(), close();
     char line[1024];
     Request *r;

     myname = *argv;

     /* Ordinarily, the term Initialize() procedure would do this */
     fakeWidget.term.cb = Fresh(ComBlock);

     parse_init(&fakeWidget);
     prompt();
     
     while (gets(line)) {
	  int action = line[0];
	  char *key;
	  int i;

	  key = line + 1;
	  while (*key == ' ' || *key == '\t')
	       key++;
	  
	  switch (action) {
	  case 'a':
	       parse_add(&fakeWidget, key);
	       break;

	  case 'c':
	       for (i = 0; i < CB_NREGS; i++) {
		    cb_reg_data(fakeWidget.term.cb, i) = (Generic)0;
		    cb_reg_type(fakeWidget.term.cb, i) = CB_INT_TYPE;
	       } 
	       break;

	  case 'e':
	       r = exactly_in_trie(&fakeWidget, key);
	       if (r) {
		    printf("request ids are ");
		    while (r) {
			 printf("%d ", r->request_id);
			 r = r->next;
		    }
		    printf("\n");
	       }
	       else
		    printf("no\n");
	       break;

	  case 'h':
	       help();
	       break;

	  case 'i':
	       parse_init(&fakeWidget);
	       break;

	  case 'l': {
	       char *tmp = key + 1;
	       int val = 0;
	       int reg = *key;
	       while (*tmp == ' ' || *tmp == '\t')
		    tmp++;
	       while (isdigit(*tmp)) {
		    val = val * 10 + *tmp - '0';
		    tmp++;
	       }
	       cb_reg_data(fakeWidget.term.cb, reg) = (Generic)val;
	       cb_reg_type(fakeWidget.term.cb, reg) = CB_INT_TYPE;
	       break;
	  }
	  case 'm':
	       trie_memory();
	       break;

	  case 'f': {
	       Import void parse();
	       int d;
	       
	       d = open(key, O_RDONLY);
	       printf("parsing '%s'\n", key);
	       parse(&fakeWidget, &d, 0);
	       close(d);
	       break;
	  }

	  case 'p': {
	       /* Put the string into a file and then open it. */
	       Import void parse();
	       Import int unlink();
	       char *f = "..gobbledygook";
	       FILE *fp = fopen(f, "w");
	       int d;

	       if (!fp) {
		    printf("could not fopen %s\n", f);
		    break;
	       }
	       fprintf(fp, "%s", key);
	       fclose(fp);
	       d = open(f, O_RDONLY);
	       printf("parsing '%s'\n", key);
	       parse(&fakeWidget, &d, 0);
	       close(d);
	       unlink(f);
	       break;
	  }

	  case '+':
	       parse_file(&fakeWidget, key);
	       printf("added %s\n", key);
	       break;

	  case 'r':
	       print_reverse(&fakeWidget);
	       break;

	  case 'R':
	       print_regs(&fakeWidget, key, fakeWidget.term.cb);
	       break;

	  case 'u':
	       cb_opcode(fakeWidget.term.cb) = atoi(key);
	       rparse(&fakeWidget);
	       printf("\n");
	       break;

	  case 'w': {
	       /* write out the trie contents. */
	       FILE *d;
	       if (!*key) {
		    print_trie(&fakeWidget, stdout);
		    break;
	       }
	       
	       d = fopen(key, "w");
	       if (!d){
		    printf("could not fopen '%s'\n", key);
		    break;
	       }
	       print_trie(&fakeWidget, d);
	       printf("wrote '%s'\n", key);
	       if (fclose(d) == EOF) {
		    fprintf(stderr, "%s: Could not fclose '%s'.\n", myname,
			    key);
		    exit(1);
	       }
	       break;
	  }
	  case 'x':
	  case 'q':
	       exit(0);

	  case '\0':
	       break;

	  case '?':
	       help();
	       break;

	  default:
	       printf("?\n");
	       break;
	  }
	  prompt();
     }
     return 0;
}

Local void prompt()
{
     printf("trie ==> ");
}

Local void help()
{
     printf("\
    a ID[,ID,..]<string>  add <string> to the parser.\n\
    c             clear the common block registers.\n\
    e <string>    is the string exactly in the trie?\n\
    help          this help.\n\
    i             initialise parser.\n\
    l x y         let register x have value y.\n\
    m             show memory usage.\n\
    f <file>      parse the contents of <file>.\n\
    p <string>    parse string.\n\
    + <file>      add the sequences in file to the parser.\n\
    q             quit.\n\
    r             show the contents of the reverse parse table.\n\
    R             print the registers in the common block.\n\
    u opcode      unparse (i.e. rparse) opcode.\n\
    w [<file>]    dump the trie in a readable form.\n\
    x             exit.\n\
    ?             this help.\n\
");
     
     return;
}
