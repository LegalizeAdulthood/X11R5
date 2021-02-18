#ifndef LIBPIP_H
#define LIBPIP_H
/*
** File:
**
**   libpip.h --- libpip.c header file
**
** Author:
** 
**   David Carver (Digital Workstation Engineering/Project Athena)
**
** Revisions:
**
**   29.08.91 Carver
**     - added support for gray scaled video
**     - changed all interfaces to take RopPtr as first argument
**       RopPtr is defined in rop.h
**
**   Carver --- 04.06.91
**     - original
**
*/

#define PIP_NTSC      (1L<<0)
#define PIP_PAL       (1L<<1)
#define PIP_SECAM     (1L<<2)
#define PIP_COMPOSITE (1L<<3)
#define PIP_SVIDEO    (1L<<4)
#define PIP_RGB       (1L<<5)
#define PIP_GRAY      (1L<<6)

#define PIP_SOURCE_TIMING (PIP_NTSC | PIP_PAL | PIP_SECAM)
#define PIP_SOURCE_TYPE (PIP_COMPOSITE | PIP_SVIDEO | PIP_RGB)

#endif LIBPIP_H
