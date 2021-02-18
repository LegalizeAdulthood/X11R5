/*
    $Header: dump_start.c,v 1.0 91/10/04 17:01:09 rthomson Exp $
*/

#include <stdio.h>
#include <X11/phigs/phigscfunc.h>
#include <X11/phigs/phigserr.h>

typedef struct _PEXtNameMap {
  int number;
  char *name;
} PEXtNameMap;

static PEXtNameMap function_names[] = {
