char *PEXlib_version = "$Revision: 2.2 $ PEXlib";


/*****************************************************************
Copyright 1991 by Sony Microsystems Company, San Jose, California

					All Rights Reserved

Permission to use, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sony not be used
in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

SONY DISCLAIMS ANY AND ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL EXPRESS WARRANTIES AND ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, FOR A PARTICULAR PURPOSE. IN NO EVENT
SHALL SONY BE LIABLE FOR ANY DAMAGES OF ANY KIND, INCLUDING BUT NOT
LIMITED TO SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING FROM
LOSS OF USE, DATA OR LOSS OF ANY PAST, PRESENT, OR PROSPECTIVE PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
SOFTWARE.
*****************************************************************/

/*
:TOC:
     $./Imakefile,v 2.3 91/09/11 16:31:48 sinyaw Exp $
     $convenience/XpexQueryExt.c,v 2.2 91/09/11 16:06:06 sinyaw Exp $
     $convenience/XpexInit.c,v 2.2 91/09/11 16:06:08 sinyaw Exp $
     $convenience/XpexState.c,v 2.3 91/09/11 16:06:11 sinyaw Exp $
     $convenience/Imakefile,v 2.3 91/09/11 16:06:13 sinyaw Exp $
     $convenience/XpexFontUtils.c,v 2.2 91/09/11 16:06:15 sinyaw Exp $
     $convenience/XpexColormap.c,v 2.2 91/09/11 16:06:18 sinyaw Exp $
     $convenience/XpexVisual.c,v 2.2 91/09/11 16:06:20 sinyaw Exp $
     $convenience/XpexNames.c,v 1.2 91/09/11 16:06:23 sinyaw Exp $
     $include/Xpexlibint.h,v 2.5 91/09/11 16:06:26 sinyaw Exp $
     $include/Imakefile,v 2.3 91/09/11 16:06:28 sinyaw Exp $
     $include/Xpextutil.h,v 2.2 91/09/11 16:06:30 sinyaw Exp $
     $output_cmds/XpexCellArray.c,v 2.4 91/09/11 16:06:33 sinyaw Exp $
     $output_cmds/XpexColorSpec.c,v 2.2 91/09/11 16:06:35 sinyaw Exp $
     $output_cmds/XpexFillArea.c,v 2.7 91/09/11 16:06:38 sinyaw Exp $
     $output_cmds/XpexMarker.c,v 2.3 91/09/11 16:06:41 sinyaw Exp $
     $output_cmds/XpexPolyline.c,v 2.4 91/09/11 16:06:44 sinyaw Exp $
     $output_cmds/XpexReflAttr.c,v 2.3 91/09/11 16:06:46 sinyaw Exp $
     $output_cmds/Imakefile,v 2.2 91/09/11 17:43:59 sinyaw Exp $
     $output_cmds/XpexGeoAttrs.c,v 2.3 91/09/11 16:06:52 sinyaw Exp $
     $output_cmds/XpexStrCon.c,v 2.3 91/09/11 16:06:54 sinyaw Exp $
     $output_cmds/XpexText.c,v 2.5 91/09/11 16:06:57 sinyaw Exp $
     $output_cmds/XpexGdp.c,v 2.3 91/09/11 16:06:59 sinyaw Exp $
     $output_cmds/XpexColorAttrs.c,v 2.5 91/09/11 16:07:01 sinyaw Exp $
     $output_cmds/XpexMiscAttrs.c,v 2.3 91/09/11 16:07:04 sinyaw Exp $
     $output_cmds/XpexColor.c,v 2.3 91/09/11 16:07:07 sinyaw Exp $
     $output_cmds/XpexAsf.c,v 2.3 91/09/11 16:07:09 sinyaw Exp $
     $output_cmds/XpexTrimCurve.c,v 2.3 91/09/11 16:07:11 sinyaw Exp $
     $resources/XpexLut.c,v 2.5 91/09/11 16:07:14 sinyaw Exp $
     $resources/XpexStr.c,v 2.4 91/09/11 17:46:21 sinyaw Exp $
     $resources/XpexNameSets.c,v 2.2 91/09/11 16:07:18 sinyaw Exp $
     $resources/XpexPC.c,v 2.8 91/09/11 16:07:21 sinyaw Exp $
     $resources/XpexRenderers.c,v 2.11 91/09/11 16:07:25 sinyaw Exp $
     $resources/XpexSC.c,v 2.4 91/09/11 16:07:27 sinyaw Exp $
     $resources/XpexFont.c,v 2.3 91/09/11 16:07:30 sinyaw Exp $
     $resources/XpexlibInt.c,v 2.2 91/09/11 16:07:32 sinyaw Exp $
     $resources/XpexInfo.c,v 2.2 91/09/11 16:07:34 sinyaw Exp $
     $resources/XpexPhigsWks.c,v 2.6 91/09/11 15:50:59 sinyaw Exp $
     $resources/XpexPick.c,v 2.4 91/09/11 15:51:02 sinyaw Exp $
     $resources/Imakefile,v 2.2 91/09/11 15:51:04 sinyaw Exp $
     $utilities/Imakefile,v 2.2 91/09/11 15:51:07 sinyaw Exp $
     $utilities/XpexTransform.c,v 2.2 91/09/11 15:51:09 sinyaw Exp $
     $utilities/Xpexmatrixutil.h,v 2.2 91/09/11 15:51:11 sinyaw Exp $
     $utilities/XpexBuild.c,v 2.2 91/09/11 15:51:14 sinyaw Exp $
     $utilities/XpexCompose.c,v 2.2 91/09/11 15:51:16 sinyaw Exp $
     $utilities/XpexCompute.c,v 2.3 91/09/11 15:51:18 sinyaw Exp $
     $utilities/XpexEvaluate.c,v 2.2 91/09/11 15:51:20 sinyaw Exp $
     $utilities/XpexRotate.c,v 2.2 91/09/11 15:51:23 sinyaw Exp $
     $utilities/XpexScale.c,v 2.2 91/09/11 15:51:25 sinyaw Exp $
     $utilities/XpexTranslate.c,v 2.2 91/09/11 15:51:27 sinyaw Exp $
     $utilities/XpexErr.h,v 2.2 91/09/11 15:51:29 sinyaw Exp $
     $utilities/XpexFree.c,v 2.2 91/09/11 15:51:32 sinyaw Exp $
*/
