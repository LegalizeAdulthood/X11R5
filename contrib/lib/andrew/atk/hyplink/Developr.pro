\begindata{text,268720648}
\textdsversion{12}
\template{default}
\majorheading{Link Developer Documentation}

\center{How to Make Linked Documents}


\quotation{\center{Abstract}

The Link inset provides a simple, fast way of building a set of 
cross-referenced hypermedia documents.  Using a Link inset requires no 
programming and can be done by anyone.

}
\heading{Why do I want to use Links?}

By using Linked documents, you, the document developer, can tie together large 
amounts of information which can be conveniently navigated by users of your 
document (for a description of the user interface, see 
\begindata{link,268747656}
Datastream version: 1
Link Help
$DESTDIR/Help.d
andysans8b
\enddata{link,268747656}
\view{linkv,268747656,0,0,0}).  Links also give you new freedom in structuring 
your documents, because with links, you can break large pieces of information 
into smaller, more manageable chunks which cross-reference each other.


Links are a file-reference tool.  They do not support offsets into documents, 
and so it is recommended that links be used to help users find whole 
documents, rather than pieces of it. (For instance, Links wouldn't be useful 
for producing a table of contents warp for a large document.  However, if that 
document is broken up into chapters and sections, then a table of contents 
link document could be used to reference each of the pieces of the document.)


\heading{How do I insert a Link?}

\quotation{Note:  The following procedure assumes that you have customized 
your ".ezinit" file to allow for Autolinking.  If you haven't, please see 
\begindata{link,268740664}
Datastream version: 1
Link Procs
$DESTDIR/Procs.d
andysans8b
\enddata{link,268740664}
\view{linkv,268740664,1,0,0} before continuing.}

Links are inserted just like any other inset.  For instance, most ATK objects 
capable of holding insets (like text) use "Esc-Tab" to specify the inset.  The 
name of the Link dataobject is "link", so, to insert a link in a text 
document, you would:

\indent{\description{- select an insertion point (where you want the Link 
inset to appear)

- press the "Esc" key and then the "Tab" key

- type "link" and press the "Enter" key}}

At this point, you should see an empty Link inset (which has no target, and no 
label, displaying itself as 
"\
\begindata{link,268768672}
Datastream version: 1
Link (v30)

andy12b
\enddata{link,268768672}
\view{linkview,268768672,2,0,0}").  The easiest way to specify the target (and 
a label for the link) is to use the "Autolink" procedure.  To do this, with 
the Link inset selected:

\indent{\description{- press the menu button on your mouse (the middle button 
on a three-button mouse or the left and right buttons together on a two button 
mouse) and choose "Autolink" from the menu.

- move the mouse to the window of the file you want to link to, and bring up 
the menus again.  Choose "Autolink target here".}}

At this point, the Link inset will change its name to the filename of the 
target document, and it will be ready for use as a Link.  To test it, just 
click your left mouse button over the link inset, and you should be warped to 
the target document.  You can insert as many links as you want into a 
document, and you can link to any other document (even system files)--you can 
even link to the same document the link is in (although it's of dubious worth, 
since there are no offests).


\heading{How do I change a Link?}

You can autolink again, but it won't change the label.  The reason for this is 
that you may want to set your label manually before autolinking, and the 
autolink procedure won't change an already defined label.  You can change the 
label manually using the Set Label menu item (on the Pushbutton card), which 
will prompt you for a new text string.  You can also change the font, using 
the Set Font menu item.  Finally, you can set the link manually, using the Set 
Link menu item, which uses the "completion" package to help you specify a 
filename.  The Set Link procedure will also set the label of the link if there 
isn't already one defined (like autolink).


\heading{How do I remove a Link?}

You can remove a Link inset like any other inset:  just select it and choose 
the cut option from the menu, or backspace over it (and confirm the dialog 
prompt).


\heading{Caveats}

\description{You shouldn't put Links (or any other inset or style) into system 
documents (like programs and init files).  This will cause the document to 
become unreadable by the system.

Don't change the name of a link to the empty string.  It will become invisible 
and very hard to select again.  You will probably need to backspace over it.}


\heading{How can I find out more?}

You can use the overview document 
\begindata{link,268768468}
Datastream version: 1
Link
$DESTDIR/Link.d
andy12b
\enddata{link,268768468}
\view{linkv,268768468,3,0,0} to find out more about Links.

\enddata{text,268720648}
