/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
class chlistview[chlistv]:textview[textv] {
    classprocedures:
      InitializeObject(struct chlistview *self) returns boolean;
      FinalizeObject(struct chlistview *self);
    overrides:
      Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
      GetStyleInformation(struct text_statevector *sv, long pos, long *length) returns struct environment *;
    methods:
      ActivateItem(int pos);
      HighlightItem(long index);
      UnhighlightItem(long index);
      GetRegionStyle(long regionID, boolean highlighted) returns struct style *;
      SetRegionStyles(long regionID, struct style *normalStyle, struct style highlightStyle);

      SetUpdateRegion(long pos, long len);
    data:
      long highlightedItem;
      long numStylesAllocated;
      struct style **normalStyles;
      struct style **highlightedStyles;
};
