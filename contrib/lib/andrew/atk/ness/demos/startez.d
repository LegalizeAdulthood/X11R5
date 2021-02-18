\begindata{text,270098528}
\textdsversion{12}
\template{default}


This file shows how to start an editor (or other program) from Ness.


\begindata{cel,270292884}
\V 2
\begindata{value,270292816}
>1
\enddata{value,270292816}
10 270292816 1 0 0 0 
>OBJ< value
>VIEW< buttonV
>REF< Button
\begindata{text,270289464}
\textdsversion{12}
[long] <bodyfont-size> ()

[string] <bodyfont> ()

[string] <label> (Start ez)

\enddata{text,270289464}
\enddata{cel,270292884}
\view{celview,270292884,25,73,24}

\begindata{ness,270120952}
\origin{00\1 Jun 1989 at 14:28:28 EDT\wjh:  Fred Hansen\}
\template{default}
\define{fullwidth
menu:[Justify,Full Width]
attr:[LeftMargin LeftMargin Cm -25431]
attr:[RightMargin RightMargin Cm -27743]}
\define{sans
menu:[Font,Sans]
attr:[FontFamily AndySans Int 0]}


extend "Button" on mouse "any"

	marker andy;

	if mouseaction = mouseleftup then

		andy := system("echo $$ANDREWDIR");

		if first(andy) /= "/" then 

			andy := "/usr/andrew";

		end if;

		TellUser("Wait while I start an editor process");

		im_ForceUpdate();		-- make message appear 

		system(" ez " ~ andy 

			~ "/lib/ness/demos/Birth.db "

			~ " </dev/null  >/dev/null 2>&1 &");

		TellUser("Editor will appear shortly");

	end if;

end mouse end extend

\enddata{ness,270120952}
\view{nessview,270120952,24,0,0\
}\enddata{text,270098528}
