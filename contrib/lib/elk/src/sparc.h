#ifndef PORTABLE
#  include "../include/alloca.h"   /* /usr/include/alloca.h */
#endif

extern int saveenv(), jmpenv();

#pragma unknown_control_flow(saveenv,jmpenv)
