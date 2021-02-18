#!/bin/sh
IMAKEINCLUDE=-I`pwd`/config
export IMAKEINCLUDE
echo "Making New base level makefile (with xmkmf)"
echo "Warning, do not re run xmkmf unless you have previously defined."
echo "IMAKEINCLUDE, see setup.csh.  However make Makefile[s] will work "
echo "Correctly."
xmkmf 
echo "Making world, good luck."
make World
