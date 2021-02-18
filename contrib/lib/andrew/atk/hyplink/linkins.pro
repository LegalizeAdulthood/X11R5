\begindata{text,268720652}
\textdsversion{12}
\template{help}
\chapter{Link Inset:  A Static Hyperlink

}
\section{What Link is

}\leftindent{
The \bold{link} inset is a button that resides in the text of a document. It 
``links'' one document to another: a new window containing a target document 
is created when the link inset is clicked on with the left mouse button.  Link 
is a basic hypermedia building block.


Use Delete Window (\bold{Ctrl-X Ctrl-D}) to remove the new window.  See also 
\bold{Warnings}.

}
\section{Starting Link

}\leftindent{
You can ``follow'' a link by left-clicking on the link inset.  By default, a 
link inset looks like a shaded button, but its appearance may be changed with 
the linkstyle preference.  See the  \italic{preferences}  help document for 
more information on setting preferences.


You insert a link in a document in the same manner as any other inset.  Type 
ESC-Tab, and answer ``link'' to the prompt ``Data object to insert here''. 
 See the  \italic{inset } help document for more information on inserting 
insets into documents.

}
\section{Warnings

}\leftindent{
Following a link results in the creation of a new window, with the target of 
the link in that new window.  Note that with some window managers (e.g.:  twm 
on X-Windows), the new window may appear directly on top of the window in 
which you invoked the link--just move the new window to see the old one.


Also, note that \bold{\italic{to remove a window}} created by following a 
link, \bold{\italic{do \underline{not}}} use Quit (either from the menu or a 
keystroke sequence, most likely bound to Ctrl-X Ctrl-C), or ``Zap'' the 
window, as this will quit the application (EZ, Help, Messages, etc.) from 
which you invoked the link.  You want to use \bold{Delete Window} (usually 
bound to the keystroke sequence \bold{Ctrl-X Ctrl-D}).

}
\section{Pop-up menu meanings

}
\leftindent{Link provides only one menu card, entitled Link, for configuring 
links.  The menu card becomes visible when the link is selected.  You can 
select a link without bringing up the target by right-mouse-clicking on the 
link inset.  These are the options:


\bold{Autolink Target Here}:  Select this menu option from the target window, 
after selecting ``\bold{Autolink''} on the source link.


\bold{Autolink}:  Select this menu option on the source link, and then specify 
the target window with the ``\bold{Autolink Target Here''} menu option.  If 
you can't find ``\bold{Autolink Target Here''}, you need to modify your 
.ezinit file.


\bold{Set Link}:  Specify the name of the target file for this link manually 
(assisted by the completion package).


Because link is a subclass of the pushbutton inset, you may use the Pushbutton 
menu to program link as well.

}
\section{How Link works, Using Link, Link Concepts, The default Link, 
Preferences, Programming Link

}\leftindent{
For more information on these topics, 
\begindata{link,268728840}
Datastream version: 1
Press Me
$DESTDIR/Link.d
andysans8b
\enddata{link,268728840}
\view{linkview,268728840,0,0,0}.

}
\section{Program Author}

\leftindent{
Michael McInerny, ITC (address mail to mcinerny+@andrew.cmu.edu).}


\section{Related tools}  


Select (highlight) one of the italicized names and choose "Show Help on 
Selected Word" from the pop-up menu to see the help file for:


\leftindent{\italic{pushbutton

insets

ez}

}\enddata{text,268720652}
