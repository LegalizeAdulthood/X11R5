.ll +5
.PH ""
.ce
WLM TUTORIAL
.sp
.ta 0.25i 0.50i 0.75i 1.00i 1.25i 1.50i 1.75i 2.00i 2.25i 2.50i 2.75i 3.00i
Suppose you want to make a small menu for a client application that is going
to rotate the color look up tables of your graphics box.
You want the menu to contain several items, not all visible at once: 
.AL A
.LI
A page for LUT modification.
.AL 1
.LI
A set of three buttons, each with the name of a LUT description file.
.LI
A one button toggle that converts the LUTs between negative and positive.
.LE
.LI
A page for rotation control.
.AL 1
.LI
A button to execute a single rotation increment.
.LI
A two button toggle that will control whether or not continuous
rotation is enabled.
.LI
A set of three buttons that indicate which color banks are enabled for rotation.
.LI
A label that shows the current LUT description file.
.LE
.LI
An ever-present pair of buttons that control which of the two pages is visible.
The two pages are to exactly overlay each other, so that only one is visible
at a time.
.LE
.P
You now have a choice - to write all the code yourself to handle this menu,
or to use the Widget Layout Manager (\fBXcuWlm\fR) widget.
When you have finished this tutorial, I hope to have convinced you that
the \fBXcuWlm\fR is the obvious choice.
.P
In either case, the first task is to choose the widgets needed to accomplish the
required interaction.
These, of course, can come from a variety of toolkits.
The Xcu toolkit alone contains all the required widgets for this example.
.P
To depict the hierarchy of widgets, we might picture it as follows,
using typical C language curly braces and indentation:
.sp
XcuWlm {
.br
	XcuTbl {
.br
		XcuDeck {
.br
			XcuTbl {
.br
				RadioButtons{} /* file choices */
.br
				ToggleButton{} /* negative/positive */
.br
				}
.br
			XcuTbl {
.br
				Command{}         /* rotation increment */
.br
				ToggleButtons{}   /* continuous rotation */
.br
				CheckBoxButtons{} /* red, green, blue */
.br
				Label{}           /* current file */
.br
				}
.br
			}
.br
		ToggleButtons{} /* page control */
.br
		}
.br
	}
.sp
This is exactly the basic syntax for the
Widget Layout Description Language (WLDL) -- a widget class name with
the body of the widget enclosed in curly braces.
All that is left is the grammar for the two types of things that the
body of a widget may contain: resources and directives.
In the following examples, \fBbold\fR indicates keywords in the grammar,
\fIitalics\fR indicate strings which are to be substituted by the programmer,
and words in regular font indicate sample substitutions.
.sp
To take the \fBXcuLabel\fR widget as an example, one can modify the \fBXtNlabel\fR
resource merely by specifying the resource name and the resource value as a
pair, with the value in double quotes:
.sp
XcuLabel { label "File 1" }
.sp
.P
It is also possible to change the default resources for all widgets
in the subtree rooted at the widget in which a default resource specification
occurs.  For example, to change all the background colors for the widgets
beneath a \fBXcuTbl\fR, use a default specification and a resource value:
.sp
XcuTbl {
.br
	*Background "yellow"
.br
	\fIchildren\fR
.br
	}
.sp
The default specification must begin with a "*" and has the same syntax
as the specifications found in a resource file. (The value for the resource,
however, is found between double quotes.
It is not separated from the specification by a colon.)
.P
The other thing that a widget may contain is a directive.
Directives are a mechanism by which widgets may communicate with each other.
The messages are controlled by the \fBXcuWlm\fR, thus
a programmer may implement complex menu semantics with the WLDL instead
of client code.
Directives are executed in response to widget callbacks, and are only
executed if the call_data in the callback matches a trigger.
The action taken can be either to modify a widget's resource with
\fBXtSetValues()\fR or to call a widget's public function.
Grammatically, a directive is of the form:
.sp
	\fBif\fR \fIcallback_name\fR ( \fItrigger\fR ) \fIaction\fR
