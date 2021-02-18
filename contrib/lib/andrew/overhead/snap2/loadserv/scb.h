/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
 * Fake	Session Control Block for IBM PC/XT/AT/PS/2 SNAP Connections
 * This is so loadav/loadserv can do fake scb calls
 */

#define MAXNAME 17
#define MAXHOST 65
#define MAXPATH 51
#define MAXCIDS 10
#define NULLCID 0xFFFF

int  SCB_CreateSCB ();
void SCB_DestroySCB ();
void SCB_GetSessionParms ();
int  SCB_PutSessionParms ();
int  SCB_GetCID ();
int  SCB_GetLastCID ();
void SCB_ClearSession ();
int  SCB_ServerMatch ();

