TROFF= rof -t

oops.PS:	oops.ms index.ms
	(cat oops.ms ../tmac/tmac.index index.ms; echo ".Tc")\
	    | $(TROFF) 2> /dev/null > oops.PS

index.ms:	oops.ms index.raw
	sort -f -t: +1 -3 +0n index.raw |  awk -f ../tmac/fixindex.awk\
	    | awk -f ../tmac/block.awk >index.ms

index.raw:	oops.ms
	$(TROFF) oops.ms 2> index.raw >/dev/null

clean:
	rm -f index.raw index.ms oops.PS

