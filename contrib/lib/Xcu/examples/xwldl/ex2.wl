/*
 * The four new lines in this example show how to specify default resources
 * for widgets that appear further down in the widget tree than where
 * the declarations are found.
 *
 * In this trivial example all XcuLabel widgets will have the specified
 * defaults, which could, of course, be overridden by a local resource
 * specification, as is demonstrated here with the background resource.
 */

XcuWlm
    {
    include "shadow.bool"
    *Label.internalWidth "10"			/* New */
    *Label.internalHeight "10"		/* New */
    *dimFacetColor "SteelBlue"			/* New */
    *brightFacetColor "LightSteelBlue"		/* New */
    *Label.background "Red"			/* New */
    XcuLabel
	{
	name "Xcu"
	background "wheat"			/* New */
	}
    }
