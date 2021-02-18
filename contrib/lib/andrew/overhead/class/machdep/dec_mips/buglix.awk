## ###################################################################### ##
##         Copyright IBM Corporation 1988,1991 - All Rights Reserved      ##
##        For full copyright information see:'andrew/config/COPYRITE'     ##
## ###################################################################### ##

# Note: This only works if more than one library is passed to nm
# Usage: nm -g lib1 lib2 lib3... | awk -f filename
# Ouput - similar to nm -og lib1 lib2 lib3
# Differences: hidden 15th character in objname shown
BEGIN {
	lib=""
	obj=""
}
/^[^ \t]*\.a:/ { # print "library", $1; 
		lib=$1 ;
		obj="" ;
		next}
/^[^ \t]*\:/ { # print "object", $1; 
		 obj=$1; 
		next}
(NF != 0) {print lib obj "\t" $0}
