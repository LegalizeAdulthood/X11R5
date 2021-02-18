#Change MOTIF_LIBS and MOTIF_INCLUDES for your machine
MOTIF_LIBS = -L/usr/lib/motif
MOTIF_INCLUDES = -I/usr/include/motif

mo_demo : mo_demo.o SelfM.o
	cc -o mo_demo mo_demo.o SelfM.o \
	 $(MOTIF_LIBS) -lXm -lXt -lX11 

mo_demo.o : mo_demo.c SelfM.h
	cc -c $(MOTIF_INCLUDES)  mo_demo.c

SelfM.o : SelfM.c SelfMP.h SelfM.h
	cc -c $(MOTIF_INCLUDES)  SelfM.c

