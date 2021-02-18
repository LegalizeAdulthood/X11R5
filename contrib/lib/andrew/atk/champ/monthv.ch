/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
class monthview[monthv]:view {
    classprocedures:
      InitializeClass() returns boolean;
      InitializeObject(struct monthview *self) returns boolean;
      FinalizeObject(struct monthview *self);
    overrides:
      FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
      Update();
      Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
      DesiredSize(long width, long height, enum view_DSpass pass, long *dWidth, long *dheight) returns enum view_DSattributes;
    methods:
      SetTextview(struct textview *tv);
      GrabTextview(boolean ForceRedraw);
      ResetMonth(boolean ForceRedraw);
    data:
      struct textview *tv;
      int mymonth, myyear;
      int skippedatstart;
      int EventCt[31];
      int textpositions[31];
      struct tm FullTimes[31];
      struct text *t;
      char *AnnounceArray[11];
      int AnnounceArraySize;
      int pendingtop;
      int firstpendingtop;
};
