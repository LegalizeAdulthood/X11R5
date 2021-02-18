XcuWlm
    {
    *dimFacetColor "SteelBlue"
    *brightFacetColor "LightSteelBlue"
    *Label.background "LightGray"
    *Label.borderColor "LightGray"
    *Label.foreground "black"
    *Label.facetWidth "0"
    XcuTbl
	{
	formatString "c c c."
	alignedColumns "True"
	background "White"
	internalWidth "20"
	internalHeight "20"
	interWidth "10"
	interHeight "10"
	XcuLabel { label "UL" }
	XcuTbl
	    {
	    formatString "c s\nc c."
	    alignedColumns "True"
	    XcuLabel { label "PALETTE" }
	    XcuTbl
		{
		equalRows "True"
		formatString "c\nc\nc\nc\nc\n^."
		borderColor "LightGray"
		XcuLabel { label "Action" }
		XcuLabel { label "Frequency" }
		XcuLabel { label "Sequence" }
		XcuLabel { label "N Colors" }
		XcuLabel { label "Current\nPalette" }
		}
	    XcuTbl
		{
		formatString "c."
		*equalColumns "True"
		XcuTbl
		    {
		    XcuBmgr
			{
			bmgrType "Transient"
			manage "palette_initial" "palette_next" "palette_prev"
			if setCallback (x == String "palette_initial")
			    XcuCellFirstMap (XcuCell "cell")
			if setCallback (x == String "palette_next")
			    XcuCellNextMap (XcuCell "cell", Int "1")
			if setCallback (x == String "palette_prev")
			    XcuCellNextMap (XcuCell "cell", Int "-1")
			XcuButton { name "palette_initial" label "Initial" set "True" }
			XcuButton { name "palette_next" label "Next" }
			XcuButton { name "palette_prev" label "Previous" }
			}
		    }
		XcuTbl
		    {
		    XcuBmgr
			{
			bmgrType "OneOfMany"
			manage "palette_fixed"
			manage "palette_page"
			manage "palette_rule"
			manage "palette_line"
			if setCallback (x == String "palette_fixed")
			    XtSetValues (XcuCell "cell", frequency "fixed_frequency")
			if setCallback (x == String "palette_page")
			    XtSetValues (XcuCell "cell", frequency "page_frequency")
			if setCallback (x == String "palette_rule")
			    XtSetValues (XcuCell "cell", frequency "rule_frequency")
			if setCallback (x == String "palette_line")
			    XtSetValues (XcuCell "cell", frequency "line_frequency")
			XcuButton { name "palette_fixed" label "Fixed" set "True" }
			XcuButton { name "palette_page" label "Page" }
			XcuButton { name "palette_rule" label "Rule" }
			XcuButton { name "palette_line" label "Line" }
			}
		    }
		XcuTbl
		    {
		    XcuBmgr
			{
			bmgrType "OneOfMany"
			manage "next_permute"
			manage "random_permute"
			manage "random_colors"
			if setCallback (x == String "next_permute")
			    XtSetValues (XcuCell "cell", sequence "orderedPermute")
			if setCallback (x == String "random_permute")
			    XtSetValues (XcuCell "cell", sequence "randomPermute")
			if setCallback (x == String "random_colors")
			    XtSetValues (XcuCell "cell", sequence "randomColor")
			XcuButton { name "next_permute" label "Ordered Permute" set "True" }
			XcuButton { name "random_permute" label "Random Permute" }
			XcuButton { name "random_colors" label "Random Colors" }
			}
		    }
		XcuTbl
		    {
		    XcuBmgr
			{
			bmgrType "OneOfMany"
			manage "n2" : Int "2"
			manage "n3" : Int "3"
			manage "n4" : Int "4"
			manage "n5" : Int "5"
			manage "n6" : Int "6"
			manage "n7" : Int "7"
			manage "n8" : Int "8"
			if setCallback ()
			    XtSetValues (XcuCell "cell", nColors "CALL")
			XcuButton { name "n2" label "2" }
			XcuButton { name "n3" label "3" }
			XcuButton { name "n4" label "4" }
			XcuButton { name "n5" label "5" }
			XcuButton { name "n6" label "6" set "True" }
			XcuButton { name "n7" label "7" }
			XcuButton { name "n8" label "8" }
			}
		    }
		XcuTbl
		    {
		    formatString "c c c c."
		    *colorEntry "True"
		    *signalChar "\n"
		    *biggestLabel " MediumAquamarine "
		    XcuEntry
			{
			if callback ()
			    XtSetValues (XcuCell "cell", color0 "CALL")
			name "c0" label "Black"
			}
		    XcuEntry
			{
			if callback ()
			    XtSetValues (XcuCell "cell", color1 "CALL")
			name "c1" label "White"
			}
		    XcuEntry
			{
			if callback ()
			    XtSetValues (XcuCell "cell", color2 "CALL")
			name "c2" label "Blue"
			}
		    XcuEntry
			{
			if callback ()
			    XtSetValues (XcuCell "cell", color3 "CALL")
			name "c3" label "Green"
			}
		    XcuEntry
			{
			name "c4" label "Red"
			if callback ()
			    XtSetValues (XcuCell "cell", color4 "CALL")
			}
		    XcuEntry
			{
			name "c5" label "Yellow"
			if callback ()
			    XtSetValues (XcuCell "cell", color5 "CALL")
			}
		    XcuEntry
			{
			name "c6" label "Cyan"
			if callback ()
			    XtSetValues (XcuCell "cell", color6 "CALL")
			}
		    XcuEntry
			{
			name "c7" label "Magenta"
			if callback ()
			    XtSetValues (XcuCell "cell", color7 "CALL")
			}
		    }
		}
	    }
	XcuLabel { label "UR" }
	XcuLabel { label "L" }

	XcuCell {
		name "cell"
		verticalMerge "True"
		pickFile "PICK"
		perCol "2"
		/*
		 * This rulesFile is a NOP
		 * Note that the rulesFile will actually be determined
		 * by an XcuEntry!
		 */
		rulesFile "LCR4/f.all"
		width "401" height "401" name "cell"
		facetWidth "10"
		shadow "false"
		}
	XcuTbl
	    {
	    formatString "c."
	    interWidth "5"
	    interHeight "5"
	    *Tbl.internalWidth "5"
	    *Tbl.internalHeight "5"
	    *Tbl.background "LightGray"
	    *Tbl.equalColumns "True"
	    borderColor "White"
	    XcuTbl
		{
		formatString "c\nc c c."
		XcuLabel { name "Cols" }
		XcuBmgr
		    {
		    bmgrType "OneOfMany"
		    manage "v1" : Int "1"
		    manage "v2" : Int "2"
		    manage "v3" : Int "3"
		    manage "v4" : Int "4"
		    manage "v5" : Int "5"
		    manage "v6" : Int "6"
		    manage "v8" : Int "8"
		    manage "v10" : Int "10"
		    manage "v12" : Int "12"
		    if setCallback ()
			XtSetValues (XcuCell "cell", perRow "CALL")
		    XcuButton { name "v1" label "1" set "True" }
		    XcuButton { name "v2" label "2" }
		    XcuButton { name "v3" label "3" }
		    XcuButton { name "v4" label "4" }
		    XcuButton { name "v5" label "5" }
		    XcuButton { name "v6" label "6" }
		    XcuButton { name "v8" label "8" }
		    XcuButton { name "v10" label "10" }
		    XcuButton { name "v12" label "12" }
		    }
		}
	    XcuTbl
		{
		formatString "c\nc c c\nc c c\nc c c."
		XcuLabel { name "Rows" }
		XcuBmgr
		    {
		    bmgrType "OneOfMany"
		    manage "h1" : Int "1"
		    manage "h2" : Int "2"
		    manage "h3" : Int "3"
		    manage "h4" : Int "4"
		    manage "h5" : Int "5"
		    manage "h6" : Int "6"
		    manage "h8" : Int "8"
		    manage "h10" : Int "10"
		    manage "h12" : Int "12"
		    if setCallback ()
			XtSetValues (XcuCell "cell", perCol "CALL")
		    XcuButton { name "h1" label "1" set "True" }
		    XcuButton { name "h2" label "2" }
		    XcuButton { name "h3" label "3" }
		    XcuButton { name "h4" label "4" }
		    XcuButton { name "h5" label "5" }
		    XcuButton { name "h6" label "6" }
		    XcuButton { name "h8" label "8" }
		    XcuButton { name "h10" label "10" }
		    XcuButton { name "h12" label "12" }
		    }
		}
	    XcuTbl
		{
		XcuBmgr
		    {
		    bmgrType "SingleToggle"
		    manage "Alternate"
		    if setCallback ()
			XtSetValues (XcuCell "cell", alternating "True")
		    if unsetCallback ()
			XtSetValues (XcuCell "cell", alternating "False")
		    XcuButton { name "Alternate" set "False" }
		    }
		}
	    XcuTbl
		{
		formatString "c\nc c."
		XcuLabel { name "Vertical Merge" }
		XcuBmgr
		    {
		    bmgrType "DoubleToggle"
		    manage "vmt" "vmf"
		    if setCallback (x == String "vmt")
			XtSetValues (XcuCell "cell", verticalMerge "True")
		    if setCallback (x == String "vmf")
			XtSetValues (XcuCell "cell", verticalMerge "False")
		    XcuButton { name "vmt" label "True" }
		    XcuButton { name "vmf" label "False" set "True" }
		    }
		}
	    XcuTbl
		{
		formatString "c\nc c."
		XcuLabel { name "Horizontal Merge" }
		XcuBmgr
		    {
		    bmgrType "DoubleToggle"
		    manage "hmt" "hmf"
		    if setCallback (x == String "hmt")
			XtSetValues (XcuCell "cell", horizontalMerge "True")
		    if setCallback (x == String "hmf")
			XtSetValues (XcuCell "cell", horizontalMerge "False")
		    XcuButton { name "hmt" label "True" }
		    XcuButton { name "hmf" label "False" set "True" }
		    }
		}
	    XcuTbl
		{
		formatString "c\nc c."
		XcuLabel { name "Perturbation" }
		XcuBmgr
		    {
		    name "perturb"
		    bmgrType "DoubleToggle"
		    manage "perturb_true" "perturb_false"
		    if setCallback (x == String "perturb_true")
			XtSetValues (XcuCell "cell", perturb "True")
		    if setCallback (x == String "perturb_false")
			XtSetValues (XcuCell "cell", perturb "False")
		    XcuButton { name "perturb_true" label "True" }
		    XcuButton { name "perturb_false" label "False" set "True" }
		    }
		}
	    XcuTbl
		{
		formatString "c c."
		XcuBmgr
		    {
		    name "Pause"
		    bmgrType "DoubleToggle"
		    manage "pause" : Boolean "False"
		    manage "go" : Boolean "True"
		    XcuButton { name "pause" label "Pause" }
		    XcuButton { name "go" label "Run" set "True" }
		    }
		}
	    }
	XcuLabel { label "LL" }
	XcuTbl
	    {
	    formatString "padding(5);c c c c\nc c s s."
	    alignedColumns "True"
	    *Tbl.internalWidth "5"
	    *Tbl.internalHeight "5"
	    *Tbl.background "LightGray"
	    background "LightGray"
		XcuLabel { name "Rule File" }
		XcuEntry
		    {
		    name "RuleFile"
		    /*
		    label "random"
		    */
		    label "LCR4/f.all"
		    if callback ()
			XtSetValues (XcuCell "cell", rulesFile "CALL")
		    if callback (x == String "")
			SetChild (XcuBmgr "perturb", XcuButton "perturb_false")
		    if callback (x == String "")
			XtSetValues (XcuEntry "RuleFile", label "random")
		    if callback (x == String "random")
			SetChild (XcuBmgr "perturb", XcuButton "perturb_false")
		    biggestLabel " LCR4/f.0000000000 "
		    justify "left"
		    signalChar "\n"
		    }
		XcuBmgr
		    {
		    name "order"
		    bmgrType "DoubleToggle"
		    manage "in_order"
		    manage "random_order"
		    if setCallback (x == String "in_order")
			XtSetValues (XcuCell "cell", orderedRules "True")
		    if setCallback (x == String "random_order")
			XtSetValues (XcuCell "cell", orderedRules "False")
		    XcuButton { name "in_order" label "Ordered" }
		    XcuButton { name "random_order" label "Random" set "True" }
		    }
		XcuLabel { name "Pick File" }
		XcuEntry
		    {
		    name "PickFile"
		    biggestLabel "OneHeckuvaBigFilename"
		    label "./_pick"
		    if callback ()
			XtSetValues (XcuCell "cell", pickFile "CALL")
		    justify "left"
		    signalChar "\n"
		    }
		XcuLabel { name "Ignore String" }
		XcuEntry
		    {
		    name "IgnoreString"
		    label ""
		    biggestLabel "1, 2, 3, 4, 5, 6"
		    if callback ()
			XtSetValues (XcuCell "cell", ignoreColors "CALL")
		    justify "left"
		    signalChar "\n"
		    }
		XcuLabel { name "Seed:" }
		XcuLabel { name "seedValue" label "" }
	    XcuLabel { name "Rule Type" }
	    XcuTbl
		{
		borderColor "LightGray"
		XcuBmgr
		    {
		    name "types"
		    bmgrType "OneOfMany"
		    manage "lcr3" "lcr4" "lcr5"
		    manage "lccr4" "lccrrr4"
		    manage "klcrt3" "klcrt4" "klcrt5"
		    if setCallback ()
			XtSetValues (XcuCell "cell", cellType "CALL")
		    if setCallback ()
			SetChild (XcuBmgr "perturb", XcuButton "perturb_false")
		    XcuButton { name "lcr3" }
		    XcuButton { name "lcr4" set "True" }
		    XcuButton { name "lcr5" }
		    XcuButton { name "lccr4" }
		    XcuButton { name "lccrrr4" }
		    XcuButton { name "klcrt3" }
		    XcuButton { name "klcrt4" }
		    XcuButton { name "klcrt5" }
		    }
		}
	    }
	XcuLabel { label "LR" }
	}
    }
