/*
 * Copyright (c) 1991 Old Dominion University.
 * Copyright (c) 1991 University of North Carolina at Chapel Hill.
 * All rights reserved.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  Old Dominion University and the University of North
 * Carolina at Chapel Hill make no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

/*
 *
 * Based on C++ list implementation by Steve Tell and Murray Anderegg.
 * Based on C list implementation in Amiga Kernel. 
 *   However, this one works and the Amiga one did not.
 */

#ifndef _PTRLISTS_H
#define _PTRLISTS_H

#include "lists.h"

typedef struct listitem {
    NODE my_node;
    void *ln_Val;
  } LISTITEM;

typedef struct ptrlist {
    LIST my_list;
    int nitems;
    LISTITEM *cursor;
    LISTITEM *sav_cursor;
  } PTRLIST;

#ifdef MPW
#define __ANSILIST__
#elif defined(__STRICT_ANSI__)
#define __ANSILIST__
#endif

#ifdef __ANSILIST__
#include "ANSIptrlist.h"
#else
#include "KRptrlists.h"
#endif

#endif _PTRLISTS_H
