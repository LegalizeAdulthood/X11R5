/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *suiteta_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/apt/suite/RCS/suiteta.c,v 1.18 1991/09/12 15:56:50 bobg Exp $";

/**********************************\
**                                **
**  suite CLIENT/TEST PROGRAM     **
**                                **
\**********************************/

#include  <frame.ih>
#include  <im.ih>
#include  <filetype.ih>
#include  <lpair.ih>
#include  <text.ih>
#include  <textv.ih>
#include  <cursor.ih>
#include  <suite.ih>
#include  <apt.h>
#include  <apts.ih>
#include  <suiteta.eh>

struct view 		 *Name_Choice(), *RW_Hit_Handler(),
			 *First_Test(), *Last_Test(), *Number_Test(), 
			 *Next_Test(),*Prior_Test(), *Quit_Test(),
			 *Text_Object_Handler(), *TextView_Object_Handler();
char			*names[] =
  {
  "Adams, Joe",	"Brown, Sally",   "Crandall, Harry",
  "Donovan, Joe",	"Ellison, Mark",    "Frankfurter, Nancy",
  "Gregson, Tom",	"Henry, Betty",	    "Iverson, Greg",
  "Johansohn, Harry",	"Klein, Larry",	    "Lewis, Donna", "Morgan, Ken",
  "Noonan, Curt",	"Oppenheimer, Sam", "Peterson, Robert",
  "Quiz, Don",		"Robertson, Jack",  "Samuals, Frank",
  "Thompson, Andrew",	"Ullanlong, Waldo", "Victor, Wally",
  "Wallace, Charles",	"Xanadu, Paul",	    "Yellow, Edward",
  "Zweig, Bronson",
  0
  };
char			*alphabet[] =
  { "A","B","C","D","E","F","G","H","I","J","K","L","M",
    "N","O","P","Q","R","S","T","U","V","W","X","Y","Z", NULL };

suite_Specification	 phones_1[] =
  {
  suite_TitleCaption( "Phone Names -- ONE" ),
  suite_TitleBorderStyle( suite_Oval ),
  suite_ItemCaptionList( names ),
  suite_HitHandler( Name_Choice ),
  0
  };

suite_Specification	 phones_2[] =
  {
  suite_TitleCaption( "Phone Names -- TWO" ),
  suite_TitleBorderStyle( suite_Roundangle ),
  suite_TitleFontName( "andy14i" ),
  suite_ItemCaptionList( names ),
  suite_ItemOrder( suite_ColumnMajor ),
  suite_HitHandler( Name_Choice ),
  0
  };

suite_Specification	 phones_3[] =
  {
  suite_TitleCaption( "Phone Names -- THREE" ),
  suite_TitleBorderStyle( suite_Invisible ),
  suite_TitleFontName( "andysans10bi" ),
  suite_ItemCaptionList( names ),
  suite_ItemCaptionFontName( "andysans8" ),
  suite_ItemHighlightStyle( suite_Bold | suite_Italic ),
  suite_ItemOrder( suite_RowMajor ),
  suite_ItemBorderStyle( suite_Invisible ),
  suite_HitHandler( Name_Choice ),
  0
  };

suite_Specification	 phones_4[] =
  {
  suite_TitleCaption( "Phone Names -- FOUR" ),
  suite_TitleFontName( "andy22" ),
  suite_TitleBorderStyle( suite_Line ),
  suite_ItemCaptionList( names ),
  suite_ItemCaptionFontName( "andy16" ),
  suite_ItemHighlightStyle( suite_Border ),
  suite_ItemOrder( suite_ColumnMajor ),
  suite_ItemBorderStyle( suite_Invisible ),
  suite_Scroll( suite_Left ),
  suite_HitHandler( Name_Choice ),
  0
  };

suite_Specification	 phones_5[] =
  {
  suite_TitleCaption( "Phone Names -- FIVE" ),
  suite_TitleFontName( "andysans10bi" ),
  suite_ItemCaptionList( names ),
  suite_ItemCaptionFontName( "andysans8" ),
  suite_ItemHighlightStyle( suite_Bold | suite_Italic ),
  suite_ItemOrder( suite_RowMajor ),
  suite_ItemBorderSize( 3 ),
  suite_BorderSize( 10 ),
/*===  suite_Arrangement( suite_RowLine ),===*/
  suite_SelectionMode( suite_Inclusive ),
  suite_HitHandler( Name_Choice ),
  0
  };

