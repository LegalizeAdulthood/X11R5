
#include <strings.h>
#include <stdio.h>

main(argc,argv)
     int argc;
     char *argv[];
{
  FILE *nodefile, *new_nodefile;
  char filename[256];
  char newname[256];
  char buffer[256];
  char new_buffer[256];
  char sys_buffer[256];
  char *ptr;
  int nchar;
  int j = 1;


  strcat(filename,"AGNodeFile");

  if ((nodefile = fopen(filename,"r")) == NULL)
    {
      printf("can't find %s\n",filename);
      return;
    }

  strcpy(filename,"AGNodeFile.new");

  if ((new_nodefile = fopen(filename,"w")) == NULL)
    {
      printf("can't open %s\n",filename);
      return;
    }

  while (fgets(buffer,256,nodefile) != NULL)
    {
      if (j == 100)
	printf("here\n");
      buffer[strlen(buffer)-1] = (char)0;
      ptr = rindex(buffer,'#');
      if (ptr == NULL)
	continue;
      strcpy(filename,ptr+1);
      sprintf(newname,"xag%05d",j++);
      nchar = ptr-buffer+1;
      strncpy(new_buffer,buffer,nchar);
      new_buffer[nchar] = (char)0;
      strcat(new_buffer,newname);
      
      fprintf(new_nodefile,"%s\n",new_buffer);
      
      sprintf(sys_buffer,"mv %s %s\n",filename,newname);
      system(sys_buffer);
    }

}
