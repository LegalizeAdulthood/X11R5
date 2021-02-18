##
# $XConsortium: pexRTab.awk,v 5.1 91/02/16 09:32:10 rws Exp $
###########################################################################
## Copyright 1990, 1991 by Sun Microsystems, Inc. and the X Consortium.
## 
##                         All Rights Reserved
## 
## Permission to use, copy, modify, and distribute this software and its 
## documentation for any purpose and without fee is hereby granted, 
## provided that the above copyright notice appear in all copies and that
## both that copyright notice and this permission notice appear in 
## supporting documentation, and that the names of Sun Microsystems,
## the X Consortium, and MIT not be used in advertising or publicity 
## pertaining to distribution of the software without specific, written 
## prior permission.  
## 
## SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
## INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
## SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
## DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
## WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
## ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
## SOFTWARE.
## 
###########################################################################
## Usage:
##	rm -f temp.dat
##	awk -f pex_oc_table.awk <pex-include-path>/PEX.h > <output_file>
##
BEGIN { num=0; i=0; gap_begin=0; val=0; gap=0; j=0
    print "/* Automatically generated OC table"
    print " */"
    print "/******************************************************************"
    print "Copyright 1990, 1991 by Sun Microsystems, Inc. and the X Consortium."
    print ""
    print "                        All Rights Reserved"
    print ""
    print "Permission to use, copy, modify, and distribute this software and its "
    print "documentation for any purpose and without fee is hereby granted, "
    print "provided that the above copyright notice appear in all copies and that"
    print "both that copyright notice and this permission notice appear in "
    print "supporting documentation, and that the names of Sun Microsystems,"
    print "the X Consortium, and MIT not be used in advertising or publicity "
    print "pertaining to distribution of the software without specific, written "
    print "prior permission.  "
    print ""
    print "SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, "
    print "INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT "
    print "SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL "
    print "DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,"
    print "WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,"
    print "ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS"
    print "SOFTWARE."
    print ""
    print "******************************************************************/"
    }
##
## Look only for lines starting with PEXOC (in PEX.h).
## Name (minus PEXOC) is saved in array for printing at the end
## (signalled by reaching "PEXMaxOC").  This allows us to do the
## extern declarations and the table in one pass.
##
 $2 == "PEX_GetExtensionInfo" {
    str[num] = ""
    num++ }
 num > 0 {
    if ($1 == "#define") {
	if ($2 == "PEXMaxRequest") {
	    print "static char *pex_req_name [] = {"
	    for (i=0; i<(num-1); i++) {	print "\t\"" str[i] "\"," }
	    print "\t\"" str[i] "\""
	    print "};\n\n" }
	else if (index($2,"PEX_") == 1) {
	    str[num] = substr($2,5,(length($2)-4))
	    val = $3
	    if ((gap_begin != 0) && (gap == 0)) { gap = val - gap_begin - 1  }
	    num++ }
    } }
##
##
END { }