suite_Specification	 phones_6[] =
  {
  suite_TitleCaption( "Phone Names -- SIX" ),
  suite_TitleFontName( "andysans10bi" ),
  suite_TitleBorderStyle( suite_Rectangle ),
  suite_ItemCaptionList( names ),
  suite_ItemCaptionFontName( "andysans10b" ),
  suite_ItemHighlightStyle( suite_Invert ),
  suite_ItemOrder( suite_RowMajor ),
  suite_ItemBorderStyle( suite_Roundangle ),
  suite_HitHandler( Name_Choice ),
  suite_Scroll( suite_Right ),
  0
  };

suite_Specification	 phones_7[] =
  {
  suite_TitleCaption( "Phone Names -- SEVEN" ),
  suite_TitleFontName( "andysans10bi" ),
  suite_TitleBorderStyle( suite_Oval ),
  suite_ItemCaptionList( names ),
  suite_ItemCaptionFontName( "andysans8" ),
  suite_ItemHighlightStyle( suite_Bold | suite_Italic ),
  suite_ItemOrder( suite_RowMajor ),
  suite_ItemBorderStyle( suite_Rectangle ),
  suite_ItemCaptionAlignment( suite_Right | suite_Middle),
  suite_SelectionMode( suite_Inclusive ),
  suite_HitHandler( Name_Choice ),
  0
  };

suite_Specification	 phones_8[] =
  {
  suite_TitleCaption( "Phone Names -- EIGHT" ),
  suite_TitleFontName( "andysans12b" ),
  suite_TitlePlacement( suite_Bottom ),
  suite_TitleBorderSize( 4 ),
  suite_ItemCaptionList( names ),
  suite_ItemCaptionFontName( "andysans8" ),
  suite_ItemHighlightStyle( suite_Bold | suite_Italic ),
  suite_ItemCaptionAlignment( suite_Center | suite_Bottom ),
  suite_ItemBorderStyle( suite_Invisible ),
  suite_HitHandler( Name_Choice ),
  0
  };

suite_Specification	 phones_9[] =
  {
  suite_TitleCaption( "Phone Names -- NINE" ),
  suite_TitleFontName( "andysans10bi" ),
  suite_TitlePlacement( suite_Left ),
  suite_TitleBorderStyle( suite_Roundangle ),
  suite_TitleBorderSize( 7 ),
  suite_ItemCaptionList( names ),
  suite_ItemCaptionFontName( "andysans8" ),
  suite_ItemCaptionAlignment( suite_Left | suite_Middle),
  suite_ItemHighlightStyle( suite_Bold | suite_Italic ),
  suite_ItemBorderStyle( suite_Invisible ),
  suite_HitHandler( Name_Choice ),
  0
  };

suite_Specification	 phones_10[] =
  {
  suite_TitleCaption( "Phone Names -- TEN" ),
  suite_TitleFontName( "andysans10b" ),
  suite_TitlePlacement( suite_Top ),
  suite_TitleBorderStyle( suite_Line ),
  suite_ItemCaptionList( names ),
  suite_ItemCaptionFontName( "andysans8" ),
  suite_ItemCaptionAlignment( suite_Left | suite_Top),
  suite_ItemHighlightStyle( suite_Bold ),
  suite_Arrangement( suite_List | suite_Column ),
  suite_ItemBorderStyle( suite_Invisible ),
  suite_Scroll( suite_Left ),
  suite_HitHandler( Name_Choice ),
  0
  };

suite_Specification	 phones_11[] =
  {
  suite_TitleCaption( "Phone Names -- Eleven" ),
  suite_TitleFontName( "andysans10b" ),
  suite_TitlePlacement( suite_Top ),
  suite_TitleBorderStyle( suite_Roundangle ),
  suite_ItemCaptionList( names ),
  suite_ItemCaptionFontName( "andytype10" ),
  suite_ItemBorderStyle( suite_Invisible ),
  suite_ItemBorderSize( 3 ),
  suite_ItemHighlightStyle( suite_Border ),
  suite_Arrangement( suite_List | suite_Unbalanced ),
  suite_ItemOrder( suite_ColumnMajor ),
  suite_ItemBorderStyle( suite_None ),
  suite_Scroll( suite_Right | suite_Bottom ),
  suite_HitHandler( Name_Choice ),
  0
  };

