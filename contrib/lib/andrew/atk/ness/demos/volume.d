\begindata{text, 269590664}
\textdsversion{12}
\template{default}
This demo file shows adjusting the volume logarithmically.


 \
\begindata{cel,269640164}
\V 2
\begindata{value,269637164}
>15
\enddata{value,269637164}
10 269637164 1 0 0 0 
>OBJ< value
>VIEW< sliderV
>REF< volume
\begindata{text, 269611648}
\textdsversion{12}
[long] <min_value> (01)

[long] <max_value> (60)

[long] <bodyfont-size> (8)

[string] <bodyfont> (andysans)

[string] <label> (Volume)

\enddata{text,269611648}
\enddata{cel,269640164}
\view{celview,269640164,21,0,181} 
      \
\begindata{cel,269611316}
\V 2
\begindata{value,269632460}
>349
\enddata{value,269632460}
10 269632460 1 0 0 0 
>OBJ< value
>VIEW< bargraphV
>REF< speaker
\begindata{text, 269611136}
\textdsversion{12}
[long] <min_value> (0)

[long] <max_value> (409)

[long] <bodyfont-size> (8)

[string] <bodyfont> (andysans)

[string] <label> (Speaker)

\enddata{text,269611136}
\enddata{cel,269611316}
\view{celview,269611316,22,0,183} 
    \
\begindata{cel,270000028}
\V 2
10 269632460 1 0 0 0 
>OBJ< value
>VIEW< sliderV
>REF< speakerknob
>LINK< speaker
\begindata{text, 269998952}
\textdsversion{12}
[long] <min_value> (-100)

[long] <max_value> (600)

[long] <bodyfont-size> (8)

[string] <bodyfont> (andysans)

[string] <label> (Speaker Knob)

\enddata{text,269998952}
\enddata{cel,270000028}
\view{celview,270000028,28,81,185}


Speaker and Speaker Knob share the same data object, so they always have the 
same values.


\begindata{ness, 269631808}
\textdsversion{12}
\template{default}
\italic{extend} \bold{"volume"}

	\italic{on mouse} \bold{"any"}

		dohit(currentinset, mouseaction, mousex, mousey);

		if mouseaction = mouseleftup then

			integer volume;

			volume := value_GetValue(currentinset);

			volume := round(100.0 * log(float(volume)));

			value_setValue(inset("speaker"), volume);

		end if;

	\italic{end mouse}

\italic{end extend}


function main()

	writefile("/tmp/t", "\bold{Now} is the \bigger{\bigger{\underline{season}}} 
to be \italic{Jolly}.");

end function;\
\enddata{ness,269631808}
\view{nessview,269631808,23,0,0\
}\enddata{text,269590664}
