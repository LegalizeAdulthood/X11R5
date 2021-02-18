#!/bin/sh
rm -f Imakefile.bak
mv Imakefile Imakefile.bak
echo "#include <GLLibrary.tmpl>" >Imakefile
echo "#include <GraphLib.tmpl>" >>Imakefile
echo " " >>Imakefile
echo -n "OBJS = " >>Imakefile
rm -f objs.local objs.shared
for i in `/bin/ls ../*/*.c`
do
dir=`dirname $i`
src=`basename $i .c`
echo "\\" >>objs.local
echo "\\" >>objs.shared
echo -n "     $dir/$src.o " >>objs.local
echo -n "     $dir/shared/$src.o " >>objs.shared
done

cat objs.local >>Imakefile
echo " " >>Imakefile
echo " " >>Imakefile
echo -n "SHOBJS = " >>Imakefile
cat objs.shared >>Imakefile
rm -f objs.local objs.shared
echo " " >>Imakefile
echo " " >>Imakefile
echo "NormalLibraryTarget(gl,\$(OBJS))" >>Imakefile
echo "InstallLibrary(gl,\$(USRLIBDIR))" >>Imakefile
echo "#if SharedLibGL" >>Imakefile
echo "GraphLibSharedLibraryTarget(gl,\$(SOGLLIBREV),\$(SHOBJS))" >>Imakefile
echo " " >>Imakefile
echo "InstallSharedLibrary(gl,\$(SOGLLIBREV),\$(USRLIBDIR))" >>Imakefile
echo "#endif" >>Imakefile
echo " " >>Imakefile
echo "depend::" >>Imakefile