suite_Specification	rw_item_one_1[] =
  {
  suite_ItemAccessMode( suite_ReadWrite ),
  suite_ItemTitleCaption( "Name:" ),
  suite_ItemTitlePlacement( suite_Left ),
  suite_ItemHitHandler( RW_Hit_Handler ),
  suite_ItemData( "rw_item_one_1" ),
  0
  };
suite_Specification	rw_item_one_2[] =
  {
  suite_ItemAccessMode( suite_ReadWrite ),
  suite_ItemTitleCaption( "Address:" ),
  suite_ItemTitlePlacement( suite_Left ),
  suite_ItemHitHandler( RW_Hit_Handler ),
  suite_ItemData( "rw_item_one_2" ),
  0
  };
suite_Specification	read_write_one[] = 
  {
  suite_TitleCaption( "Read/Write Alpha" ),
  suite_Item( rw_item_one_1 ),
  suite_Item( rw_item_one_2 ),
  0
  };

suite_Specification	rw_item_two_1[] =
  {
  suite_ItemTitleCaption( "ID:" ),
  suite_ItemData( "rw_item_two_1" ),
  suite_ItemHitHandler( RW_Hit_Handler ),
  0
  };
suite_Specification	rw_item_two_2[] =
  {
  suite_ItemTitleCaption( "Serial:" ),
  suite_ItemData( "rw_item_two_2" ),
  suite_ItemHitHandler( RW_Hit_Handler ),
  0
  };
suite_Specification	rw_item_two_3[] =
  {
  suite_ItemTitleCaption( "Rank:" ),
  suite_ItemData( "rw_item_two_3" ),
  suite_ItemHitHandler( RW_Hit_Handler ),
  0
  };
suite_Specification	rw_item_two_4[] =
  {
  suite_ItemTitleCaption( "Weight:" ),
  suite_ItemData( "rw_item_two_4" ),
  suite_ItemHitHandler( RW_Hit_Handler ),
  0
  };
suite_Specification	read_write_two[] = 
  {
  suite_TitleCaption( "Read/Write Beta" ),
  suite_Item( rw_item_two_1 ),
  suite_Item( rw_item_two_2 ),
  suite_Item( rw_item_two_3 ),
  suite_Item( rw_item_two_4 ),
  suite_ItemAccessMode( suite_ReadWrite ),
  suite_ItemTitleFontName( "andysans14b" ),
  suite_ItemTitlePlacement( suite_Top ),
  suite_ItemCaptionFontName( "andy12" ),
  0
  };


suite_Specification	    icons_1[] =
  {
  suite_ItemCaptionFontName( "zipicon20" ),
  suite_ItemCaptionList( alphabet ),
  suite_ItemWidth( 50 ),
  suite_ItemHeight( 50 ),
  suite_Cursor( 'T' ),
  NULL
  };

struct view	*Alphabet_Sort();
suite_Specification	     alphabet_0[] =
  {
  suite_TitleCaption( "Alphabet --- ZERO" ),
  suite_ItemCaptionList( alphabet ),
  suite_HitHandler( Alphabet_Sort ),
  NULL
  };

suite_Specification	     alphabet_1[] =
  {
  suite_TitleCaption( "Alphabet --- ONE" ),
  suite_ItemCaptionList( alphabet ),
  suite_VerticalGutterSize( 3 ),
  suite_Arrangement( suite_RowLine ),
  NULL
  };

suite_Specification	     alphabet_2[] =
  {
  suite_TitleCaption( "Alphabet --- TWO" ),
  suite_ItemCaptionList( alphabet ),
  suite_HorizontalGutterSize( 3 ),
  suite_ItemOrder( suite_ColumnMajor ),
  suite_Arrangement( suite_ColumnLine | suite_Unbalanced ),
  NULL
  };

suite_Specification	     alphabet_3[] =
  {
  suite_TitleCaption( "Alphabet --- THREE" ),
  suite_ItemCaptionList( alphabet ),
  suite_VerticalGutterSize( 3 ),
  suite_HorizontalGutterSize( 3 ),
  suite_Arrangement( suite_RowLine | suite_ColumnLine | suite_Unbalanced ),
  NULL
  };

suite_Specification	     alphabet_4[] =
  {
  suite_TitleCaption( "Alphabet --- FOUR" ),
  suite_TitleBorderStyle( suite_Oval ),
  suite_ItemCaptionList( alphabet ),
  suite_Arrangement( suite_Matrix | suite_Unbalanced ),
  suite_ItemBorderStyle( suite_Circle ),
  suite_ItemWidth( 50 ),
  suite_ItemHeight( 50 ),
  NULL
  };

