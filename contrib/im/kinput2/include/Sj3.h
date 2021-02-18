/* $Header: Sj3.h,v 1.1 91/09/28 10:23:51 nao Locked $ */

#ifndef _Sj3_h
#define _Sj3_h

#include "InputConv.h"

/*
  Sj3 new resources:

  name          class       type        default     access
  ----------------------------------------------------------------------------
  sj3serv       Sj3serv     String      *1          CG
  sj3serv2      Sj3serv2    String      *2          CG
  sj3user       Sj3user     String      *3          CG
  rkfile        Rkfile      String      *4          CG
  sbfile        Sbfile      String      *5          CG
  rcfile        Rcfile      String      *6          CG
#ifndef HANTOZEN
  hkfile        Hkfile      String      *7          CG
  zhfile        Zhfile      String      *8          CG
#endif 

  note: *1) if not specified, use value of an environment variable "SJ3SERV"
                if not specified an environment "SJ3SERV", read from sjrc file
        *2) if not specified, use value of an environment variable "SJ3SERV2"
                if not specified an environment "SJ3SERV2", read from sjrc file
        *3) if not specified, use value of login name
        *4) if not specified, use value of an environment variable "SJRK"
        *5) if not specified, use value of an environment variable "SJSB"
        *6) if not specified, use value of an environment variable "SJRC"
#ifndef HANTOZEN
        *7) if not specified, use value of an environment variable "SJHK"
        *8) if not specified, use value of an environment variable "SJZH"
#endif 
*/

#define XtNsj3serv      "sj3serv"
#define XtCSj3serv      "Sj3serv"
#define XtNsj3serv2     "sj3serv2"
#define XtCSj3serv2     "Sj3serv2"
#define XtNsj3user      "sj3user"
#define XtCSj3user      "Sj3user"
#define XtNrcfile       "rcfile"
#define XtCRcfile       "Rcfile"
#define XtNsbfile       "sbfile"
#define XtCSbfile       "Sbfile"
#define XtNrkfile       "rkfile"
#define XtCRkfile       "Rkfile"
#ifndef HANTOZEN
#define XtNhkfile       "hkfile"
#define XtCHkfile       "Hkfile"
#define XtNzhfile       "zhfile"
#define XtCZhfile       "Zhfile"
#endif /* HANTOZEN */

typedef struct _Sj3ClassRec *Sj3ObjectClass;
typedef struct _Sj3Rec      *Sj3Object;

extern WidgetClass  sj3ObjectClass;

#endif /* _Sj3_h */

