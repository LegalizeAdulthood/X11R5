#include <stdio.h>
#include "/work/nk/iroha/RK.h"

main(argc, argv)
int argc;
char **argv;
{
  int cx;
  char buf[1024];

  char *yomi;

  if(argc < 2) {
    fprintf(stderr, "usage: %s dic \n", argv[0]);
    exit(1);
  }
  yomi = argv[1];

  if ((cx =RkInitialize("/usr/lib/iroha/dic")) == -1) {
    printf("Init Error\n");
    exit(1);
  }

  RkSetDicPath(cx, "mako:iroha");

  if(RkMountDic(cx, "iroha", 0) == -1) {
    printf("iroha cannot mount\n");
    exit(1);
  }
    
  printf("��ߤϡ�����%s\n", yomi);
  if(RkBgnBun(cx, yomi, strlen(yomi), 0) == -1) {
    printf("cannot bgnbun\n");
    exit(1);
  }

  if(RkGetKanji(cx, buf, 1024) < 0) {
    printf("cannot getkanji\n");
    exit(1);
  }

  printf("��� <%s>\n", buf);

  shukushou(cx);
  shukushou(cx);

  RkGetYomi(cx, buf, 1024);
  printf("��� <%s>\n", buf);

  kaeru(cx);

  RkEndBun(cx, 0);

  RkFinalize();
}

shukushou(cx)
int cx;
{
  char buf[1024];

  if(RkShorten(cx) == -1) {
    printf("cannot Shorten\n");
    exit(1);
  }

  if(RkGetKanji(cx, buf, 1024) < 0) {
    printf("cannot getkanji\n");
    exit(1);
  }

  printf("��� S<%s>\n", buf);

  if(RkGetKanjiList(cx, buf, 1024) < 0) {
    printf("cannot getkanjilist\n");
    exit(1);
  }

}

kakudai(cx)
int cx;
{
  char buf[1024];

  if(RkEnlarge(cx) == -1) {
    printf("cannot Enlarge\n");
    exit(1);
  }

  if(RkGetKanji(cx, buf, 1024) < 0) {
    printf("cannot getkanji\n");
    exit(1);
  }

  printf("��� E<%s>\n", buf);
}

kaeru(cx)
int cx;
{
  char buf[1024];

  if(RkResize(cx, 6) == -1) {
    printf("cannot Resize\n");
    exit(1);
  }

  if(RkGetKanji(cx, buf, 1024) < 0) {
    printf("cannot getkanji\n");
    exit(1);
  }

  printf("��� R<%s>\n", buf);
}
