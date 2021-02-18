/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  		       COPYRIGHT 1991 HUBERT RECHSTEINER		      %
%			                                                      %
%                                                                             %
%  Permission to use, copy, modify, distribute, and sell this software and    %
%  its documentation for any purpose is hereby granted without fee, provided  %   %  that the above copyright notice appear in all copies and that both that    %    %  copyright notice and this permission notice appear in  supporting          %
%  documentation, and that the name of the author not be used in advertising  % 
%  or publicity pertaining to distribution of the software without specific,  % 
%  written prior permission. The author makes no representations about the    %
%  suitability of this software for any purpose.  It is provided "as is"      %
%  without express or implied warranty.                                       %
%                                                                             %
%  THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,          %
%  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN        %
%  NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR           %
%  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS        %
%  OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE      %
%  OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE         %
%  USE OR PERFORMANCE OF THIS SOFTWARE.                                       %
%                                                                             %
%                                                                             %
%  Software Design       : Hubert Rechsteiner January 1991                    %
%									      %
%  Further developments  : Doris Hirt 					      %
%                          Nicolas Repond 				      %
%                          Hubert Rechsteiner				      %
%                          October 1991					      %
%									      %
%									      %
%  Please send any improvements, bug fixes, useful modifications, and         %
%  comments to:  selfmoving@uliis.unil.ch                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

/*SccsId "@(#)SelfM.h	3.1 	 10/4/91"*/


#ifndef	   HR_SELFMOVING_H
#define	   HR_SELFMOVING_H

    extern WidgetClass hrSelfMovingWidgetClass;
    typedef struct _HrSelfMovingClassRec *HrSelfMovingWidgetClass;
    typedef struct _HrSelfMovingRec      *HrSelfMovingWidget;

    /*Values for the XtNalignment resource*/
    typedef enum {
	HrALIGN_LEFT, HrALIGN_RIGHT, HrALIGN_CENTER,
	HrALIGN_TOP, HrALIGN_BOTTOM, HrALIGN_CENTER_VERTICAL,
        HrALIGN_NONE /*FOR INTERNAL USE*/
    } HrAlignType;

    typedef enum { 
        HrNO_HANDLE, HrREGULAR_HANDLE, HrSIZING_HANDLE,
        HrHORIZONTAL_SIZING_HANDLE,HrVERTICAL_SIZING_HANDLE 
    } HrHandleType;

    /*Structure returned with the callbacks*/
    typedef struct  {
        int	    reason;
        XEvent      *event;
        Position    x;
        Position    y;
    } HrAnyCallbackStruct;

/*Callback reasons*/
#define	HrCR_SELECT	    1
#define	HrCR_UNSELECT	    2
#define	HrCR_MOVE	    3
#define	HrCR_DURING_MOVE    4
#define	HrCR_RESIZE	    5
#define	HrCR_DURING_RESIZE  6
#define	HrCR_DOUBLE_CLICK   7
#define HrCR_ACC_MOVE	    8



/*Resource strings*/
#define XtNactive		    "active"
#define XtNxChild		    "xChild"
#define XtNyChild	            "yChild"
#define XtNselected		    "selected"
#define XtNselectInReverseVideo     "selectInReverseVideo"
#define XtNselectionBorder	    "selectionBorder"
#define XtNallowMove		    "allowMove"
#define XtNgridOn		    "gridOn"
#define XtNgrid			    "grid"
#define XtNhorizontalLineOffset	    "horizontalLineOffset"
#define XtNverticalLineOffset	    "verticalLineOffset"
#define XtNshowHorizontalLine	    "showHorizontalLine"
#define XtNshowVerticalLine	    "showVerticalLine"
#define XtNsizingHandlesWidth	    "sizingHandlesWidth"
#define XtNsizingHandlesBorderWidth "sizingHandlesBorderWidth"
#define XtNnorthWestHandle	    "northWestHandle"
#define XtNnorthHandle		    "northHandle"
#define XtNnorthEastHandle	    "northEastHandle"
#define XtNeastHandle		    "eastHandle"
#define XtNsouthEastHandle	    "southEastHandle"
#define XtNsouthHandle		    "southHandle"
#define XtNsouthWestHandle	    "southWestHandle"
#define XtNwestHandle		    "westHandle"
#define XtNminimumWidth		    "minimumWidth"
#define XtNminimumHeight            "minimumHeight"
#define XtNmaximumWidth		    "maximumWidth"
#define XtNmaximumHeight	    "maximumHeight"
#define XtNalignment		    "alignment"
#define	XtNinstallAccelerators	    "installAccelerators"
#define XtNhandleTranslations	    "handleTranslations"

#define XtNselectCallback	    "selectCallback"
#define XtNunselectCallback	    "unselectCallback"
#define XtNmoveCallback		    "moveCallback"
#define XtNduringMoveCallback	    "duringMoveCallback"
#define XtNresizeCallback	    "resizeCallback"
#define XtNduringResizeCallback	    "duringResizeCallback"
#define XtNdoubleClickCallback	    "doubleClickCallback"
#define XtNaccMoveCallback	    "accMoveCallback"

#define XtCActive		    "Active"
#define XtCXChild		    "XSon"
#define XtCYChild		    "YSon"
#define XtCSelected		    "Selected"
#define XtCSelectInReverseVideo     "SelectInReverseVideo"
#define XtCSelectionBorder	    "SelectionBorder"
#define XtCAllowMove		    "AllowMove"
#define XtCGridOn		    "GridOn"
#define XtCGrid			    "Grid"
#define XtCLineOffset		    "LineOffset"
#define XtCShowLine		    "ShowLine"
#define XtCSizingHandlesWidth	    "SizingHandlesWidth"
#define XtCSizingHandlesBorderWidth "SizingHandlesBorderWidth" 
#define XtCHorizontalHandle	    "HorizontalHandle"
#define XtCVerticalHandle	    "VerticalHandle"
#define XtCDiagonalHandle	    "DiagonalHandle"
#define XtCMinimum		    "Minimum"
#define XtCMaximum		    "Maximum"
#define XtCAlignment		    "Alignment"
#define XtCInstallAccelerators	    "InstallAccelerators"

#endif HR_SELFMOVING_H
