/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *bebea_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/be1be2a.c,v 2.4 1991/09/12 16:34:25 bobg Exp $";

/*
 * BE1 to BE2 conversion utility
 */

#include <class.h>
#include <be1be2.ih>
#include <text.ih>
#include <be1be2a.eh>

extern int errno;
extern char *sys_errlist[];

/*
 * Obtain list of input files
 */

char *progName = "be1be2";
char *fileList[1000];
int fileCount;

boolean be1be2app__ParseArgs(self, argc, argv)
struct be1be2app *self;
int argc;
char **argv;
{
    int i;

    if (super_ParseArgs(self, argc, argv) == FALSE)
        return FALSE;

    for (fileCount = 0, i = 1; i < argc; i++) {
        fileList[fileCount] = malloc(strlen(argv[i]) + 1);
        strcpy(fileList[fileCount++], argv[i]);
    }

    return TRUE;
}

/*
 * Ruotines to convert one file
 */

char *OutputName(inputName)
char *inputName;
{
    static char outName[256];
    int i;

    strcpy(outName, inputName);

    for (i = strlen(outName) - 1; i > 0; i--)
        if (outName[i] == '.')
            break;

    if (i == 0)
        strcat(outName, ".d");
    else {
        if (strcmp(outName + i + 1, "d") == 0)
            strcpy(outName + i + 1, "ez");
        else
            strcpy(outName + i + 1, "d");
    }

    return outName;
}

static void Convert(fileName)
char *fileName;
{
    char *outName;
    struct text *text;
    FILE *fp;

    text = text_New();

    fp = fopen(fileName, "r");
    if (fp == NULL) {
        fprintf(stderr, "%s: Cannot open %s (%s)\n", progName, fileName, sys_errlist[errno]);
        return;
    }

    /* If text_Read were used, the conversion would be done */
    /* automatically since text_Read does conversions.  We would */
    /* then not have any control over the user intrface. */

    if (text_ReadSubString(text, 0, fp, FALSE) <= 0) {
        text_Destroy(text);
        fprintf(stderr, "%s: Unable to read from %d\n", progName, fileName);
        return;
    }

    fclose(fp);

    if (! be1be2_CheckBE1(text)) {
        text_Destroy(text);
        fprintf(stderr, "%s: %s is not a BE1 file\n", progName, fileName);
        return;
    }

    if (be1be2_Convert(text) == FALSE)
        fprintf(stderr, "%s: Possible conversion errors in %s\n", progName, fileName);

    outName = OutputName(fileName);

    fp = fopen(outName, "w");
    if (fp == NULL) {
        fprintf(stderr, "%s: Could not open output file %s (%s)\n", progName, outName, sys_errlist[errno]);
        text_Destroy(text);
        return;
    }

    if (text_Write(text, fp, 1, 1) < 0) {
        fclose(fp);
        unlink(outName);
        fprintf(stderr, "%s: Error writing output file %s (%s)\n", progName, outName, sys_errlist[errno]);
        text_Destroy(text);
        return;
    }

    fclose(fp);
    text_Destroy(text);

    fprintf(stderr, "%s: %s ==> %s\n", progName, fileName, outName);
    return;
}

/*
 * Convert each file
 */

boolean be1be2app__Run(self)
struct be1be2app *self;
{
    int i;

    if (fileCount == 0) {
        fprintf(stderr, "%s: No files specified\n");
        return TRUE;
    }

    for (i = 0; i < fileCount; i++)
        Convert(fileList[i]);

    return TRUE;
}
