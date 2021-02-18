/***********************************************************************

zdebug.h - macros for debugging

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

**********************************************************************/

extern char _zdebug_flag[];

#define Z(f) if(_zdebug_flag[f] > 0)
#define ZZ(f) if(_zdebug_flag[f] >= 2)
#define ZZZ(f) if(_zdebug_flag[f] >= 3)
#define ZZZZ(f) if(_zdebug_flag[f] >= 4)

extern double kludge[];
