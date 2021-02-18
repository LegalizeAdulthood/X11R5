#include <stdio.h>
#include <signal.h>

#include "config.h"

#ifdef PORTABLE
#  include <setjmp.h>
#endif

#include "object.h"
#include "extern.h"
#include "macros.h"
#include "alloca.h"
