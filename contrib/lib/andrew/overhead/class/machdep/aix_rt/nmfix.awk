## ###################################################################### ##
##         Copyright IBM Corporation 1988,1991 - All Rights Reserved      ##
##        For full copyright information see:'andrew/config/COPYRITE'     ##
## ###################################################################### ##

# This awk program will covert the output from the AIX "nm -ex" command to
# a form which resembles the output from the 4.3 "nm" command.  It is currently
# only used on AIX systems.

$1 == "Symbols" {
	split($3, NameArray,"[");
	split(NameArray[2], FileName, "]");
	ElementName = NameArray[1] ":" FileName[1] FileName[2];
};
!($1 == "" || $1 == "Symbols" || $1 == "Name" || NF == 0) { 
	if ($5 == "|.data") Type = "D";
	else {
		if ($5 == "|.text") Type= "T";
		else {
			if ($5 == "|.bss") Type= "B";
			else {
				if ($5 == "|") Type = "U";
				else {
					Type = "U";
					print "Unknown type:", $5, "."|"cat>& 2";
				}
			}
		}
	}
	Value = substr($2, 4, 8);
	print ElementName Value, Type, $1	;
};