.sp
We will need several directives in our example.
For instance, when the user clicks on a new filename while in the modification
page, we will want to change the label of the \fBXcuLabel\fR widget in the
rotation control page.
This is easily accomplished by putting this directive in the body of the
appropriate widget:
.sp
\fBif\fR callback ()
.br
	\fBXtSetValues\fR ( \fIwidget_class\fR "\fIwidget_name\fR",
\fIresource_name\fR, "\fIresource_value\fR" )
.sp
Since there is no trigger, the action is always taken.
.P
We will also need the kind of directive that calls a widget's public function.
In our example, when the user clicks on a button to change pages, the proper
page can be placed on top by calling the \fBXcuDeckRaiseWidget()\fR procedure
of the \fBXcuDeck\fR widget:
.sp
\fIWidget_class\fR {
.br
	...
.br
	\fBif\fR callback ()
.br
		RaiseWidget ( XcuDeck "\fIdeck_name\fR",
\fIdeck_child_class\fR "\fIdeck_child_name\fR" ) ;
.br
	}
.sp
Note that this directive has the same form as the first directive we looked at.
The only difference is that the action is a public
procedure name instead of \fBXtSetValues()\fR.
The arguments are separated by commas, and the \fBXcuWlm\fR takes care of
converting them to the real values needed.
.P
Now we need an example without an empty trigger.
Suppose we wanted something to happen based on a page's appearance
at the top of the \fBXcuDeck\fR.
This could be handled this way:
.sp
XcuDeck {
.br
	...
.br
	\fBif\fR callback
( \fIwidget_class\fR "\fIwidget_name\fR" == x[0] ) \fIaction\fR
.br
	}
.sp
.P
To understand a trigger, one must understand what the value of the callback's
call_data is.
In the case of the \fBXcuDeck\fR widget, the call_data is an array of widgets.
The order of the widgets within the array represents their current stacking
order.
So, this trigger says that when the specified widget is at the top of the
stacking order, the function should be called.
.P
Many widgets will have callbacks that are simpler values, such as an integer.
If one wanted to execute a directive when the call_data of a callback was less
than 10, for example:
.sp
\fIWidget_class\fR {
.br
	...
.br
	\fBif\fR callback ( x  <  Int "10" ) \fIaction\fR
.br
	}
.sp
.P
The general form of a trigger is:
.sp
	\fIRepresentation_type\fR "\fIvalue\fR" 
\fIcomparison_operator\fR  \fIcall_data\fR
.br
				or
.br
	\fIcall_data\fR  \fIcomparison_operator\fR  \fIRepresentation_type\fR "\fIvalue\fR"
.sp
Note that the call_data variable has to have a name, but the use of
the name "x" is arbitrary.
.P
There may also be more than one condition separated by logical operators:
.sp
\fIWidget_class\fR {
.br
	...
.br
	\fBif\fR callback ( x  >  Int "5"  &&  x  <  Int "10" ) \fIaction\fR
.br
	}
.sp
.P
We now have all the grammar necessary to layout the menu.
Now we need to find out how communication between the menu and the client
can be achieved.
This is accomplished with the \fBXcuWlm\fR widget's public functions.
Suppose the client has a routine that, when called, will perform one
rotation increment.
All we need to do is to add this procedure to the callback
list of the \fBXcuCommand\fR
button:
.DS
XcuWlmAddCallback ( \fIwlm_id\fR,  \fItype\fR,
		\fIwidget_class\fR,  \fIwidget_name\fR,
		\fIcallback_name\fR,  \fIcallback\fR,  \fIclient_data\fR )
