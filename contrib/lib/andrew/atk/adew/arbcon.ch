/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* user code begins here for HeaderInfo */
/* user code ends here for HeaderInfo */
class arbcon : observable [observe] { 
classprocedures :
        InitializeClass() returns boolean;
	FinalizeObject(struct arbcon *self);
	InitializeObject(struct arbcon *self) returns boolean;    
/* user code begins here for classprocedures */
AddCel(struct arbiterview *arb,struct cel *cl,boolean notify);
currentcelview() returns struct celview *;
SetCurrentCelview(struct celview *cv);
DeleteCelview(struct arbiterview *arb,struct celview *cv);
EditCurrentCelview();
SaveCurrentCelview();
DestroyCurrentCelview();
SetCurrentArbiterview(struct arbiterview *ab);
DeleteArbiter(struct arbiterview *arb);
AddArbiter(struct arbiterview *arb);
InitCel(struct celview *cv,struct arbiterview *abv) returns boolean;
InitArbiters();

/* user code ends here for classprocedures */
overrides:
	ObservedChanged( struct observable * observed, long status );
/* user code begins here for overrides */
/* user code ends here for overrides */
data:
	struct value *ArbLinkCel;
	struct onoffV *ArbLinkCelView;
	struct value *ArbCutCel;
	struct buttonV *ArbCutCelView;
	struct value *ArbApplicationChoice;
	struct onoffV *ArbApplicationChoiceView;
	struct value *Arbobviewlist;
	struct clicklistV *ArbobviewlistView;
	struct value *ArbTextEdit;
	struct menterstrV *ArbTextEditView;
/* user code begins here for classdata */
	struct celview *currentcelview;
	struct arbiterview *CurrentArbiterview;
	int obcount,vwcount;
	int obsize,vwsize,celsize,celnamesize,NeedsInit;
	char **obnamelist, **vwlist,buf[256];
	struct cel *cl;
	char *arr; 
	long arrlen;
	struct view *overlay;
 	struct text *ArbCelList;
 	struct cltextview *ArbCelListView;
 	struct text *ArbArbList;
 	char *ArbiterName;
 	char *ObjectName;
 	char *ViewName;

/* user code ends here for classdata */
	struct view *v;
	struct arbiterview *arbv;
	struct arbcon *next;
};

