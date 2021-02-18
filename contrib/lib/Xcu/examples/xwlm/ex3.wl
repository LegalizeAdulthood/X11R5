XcuWlm {
    #include "defaults.wl"
    XcuTbl
	{
	Command { name "Exit" }
	XcuBmgr
	    {
	    name "Set"
	    manage "Set1" : Int "1"
	    manage "Set2" : Int "2"
	    bmgrType "Transient"
	    XcuButton { name "Set1" }
	    XcuButton { name "Set2" }
	    }
	}
    }
