/*
 * This example shows how a directive can be used to manipulate the
 * resources of a widget.  Note that the syntax is fairly concise,
 * it's only the explanation here that is verbose...
 *
 * The elements of a directive are as follows:
 *
 *  if
 *              Keyword
 *
 *  callback
 *              name of the callback list (sorry about the possibility of
 *              confusion here, but the name of the callback list
 *              in an XcuCommand widget IS "callback")
 *
 * ( call_data_value  boolean  resource_type  "resource_value" )
 *
 *              The condition under which the action is taken.
 *              In this example, the "x" is a name that
 *              represents the call_data value to be tested.
 *              The name is arbitrary, but its presence makes the trigger
 *              look more meaningful, particularly when it is indexed,
 *              which is a possibility.
 *
 *              Since an XcuCommand widget's call_data is always zero,
 *              the trigger could have consisted only of a pair of
 *              empty parentheses.  A null trigger is taken to be True.
 *
 * XtSetValues
 *              The action to be taken; in this case, modify a resource
 *              of a widget.  The other possibility is to specify the
 *              handle for a widget's public function.
 *
 * (widget_class "widget_name", resource_name "new_resource_value")
 *
 *              The arguments of an XtSetValues action.
 *              They indicate the target widget and resource.
 */

XcuWlm
    {
    include "shadow.bool"
    include "defaults.wl"
    XcuTbl
        {
        XcuCommand
            {
            name "Xcu"
            if callback (x == Int "0")
                XtSetValues (XcuLabel "Target", background "Pink")
            if callback (x == Int "0")
                XtSetValues (XcuLabel "Target", label "Pink")
            }
        XcuLabel
            {
            name "Target"
            }
        }
    }