.DE
The last 5 arguments are identical to the arguments needed by
\fBXtAddCallback()\fR
(\fIwidget_class\fR and \fIwidget_name\fR
get converted to the widget id), and the first
argument is the \fBXcuWlm\fR widget id.
The only argument that needs some explanation is the second argument, and we'll
defer that discussion to later.
.P
Consider the button that controls whether or not the LUTs should display
positive or negative color.
Normally, one would write a callback routine and register
it with a \fBXcuCommand\fR widget.
This callback routine would then flip the value of a Boolean variable between
True and False.
The \fBXcuWlm\fR, however,  has public functions that can modify a client variable.
By choosing a widget that knows how to toggle between two states and has
a callback list for each state, the following two function calls, executed once,
will ensure that the client variable reflects the state of the menu button:
.DS
XcuWlmSetBoolean  ( wlm_id,  XcuWLM_SAMPLING,  "XcuBmgr",  "+-",  "setCallback",  &value,  True ) ;
XcuWlmSetBoolean  ( wlm_id,  XcuWLM_SAMPLING,  "XcuBmgr",  "+-",  "unsetCallback",  &value,  False ) ;
.DE
Each of these functions tells the \fBXcuWlm\fR to store the value given in the
last argument at the address given in the preceding argument whenever the
indicated callback is made by the widget.
So, what happens here is that whenever the button is clicked by the user,
the widget
changes state and calls the callbacks in one of its two callback lists.
In either case, the \fBXcuWlm\fR has registered a callback with the widget,
and this callback assigns the appropriate value to the variable.
Furthermore, since the second argument has been set to \fBXcuWLM_SAMPLING\fR,
the widget will be instructed to make the appropriate callback to indicate
its initial state, which will in turn initialize the client variable.
This ensures that the state of the client accurately reflects what the
user sees in the appearance of the menu.
Similar public functions exist for "Int", "Float", and "String".
.P
There are also four "Get" functions similar to the "Set" functions which
retrieve values from the call_data of the widget callback, for example:
.DS
XcuWlmGetInt  ( wlm_id,  XcuWLM_SAMPLING,  "XcuBmgr",  "temperature",  "callback",  &value ) ;
.DE
This function tells the \fBXcuWlm\fR to take the integer value returned as
the widget's callback data and store it at the indicated address.
.P
Taken together, all of these public functions discussed so far 
are called "Associations", in that they associate user activity
in the menu with actions in the client program.
We will use the term \fIWEvent\fR to indicate the activity of the \fBXcuWlm\fR
when one of the callback routines registered as a result of an
Association is called by a widget.
A queue of pending \fIWEvents\fR is maintained by the \fBXcuWlm\fR until the 
client requests that they be executed.
.P
The final set of \fBXcuWlm\fR public functions deals with how to control when
\fIWEvents\fR are to be executed.
For this purpose, there are three functions which make the \fBXcuWlm\fR look
like an input device.
.BL
.LI
\fBXcuWlmRequest ( wlm_id )\fR will block until a \fBWEvent\fR
is available in the queue, then executes it and returns.
.LI
\fBXcuWlmEvent ( wlm_id )\fR checks the queue to see if there is a \fIWEvent\fR
pending.  If there is, it executes it and returns True.
If there is no \fIWEvent\fR pending, then it immediately returns False.
.LI
\fBXcuWlmSample ( wlm_id )\fR will initiate a \fIWEvent\fR for every association
that had \fBXcuWLM_SAMPLING\fR in the second argument of the association function.
.LE
.P
Note that in the LUT rotation application, we will need at least
\fBXcuWlmEvent()\fR, since we need to be busy rotating the LUTs when
continuous rotation is True, but we also need to peek now and then to see
if anything has happened in the menu.
.P
Suppose that when you are using an Association function, you would like
the associated action to occur, but you don't want the \fBXcuWlm\fR to be
notified with a \fIWEvent\fR.
Setting the second argument of the Association function to \fBXcuWLM_BYPASS\fR
will accomplish this. (\fBXcuWLM_BYPASS\fR and \fBXcuWLM_SAMPLING\fR may be or'd
together.)
.P
So, let's put it all together.
Figure 1 is the complete menu, using widgets contained in the Xcu toolkit.
Figure 2 is the applicable program fragment from the client application.
Figure 3 is the equivalent program without using the \fBXcuWlm\fR widget.
.sp
.ce
FIGURE 1
.sp
.ps -1
XcuWlm {
.br
	XcuTbl {
.br
		formatString "padding(10);\\nc\\nc\\nc."
.br
		include "page_widgets"
.br
		include "perm_widgets"
.br
		}
.br
	}
