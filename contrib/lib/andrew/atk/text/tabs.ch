/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* Nick Williams, August 1990 */

/* tabs are a package. 
 * However, we want to be able to define methods and data,
 * so we have it as a basic class.
 */

class tabs {
classprocedures:
    InitializeObject(struct tabs *self) returns boolean;
    FinalizeObject(struct tabs *self);
    Create() returns struct tabs *;
    Death(struct tabs *self);
methods:
    OutputTroff(long indent, FILE *file);
    Different(struct tabs *b) returns int;
    Delete(int n) returns struct tabs *;
    Add(long pos, enum style_TabAlignment op) returns struct tabs *;
    Clear() returns struct tabs *;
    ApplyChange(struct tabentry *change) returns struct tabs *;
    Copy() returns struct tabs *;
data:
    long *Positions;
    long *Types;
    int number;
    int links;
};
