#include <scheme.h>

#define MONSTART 2

static monitoring;

static Object P_Monitor (on) Object on; {
    char *brk;

    Check_Type (on, T_Boolean);
    if (Truep (on)) {
	if (!monitoring) {
	    brk = sbrk (0);
	    monstartup (MONSTART, (int (*)())brk);
	    monitoring = 1;
	}
    } else {
	monitor (0);
	monitoring = 0;
    }
    return Void;
}

init_lib_monitor () {
    Define_Primitive (P_Monitor, "monitor", 1, 1, EVAL);
}
