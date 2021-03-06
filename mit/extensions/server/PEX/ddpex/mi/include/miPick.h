/* $XConsortium: miPick.h,v 5.1 91/02/16 09:54:56 rws Exp $ */

/***********************************************************
Copyright 1989, 1990, 1991 by Sun Microsystems, Inc. and the X Consortium.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Sun Microsystems,
the X Consortium, and MIT not be used in advertising or publicity 
pertaining to distribution of the software without specific, written 
prior permission.  

SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#include "mipex.h"
#include "ddpex4.h"

#ifndef MIPICK_H
#define MIPICK_H

 /* there is one data rec and one input record for each pick device */
 /* supported pick devices are defined in miInfo.h */
#define MIWKS_NUM_PICK_DEVICES	SI_PICK_DEVICE_NUM

typedef struct {
    ddEnumTypeIndex	    type;	/* dc_hitbox or npc_hitvolume */
    ddUSHORT                status;	/* PEXOk or PEXNoPick */
    listofObj  		    *path;	/* start path */
    ddUSHORT                pathOrder;
    diNSHandle              inclusion;
    diNSHandle              exclusion;
    union {
    	char    dc_data_rec;	/* none */ 
    	char    npc_data_rec; 	/* none */
    }	data_rec;	/* place holder */
    ddEnumTypeIndex         pet;
    ddViewport              echoVolume;
    ddUSHORT                echoSwitch;
    ddPointer		    devPriv;
} miPickDevice;

typedef struct {
	/* the resource id is in the dipex resource structure */
	diWKSHandle	    pWks;	
	ddEnumTypeIndex	    type;	/* dc_hitbox or npc_hitvolume */
	ddUSHORT	    status;	/* PEXOk or PEXNoPick */
	listofObj	   *path;	/* list of pick element refs */
	ddUSHORT            pathOrder;
    diNSHandle              incl_handle;
    diNSHandle              excl_handle;
	union {
		char    dc_data_rec; 
		char    npc_data_rec; 
	}	data_rec;
	union {
		ddPC_DC_HitBox		dc_hit_box;
		ddPC_NPC_HitVolume	npc_hit_volume;
	} input_rec;
    ddPointer		    devPriv;
} miPickMeasureStr;


/* macros for pick devices and pick measures */

/* set this to return the index into array of pick devices based
 * on the device type specified
 * device types:
 * PEXPickDeviceDC_HitBox 	1
 * PEXPickDeviceNPC_HitVolume	2
 */
#define	MIWKS_PICK_DEV_INDEX(devindex, devtype)	\
	(devindex) = (devtype) - 1

/* for both pick device and pick measure */
#define MIWKS_SIZE_DATA_REC_1	\
	sizeof(char)

#define MIWKS_SIZE_DATA_REC_2	\
	sizeof(char)

/* for pick measure */
#define MIWKS_PM_DATA_REC_1(pPick) \
	(pPick)->data_rec.dc_data_rec

#define MIWKS_PM_DATA_REC_2(pPick) \
	(pPick)->data_rec.npc_data_rec

#define MIWKS_PM_INPUT_STR_1	ddPC_DC_HitBox
#define MIWKS_PM_INPUT_REC_1(pPick) \
	(pPick)->input_rec.dc_hit_box

#define MIWKS_PM_INPUT_STR_2	ddPC_NPC_HitVolume
#define MIWKS_PM_INPUT_REC_2(pPick) \
	(pPick)->input_rec.npc_hit_volume

/* for pick device */
#define MIWKS_PD_DATA_REC_1(pPick) \
	(pPick)->data_rec.dc_data_rec

#define MIWKS_PD_DATA_REC_2(pPick) \
	(pPick)->data_rec.npc_data_rec

#endif	/* MIPICK_H */
