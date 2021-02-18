/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
struct listitem {
    char *str;
    int loc;
    int (*proc)();
    long rock;
    char **regionStrings;
};

class chlist:text {
    classprocedures:
      InitializeObject(struct chlist *self) returns boolean;
      FinalizeObject(struct chlist *self);
    overrides:
      Clear();
    methods:
      AddItemAtIndex(char *str, long index, procedure proc, long rock) returns boolean;
      AddItemToEnd(char *str, procedure proc, long rock) returns boolean;
      DeleteItem(char *str) returns boolean;
      DeleteItemByIndex(long index) returns boolean;
      GetIndexByString(char *str) returns long;
      GetIndexByData(long rock) returns long;
      FindItem(char *str) returns struct listitem *;
      FindItemByIndex(unsigned long index) returns struct listitem *;
      SetFreeProcedure(procedure freeProc) returns procedure;
      ChangeItem(char *oldstr, char *newstr) returns boolean;
      ChangeItemByIndex(long index, char *newstr) returns boolean;
      ChangeData(char *oldstr, long rock) returns boolean;
      ChangeDataByIndex(long index, long rock) returns boolean;
      DefineRegion(long regionNum);
      DefineStringRegion(long regionNum);
      SetRegionStringByIndex(long index, long regionNum, char *regionStr);
      SetRegionString(char *str, long regionNum, char *regionStr);
      GetRegionInfoForPosition(long index, long position, long *size, long *offset) returns long;
      GetIndexByPosition(long position, long *regionID, long *size, long *offset) returns long;
      EnumerateItems(long startIndex, long length, procedure proc, long rock);

    macromethods:
      GetItemList() (self->ItemList)
      GetNumItems() (self->numitems)
      GetNumRegions() (self->numRegions)
    data:
      struct listitem *ItemList;
      int numitems, numitemsallocated;
      procedure freeProc;
      int numRegions, numRegionsAllocated;
      int strRegionNum;
};
