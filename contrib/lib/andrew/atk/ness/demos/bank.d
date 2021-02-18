\begindata{arbiter,270835604}
\V 2
\begindata{text,270869572}
\textdsversion{12}
\template{default}
\define{plus
}
\define{upbase
menu:[Font~1,UpBase]
attr:[Script PreviousScriptMovement Point -15]}
\plus{


\begindata{lset,270924816}
\V 1
\begindata{lset,270551156}
\V 1
\begindata{lset,270863840}
\V 1
\begindata{cel,270884692}
\V 2
\begindata{value,269487740}
>137
\enddata{value,269487740}
10 269487740 1 0 0 0 
>OBJ< value
>VIEW< sliderV
>REF< Amount
\begindata{text,270863728}
\textdsversion{12}
[long] <min_value> (20)

[long] <max_value> (300)

[long] <bodyfont-size> (14)

[string] <bodyfont> (andysans)

[string] <label> (Amount)

\enddata{text,270863728}
\enddata{cel,270884692}
0 0 0 270884692 0 0 0
>OBJ< cel
>VIEW< celview
>REF< 
\enddata{lset,270863840}
\begindata{lset,270884464}
\V 1
\begindata{lset,270920852}
\V 1
\begindata{cel,270550992}
\V 2
\begindata{value,270553288}
>48
\enddata{value,270553288}
10 270553288 1 0 0 0 
>OBJ< value
>VIEW< thumbstrV
>REF< Months
>LINK< t
\begindata{text,270834920}
\textdsversion{12}
[long] <increment> (6)

[long] <min_value> (6)

[long] <max_value> (300)

[long] <bodyfont-size> (14)

[string] <bodyfont> (andysans)

[string] <label> (Months)

\enddata{text,270834920}
\enddata{cel,270550992}
0 0 0 270550992 0 0 0
>OBJ< cel
>VIEW< celview
>REF< 
\enddata{lset,270920852}
\begindata{lset,270920624}
\V 1
\begindata{cel,270550800}
\V 2
\begindata{value,270863580}
>12
\enddata{value,270863580}
10 270863580 1 0 0 0 
>OBJ< value
>VIEW< thumbstrV
>REF< Interest
>LINK< INK< t
\begindata{text,270884092}
\textdsversion{12}
[long] <increment> (1)

[long] <min_value> (5)

[long] <max_value> (20)

[long] <bodyfont-size> (14)

[string] <bodyfont> (andysans)

[string] <label> (Interest)

\enddata{text,270884092}
\enddata{cel,270550800}
0 0 0 270550800 0 0 0
>OBJ< cel
>VIEW< celview
>REF< 
\enddata{lset,270920624}
2 53 0 0 270920852 270920624 0
>OBJ< 
>VIEW< 
>REF< 
\enddata{lset,270884464}
2 50 0 0 270863840 270884464 0
>OBJ< 
>VIEW< 
>REF< 
\enddata{lset,270551156}
\begindata{lset,270920072}
\V 1
\begindata{cel,270550548}
\V 2
\begindata{text,270919960}
\textdsversion{12}
\template{default}
\define{upbase

attr:[Script PreviousScriptMovement Point -15]}
\leftindent{Dear Valued Customer,

Your application for a loan of\upbase{ 
\begindata{cel,270912180}
\V 2
\begindata{value,270912104}
>0
\enddata{value,270912104}
10 270912104 1 0 0 0 
>OBJ< value
>VIEW< stringV
>REF< ShowPrincipal
>LINK< 
\begindata{text,270911932}
\textdsversion{12}
[long] <bodyfont-size> (14)

[string] <bodyfont> (andysans)

[string] <label> (Principal)

\enddata{text,270911932}
\enddata{cel,270912180}
\view{celview,270912180,3,82,23} }dollars has been approved at a rate 
of\upbase{ 
\begindata{cel,270911548}
\V 2
\begindata{value,270911472}
>0
\enddata{value,270911472}
10 270911472 1 0 0 0 
>OBJ< value
>VIEW< stringV
>REF< ShowInterest
>LINK< 
\begindata{text,270911300}
\textdsversion{12}
[long] <bodyfont-size> (14)

[string] <bodyfont> (andysans)

[string] <label> (Interest)

\enddata{text,270911300}
\enddata{cel,270911548}
\view{celview,270911548,4,72,25} }with repayment over a period of\upbase{ 
\begindata{cel,270910676}
\V 2
\begindata{value,270920524}
>0
\enddata{value,270920524}
10 270920524 1 0 0 0 
>OBJ< value
>VIEW< stringV
>REF< ShowMonths
>LINK< 
\begindata{text,270910504}
\textdsversion{12}
[long] <bodyfont-size> (14)

[string] <bodyfont> (andysans)

[string] <label> (Months)

\enddata{text,270910504}
\enddata{cel,270910676}
\view{celview,270910676,5,78,24} }months.   Each month you will make a payment 
of 
\upbase{ \
\begindata{cel,270910120}
\V 2
\begindata{value,270910044}
>0
\enddata{value,270910044}
10 270910044 1 0 0 0 
>OBJ< value
>VIEW< stringV
>REF< Payment
>LINK< 
\begindata{text,270909848}
\textdsversion{12}
[long] <bodyfont-size> (14)

[string] <bodyfont> (andysans)

[string] <label> (Payment)

\enddata{text,270909848}
\enddata{cel,270910120}
\view{celview,270910120,6,86,27} }and the total repaid to the bank over the 
lifetime of the loan will be\upbase{ 
\begindata{cel,270908956}
\V 2
\begindata{value,270910928}
>0
\enddata{value,270910928}
10 270910928 1 0 0 0 
>OBJ< value
>VIEW< stringV
>REF< Total
>LINK< t
\begindata{text,270908760}
\textdsversion{12}
[long] <bodyfont-size> (14)

[string] <bodyfont> (andysans)

[string] <label> (Total Paid)

\enddata{text,270908760}
\enddata{cel,270908956}
\view{celview,270908956,7,96,26}}.



With best wishes,


Friendly National Bunk\
}\enddata{text,270919960}
0 270919960 0 0 0 0 
>OBJ< text
>VIEW< textview
>REF< text
\enddata{cel,270550548}
0 0 0 270550548 0 0 0
>OBJ< cel
>VIEW< celview
>REF< 
\enddata{lset,270920072}
1 85 0 0 270551156 270920072 0
>OBJ< 
>VIEW< 
>REF< 
\enddata{lset,270924816}
\view{lsetview,270924816,8,464,370}
\leftindent{
}
}
\begindata{ness,270908012}
\origin{00\\31 Jul 1990 at 17:17:12 EDT\\wjh:  Fred Hansen\\00}
\template{default}
\define{sans
menu:[Font,Sans]
attr:[FontFamily AndySans Int 0]}
--edit_dollars(v) convert to a string with two digits after decimal place

