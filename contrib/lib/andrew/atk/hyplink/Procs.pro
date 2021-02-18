\begindata{text,268720652}
\textdsversion{12}
\template{default}
\majorheading{Link Procedure Table Entries}

\center{Link and your .ezinit file}


\quotation{\center{Abstract}

Link defines several proctable entries.  Only one, linkview-set-target, is 
intended for use in your .ezinit file.  The rest are used by Link itself (and 
posted on its own menu), but you could bind keys to them, or use them in 
compound procedures.

}
\heading{Modifying your .ezinit file}

You will probably want to modify your .ezinit file if you want to develop 
Linked documents, so that autolinking will work.  You don't need to do this, 
as you can make links without autolink, but this will make development easier.


Basically, in order for Autolinking to work properly, you will need a way of 
executing the "linkview-set-target" procedure in the target frame (window). 
 One way to do this, is to add the following line to your ~/.ezinit file 
(please read the help document on Init files before attempting this 
operation):

\example{addmenu linkview-set-target "Link,Autolink target here" frame

}
\heading{Proctable Entries}

These are the five proctable entries defined by the Link inset.


\bold{Name:} \typewriter{\smaller{ linkview-set-target}}

\bold{Type of object}:\typewriter{\smaller{ frame}}

\bold{Documentation string}: \typewriter{\smaller{Execute this proc from the 
frame of the the buffer for the target file of a link.  To be called after 
linkview-autolink.}}

\bold{Comments:}  Usually bound to a menu item reading something like 
"Autolink target here".  Execution of this procedure after starting an 
Autolink with "linkview-autolink" (see next proc) completes the autolink 
procedure by indicating to the source link that this frame contains a buffer 
on a file which is to be the target.


\bold{Name}: \typewriter{\smaller{linkview-autolink}}

\bold{Type of object}: \typewriter{\smaller{linkview}}

\bold{Documentation string}: \typewriter{\smaller{Starts the autolink process. 
 Waits for linkview-set-target to be invoked, which tells this link what file 
to link to.}}

\bold{Comments}:  This item will be bound to the Link card's "Autolink" item. 
 Selecting this item will start the autolinking procedure.  You will need to 
invoke \typewriter{\smaller{linkview-set-target}} in order to complete the 
autolink procedure, which is preferably bound to "Autolink target here" on the 
Link menu card.


\bold{Name}: \typewriter{\smaller{linkview-set-link}}

\bold{Type of object}:\typewriter{\smaller{ linkview}}

\bold{Documentation string}: \typewriter{\smaller{Prompts for user to set 
target filename of the link button.}}

\bold{Comments}:  The Link inset binds this proc entry to the Link card's "Set 
Link" entry.


\heading{Where can I find out more?}

The overview document for Link is 
\begindata{link,268748956}
Datastream version: 1
Link
$DESTDIR/Link.d
andy12b
\enddata{link,268748956}
\view{linkv,268748956,0,0,0}.

\enddata{text,268720652}
