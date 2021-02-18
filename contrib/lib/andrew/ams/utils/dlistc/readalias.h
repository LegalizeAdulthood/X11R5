/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*

  readalias.h -- Read in an alias file


*/

#include <mail.h>
#include <parseadd.h>

/* Format of the .AMS_aliases file:

--  do not to leave any blank lines at the top of the .AMS_aliases file
--  comments can be placed in the file with a #
--  end every line in the file with a return
--  do not put any tabs or spaces before the alias ("joe")
--  put only one space (or one tab) between each alias and its address
--  when aliasing several addresses to one alias (explained below), separate 
    the addresses with a comma
--  aliases are case-insensitive, which means that the Andrew Message System 
    doesn't distinguish between upper case and lower case letters either in 
    your .AMS_aliases file or at the To: header when sending.
--  @shorthost @full.host.name is an abbreviation for hostnames
--  several commands:
       $forceformat user@host
       $forcestrip user@host
       $foretrust user@host

*/

typedef struct alias {
  PARSED_ADDRESS *parsed_alias;	/* always a GROUP_ADDRESS */
  int resolving;
  int resolved;
  int circular;
} *alias_t;

#define ALIASNAME(x) (x)->parsed_alias->LocalPart
#define ALIASMEMBERS(x) (x)->parsed_alias->Members

typedef struct host {
  char *shorthostname, *longhostname;
} *host_t;

#define SHORTNAME(x) (x)->shorthostname
#define LONGNAME(x) (x)->longhostname

typedef struct alias_set {
  alias_t *aliases;
  int alias_count;
  long aliases_size;
  int sorted;
  host_t *hosts;
  int host_count;
  long hosts_size;
} *alias_set_t;

#define DEFAULTALIASFILE "/.AMS_aliases"

extern int errno;

extern alias_set_t ReadAliases(/* FILE *aliasfile */);

extern alias_t FindAlias(/* char *name, alias_set_t aliases */);

extern char *FindHost(/* char *short, alias_set_t aliases */);

extern void DumpAliases(/* FILE *stream, alias_set_t aliases */);

extern char *FlatUnparseAddress(/* PARSED_ADDRESS *pa */);