.sp
"page_widgets"
.sp
XcuDeck {
.br
	name "pages"
.br
	include "rotate_page"
.br
	include "modify_page"
.br
	}
.sp
"modify_page"
.sp
XcuTbl {
.br
	formatString "c\\nc."
.br
	name "modification page"
.br
	XcuBmgr {
.br
		name "files"
.br
		if setCallback ( String "FILE1"  ==  x )
.br
			XtSetValues ( Lbl "FILEX",  label "File1" )
.br
		if setCallback ( String "FILE2"  ==  x )
.br
			XtSetValues ( Lbl "FILEX",  label "File2" )
.br
		if setCallback ( String "FILE3"  ==  x )
.br
			XtSetValues ( Lbl "FILEX",  label "File3" )
.br
		bmgrType "OneOfMany"
.br
		manage "FILE1" "FILE2" "FILE3"
.br
		XcuTbl {
.br
			XcuButton { name "FILE1" set "True" }
.br
			XcuButton { name "FILE2" }
.br
			XcuButton { name "FILE3" }
.br
			}
.br
		}
.br
	XcuBmgr {
.br
		name "pos_neg"
.br
		bmgrType "SingleToggle"
.br
		manage "NEGATIVE"
.br
		XcuButton { name "NEGATIVE" set "False" }
.br
		}
.br
	}
.sp
"rotate_page"
.sp
XcuTbl {
.br
	name "rotation page"
.br
	formatString "c\\nc\\nc\\nc."
.br
	XcuBmgr {
.br
		name "rotation increment"
.br
		bmgrType "Transient"
.br
		manage "ROTATION INCREMENT"
.br
		XcuButton { name "ROTATION INCREMENT" }
.br
		}
.br
	XcuBmgr {
.br
		name "switch"
.br
		bmgrType "DoubleToggle"
.br
		manage  "ROTATION ON"  :  Boolean "True"
.br
		manage  "ROTATION OFF"  :  Boolean "False"
.br
		XcuTbl {
.br
			XcuButton { name "ROTATION ON" }
.br
			XcuButton { name "ROTATION OFF" set "True" }
.br
			}
.br
		}
.br
	XcuBmgr {
.br
		name "banks"
.br
		bmgrType "AnyOfMany"
.br
		manage "RED" : Int "0",  "GRN" : Int "1",  "BLU" : Int "2"
.br
		XcuTbl {
.br
			XcuButton { name "RED" }
.br
			XcuButton { name "GRN" }
.br
			XcuButton { name "BLU" }
.br
			}
.br
		}
.br
	Lbl { name "FILEX" }  /* current file */
.br
	}
.sp
"perm_widgets"
.sp
XcuBmgr {
.br
	if setCallback ( String "ROTATION PAGE"  ==  x )
.br
		RaiseWidget ( XcuDeck "pages",  XcuTbl "rotation page" )
.br
	if setCallback ( String "MODIFICATION PAGE"  ==  x )
.br
		RaiseWidget ( XcuDeck "pages",  XcuTbl "modification page" )
.br
	bmgrType "DoubleToggle"
.br
	manage "MODIFICATION PAGE" "ROTATION PAGE"
.br
	XcuTbl {
.br
		XcuButton { name "MODIFICATION PAGE" set "True" }
.br
		XcuButton { name "ROTATION PAGE" }
.br
		}
.br
	}
