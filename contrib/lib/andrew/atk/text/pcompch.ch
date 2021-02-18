/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

#define MAXCHAR 127
#define BADCHAR MAXCHAR+1

struct composites {
    unsigned char exts[16],style[32];
    unsigned short code;
    struct composites *next;
};

class pcompch {
classprocedures:
    InitializeClass() returns boolean;
    ATKToASCII(struct text *text,long pos, long len, procedure func, long rock);
    ASCIIToATK(struct text *text,long pos, long len, procedure func, long rock);
    ReadCompositesFile(char *file) returns boolean;
    ClearComposites();
    DeleteComposite(char key,char *exts);
    EnumerateComposites(char key, procedure func,long rock) returns long;
    CharacterToTroff(unsigned char ch, struct environment *env,  struct textview *tv) returns char *;
    StringToTroff(unsigned char *str,char *buf, long bufsize, struct environment *env,struct textview *tv) returns unsigned char *;
};
