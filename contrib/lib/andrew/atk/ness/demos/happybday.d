\begindata{text,270171368}
\textdsversion{12}
\template{default}
\define{sans
menu:[Font,Sans]
attr:[FontFamily AndySans Int 0]}


\center{\bigger{\bigger{\italic{\bigger{\bigger{\bigger{Happy Birthday}}}}}}}


\begindata{cel,270091164}
\V 2
\begindata{lset,270088972}
\V 1
\begindata{lset,270241040}
\V 1
\begindata{cel,270091048}
\V 2
\begindata{raster,270088888}
2 0 68266 68266 0 0 112 109
bits 270088888 112 109
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
l08m |
l08m |
l7em |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
k03e3e0l |
j03Ge3Gf0k |
jGc062g3fe0j |
i1fc0g62h7ej |
h01fch62h07c0	i |
h0780h62if0i |
h0cie3i1ci |
h18h016280h06	i |
h10h016280h03	i |
h38h018180h01	i |
h3ciGi0180h |
h36i2ai0380h |
h33m0680h |
h30c0l0880h |
h3070l3080h |
h301cle080h |
h300780j03!80	h |
h30gf8j1eg80h |
h30g1f80h01f8	g80h |
h30g01f8hbf40	g80h |
h30h3fHf8h80h |
h30h027ff2i80	h |
h30n80h |
h30n80h |
h30n80h |
h30n80h |
h30n80h |
h38n80h |
h3cm0180h |
h33m0380h |
h3180l0680h |
h30e0l0880h |
h303cl3080h |
h3007le080h |
h3001c0j03!80	h |
h30g78j1eg80h |
h30g0f80h03f8	g80h |
h30g01f8h7fh80	h |
h30h3fHf8h80h |
h30h037ffai80	h |
h30n80h |
h10n80h |
h18m0180h |
h0cm01i |
h07m03i |
h0180l06i |
ie0l08i |
i38l30i |
i0ele0i |
i03c0j0380i |
j78j16j |
j0e80h01f8j |
j01d0h5740j |
k3fHfck |
k016facl |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
\enddata{raster, 270088888}
0 270088888 0 0 0 0 
>OBJ< raster
>VIEW< rasterview
>REF< visible cake
>LINK< 4ÿÀÄ
\enddata{cel,270091048}
0 0 0 270091048 0 0 0
>OBJ< cel
>VIEW< celview
>REF< 
\enddata{lset,270241040}
\begindata{lset,270240652}
\V 1
\begindata{cel,270089248}
\V 2
\begindata{text,270240540}
\textdsversion{12}
\template{default}
\define{greeting
menu:[Font~1,Greeting]
attr:[FontFamily AndySans Int 0]
attr:[FontSize ConstantFontSize Point 18]}
\greeting{ 

 \
}\enddata{text,270240540}
0 270240540 0 0 0 0 
>OBJ< text
>VIEW< textview
>REF< song text
>LINK< cake_1
\enddata{cel,270089248}
0 0 0 270089248 0 0 0
>OBJ< cel
>VIEW< celview
>REF< 
\enddata{lset,270240652}
1 66 0 0 270241040 270240652 0
>OBJ< 
>VIEW< 
>REF< 
\enddata{lset,270088972}
0 270088972 0 0 0 0 
>OBJ< lset
>VIEW< lsetview
>REF< lset
>LINK< 
\enddata{cel,270091164}
\view{celview,270091164,12,455,160}


\italic{Empower the Ness (below) and click on the cake for Birthday greetings. 
 

For best results, use an IBM RT/PC workstation.}


\begindata{cel,270271572}
\V 2
\begindata{ness,270271312}
\origin{00\\5 Jan 1990 at 16:09:23 EST\\wjh:  Fred Hansen\\00}
\template{default}
\define{fullwidth
menu:[Justify,Full Width]
attr:[LeftMargin LeftMargin Cm -25431]
attr:[RightMargin RightMargin Cm -27743]}
\define{sans
menu:[Font,Sans]
attr:[FontFamily AndySans Int 0]}
\define{greeting
menu:[Font~1,Greeting]
attr:[FontFamily AndySans Int 0]
attr:[FontSize ConstantFontSize Point 18]}


-- \bigger{\italic{Happy Birthday to Ness}} ! 


 

function showcake(object cake)

	raster_copy_subraster(cake)

	raster_select_entire(inset("visible cake"))

	raster_replace_subraster(inset("visible cake"))

	raster_center_image(inset("visible cake"))

end function


boolean lit:= False	-- the file is saved with the unlit cake above


