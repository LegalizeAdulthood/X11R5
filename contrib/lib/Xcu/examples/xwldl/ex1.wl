/*
 * Minimal WLDL (except for the include line)
 *
 * This demonstrates both how to specify the widget hierarchy and
 * how to specify a resource for a widget.
 *
 * XcuWlm contains a single child.  In the usual case the child will be
 * a hierarchy of widgets, so the XcuWlm child would normally be
 * a geometry manager.
 *
 * Note that the default label for a XcuLabel derived widget is the name.
 *
 * Note that these 'C' style comments are allowed in a WLDL file.
 *
 * Note the include file mechanism.
 */

XcuWlm
    {
    include "shadow.bool"
    XcuLabel
	{
	name "Xcu"
	}
    }
