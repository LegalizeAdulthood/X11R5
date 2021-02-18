echo "Graphlib Configuration Program"
echo "Designed to make installation as painless as possible."
echo "(Yea right, i wish.)"
echo
echo "Extra Compile Flags []"
echo -n "(You may need -I/usr/local/include) ?"
read cflags
echo ""
echo "Extra Loader Flags []"
echo -n "(You may need -L/usr/local/lib) ?"
read ldflags
echo ""
echo -n "Build With Debugging [N] (Y/N) ?"
read debug
debug=`./yesnoto10 $debug n`
echo -n "Build Shared Libraries (if possible) [Y] (Y/N) ?"
read shlibs
shlibs=`./yesnoto10 $shlibs y`
echo "Making configuration files."
cd config

rm -f GraphLib.tmpl GraphLib.cf
imakedef="-I`pwd`"

sedmagic="s/\//\\\\\//g"
#Escape /'s
#echo $sedmagic
ldflags=`echo $ldflags | sed $sedmagic`
#echo $ldflags
cflags=`echo $cflags | sed $sedmagic`
imakedef=`echo $imakedef | sed $sedmagic`

sed "s/@@LDOPTIONS@@/$ldflags/" GL.tmpl.dist >,1
sed "s/@@CFLAGS@@/$cflags/" ,1 >,2
sed "s/@@IMAKEDEF@@/$imakedef/" ,2 >GraphLib.tmpl
rm -f ,*
sed "s/\@\@DEBUG\@\@/$debug/" GL.cf.dist >,1
sed "s/\@\@SHLIBS\@\@/$shlibs/" ,1 >GraphLib.cf
rm -f ,*

cd ..
echo "Configuration Finished."
