## ###################################################################### ##
##         Copyright IBM Corporation 1988,1991 - All Rights Reserved      ##
##        For full copyright information see:'andrew/config/COPYRITE'     ##
## ###################################################################### ##

BEGIN	{{OFS = ""; ORS = "\n";
	TransCount = 0;
	}}
{
	if (HaveMap[$1 ";" $2] == 0) {
		Mapping[$1] = Mapping[$1] " " $2;
		HaveMap[$1 ";" $2] = 1;
	}
	if (HaveNick[$1 ";" $3] == 0) {
		RealNick[$1] = RealNick[$1] "/" $3;
		HaveNick[$1 ";" $3] = 1;
	}
	if (HaveGiven[$2 ";" $4] == 0) {
		RealGiven[$2] = RealGiven[$2] "/" $4;
		HaveGiven[$2 ";" $4] = 1;
	}
}
END	{
	for (z in Mapping) {
		Nick = z " (" substr(RealNick[z], 2, length(RealNick[z]) - 1) ")	";
		GivStr = substr(Mapping[z], 2, length(Mapping[z]) - 1);
		n = split(GivStr, Givs, " ");
		for (i = 1; i <= n; ++i) {
		    Nick = Nick " " Givs[i] " (";
		    Nick = Nick substr(RealGiven[Givs[i]], 2, length(RealGiven[Givs[i]]) - 1) ")";
		}
		print Nick;
	}
	}
