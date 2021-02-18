\begindata{text,268687040}
\textdsversion{12}
\template{default}
\majorheading{Link User Documentation

}\center{How to use Linked Documents}


\quotation{\center{Abstract}

As a link user, you can easily navigate a set of Linked documents by using 
your left mouse button.

}
\heading{What is a Link?}

A link is an ATK Inset.  Hypermedia Developers place Links in documents to 
help you locate additional information (if you'd like information on making 
your own Linked documents, see 
\begindata{link,268829692}
Datastream version: 1
Link Developer
$DESTDIR/Developr.d
andysans8b
\enddata{link,268829692}
\view{linkv,268829692,10,0,0}).  By default, a link looks like a pushbutton. 
 However, you may set your "\typewriter{linkstyle}" preference to "0", which 
makes a link look like regular text, typically in \bold{Boldface} (although it 
may not be), (see 
\begindata{link,268707156}
Datastream version: 1
Link Preferences
$DESTDIR/Prefs.d
andysans8b
\enddata{link,268707156}
\view{linkview,268707156,11,0,0}).  The Hypermedia Developer will usually give 
some kind of contextual indication that there is a link here, like "see 
such-and-such".  Also, the mouse cursor will change from a North-West arrow to 
a crosshairs sight cursor.


\heading{How do I use a Link?}

If you would like to follow the Link, just press your left mouse button over 
the link inset (i.e., anywhere in the region where the target cursor is 
visible).  The Link inset will then bring up a window with the referenced 
document in it, and your mouse pointer will be warped to that window.  The 
original source document will remain in its window.


\heading{What else should I know?}

\description{To remove a window brought up by a link, remember to use "^X^D" 
(delete window), and not "^X^C" (quit application).


When a Link has the input focus, typing to it will cause the "Unkown command" 
message to appear.  If you want to type in your document, simply use the mouse 
to select a new insertion point.


Sometimes a Link inset won't have a target document.  In this case, pressing 
on the link will cause the message "No link." to be displayed.  It will also 
give the input focus to the link (see previous note).


There is a Link menu, which is meant for Hypermedia Developers.  You should 
ignore it.  If you accidentally select one of these menu items, "^G" should 
get you out of it.

}
\heading{Where can I find out more?}

The overview document for Link is 
\begindata{link,268977524}
Datastream version: 1
Link
$DESTDIR/Link.d
andy12b
\enddata{link,268977524}
\view{linkv,268977524,12,0,0}.

\enddata{text,268687040}
