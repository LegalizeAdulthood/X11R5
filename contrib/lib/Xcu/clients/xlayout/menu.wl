XcuWlm
    {
    *dimFacetColor "SteelBlue"
    *brightFacetColor "LightSteelBlue"
    *facetWidth "5"
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

	XcuTbl
	    {
	    formatString "c."
	    background "LightGray"
	    XcuLabel { name "Variable Menu Pages" }
	    XcuDeck
		{
		name "menu"
		background "Wheat"
		internalWidth "50"
		internalHeight "100"
		*Label.background "Wheat"
		*Label.borderColor "Wheat"
		XcuLabel { name "Menu Page 1" }
		XcuLabel { name "Menu Page 2" }
		XcuLabel { name "Menu Page 3" }
		XcuLabel { name "Menu Page 4" }
		}
	    }

	XcuTbl
	    {
	    formatString "c."
	    background "LightGray"
	    XcuLabel { name "Permanent Menu Buttons" }
	    XcuTbl
		{
		formatString "c c."
		background "Wheat"
		XcuBmgr
		    {
		    bmgrType "OneOfMany"
		    if setCallback ( String "Page\n1" == x )
			RaiseWidget (XcuDeck "menu", XcuLabel "Menu Page 1")
		    if setCallback ( String "Page\n2" == x )
			RaiseWidget (XcuDeck "menu", XcuLabel "Menu Page 2")
		    if setCallback ( String "Page\n3" == x )
			RaiseWidget (XcuDeck "menu", XcuLabel "Menu Page 3")
		    if setCallback ( String "Page\n4" == x )
			RaiseWidget (XcuDeck "menu", XcuLabel "Menu Page 4")
		    manage "Page\n1" "Page\n2" "Page\n3" "Page\n4"
		    XcuButton { name "Page\n1" set "True" }
		    XcuButton { name "Page\n2" }
		    XcuButton { name "Page\n3" }
		    XcuButton { name "Page\n4" }
		    }
		}
	    }
	}
    }