suite_Specification	     catalog_1_a[] =
  {
  suite_ItemData( 1 ),
  NULL
  };
suite_Specification	     catalog_1_b[] =
  {
  suite_ItemData( 2 ),
  NULL
  };
suite_Specification	     catalog_1[] =
  {
  suite_TitleCaption( "Catalog --- ONE" ),
  suite_ItemDataObjectName( "text" ),
  suite_ItemDataObjectHandler( Text_Object_Handler ),
  suite_ItemViewObjectName( "textview" ),
  suite_ItemViewObjectHandler( TextView_Object_Handler ),
  suite_ItemHighlightStyle( suite_None ),
  suite_CursorType( Cursor_Arrow ),
  suite_Item( catalog_1_a ),
  suite_Item( catalog_1_b ),
  NULL
  };

suite_Specification	    *test_suites[] =
  {
  phones_1,
  phones_2,
  phones_3,
  phones_4,
  phones_5,
  phones_6,
  phones_7,
  phones_8,
  phones_9,
  phones_10,
  phones_11,
  read_write_one,
  read_write_two,
  icons_1,
  alphabet_0,
  alphabet_1,
  alphabet_2,
  alphabet_3,
  alphabet_4,
  catalog_1,
  NULL
  };
long		     current_test = 0, last_test = 20;
struct suite 	    *current_suite;

suite_Specification	     test_suite[] =
  {
  suite_TitleCaption( "Test Suite" ),
  NULL
  };

#define  first		    1
#define  next		    2
#define  prior		    3
#define  last		    4
#define  number		    5
#define  quit		    6

suite_Specification	    button_first[] =
  {
  suite_ItemCaption( "First" ),
  suite_ItemData( first ),
  suite_ItemHitHandler( First_Test ),
  NULL
  };

suite_Specification	    button_next[] =
  {
  suite_ItemCaption( "Next" ),
  suite_ItemData( next ),
  suite_ItemHitHandler( Next_Test ),
  NULL
  };

suite_Specification	    button_prior[] =
  {
  suite_ItemCaption( "Prior" ),
  suite_ItemData( prior ),
  suite_ItemHitHandler( Prior_Test ),
  NULL
  };

suite_Specification	    button_last[] =
  {
  suite_ItemCaption( "Last" ),
  suite_ItemData( last ),
  suite_ItemHitHandler( Last_Test ),
  NULL
  };

suite_Specification	    button_number[] =
  {
  suite_ItemAccessMode( suite_ReadWrite ),
  suite_ItemTitleCaption( "Number:" ),
  suite_ItemTitlePlacement( suite_Left ),
  suite_ItemData( number ),
  suite_ItemHitHandler( Number_Test ),
  NULL
  };

suite_Specification	    button_quit[] =
  {
  suite_ItemCaption( "Quit" ),
  suite_ItemData( quit ),
  suite_ItemHitHandler( Quit_Test ),
  NULL
  };

suite_Specification	    button_suite[] =
  {
  suite_TitleCaption( "Test Case Controller" ),
  suite_TitleCaptionFontName("andysans14b"),
  suite_TitlePlacement( suite_Top ),
  suite_Item( button_first ),
  suite_Item( button_next ),
  suite_Item( button_prior ),
  suite_Item( button_last ),
  suite_Item( button_number ),
  suite_Item( button_quit ),
  NULL
  };

struct lpair	     *pair, *triple;
struct text	     *text_data;
struct textview      *text_view;
struct view	     *button_view, *test_view;

boolean
suitetapp__ParseArgs( self, argc, argv )
  register struct suitetapp	 *self;
  register int			  argc;
  register char			**argv;
  {
  IN(suiteta_ParseArgs);
/*===  super_ParseArgs( self, argc, argv );*/
  while ( *++argv )
    { DEBUGst(ARGV,*argv);
    if ( **argv == '-' )
      {
      if ( strcmp( *argv, "-d" ) == 0 )
        debug = 1;
      else  printf( "SuiteT: ERROR -- Unrecognized switch '%s'\n", *argv );
      }
    }
  OUT(suiteta_ParseArgs);
  return TRUE;
  }


