/*
 * Same as ex3.wl, except...
 */

XcuWlm {
    #include "defaults.wl"
    XcuTbl
	{
	Command { name "Exit" }
	XcuBmgr
	    {
	    name "Set"
	    manage "Set 1" : Int "1"
	    manage "Set 2" : Int "2"
	    bmgrType "OneOfMany"		/* instead of Transient */
	    XcuButton
		{ name "Set 1" set "True" }	/* x initialized to this */
	    XcuButton { name "Set 2" }
	    }
	}
    }