--

function edit_dollars(real v)

	marker a, c

	a := textimage(round(v * 100.0))

	c := extent(previous(last(a)), last(a))

	return extent(a, start(c)) ~ "." ~ c

end function


--ipow(i, n)

--	compute i\superscript{n}

--

real function ipow(real i, integer n)

	integer j

	real v

	j := 0

	v := 1.0

	while j < n do

		j := j + 1

		v := v * i

	end while

	return v

end function


--UpdateResults()

--	Store new value in results field

--

function UpdateResults()

	integer principal, months

	real monthlyInterest, payment, totalpaid, z


	if mouseaction /= mouseleftup then 

		exit function

	end if


-- The periodic payment for

--	q periods per year

--	n years

--	i annual interest

--	P principal

-- is

--	payment = iPz/(z-1),     where  z = (1 + i/q)\superscript{nq}


	principal := value_GetValue(inset("Amount"))

	months := value_GetValue(inset("Months"))

	monthlyInterest := float(value_GetValue(inset("Interest"))) / 100.0 / 12.0

		-- divide by 100 to convert from a percentage,

		-- and then divide by 12 to get monthly interest.


	z := ipow(1.0 + monthlyInterest, months)      -- Compute  z  

	payment := float(principal * 1000) * monthlyInterest * z / (z - 1.0) 


	totalpaid := payment * float(months)

	value_SetString(inset("Payment"), edit_dollars(payment))

	value_SetString(inset("Total"), edit_dollars(totalpaid))


	value_SetString(inset("ShowPrincipal"), textimage(principal * 1000))

	value_SetString(inset("ShowInterest"),

		textimage(value_GetValue(inset("Interest"))) ~ " %")

	value_SetString(inset("ShowMonths"), textimage(months))


end function



extend "Amount"

	on mouse "any"

		dohit (currentinset, mouseaction, mousex, mousey)

		UpdateResults()

	end mouse

end extend


extend "Interest"

	on mouse "any"

		dohit (currentinset, mouseaction, mousex, mousey)

		UpdateResults()

	end mouse

end extend


extend "Months"

	on mouse "any"

		dohit (currentinset, mouseaction, mousex, mousey)

		UpdateResults()

	end mouse

end extend\
\enddata{ness,270908012}
\view{nessview,270908012,9,0,486}

\enddata{text,270869572}
1 270869572 0 0 0 0 
>OBJ< text
>VIEW< textview
>REF< 
\enddata{arbiter,270835604}