boolean
suitetapp__Start( self )
  struct suitetapp *self;
  {
  FILE			 *file;
  long			  id;
  register struct im	 *im;
  register struct frame  *frame;
  static char		  instructions[] = "Use the Buttons to step through the tests.";

  printf("Begin\n");
  suitetapp_SetFork(self,FALSE);
  button_view = (struct view *) suite_Create( button_suite, self );
  suite_SetDebug((struct suite*)button_view,TRUE);
  printf("Created button_view\n");
  test_view   = (struct view *) suite_Create( test_suite, self );
  suite_SetDebug((struct suite*)test_view,TRUE);
  printf("Created test_view\n");
  text_data = text_New();
  text_InsertCharacters( text_data, 0, instructions, sizeof(instructions)-1 );
  if ( file = fopen( "suiteta.c", "r" ) )
      {
      filetype_Lookup( file, (char *) 0, &id, 0);
      text_Read( text_data, file, id );
      fclose( file );
      }
  text_view = textview_New();
  textview_SetDataObject( text_view, text_data );
  text_view = (struct textview *) textview_GetApplicationLayer( text_view );
  printf("Created text_view\n");
  pair = lpair_New();
  lpair_HSplit( pair, text_view, test_view, 50, 1 );
  printf("Hsplit'ed\n");
  triple = lpair_New();
  lpair_VSplit( triple, pair, button_view, 20, 1 );
  printf("Vsplit'ed\n");
  frame = frame_New();
  if((im = im_Create(NULL)) == NULL) {
      fprintf(stderr,"Could not create new window on this display.\n");
      fprintf(stderr,"exiting.\n");
      exit(-1);
  }
  frame_SetView(frame,triple);
  im_SetView( im, frame );
  printf("Done\n");
  return 1;
  }

struct view *
Change_Test( self )
  struct suitetapp   *self;
  {
  register struct suite  *prior_suite = current_suite;

  printf( "Creating Suite\n" );
  StartTimer;
  current_suite = suite_Create( test_suites[current_test], self );
  if ( current_suite )
    {
    PrintTimer(Creation);
    lpair_SetNth( pair, 1, current_suite );
    suite_SetDebug( current_suite, debug );
    }
    else  printf( "ERROR -- NULL current_suite\n" );
  if ( prior_suite )
    {
    printf( "Destroying Prior Suite\n" );
    suite_Destroy( prior_suite );
    printf( "Destroyed Prior Suite\n" );
    }
  return(NULL);
  }

struct view *
First_Test( self, suite, item, type, action, x, y, clicks )
  struct suitetapp	 *self;
  struct suite		 *suite;
  struct suite_item      *item;
  long			  type;
  enum view_MouseAction	  action;
  long			  x, y, clicks;
  {
  if ( action == view_LeftUp )
    {
    current_test = 0;
    Change_Test( self );
    }
  return(NULL);
  }

struct view *
Last_Test( self, suite, item, type, action, x, y, clicks )
  struct suitetapp	 *self;
  struct suite		 *suite;
  struct suite_item	 *item;
  long			  type;
  enum view_MouseAction	  action;
  long			  x, y, clicks;
  {
  if ( action == view_LeftUp )
    {
    current_test = last_test;
    Change_Test( self );
    }
  return(NULL);
  }

struct view *
Number_Test( self, suite, item, type, action, x, y, clicks )
  struct suitetapp	 *self;
  struct suite		 *suite;
  struct suite_item	 *item;
  long			  type;
  enum view_MouseAction	  action;
  long			  x, y, clicks;
  {
  long			  test;

  printf("Number-Test\n");
  if ( action == view_LeftDown )
    suite_ChangeItemAttribute( suite, item, suite_ItemCaption("") );
    else
    if ( suite_ItemAttribute( suite, item, suite_ItemCaption(0) ) )
    {
    printf( "Number = '%s'\n", suite_ItemAttribute( suite, item, suite_ItemCaption(0) ) );
    test = atoi( suite_ItemAttribute( suite, item, suite_ItemCaption(0) ) );
    printf( "Test = %d\n", test );
    if ( test >= 0  ||  test <= last_test )
      {
      current_test = test;
      Change_Test( self );
      }
    }
  return(NULL);
  }

struct view *
Next_Test( self, suite, item, type, action, x, y, clicks )
  struct suitetapp	 *self;
  struct suite	    	 *suite;
  struct suite_item	 *item;
  long			  type;
  enum view_MouseAction	  action;
  long			  x, y, clicks;
  {
  if ( action == view_LeftUp )
    {
    if ( current_test < last_test  &&  test_suites[current_test+1] )
      {
      current_test++;
      Change_Test( self );
      }
    }
  return(NULL);
  }

