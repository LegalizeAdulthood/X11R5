#include <stdio.h>
#include "/work/nk/iroha/RK.h"

main(argc, argv)
int argc;
char **argv;
{
  int cx;
  char buf[1024];

  char *userdic;

  if(argc < 2) {
    fprintf(stderr, "usage: %s dic \n", argv[0]);
    exit(1);
  }
  userdic = argv[1];
	    
  if ((cx =RkInitialize("/usr/lib/iroha/dic")) == -1) {
    printf("Init Error\n");
    exit(1);
  }

  RkSetDicPath(cx, "mako:iroha");

  printf("%s を作成します\n", userdic);

  if(RkCreateDic(cx, userdic, 0x80) == -1) {
    printf("%s cannot create\n", userdic);
    exit(1);
  }
    
  if(RkMountDic(cx, userdic, 0) == -1) {
    printf("%s cannot mount\n", userdic);
    exit(1);
  }
    
  if(RkDefineDic(cx, userdic, "けろっぴ #JN #ケロケロケロッピ") != 0) {
    printf("cannot define\n");
    exit(1);
  }

  if(RkBgnBun(cx, "けろっぴ", 8, 0) == -1) {
    printf("cannot bgnbun\n");
    exit(1);
  }

  if(RkGetKanji(cx, buf, 1024) < 0) {
    printf("cannot getkanji\n");
    exit(1);
  }

  printf("結果 <%s>\n", buf);

  RkEndBun(cx, 0);

  RkFinalize();
  
  printf("OK!!\n");
}
