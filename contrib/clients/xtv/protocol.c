/*
 * Copyright (c) 1991 Old Dominion University.
 * Copyright (c) 1991 University of North Carolina at Chapel Hill.
 * All rights reserved.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  Old Dominion University and the University of North
 * Carolina at Chapel Hill make no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

/*
	protocol.c
	Routines to handle the protocol

*/

#define DEBUGHandleStoHProtocol	0&DEBUGON
#define DEBUGHandleHtoSProtocol	0&DEBUGON

#include <stdio.h>
#include <X11/Intrinsic.h>

#include "protocol.h"
#include "subscriber.h"
#include "network.h"
#include "htos.h"
#include "stoh.h"
#include "tool.h"


/*
	Define the protocol table.  This is simply a table of
	routines to call when certain requests come in.

	Each function takes the following arguments:

		Socket		Port	- File descriptor of the socket in question
		Subscriber	*Who	- Structure showing who sent it (NULL for HtoS)

*/

typedef struct _ProtoTableEntry {
	void	(*StoHFunc)();	/* Function to call in Viewer-To-Host Protocol */
	void	(*HtoSFunc)();	/* Function to call in Host-To-Viewer Protocol */
} ProtoTableEntry;

ProtoTableEntry	ProtoTable[31] = {
/*** Basic control mechanisms */
	{ STOH_LeavingSession,	HTOS_EndSession		},
	{ STOH_Event,		HTOS_DroppedYou		},
	{ STOH_Reply,		HTOS_X11Packet		},
	{ NULL,			HTOS_InternalAtom	},
	{ NULL,			HTOS_YouAre		},
        { NULL,                 HTOS_AllocColor         },
        { NULL,                 NULL                    },
        { NULL,                 NULL                    },
        { NULL,                 NULL                    },
        { NULL,                 NULL                    },
/** Client and tool list management */
	{ STOH_ConfirmNewTool,	HTOS_AddNewSubscriber	},
	{ NULL,			HTOS_DeleteSubscriber	},
	{ STOH_NewToolLocal,	HTOS_AddNewTool		},
        { STOH_NewToolRemote,   HTOS_IncomingTools      },
	{ STOH_TerminateTool,	HTOS_DeleteTool		},
        { STOH_StopTool,        HTOS_EndIncoming        },
        { STOH_WantJoinTools,   HTOS_ToolsToJoin        },
        { STOH_JoinTools,       HTOS_CatchUpStart	},
        { NULL,                 HTOS_CatchUpEnd		},
        { NULL,                 NULL                    },
/** Token passing mechanisms */
	{ STOH_WantToken,	HTOS_TakeToken		},
	{ STOH_GiveTokenToMe,	HTOS_ForceTokenRelease	},
	{ STOH_ReleasingToken,	HTOS_AskTokenRelease	},
	{ STOH_DeQueToken,      HTOS_CreatorTakeToken 	},
        { NULL,                 HTOS_ContinueTakeToken   },
/** others */
/*
 * Begin Murray Addition
 */
/** list requests of the host **/
        { STOH_QueuedList,      HTOS_QueuedList         },
/*
 * End Murray Addition
 */
        { NULL,                 NULL                    },
        { NULL,                 NULL                    },
        { NULL,                 NULL                    },
        { NULL,                 NULL			},
        { STOH_SendArchivePacket, NULL			}
};


/*
 *	HandleStoHProtocol()
 *
 *	Handle incoming viewer-to-host requests.
 *
 *	RETURN VALUE:	void
 */
XtInputCallbackProc HandleStoHProtocol( Who, Port, ID )
Subscriber *Who;
Socket *Port;
XtInputId *ID;
{
	BYTE	Request;

	/* Check for a bogus request */
	if ( (Request=CheckByte( *Port )) == NothingAvail) {
		return;
	}


	/* Reset the toolkit before doing anything else */
	XtRemoveInput( *ID );
	Who->InputCallbackID = XtAppAddInput( GetAppCon(), *Port, XtInputReadMask,
	    HandleStoHProtocol, Who );

	if ( (Request <= MAX_PROTO_REQUEST) && ((ProtoTable[Request].StoHFunc) != NULL) ) {
#		if DEBUGHandleStoHProtocol
		fprintf( stderr, "STOH(): Performing call [%03d]\n", Request );
#		endif
		(ProtoTable[Request].StoHFunc)( *Port, Who );
	} else {
#		if DEBUGHandleStoHProtocol
		fprintf( stderr, "STOH(): Got NULL request [%03d] from host\n", Request );
#		endif
	}
}


/*
 *	HandleHtoSProtocol()
 *
 *	Handle incoming host-to-viewer requests.
 *
 *	RETURN VALUE:	None to speak of
 */
XtInputCallbackProc HandleHtoSProtocol( Who, Port, ID )
Subscriber *Who;
Socket *Port;
XtInputId *ID;
{
	BYTE	Request;

	/* Check for a bogus request */
	if ( (Request=CheckByte( *Port )) == NothingAvail) {
		return;
	}

	/* Reset the toolkit before leaving */
	XtRemoveInput( *ID );
	XtAppAddInput( GetAppCon(), *Port, XtInputReadMask, HandleHtoSProtocol, Who );
			
	if ( (Request <= MAX_PROTO_REQUEST) && ((ProtoTable[Request].HtoSFunc) != NULL) ) {
#		if DEBUGHandleHtoSProtocol
		if ( Request != HS_X11Packet ) {
			fprintf( stderr, "HTOS(): Performing call [%03d]\n", Request );
		}
#		endif
		(ProtoTable[Request].HtoSFunc)( *Port, Who );	/* "Who" gets ignored here */
	} else {
#		if DEBUGHandleHtoSProtocol
		fprintf( stderr, "HTOS(): Got NULL request [%03d] from host\n", Request );
#		endif
	}

}