struct view *
Prior_Test( self, suite, item, type, action, x, y, clicks )
  struct suitetapp	 *self;
  struct suite		 *suite;
  struct suite_item	 *item;
  long			  type;
  enum view_MouseAction	  action;
  long			  x, y, clicks;
  {
  if ( action == view_LeftUp )
    {
    if ( current_test > 0 )
      {
      current_test--;
      Change_Test( self );
      }
    }
  return(NULL);
  }

struct view *
Quit_Test( self, suite, item, type, action, x, y, clicks )
  struct suitetapp	 *self;
  struct suite		 *suite;
  struct suite_item      *item;
  long			  type;
  enum view_MouseAction	  action;
  long			  x, y, clicks;
  {
  exit(0);
  return(NULL);
  }

struct view *
Name_Choice( self, suite, item, type, action, x, y, clicks )
  struct suitetapp	 *self;
  struct suite		 *suite;
  struct suite_item	 *item;
  long			  type;
  enum view_MouseAction	  action;
  long			  x, y, clicks;
  {
  printf("Name_Choice\n");
  printf( "Title Caption '%s'  Item Caption '%s'\n",
	    suite_SuiteAttribute( suite, suite_TitleCaption(0) ),
	    suite_ItemAttribute( suite, item, suite_ItemCaption(0) ) );
  return(NULL);
  }

struct view *
RW_Hit_Handler( self, suite, item, type, action, x, y, clicks )
  struct suitetapp	 *self;
  struct suite		 *suite;
  struct suite_item	 *item;
  long			  type;
  enum view_MouseAction	  action;
  long			  x, y, clicks;
  {
  printf("RW_Hit_Handler\n");
  printf( "Title Caption '%s'  Item Caption '%s'\n",
	    suite_SuiteAttribute( suite, suite_TitleCaption(0) ),
	    suite_ItemAttribute( suite, item, suite_ItemCaption(0) ) );
  return(NULL);
  }

struct view *
Text_Object_Handler( self, suite, item, type )
  struct suitetapp	 *self;
  struct suite		 *suite;
  struct suite_item	 *item;
  long			  type;
  {
  static char		  words[]= "Hello.  I am a Text Inset; just tickle me to make yourself a believer.  I am not showing myself with a Scrollbar, for I feel that would impinge upon your view of this textual material.  Further, I am seen only as plain text, because that is all I was born with (if my creator had elected to read me from a file, or do the necessary text-operations, I could have Bold, and all the formatted styles.)";
  printf( "Text Handler" );
  printf( "  Item %d\n",
	   suite_ItemAttribute( suite, item, suite_ItemData(0) ) );
  text_InsertCharacters( (struct text *) suite_ItemDataObject( suite, item ),
	    0, words, sizeof(words)-1 );
  return(NULL);
  }

struct view *
TextView_Object_Handler( self, suite, item, type )
  struct suitetapp	 *self;
  struct suite	    	 *suite;
  struct suite_item	 *item;
  long			  type;
  {
  printf( "TextView_Object_Handler  Item %d\n",
	    suite_ItemAttribute( suite, item, suite_ItemData(0) ) );
  textview_SetDataObject( (struct textview *) suite_ItemViewObject( suite, item ),
	    suite_ItemDataObject( suite, item ) );
  return(NULL);
  }

struct view *
Alphabet_Sort( self, suite, item, type, action, x, y, clicks ) 
  struct suitetapp	 *self;
  struct suite		 *suite;
  struct suite_item	 *item;
  long			  type;
  enum view_MouseAction	  action;
  long			  x, y, clicks;
  {
  static long		  sort = suite_Ascend;
  static char		 *sorted, *forward = "Sorted Ascending",
			 *backward = "Sorted Descending";

  printf( "Alphabet_Sort\n" );
  if(action == view_RightUp) {
      if ( sort == suite_Ascend ) {
	  sort = suite_Descend;
	  sorted = backward;
      }
      else {
	  sort = suite_Ascend;
	  sorted = forward;
      }
      suite_Sort( suite, suite_Alphabetic | sort, NULL );
      suite_ChangeSuiteAttribute( suite, suite_TitleCaption(sorted) );
  }
  return(NULL);
  }
