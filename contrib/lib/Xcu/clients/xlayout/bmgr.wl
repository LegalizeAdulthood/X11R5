XcuWlm
    {
    *dimFacetColor "SteelBlue"
    *brightFacetColor "LightSteelBlue"
    *facetWidth "5"
    *Tbl.facetWidth "10"
    *Command.background "white"
    *Command.foreground "black"
    *Command.cursor "dot"
    *Label.background "LightGray"
    *Label.borderColor "LightGray"
    *Label.foreground "black"
    *Label.facetWidth "0"
    *Tbl.internalWidth "20"
    *Tbl.internalHeight "20"
    *Tbl.interWidth "10"
    *Tbl.interHeight "10"
    XcuTbl
	{
	formatString "c."
	background "White"
	internalWidth "20"
	internalHeight "20"
	interWidth "10"
	interHeight "10"

    /*
     * Notice that the hierarchical positioning of the XcuBmgr widgets
     * is arbitrary.  They only have to appear within the scope of the
     * XcuWlm widget. The first few are shown as if they are
     * parents of the XcuButton widgets; the last two are shown near
     * the end of the file.
     */

	XcuTbl
	    {
	    formatString "c."
	    background "LightGray"
	    XcuLabel { name "Transient" }
	    XcuBmgr
		{
		bmgrType "Transient"
		manage "STEP"
		XcuButton { name "STEP" }
		}
	    }

	XcuTbl
	    {
	    formatString "c."
	    background "LightGray"
	    XcuLabel { name "Single Toggle" }
	    XcuBmgr
		{
		bmgrType "SingleToggle"
		manage "HIGHLIGHT"
		XcuButton { name "HIGHLIGHT" }
		}
	    }

	XcuTbl
	    {
	    formatString "c\nc c."
	    background "LightGray"
	    XcuLabel { name "Double Toggle" }
	    XcuBmgr
		{
		bmgrType "DoubleToggle"
		manage "ON" "OFF"
		XcuButton { name "ON" set "True" }
		XcuButton { name "OFF" }
		}
	    }

	XcuTbl
	    {
	    formatString "c\nc c c."
	    background "LightGray"
	    XcuLabel { name "One of Many" }
	    XcuButton { name "XY" }
	    XcuButton { name "YZ" }
	    XcuButton { name "ZX" set "True" }
	    }

	XcuTbl
	    {
	    formatString "c\nc c c."
	    background "LightGray"
	    XcuLabel { name "Any of Many" }
	    XcuButton { name "RED" }
	    XcuButton { name "GRN" set "True" }
	    XcuButton { name "BLU" set "True" }
	    }
	}

    XcuBmgr
	{
	bmgrType "OneOfMany"
	manage "XY" "YZ" "ZX"
	}

    XcuBmgr
	{
	bmgrType "AnyOfMany"
	manage "RED" "GRN" "BLU"
	}
    }

