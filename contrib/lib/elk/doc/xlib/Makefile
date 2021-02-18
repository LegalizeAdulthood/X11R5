TROFF= rof -t

xlib.PS:	xlib.ms index.ms
	(cat xlib.ms ../tmac/tmac.index index.ms; echo ".Tc")\
	    | $(TROFF) 2> /dev/null > xlib.PS

index.ms:	xlib.ms index.raw
	sort -f -t: +1 -3 +0n index.raw |  awk -f ../tmac/fixindex.awk\
	    | awk -f ../tmac/block.awk >index.ms

index.raw:	xlib.ms
	$(TROFF) xlib.ms 2> index.raw >/dev/null

clean:
	rm -f index.raw index.ms xlib.PS