marker Cakes := 
"\
\begindata{raster,270261204}
2 0 68266 68266 0 0 112 109
bits 270261204 112 109
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
l08m |
l08m |
l7em |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
k03e3e0l |
j03Ge3Gf0k |
jGc062g3fe0j |
i1fc0g62h7ej |
h01fch62h07c0	i |
h0780h62if0i |
h0cie3i1ci |
h18h016280h06	i |
h10h016280h03	i |
h38h018180h01	i |
h3ciGi0180h |
h36i2ai0380h |
h33m0680h |
h30c0l0880h |
h3070l3080h |
h301cle080h |
h300780j03!80	h |
h30gf8j1eg80h |
h30g1f80h01f8	g80h |
h30g01f8hbf40	g80h |
h30h3fHf8h80h |
h30h027ff2i80	h |
h30n80h |
h30n80h |
h30n80h |
h30n80h |
h30n80h |
h38n80h |
h3cm0180h |
h33m0380h |
h3180l0680h |
h30e0l0880h |
h303cl3080h |
h3007le080h |
h3001c0j03!80	h |
h30g78j1eg80h |
h30g0f80h03f8	g80h |
h30g01f8h7fh80	h |
h30h3fHf8h80h |
h30h037ffai80	h |
h30n80h |
h10n80h |
h18m0180h |
h0cm01i |
h07m03i |
h0180l06i |
ie0l08i |
i38l30i |
i0ele0i |
i03c0j0380i |
j78j16j |
j0e80h01f8j |
j01d0h5740j |
k3fHfck |
k016facl |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
\enddata{raster, 270261204}
\view{rasterview,270261204,13,0,0} 
 \
\begindata{raster,270299732}
2 0 68266 68266 0 0 112 109
bits 270299732 112 109
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
l38m |
l16m |
l09m |
l0480l |
l0480l |
l0a40l |
l0a40l |
l1240l |
l2240l |
l4440l |
l4480l |
l8880l |
l89m |
l92m |
l92m |
l54m |
l4cm |
l28m |
l18m |
l14m |
l7em |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
l62m |
k03e3e0l |
j03Ge3Gf0k |
jGc062g3fe0j |
i1fc0g62h7ej |
h01fch62h07c0	i |
h0780h62if0i |
h0cie3i1ci |
h18h016280h06	i |
h10h016280h03	i |
h38h018180h01	i |
h3ciGi0180h |
h36i2ai0380h |
h33m0680h |
h30c0l0880h |
h3070l3080h |
h301cle080h |
h300780j03!80	h |
h30gf8j1eg80h |
h30g1f80h01f8	g80h |
h30g01f8hbf40	g80h |
h30h3fHf8h80h |
h30h027ff2i80	h |
h30n80h |
h30n80h |
h30n80h |
h30n80h |
h30n80h |
h38n80h |
h3cm0180h |
h33m0380h |
h3180l0680h |
h30e0l0880h |
h303cl3080h |
h3007le080h |
h3001c0j03!80	h |
h30g78j1eg80h |
h30g0f80h03f8	g80h |
h30g01f8h7fh80	h |
h30h3fHf8h80h |
h30h037ffai80	h |
h30n80h |
h10n80h |
h18m0180h |
h0cm01i |
h07m03i |
h0180l06i |
ie0l08i |
i38l30i |
i0ele0i |
i03c0j0380i |
j78j16j |
j0e80h01f8j |
j01d0h5740j |
k3fHfck |
k016facl |
t |
t |
t |
t |
t |
t |
t |
t |
t |
t |
\enddata{raster, 270299732}
\view{rasterview,270299732,14,0,0} "



extend "visible cake"   on mouse "any"

	if mouseaction = mouseleftup then

		lit := not lit

		if lit then

			showcake(FirstObject(second(Cakes)))

			sing()

		else

			showcake(FirstObject(Cakes))

			replace(base(currentselection(inset("song text"))),

				"\greeting{ \\n }")

		end if

	end if

end mouse   end extend


function sing()

	marker m := second(base(currentselection(inset("song text"))))

	m := last(replace(m, "\\n\\n    \greeting{Happy birthday to you}\\n"))

	im_ForceUpdate()

	play_notes("L7 CC L4 DCF    E P4")

	m := last(replace(m, "\\n    \greeting{Happy birthday to you}\\n"))

	play_notes("L7 CC L4 DCG   F P4")

	im_ForceUpdate()

	m := last(replace(m, "\\n    \greeting{Happy birthday,}\\n\\t"

			~ "\greeting{Dear Nessie}\\n"))

	play_notes("L7 CC L4 >C <A  FED P4")

	im_ForceUpdate()

	m := last(replace(m, "\\n    \greeting{Happy birthday to you}"))

	play_notes("L7 A#A# L4 AF G  F.") 

end function\
\enddata{ness,270271312}
0 270271312 0 0 0 0 
>OBJ< ness
>VIEW< nessview
>REF< ness
>LINK< 
\enddata{cel,270271572}
\view{celview,270271572,15,528,211}

\enddata{text,270171368}
