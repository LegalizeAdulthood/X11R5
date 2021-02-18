};

/*
    $Header: dump_end.c,v 1.0 91/10/04 17:01:08 rthomson Exp $
*/
void dump_names(fp, function_number, error_number)
     FILE *fp;
     int function_number, error_number;
{
  register char *function_name, *error_name;
  register int i;

  function_name = "Unknown function";
  error_name = "Unknown error";

  for (i = 0; i < sizeof(function_names)/sizeof(PEXtNameMap); i++)
    if (function_names[i].number == function_number)
      {
	function_name = function_names[i].name;
	break;
      }

  for (i = 0; i < sizeof(error_names)/sizeof(PEXtNameMap); i++)
    if (error_names[i].number == error_number)
      {
	error_name = error_names[i].name;
	break;
      }

  fprintf(fp, "\tError: %s\n", error_name);
}
