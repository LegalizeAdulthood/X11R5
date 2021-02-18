## ###################################################################### ##
##         Copyright IBM Corporation 1988,1991 - All Rights Reserved      ##
##        For full copyright information see:'andrew/config/COPYRITE'     ##
## ###################################################################### ##

# awk script to list all modules and entry points from a library which are
# needed (via transitive closure) for a given list of basic entry points

# AIX version

# the input to this awk script should be generated by:  nm -go /lib/libc.a | tr "|" " "

# data structures used here:
#
#  definer[entry-point-name] = module-name
#  epcount = # entry-point-name's seen so far
#  ep[k] = k'th entry-point-name
#  refcount[entry-point-name] = number of references by wanted modules
#  referrer[entry-point-name.k] = k'th module-name with an undefined reference to this entry-point-name
#  want[module-name] = "Y" if this module wanted

# search output of nm and set up definers and referrers

$1 == "Symbols" {
	split($3, NameArray,"[");
	split(NameArray[2], FileName, "]");
	ObjName = FileName[1];
};


($3 == "extern") {
	if ($4 == ".text" || $4 == ".data" || $4 == ".bss") {
		if (definer[$1] == "") {
			definer[ep[epcount++] = $1] = ObjName;
		}
	}
	else {
		referrer[$1 "." refcount[$1]++] = ObjName;
	}
}

# postprocessing

END {


# Specify which entry points we definitely want.  Edit this list to add entry points.

  want[definer["ProgramName"]] = "Y";
  want[definer["environ"]] = "Y";
  want[definer["_exit"]] = "Y";
  want[definer["abort"]] = "Y";
  want[definer["alloca"]] = "Y";
  want[definer["blt"]] = "Y";
  want[definer["bcopy"]] = "Y";
  want[definer["brk"]] = "Y";
  want[definer["bzero"]] = "Y";
  want[definer["calloc"]] = "Y";
  want[definer["cfree"]] = "Y";
  want[definer["errno"]] = "Y";
  want[definer["close"]] = "Y";
  want[definer["errno"]] = "Y";
  want[definer["creat"]] = "Y";
  want[definer["_ctype_"]] = "Y";
  want[definer["_doprnt"]] = "Y";
  want[definer["ecvt"]] = "Y";
  want[definer["fcvt"]] = "Y";
  want[definer["sys_errlist"]] = "Y";
  want[definer["sys_nerr"]] = "Y";
  want[definer["exit"]] = "Y";
  want[definer["fcntl"]] = "Y";
  want[definer["_filbuf"]] = "Y";
  want[definer["_iob"]] = "Y";
  want[definer["_cleanup"]] = "Y";
  want[definer["_flsbuf"]] = "Y";
  want[definer["fclose"]] = "Y";
  want[definer["fflush"]] = "Y";
  want[definer["fopen"]] = "Y";
  want[definer["fprintf"]] = "Y";
  want[definer["fread"]] = "Y";
  want[definer["fstat"]] = "Y";
  want[definer["fwrite"]] = "Y";
  want[definer["gcvt"]] = "Y";
  want[definer["getdtablesize"]] = "Y";
  want[definer["getpagesize"]] = "Y";
  want[definer["ioctl"]] = "Y";
  want[definer["isatty"]] = "Y";
  want[definer["lseek"]] = "Y";
  want[definer["malloc"]] = "Y";
  want[definer["modf"]] = "Y";
  want[definer["realloc"]] = "Y";
  want[definer["free"]] = "Y";
  want[definer["open"]] = "Y";
  want[definer["perror"]] = "Y";
  want[definer["printf"]] = "Y";
  want[definer["read"]] = "Y";
  want[definer["sbrk"]] = "Y";
  want[definer["curbrk"]] = "Y";
  want[definer["sigvec"]] = "Y";
  want[definer["sprintf"]] = "Y";
  want[definer["strlen"]] = "Y";
  want[definer["syscall"]] = "Y";
  want[definer["write"]] = "Y";
  want[definer["writev"]] = "Y";
want[definer["ctype"]] = "Y";
want[definer["doprnt"]] = "Y";
want[definer["fltused"]] = "Y";
want[definer["lastbuf"]] = "Y";
want[definer["fac"]] = "Y";
want[definer["ltostr"]] = "Y";
want[definer["do_exit_funcs"]] = "Y";
want[definer["bigpow"]] = "Y";
want[definer["start"]] = "Y";
want[definer["lct_numeric"]] = "Y";
want[definer["cerror"]] = "Y";
want[definer["bufsync"]] = "Y";
want[definer["litpow"]] = "Y";
want[definer["cleanup"]] = "Y";
want[definer["iob"]] = "Y";
want[definer["bufendtab"]] = "Y";
want[definer["xflsbuf"]] = "Y";
want[definer["allocs"]] = "Y";
want[definer["findbuf"]] = "Y";
want[definer["dtop"]] = "Y";
want[definer["wrtchk"]] = "Y";
want[definer["filbuf"]] = "Y";
want[definer["findiop"]] = "Y";
want[definer["flsbuf"]] = "Y";

# now take transitive closure of wanted modules

    for (needmore = "Y"; needmore == "Y"; needmore = "N") {
	for ( i = 0; i < epcount; i++) {
	    if (want[definer[ep[i]]] != "Y") {
		want[definer[ep[i]]] = "N";
		for ( j = 0; j < refcount[ep[i]]; j++ ) {
		    if (want[referrer[ep[i] "." j]] == "Y") {
			want[definer[ep[i]]] = "Y";
			needmore = "Y";
			break;
		    }
		}
	    }
	}
    }

# write out all wanted module name and entry points

    for ( i = 0 ; i < epcount ; i++)
	if (want[definer[ep[i]]] == "Y")
	    printf "%s %s\n", definer[ep[i]], ep[i];
}
