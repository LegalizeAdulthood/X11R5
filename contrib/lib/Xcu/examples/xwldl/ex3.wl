/*
 * This example shows a slightly more complicated hierarchy.
 * The XcuWlm child is a XcuTbl widget with two children.
 */

XcuWlm
    {
    include "shadow.bool"
    *Command.internalWidth "10"
    *Command.internalHeight "10"
    *Command.background "wheat"
    *Label.internalWidth "10"
    *Label.internalHeight "10"
    *Label.background "wheat"
    *dimFacetColor "SteelBlue"
    *brightFacetColor "LightSteelBlue"
    XcuTbl
	{
	XcuCommand
	    {
	    name "Xcu"
	    }
	XcuLabel
	    {
	    name "Target"
	    }
	}
    }