.ps +1
.sp
.ce
FIGURE 2
.sp
.ps -1
wlm_id = XtCreateManagedWidget ( "wlm", xcuWlmWidgetClass, top, wlm_args, ONE ) ;
.br
XcuWlmAddCallback ( wlm_id, NULL, "XcuBmgr", "rotation increment", "setCallback",
increment, NULL ) ;
.br
XcuWlmAddCallback ( wlm_id, XcuWLM_SAMPLING, "XcuBmgr", "files", "setCallback",
file_read, NULL ) ;
.br
XcuWlmAddCallback ( wlm_id, XcuWLM_SAMPLING, "XcuBmgr", "banks", "setCallback",
bank_mod, True ) ;
.br
XcuWlmAddCallback ( wlm_id, XcuWLM_SAMPLING, "XcuBmgr", "banks", "unsetCallback",
bank_mod, False ) ;
.br
XcuWlmAddCallback ( wlm_id, XcuWLM_SAMPLING, "XcuBmgr", "pos_neg", "setCallback",
pos_neg, True ) ;
.br
XcuWlmAddCallback ( wlm_id, XcuWLM_SAMPLING, "XcuBmgr", "pos_neg", "unsetCallback",
pos_neg, False ) ;
.br
XcuWlmGetBoolean ( wlm_id, XcuWLM_SAMPLING, "XcuBmgr", "switch", "setCallback",
&on_switch ) ;
.sp
XcuWlmSample ( wlm_id )
.sp
while (1) {
.br
	while (on_switch) {
.br
		XcuWlmEvent ( wlm_id ) ;
.br
		increment () ;
.br
		}
.br
	XcuWlmRequest ( wlm_id ) ;
.br
	}
.ps +1
.sp
.ce
FIGURE 3
.sp
.ps -1
#include <Xcu/Tbl.h>
.br
#include <Xcu/Bmgr.h>
.br
#include <Xcu/BmgrR.h>
.br
#include <Xcu/Label.h>
.br
#include <Xcu/Deck.h>
.br
#include <Xcu/Button.h>
.sp
Widget top_deck, outer_tbl, pages_deck, rotation_page, incr_button, incr_bmgr,
.br
       switch_tbl, on_button, off_button, switch_bmgr, bank_tbl, red_button,
.br
       grn_button, blu_button, bank_bmgr, file_label, modification_page,
.br
       files_tbl, file1_button, file2_button, file3_button, files_bmgr,
.br
       negative_button, negative_bmgr, permanent_tbl, modpage_button,
.br
       rotpage_button, permanent_bmgr ;
.sp
Arg arg ;
.br
Widget managed_buttons[3] ;
.br
caddr_t managed_values[3] ;
.sp
top_deck = XtCreateManagedWidget ( "top deck", xcuDeckWidgetClass, top, NULL, NULL ) ;
.br
	XtSetArg ( arg, XtNformatString, "padding(6);\\nc\\nc\\nc." ) ;
.br
	outer_tbl = XtCreateManagedWidget ( "outer_tbl", xcuTblWidgetClass, top_deck, &arg, ONE ) ;
.br
		pages_deck = XtCreateManagedWidget ( "pages_deck", xcuDeckWidgetClass, outer_tbl, NULL, NULL ) ;
.br
		XtSetArg ( arg, XtNformatString, "\\nc\\nc\\nc\\nc." ) ;
.br
		rotation_page = XtCreateManagedWidget ( "rotation_page", xcuTblWidgetClass, pages_deck, &arg, ONE ) ;
.br
			incr_button = XtCreateManagedWidget ( "ROTATION INCREMENT", xcuButtonWidgetClass, rotation_page, NULL, NULL ) ;
.br
				XtSetArg ( arg, XtNbmgrType, XcuBMGR_TRANSIENT ) ;
.br
				incr_bmgr = XtCreateManagedWidget ( "incr bmgr", xcuBmgrWidgetClass, top_deck, &arg, ONE ) ;
.br
				managed_buttons[0] = incr_button ;
.br
				managed_values[0] = (caddr_t) "ROTATION INCREMENT" ;
.br
				XcuBmgrManage (incr_bmgr, managed_buttons, managed_values, ONE ) ;
.br
			switch_tbl = XtCreateManagedWidget ( "switch tbl", xcuTblWidgetClass, rotation_page, NULL, NULL ) ;
.br
				on_button = XtCreateManagedWidget ( "ROTATION ON", xcuButtonWidgetClass, switch_tbl, NULL, NULL ) ;
.br
				XtSetArg ( arg, XtNset, True ) ;
.br
				off_button = XtCreateManagedWidget ( "ROTATION OFF", xcuButtonWidgetClass, switch_tbl, NULL, NULL ) ;
