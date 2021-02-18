/*
 * $Id: final.c,v 1.1 1991/09/11 06:48:30 ohm Exp $
 */
/*
 * X11R5 Input Method Test Program
 * final.c v 1.0   Fri Mar 15 14:01:00 JST 1991
 */

/*
 * Copyright 1991 by OMRON Corporation
 *
 *      Author: Shoji kamada	 OMRON Corporation
 *			 	 kamada@ari.ncl.omron.co.jp
 *
 *		Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include "exvalue.h"
#include "func.h"

void	dtictoex(ic, im) 	/* DesTroy IC TO EXit */
XIM	im;
XIC	ic;
{
    dticonly(ic);
    fcictoex(im);
}

void	dticonly(ic)		/* DesTroy IC ONLY */
XIC	ic;
{
    destic(ic);
}

void	dtictofc(ic)		/* DesTroy IC TO File Close of ic */
XIC	ic;
{
    dticonly(ic);
    fclic();
}

void	fcictoex(im)		/* log File Close of IC TO EXit */
XIM	im;
{
    fclic();
    climtoex(im);
}

void	climtoex(im)		/* CLose IM TO EXit */
XIM	im;
{
    closeim(im);
    fcimtoex();
}

void	fcimtoex()			/* log File Close IM TO EXit */
{
    fclim();
    freexres();
    freemwin();
}












