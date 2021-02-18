\begindata{text,268687040}
\textdsversion{12}
\template{default}
\majorheading{Link Preferences

}\center{Changing the Link Appearance

}
\quotation{\center{Abstract}

You can select one of two "look-and-feels" for a Link inset.

}
\heading{The Link Style Preference

}Link currently only supports one preference, \typewriter{linkstyle}, which 
should be an integer, either 0 or 2.  Set \typewriter{linkstyle} to 0 if you 
want links to look like regular, in-line text.  A \typewriter{linkstyle} of 2 
is the default, and causes Links to look like three-dimensional buttons.


You can change link styles on a global or per-application basis only.  You 
cannot, for instance, change the way links appear on a document or per-link 
basis.


To make a global change, put the following line in your preferences file:

\example{*.linkstyle: \italic{desired-style}

}where \italic{desired-style} is either 0 or 2.


To make a per-application change, put a line of the form:

\example{\italic{application}.linkstyle: \italic{desired-style}

}where \italic{application} is the name of the application (ez, table, 
\italic{etc}.) and \italic{desired-style} is again either 0 or 2.



\heading{Where can I find out more?}

The overview document for Link is 
\begindata{link,268988628}
Datastream version: 1
Link
$DESTDIR/Link.d
andy12b
\enddata{link,268988628}
\view{linkv,268988628,13,0,0}.

\enddata{text,268687040}
