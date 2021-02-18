/* Copyright IBM Corporation 1988,1991 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

/* The next two lines need to be kept in sync */
#include <next_mach20/system.h>
        SYSTEM_H_FILE = next_mach20/system.h

/* These next two lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = next_mach20
      SYS_OS_ARCH = next_mach
   CONSOLESYSLIBS = -lmach

/* Get parent inclusions */
#include <allsys.mcr>


/* Now for the system-dependent information.  -Dconst is needed because some include files in system directories aren't properly ifdefed for STDC, -traditional is needed to avoid incompatibilities, -D__STRICT_BSD__ is needed to get BSDisms to work. */
COMPILERFLAGS= -Dconst= -traditional -D__STRICT_BSD__

/* We'll strip class programs manually in the InstallClassProgram macro defined in andrew.rls */
INSTCLASSPROGFLAGS= -ns -c -m 0555

/* Get site-specific inclusions */
#include <site.mcr>
