TROFF= rof -t

kernel.PS:	kernel.ms index.ms
	(cat kernel.ms ../tmac/tmac.index index.ms; echo ".Tc")\
	    | $(TROFF) 2> /dev/null > kernel.PS

index.ms:	kernel.ms index.raw
	sort -f -t: +1 -3 +0n index.raw |  awk -f ../tmac/fixindex.awk\
	    | awk -f ../tmac/block.awk >index.ms

index.raw:	kernel.ms
	$(TROFF) kernel.ms 2> index.raw >/dev/null

clean:
	rm -f index.raw index.ms kernel.PS