.br
					XtSetArg ( arg, XtNbmgrType, XcuBMGR_DOUBLE_TOGGLE ) ;
.br
					switch_bmgr = XtCreateManagedWidget ( "switch bmgr", xcuBmgrWidgetClass, top_deck, &arg, ONE ) ;
.br
					managed_buttons[0] = on_button ;
.br
					managed_values[0] = (caddr_t) True ;
.br
					managed_buttons[1] = off_button ;
.br
					managed_values[1] = (caddr_t) False ;
.br
					XcuBmgrManage (switch_bmgr, managed_buttons, managed_values, TWO ) ;
.br
			bank_tbl = XtCreateManagedWidget ( "bank tbl", xcuTblWidgetClass, rotation_page, NULL, NULL ) ;
.br
				red_button = XtCreateManagedWidget ( "RED", xcuButtonWidgetClass, bank_tbl, NULL, NULL ) ;
.br
				grn_button = XtCreateManagedWidget ( "GRN", xcuButtonWidgetClass, bank_tbl, NULL, NULL ) ;
.br
				blu_button = XtCreateManagedWidget ( "BLU", xcuButtonWidgetClass, bank_tbl, NULL, NULL ) ;
.br
					XtSetArg ( arg, XtNbmgrType, XcuBMGR_ANY_OF_MANY ) ;
.br
					bank_bmgr = XtCreateManagedWidget ( "bank bmgr", xcuBmgrWidgetClass, top_deck, &arg, ONE ) ;
.br
					managed_buttons[0] = red_button ;
.br
					managed_values[0] = (caddr_t) 0 ;
.br
					managed_buttons[1] = grn_button ;
.br
					managed_values[1] = (caddr_t) 1 ;
.br
					managed_buttons[2] = blu_button ;
.br
					managed_values[2] = (caddr_t) 2 ;
.br
					XcuBmgrManage (bank_bmgr, managed_buttons, managed_values, THREE ) ;
.br
			XtSetArg ( arg, XtNlabel, "File1" ) ; 
.br
			file_label = XtCreateManagedWidget ( "FILEX", lblWidgetClass, rotation_page, NULL, NULL ) ;
.br
		XtSetArg ( arg, XtNformatString, "c\\nc." ) ;
.br
		modification_page = XtCreateManagedWidget ( "modification page", xcuTblWidgetClass, pages_deck, &arg, ONE ) ;
.br
			files_tbl = XtCreateManagedWidget ( "files tbl", xcuTblWidgetClass, modification_page, NULL, NULL ) ;
.br
				XtSetArg ( arg, XtNset, True ) ;
.br
				file1_button = XtCreateManagedWidget ( "FILE1", xcuButtonWidgetClass, files_tbl, NULL, NULL ) ;
.br
				file2_button = XtCreateManagedWidget ( "FILE2", xcuButtonWidgetClass, files_tbl, NULL, NULL ) ;
.br
				file3_button = XtCreateManagedWidget ( "FILE3", xcuButtonWidgetClass, files_tbl, NULL, NULL ) ;
.br
					XtSetArg ( arg, XtNbmgrType, XcuBMGR_ONE_OF_MANY ) ;
.br
					files_bmgr = XtCreateManagedWidget ( "files bmgr", xcuBmgrWidgetClass, top_deck, &arg, ONE ) ;
.br
					managed_buttons[0] = file1_button ;
.br
					managed_values[0] = (caddr_t) "FILE1" ;
.br
					managed_buttons[1] = file2_button ;
.br
					managed_values[1] = (caddr_t) "FILE2" ;
.br
					managed_buttons[2] = file3_button ;
.br
					managed_values[2] = (caddr_t) "FILE3" ;
.br
					XcuBmgrManage (files_bmgr, managed_buttons, managed_values, THREE ) ;
.br
			negative_button = XtCreateManagedWidget ( "NEGATIVE", xcuButtonWidgetClass, modification_page, NULL, NULL ) ;
.br
				XtSetArg ( arg, XtNbmgrType, XcuBMGR_SINGLE_TOGGLE ) ;
