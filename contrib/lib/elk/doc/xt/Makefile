TROFF= rof -t

xt.PS:	xt.ms index.ms
	(cat xt.ms ../tmac/tmac.index index.ms; echo ".Tc")\
	    | $(TROFF) 2> /dev/null > xt.PS

index.ms:	xt.ms index.raw
	sort -f -t: +1 -3 +0n index.raw |  awk -f ../tmac/fixindex.awk\
	    | awk -f ../tmac/block.awk >index.ms

index.raw:	xt.ms
	$(TROFF) xt.ms 2> index.raw >/dev/null

clean:
	rm -f index.raw index.ms xt.PS

