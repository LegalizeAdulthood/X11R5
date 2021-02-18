/* ********************************************************************** *\
 * Copyright (c) AT&T Bell Laboratories	1990  -	All Rights Reserved	  *
\* ********************************************************************** */

class alink: pushbutton[pshbttn] {
    classprocedures:
      InitializeClass() returns boolean;
      InitializeObject(struct alink *self) returns boolean;
      FinalizeObject(struct alink *self);
    overrides:
      Read (FILE *file, long id) returns long;
      Write (FILE *file, long writeid, int level) returns long;
      SetText(char *txt);
    methods:
      SetAudio(long len, char *audio);
    macromethods:
      GetAudio() (self->audio)
      GetAudioLength() (self->audioLength)
      LabelSetP() (self->label_set)
    data:
      long audioLength;
      char *audio;
      int label_set;
};