.br
				negative_bmgr = XtCreateManagedWidget ( "negative bmgr", xcuBmgrWidgetClass, top_deck, &arg, ONE ) ;
.br
				managed_buttons[0] = negative_button ;
.br
				managed_values[0] = (caddr_t) "NEGATIVE" ;
.br
				XcuBmgrManage ( negative_bmgr, managed_buttons, managed_values, ONE ) ;
.br
		permanent_tbl = XtCreateManagedWidget ( "permanent", xcuTblWidgetClass, outer_tbl, NULL, NULL ) ;
.br
			modpage_button = XtCreateManagedWidget ( "MODIFICATION PAGE", xcuButtonWidgetClass, permanent_tbl, NULL, NULL ) ;
.br
			XtSetArg ( arg, XtNset, True ) ;
.br
			rotpage_button = XtCreateManagedWidget ( "ROTATION PAGE", xcuButtonWidgetClass, permanent_tbl, NULL, NULL ) ;
.br
				XtSetArg ( arg, XtNbmgrType, XcuBMGR_DOUBLE_TOGGLE ) ;
.br
				permanent_bmgr = XtCreateManagedWidget ( "permanent bmgr", xcuBmgrWidgetClass, top_deck, &arg, ONE ) ;
.br
				managed_buttons[0] = modpage_button ;
.br
				managed_values[0] = (caddr_t) "MODIFICATION PAGE" ;
.br
				managed_buttons[1] = rotpage_button ;
.br
				managed_values[1] = (caddr_t) "ROTATION PAGE" ;
.br
				XcuBmgrManage (files_bmgr, managed_buttons, managed_values, TWO ) ;

XtAddCallback (permanent_bmgr, XtNsetCallback, page_changer, NULL ) ;
.br
XtAddCallback (files_bmgr, XtNsetCallback, label_changer, NULL ) ;
.br
XtAddCallback (switch_bmgr, XtNsetCallback, switch_changer, NULL ) ;
.br
 ...
.br
static void
.br
label_changer ( w, client, call )
.br
    Widget w ;
.br
    caddr_t client ;
.br
    caddr_t call ;
.br
{
.br
Arg arg ;
.br
if (strcmp("FILE1", (String) call) == 0) {
.br
    XtSetArg ( arg, XtNlabel, "File1" ) ;
.br
    XtSetValues ( file_label, &arg, ONE ) ;
.br
    }
.br
else if (strcmp("FILE2", (String) call) == 0) {
.br
    XtSetArg ( arg, XtNlabel, "File2" ) ;
.br
    XtSetValues ( file_label, &arg, ONE ) ;
.br
    }
.br
else if (strcmp("FILE3", (String) call) == 0) {
.br
    XtSetArg ( arg, XtNlabel, "File3" ) ;
.br
    XtSetValues ( file_label, &arg, ONE ) ;
.br
    }
.br
return ;
.br
}
.br
.sp
static void
.br
page_changer ( w, client, call )
.br
    Widget w ;
.br
    caddr_t client ;
.br
    caddr_t call ;
.br
{
.br
if (strcmp("MODIFICATION PAGE", (String) call) == 0)
.br
    RaiseWidget (pages_deck, modification_page ) ;
.br
else
.br
    RaiseWidget (pages_deck, rotation_page ) ;
.br
}
.sp
static void
.br
switch_changer (w, client, call )
.br
    Widget w ;
.br
    caddr_t client ;
.br
    caddr_t call ;
.br
{
.br
on_switch = (Boolean) call ;
.br
}
.ps +1
.sp
I could rest my case here, I think.
The code without the \fBXcuWlm\fR is significantly longer, and to figure out
what it does without a lot of comments is next to impossible, whereas the
WLDL is compact and much more intelligible.
but perhaps the best is yet to come.
Consider the implications of making a slight change in the menu.
Without the \fBXcuWlm\fR, the application source code would have to be edited,
recompiled, and relinked.
With the \fBXcuWlm\fR, only the WLDL needs to be edited.
This convenience enables very rapid prototyping of menus.

