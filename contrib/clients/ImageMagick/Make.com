$!
$! Make ImageMagick X image utilities for VMS.
$!
$
$define/nolog X11 decw$include:
$define/nolog sys sys$library:
$link_options="/nodebug/notraceback"
$define/nolog lnk$library sys$library:vaxcrtl
$
$if ((p1.nes."").and.(p1.nes."Display")) then goto SkipDisplay
$write sys$output "Making Display..."
$call Make display
$call Make X
$call Make image
$call Make rotate
$call Make quantize
$call Make colors
$call Make compress
$call Make PreRvIcccm
$
$link'link_options' display,X,image,rotate,quantize,colors,compress,PreRvIcccm,sys$input:/opt
sys$share:decw$xlibshr.exe/share
$
$display:==$'f$environment("default")'display
$write sys$output "..symbol DISPLAY defined."
$
$SkipDisplay:
$if ((p1.nes."").and.(p1.nes."Import")) then goto SkipImport
$write sys$output "Making Import..."
$call Make import
$call Make X
$call Make image
$call Make rotate
$call Make quantize
$call Make compress
$call Make PreRvIcccm
$
$link'link_options' import,X,image,rotate,quantize,compress,PreRvIcccm,sys$input:/opt
sys$share:decw$xlibshr.exe/share
$
$import:==$'f$environment("default")'import
$write sys$output "..symbol IMPORT defined."
$SkipImport:
$
$if ((p1.nes."").and.(p1.nes."XtoPS")) then goto SkipXtoPS
$write sys$output "Making XtoPS..."
$call Make XtoPS
$call Make X
$call Make image
$call Make rotate
$call Make quantize
$call Make compress
$call Make PreRvIcccm
$
$link'link_options' XtoPS,X,image,rotate,quantize,compress,PreRvIcccm,sys$input:/opt
sys$share:decw$xlibshr.exe/share
$
$XtoPS:== $'f$environment("default")'XtoPS
$write sys$output "..symbol XTOPS defined."
$
$SkipXtoPS:
$if ((p1.nes."").and.(p1.nes."Animate")) then goto SkipAnimate
$write sys$output "Making Animate..."
$call Make animate
$call Make X
$call Make image
$call Make rotate
$call Make quantize
$call Make colors
$call Make compress
$call Make PreRvIcccm
$
$link'link_options' animate,X,image,rotate,quantize,colors,compress,PreRvIcccm,sys$input:/opt
sys$share:decw$xlibshr.exe/share
$
$display:==$'f$environment("default")'display
$write sys$output "..symbol DISPLAY defined."
$
$SkipAnimate:
$if ((p1.nes."").and.(p1.nes."Montage")) then goto SkipMontage
$write sys$output "Making Montage..."
$call Make montage
$call Make X
$call Make image
$call Make rotate
$call Make quantize
$call Make colors
$call Make compress
$call Make PreRvIcccm
$
$link'link_options' montage,X,image,rotate,quantize,colors,compress,PreRvIcccm,sys$input:/opt
sys$share:decw$xlibshr.exe/share
$
$display:==$'f$environment("default")'display
$write sys$output "..symbol DISPLAY defined."
$
$SkipMontage:
$type sys$input

Use this command to specify which X server to contact:

  $set display/create/node=node_name::

This can be done automatically from your LOGIN.COM with the following 
command:

  $if (f$trnlmn("sys$rem_node").nes."") then -
  $  set display/create/node='f$trnlmn("sys$rem_node")'
$exit
$
$Make: subroutine
$!
$! A very primitive "make" (or MMS) hack for DCL.
$!
$if (p1.eqs."") then exit
$source_file=f$search(f$parse(p1,".c"))
$if (source_file.nes."")
$  then
$    object_file=f$parse(source_file,,,"name")+".obj"
$    object_file=f$search( object_file )
$    if (object_file.nes."") 
$      then
$        object_time=f$file_attribute(object_file,"cdt")
$        source_time=f$file_attribute(source_file,"cdt")
$        if (f$cvtime(object_time).lts.f$cvtime(source_time)) then -
$          object_file=""
$      endif
$    if (object_file.eqs."") 
$      then
$        write sys$output "Compiling ",p1
$        cc/nodebug/optimize 'source_file'
$      endif
$  endif
$exit
$endsubroutine

