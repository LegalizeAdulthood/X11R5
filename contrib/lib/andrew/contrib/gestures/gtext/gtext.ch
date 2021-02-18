class gtext : text {
    classprocedures:
      InitializeClass() returns boolean;
      InitializeObject( struct gtext *self ) returns boolean;
      FinalizeObject( struct gtext *self ) returns void;
    overrides:
      ViewName() returns char *;
};
