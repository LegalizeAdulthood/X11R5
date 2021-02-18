
%{

/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
		Parser for address parsing.
*/


#include "parseadd.h"

#define NIL	0

extern PARSED_ADDRESS *MakeAddress(), *AppendAddresses(), *MakeAddrList();
extern ADDRESS_HOST *MakeHost(), *AppendHosts(), *MakeHostList();
extern char *StrCopy(), *StrCat3();

PARSED_ADDRESS *yyparsedaddress;
int ParseErrorReason;
extern ADDRESS_COMMENT *yycomments;

static yyerror(mesg)
    char *mesg;
{
}

%}

%token ATOM
%token QUOTEDSTRING
%token ATOMLIST
%token DOTWORD
%token DOTLIST
%token DOMAINLIT
%token BADTOKEN

%union {
    PARSED_ADDRESS	*u_address;
    ADDRESS_HOST	*u_host;
    char		*u_string;
}

%type <u_address> mailbox group addrSpec routeAddr
%type <u_address> mailboxList address addrList

%type <u_host> route domainList

%type <u_string> phrase word wordPhrase ATOMLIST ATOM QUOTEDSTRING
%type <u_string> domain subDomain DOMAINLIT DOTLIST
%type <u_string> localPart DOTWORD

%start goal

%%

goal		: addrList
			{ yyparsedaddress = MakeAddrList($1); }
		| error
			{ yyparsedaddress = NIL; }
		;

addrList	: address
			{ $$ = $1; }
		| addrList ',' address
			{ $$ = AppendAddresses($1, $3); }
		;

address		: mailbox
			{
			  $$ = $1;
			  $$ -> Comments = yycomments;
			  yycomments = NIL;
			}
		| group
			{
			  $$ = $1;
			  $$ -> Comments = yycomments;
			  yycomments = NIL;
			}
		| empty
			{ $$ = NIL; }
		;

addrSpec	: localPart
			{
			  $$ = MakeAddress(SIMPLE_ADDRESS, $1);
			  if ($$ == NIL) YYERROR;
			  $$ -> Hosts = MakeHostList(NIL);
			  if ($$->Hosts == NIL) YYERROR;
			}
		| localPart domainList
			{
			  $$ = MakeAddress(SIMPLE_ADDRESS, $1);
			  if ($$ == NIL) YYERROR;
			  $$ -> Hosts = MakeHostList($2);
			  if ($$->Hosts == NIL) YYERROR;
			}
		;

localPart	: phrase
			{ $$ = $1; }
		;

domainList	: '@' domain
			{
			  $$ = MakeHost($2);
			  if ($$ == NIL) YYERROR;
			}
		| '@' domain domainList
			{
			  ADDRESS_HOST *h;
			  h = MakeHost($2);
			  if (h == NIL) YYERROR;
			  $$ = AppendHosts(h, $3);
			}
		;

group		: phrase ':' mailboxList optSemi
			{
			   $$ = MakeAddress(GROUP_ADDRESS, $1);
			   if ($$ == NIL) YYERROR;
			   $$ -> Members = MakeAddrList($3);
			   if ($$->Members == NIL) YYERROR;
			}
		;

optSemi		: ';'
		| empty
		;

mailbox		: addrSpec
			{ $$ = $1; }
		| routeAddr
			{ $$ = $1; }
		| phrase routeAddr
			{
			    $$ = $2;
			    $$ -> RoutePhrase = $1;
			}
		;

mailboxList	: empty
			{ $$ = NIL; }
		| mailbox
			{ $$ = $1; }
		| mailboxList ','
			{ $$ = $1; }
		| mailboxList ',' mailbox
			{ $$ = AppendAddresses($1, $3); }
		;

route		: '@' domain
			{
			  $$ = MakeHost($2);
			  if ($$ == NIL) YYERROR;
			}
		| route routeSep '@' domain
			{
			  ADDRESS_HOST *h;
			  h = MakeHost($4);
			  if (h == NIL) YYERROR;
			  $$ = AppendHosts(h, $1);
			}
		;

routeSep	: ','
		| ':'
		;

routeAddr	: '<' addrSpec '>'
			{ $$ = $2; }
		| '<' route ':' addrSpec '>'
			{
			  AppendHosts($4->Hosts, $2);
			  $$ = $4;
			}
		;

phrase		: wordPhrase
			{ $$ = $1; }
		| phrase wordPhrase
			{
			  $$ = StrCat3($1, " ", $2);
			  if ($$ == NIL) YYERROR;
			  StrFree($1);
			  StrFree($2);
			}
		;

wordPhrase	: word
			{ $$ = $1; }
		| ATOMLIST
			{ $$ = $1; }
		| DOTWORD
			{ $$ = $1; }
		| DOTLIST
			{ $$ = $1; }
		| '.'
			{
			  $$ = StrCopy(".");
			  if ($$ == NIL) YYERROR;
			}
		;

domain		: subDomain
			{ $$ = $1; }
		| domain '.' subDomain
			{
			  $$ = StrCat3($1, ".", $3);
			  if ($$ == NIL) YYERROR;
			  StrFree($1);
			  StrFree($3);
			}
		;

subDomain	: ATOM
			{ $$ = $1; }
		| DOTWORD
			{ $$ = $1; }
		| DOMAINLIT
			{ $$ = $1; }
		;

word		: ATOM
			{ $$ = $1; }
		| QUOTEDSTRING
			{ $$ = $1; }
		;

empty		:
		;

%%
static char *parsey_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/mail/lib/RCS/parsey.y,v 2.4 1991/09/13 16:47:24 bobg Exp $";

