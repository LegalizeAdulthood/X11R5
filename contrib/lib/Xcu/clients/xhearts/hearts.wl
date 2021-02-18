XcuWlm
    {
    *facetWidth "10"
    *dimFacetColor "SteelBlue"
    *brightFacetColor "LightSteelBlue"

    *Tbl.background "gray90"
    *Tbl.borderWidth "0"
    *Tbl.borderColor "gray90"

    *Command.background "white"
    *Command.foreground "black"
    *Command.internalWidth "10"
    *Command.internalHeight "10"
    *Command.lineFactor "1.2"
    *Command.facetWidth "0"
    *Command.borderWidth "0"
    *Command.internalWidth "0"
    *Command.internalHeight "0"

    *Label.borderColor "gray90"
    *Label.background "gray90"
    *Label.foreground "Blue"
    *Label.facetWidth "0"
    *Label.internalWidth "0"
    *Label.internalHeight "0"
    /*
    *font "7x13B"
    *font "12x24rk"
    *font "pica.18x30"
    */

    XcuTbl
      {
      formatString "c."
      facetWidth "0"
      XcuTbl
	{
	*Tbl.facetWidth "5"
	*Tbl.borderColor "Gray"
	  dimFacetColor "SteelBlue"
	  brightFacetColor "SteelBlue"
	background "Gray"
	internalWidth "20"
	internalHeight "20"
	equalColumns "True"
	formatString "c c c."
	resizeParticipants "internals+children"
	XcuLabel { name "dummy" mappedWhenManaged "False" }
	include "north.wl"
	XcuLabel { name "dummy" mappedWhenManaged "False" }
	include "west.wl"
	include "hand.wl"
	include "east.wl"
	XcuLabel { name "dummy" mappedWhenManaged "False" }
	include "south.wl"
	XcuLabel { name "dummy" mappedWhenManaged "False" }
	}
      XcuTbl
	{
	*Tbl.facetWidth "5"
	*Tbl.borderColor "Gray"
	  dimFacetColor "SteelBlue"
	  brightFacetColor "SteelBlue"
	background "Gray"
	internalWidth "20"
	internalHeight "20"
	formatString "c c c."
	resizeParticipants "internals+children"
	XcuLabel { name "dummy" mappedWhenManaged "False" }
	include "south_hand.wl"
	XcuLabel { name "dummy" mappedWhenManaged "False" }
	}
      }
    }
