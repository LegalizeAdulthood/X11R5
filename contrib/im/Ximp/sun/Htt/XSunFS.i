





























struct lconv {
	char	*decimal_point;		
	char	*thousands_sep;		
	char	*grouping;		
	char	*int_curr_symbol;	
	char	*currency_symbol;	
	char	*mon_decimal_point;	
	char	*mon_thousands_sep;	
	char	*mon_grouping;		
	char	*positive_sign;		
	char	*negative_sign;		
	char	int_frac_digits;	
	char	frac_digits;		
	char	p_cs_precedes;		
	char	p_sep_by_space;		
	char	n_cs_precedes;		
	char	n_sep_by_space;		
	char	p_sign_posn;		
	char	n_sign_posn;		
};




struct dtconv {
	char	*abbrev_month_names[12];	
	char	*month_names[12];	
	char	*abbrev_weekday_names[7];	
	char	*weekday_names[7];	
	char	*time_format;	
	char	*sdate_format;	
	char	*dtime_format;	
	char	*am_string;	
	char	*pm_string;	
	char	*ldate_format;	
};




struct langinfo {
	char *yesstr;	
	char *nostr;	
};










extern char *setlocale();
extern struct lconv *localeconv();
extern struct dtconv *localdtconv();


































typedef unsigned short wchar_t;












typedef	int		sigset_t;	

typedef	unsigned int	speed_t;	
typedef	unsigned long	tcflag_t;	
typedef	unsigned char	cc_t;		
typedef	int		pid_t;		

typedef	unsigned short	mode_t;		
typedef	short		nlink_t;	

typedef	long		clock_t;	
typedef	long		time_t;		

typedef	int		size_t;		
typedef int		ptrdiff_t;	







extern char *	strcat();
extern char *	strchr();
extern int	strcmp();
extern char *	strcpy();
extern size_t	strcspn();

extern char *	strdup();

extern size_t	strlen();
extern char *	strncat();
extern int	strncmp();
extern char *	strncpy();
extern char *	strpbrk();
extern char *	strrchr();
extern size_t	strspn();
extern char *	strstr();
extern char *	strtok();






















































































typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;
typedef	unsigned short	ushort;		
typedef	unsigned int	uint;		





typedef	struct  _physadr_t { int r[1]; } *physadr_t;
typedef	struct label_t {
	int	val[2];
} label_t;


typedef	struct	_quad_t { long val[2]; } quad_t;
typedef	long	daddr_t;
typedef	char *	caddr_t;
typedef	unsigned long	ino_t;
typedef	short	dev_t;
typedef	long	off_t;
typedef	unsigned short	uid_t;
typedef	unsigned short	gid_t;
typedef	long	key_t;
typedef	char *	addr_t;














typedef	long	fd_mask;







typedef	struct fd_set {
	fd_mask	fds_bits[(((256)+(( (sizeof (fd_mask) * 8))-1))/( (sizeof (fd_mask) * 8)))];
} fd_set;


















































typedef unsigned long XID;

typedef XID Window;
typedef XID Drawable;
typedef XID Font;
typedef XID Pixmap;
typedef XID Cursor;
typedef XID Colormap;
typedef XID GContext;
typedef XID KeySym;

typedef unsigned long Mask;

typedef unsigned long Atom;

typedef unsigned long VisualID;

typedef unsigned long Time;

typedef unsigned char KeyCode;

































































































































































		       

































































































































































































































































































































































































































































































































































































typedef char *XPointer;



































































typedef struct _XExtData {
	int number;		
	struct _XExtData *next;	
	int (*free_private)();	
	XPointer private_data;	
} XExtData;




typedef struct {		
	int extension;		
	int major_opcode;	
	int first_event;	
	int first_error;	
} XExtCodes;





typedef struct {
    int depth;
    int bits_per_pixel;
    int scanline_pad;
} XPixmapFormatValues;





typedef struct {
	int function;		
	unsigned long plane_mask;
	unsigned long foreground;
	unsigned long background;
	int line_width;		
	int line_style;	 	
	int cap_style;	  	

	int join_style;	 	
	int fill_style;	 	

	int fill_rule;	  	
	int arc_mode;		
	Pixmap tile;		
	Pixmap stipple;		
	int ts_x_origin;	
	int ts_y_origin;
        Font font;	        
	int subwindow_mode;     
	int graphics_exposures;
	int clip_x_origin;	
	int clip_y_origin;
	Pixmap clip_mask;	
	int dash_offset;	
	char dashes;
} XGCValues;








typedef struct _XGC {
    XExtData *ext_data;	
    GContext gid;	
    int rects;		
    int dashes;	
    unsigned long dirty;
    XGCValues values;	
} *GC;





typedef struct {
	XExtData *ext_data;	
	VisualID visualid;	

	int class;		

	unsigned long red_mask, green_mask, blue_mask;	
	int bits_per_rgb;	
	int map_entries;	
} Visual;



	
typedef struct {
	int depth;		
	int nvisuals;		
	Visual *visuals;	
} Depth;




typedef struct {
	XExtData *ext_data;	
	struct _XDisplay *display;
	Window root;		
	int width, height;	
	int mwidth, mheight;	
	int ndepths;		
	Depth *depths;		
	int root_depth;		
	Visual *root_visual;	
	GC default_gc;		
	Colormap cmap;		
	unsigned long white_pixel;
	unsigned long black_pixel;	
	int max_maps, min_maps;	
	int backing_store;	
	int save_unders;	
	long root_input_mask;	
} Screen;




typedef struct {
	XExtData *ext_data;	
	int depth;		
	int bits_per_pixel;	
	int scanline_pad;	
} ScreenFormat;






typedef struct {
    Pixmap background_pixmap;	
    unsigned long background_pixel;	
    Pixmap border_pixmap;	
    unsigned long border_pixel;	
    int bit_gravity;		
    int win_gravity;		
    int backing_store;		
    unsigned long backing_planes;
    unsigned long backing_pixel;
    int save_under;		
    long event_mask;		
    long do_not_propagate_mask;	
    int override_redirect;	
    Colormap colormap;		
    Cursor cursor;		
} XSetWindowAttributes;

typedef struct {
    int x, y;			
    int width, height;		
    int border_width;		
    int depth;          	
    Visual *visual;		
    Window root;        	

    int class;			

    int bit_gravity;		
    int win_gravity;		
    int backing_store;		
    unsigned long backing_planes;
    unsigned long backing_pixel;
    int save_under;		
    Colormap colormap;		
    int map_installed;		
    int map_state;		
    long all_event_masks;	
    long your_event_mask;	
    long do_not_propagate_mask; 
    int override_redirect;	
    Screen *screen;		
} XWindowAttributes;






typedef struct {
	int family;		
	int length;		
	char *address;		
} XHostAddress;




typedef struct _XImage {
    int width, height;		
    int xoffset;		
    int format;			
    char *data;			
    int byte_order;		
    int bitmap_unit;		
    int bitmap_bit_order;	
    int bitmap_pad;		
    int depth;			
    int bytes_per_line;		
    int bits_per_pixel;		
    unsigned long red_mask;	
    unsigned long green_mask;
    unsigned long blue_mask;
    XPointer obdata;		
    struct funcs {		
	struct _XImage *(*create_image)();

	int (*destroy_image)();
	unsigned long (*get_pixel)();
	int (*put_pixel)();
	struct _XImage *(*sub_image)();
	int (*add_pixel)();

	} f;
} XImage;




typedef struct {
    int x, y;
    int width, height;
    int border_width;
    Window sibling;
    int stack_mode;
} XWindowChanges;




typedef struct {
	unsigned long pixel;
	unsigned short red, green, blue;
	char flags;  
	char pad;
} XColor;






typedef struct {
    short x1, y1, x2, y2;
} XSegment;

typedef struct {
    short x, y;
} XPoint;
    
typedef struct {
    short x, y;
    unsigned short width, height;
} XRectangle;
    
typedef struct {
    short x, y;
    unsigned short width, height;
    short angle1, angle2;
} XArc;




typedef struct {
        int key_click_percent;
        int bell_percent;
        int bell_pitch;
        int bell_duration;
        int led;
        int led_mode;
        int key;
        int auto_repeat_mode;   
} XKeyboardControl;



typedef struct {
        int key_click_percent;
	int bell_percent;
	unsigned int bell_pitch, bell_duration;
	unsigned long led_mask;
	int global_auto_repeat;
	char auto_repeats[32];
} XKeyboardState;



typedef struct {
        Time time;
	short x, y;
} XTimeCoord;



typedef struct {
 	int max_keypermod;	
 	KeyCode *modifiermap;	
} XModifierKeymap;






typedef struct _XDisplay {
	XExtData *ext_data;	
	struct _XFreeFuncs *free_funcs; 
	int fd;			
	int lock;		
	int proto_major_version;
	int proto_minor_version;
	char *vendor;		
        XID resource_base;	
	XID resource_mask;	
	XID resource_id;	
	int resource_shift;	
	XID (*resource_alloc)(); 
	int byte_order;		
	int bitmap_unit;	
	int bitmap_pad;		
	int bitmap_bit_order;	
	int nformats;		
	ScreenFormat *pixmap_format;	
	int vnumber;		
	int release;		
	struct _XSQEvent *head, *tail;	
	int qlen;		
	unsigned long last_request_read; 
	unsigned long request;	
	char *last_req;		
	char *buffer;		
	char *bufptr;		
	char *bufmax;		
	unsigned max_request_size; 
	struct _XrmHashBucketRec *db;
	int (*synchandler)();	
	char *display_name;	
	int default_screen;	
	int nscreens;		
	Screen *screens;	
	unsigned long motion_buffer;	
	Window current;		
	int min_keycode;	
	int max_keycode;	
	KeySym *keysyms;	
	XModifierKeymap *modifiermap;	
	int keysyms_per_keycode;
	char *xdefaults;	
	char *scratch_buffer;	
	unsigned long scratch_length;	
	int ext_number;		
	struct _XExten *ext_procs; 
	







	int (*event_vec[128])();  
	int (*wire_vec[128])(); 
	KeySym lock_meaning;	   
	struct _XKeytrans *key_bindings; 
	Font cursor_font;	   
	struct _XDisplayAtoms *atoms; 
	struct {		   
	    long sequence_number;
	    int (*old_handler)();
	    int succeeded;
	} reconfigure_wm_window;
	unsigned long flags;	   
	unsigned int mode_switch;  
	struct _XContextDB *context_db; 
	int (**error_vec)();      
	




	struct {
	   XPointer defaultCCCs;  
	   XPointer clientCmaps;  
	} cms;
	struct _XIMFilter *im_filters;
} Display;
























typedef struct {
	int type;		
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;	        
	Window root;	        
	Window subwindow;	
	Time time;		
	int x, y;		
	int x_root, y_root;	
	unsigned int state;	
	unsigned int keycode;	
	int same_screen;	
} XKeyEvent;
typedef XKeyEvent XKeyPressedEvent;
typedef XKeyEvent XKeyReleasedEvent;

typedef struct {
	int type;		
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;	        
	Window root;	        
	Window subwindow;	
	Time time;		
	int x, y;		
	int x_root, y_root;	
	unsigned int state;	
	unsigned int button;	
	int same_screen;	
} XButtonEvent;
typedef XButtonEvent XButtonPressedEvent;
typedef XButtonEvent XButtonReleasedEvent;

typedef struct {
	int type;		
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;	        
	Window root;	        
	Window subwindow;	
	Time time;		
	int x, y;		
	int x_root, y_root;	
	unsigned int state;	
	char is_hint;		
	int same_screen;	
} XMotionEvent;
typedef XMotionEvent XPointerMovedEvent;

typedef struct {
	int type;		
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;	        
	Window root;	        
	Window subwindow;	
	Time time;		
	int x, y;		
	int x_root, y_root;	
	int mode;		
	int detail;
	



	int same_screen;	
	int focus;		
	unsigned int state;	
} XCrossingEvent;
typedef XCrossingEvent XEnterWindowEvent;
typedef XCrossingEvent XLeaveWindowEvent;

typedef struct {
	int type;		
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;		
	int mode;		
	int detail;
	




} XFocusChangeEvent;
typedef XFocusChangeEvent XFocusInEvent;
typedef XFocusChangeEvent XFocusOutEvent;


typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;
	char key_vector[32];
} XKeymapEvent;	

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;
	int x, y;
	int width, height;
	int count;		
} XExposeEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Drawable drawable;
	int x, y;
	int width, height;
	int count;		
	int major_code;		
	int minor_code;		
} XGraphicsExposeEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Drawable drawable;
	int major_code;		
	int minor_code;		
} XNoExposeEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;
	int state;		
} XVisibilityEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window parent;		
	Window window;		
	int x, y;		
	int width, height;	
	int border_width;	
	int override_redirect;	
} XCreateWindowEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window event;
	Window window;
} XDestroyWindowEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window event;
	Window window;
	int from_configure;
} XUnmapEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window event;
	Window window;
	int override_redirect;	
} XMapEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window parent;
	Window window;
} XMapRequestEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window event;
	Window window;
	Window parent;
	int x, y;
	int override_redirect;
} XReparentEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window event;
	Window window;
	int x, y;
	int width, height;
	int border_width;
	Window above;
	int override_redirect;
} XConfigureEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window event;
	Window window;
	int x, y;
} XGravityEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;
	int width, height;
} XResizeRequestEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window parent;
	Window window;
	int x, y;
	int width, height;
	int border_width;
	Window above;
	int detail;		
	unsigned long value_mask;
} XConfigureRequestEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window event;
	Window window;
	int place;		
} XCirculateEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window parent;
	Window window;
	int place;		
} XCirculateRequestEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;
	Atom atom;
	Time time;
	int state;		
} XPropertyEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;
	Atom selection;
	Time time;
} XSelectionClearEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window owner;
	Window requestor;
	Atom selection;
	Atom target;
	Atom property;
	Time time;
} XSelectionRequestEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window requestor;
	Atom selection;
	Atom target;
	Atom property;		
	Time time;
} XSelectionEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;
	Colormap colormap;	

	int new;

	int state;		
} XColormapEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;
	Atom message_type;
	int format;
	union {
		char b[20];
		short s[10];
		long l[5];
		} data;
} XClientMessageEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;		
	int request;		

	int first_keycode;	
	int count;		
} XMappingEvent;

typedef struct {
	int type;
	Display *display;	
	XID resourceid;		
	unsigned long serial;	
	unsigned char error_code;	
	unsigned char request_code;	
	unsigned char minor_code;	
} XErrorEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;
	Window window;	
} XAnyEvent;





typedef union _XEvent {
        int type;		
	XAnyEvent xany;
	XKeyEvent xkey;
	XButtonEvent xbutton;
	XMotionEvent xmotion;
	XCrossingEvent xcrossing;
	XFocusChangeEvent xfocus;
	XExposeEvent xexpose;
	XGraphicsExposeEvent xgraphicsexpose;
	XNoExposeEvent xnoexpose;
	XVisibilityEvent xvisibility;
	XCreateWindowEvent xcreatewindow;
	XDestroyWindowEvent xdestroywindow;
	XUnmapEvent xunmap;
	XMapEvent xmap;
	XMapRequestEvent xmaprequest;
	XReparentEvent xreparent;
	XConfigureEvent xconfigure;
	XGravityEvent xgravity;
	XResizeRequestEvent xresizerequest;
	XConfigureRequestEvent xconfigurerequest;
	XCirculateEvent xcirculate;
	XCirculateRequestEvent xcirculaterequest;
	XPropertyEvent xproperty;
	XSelectionClearEvent xselectionclear;
	XSelectionRequestEvent xselectionrequest;
	XSelectionEvent xselection;
	XColormapEvent xcolormap;
	XClientMessageEvent xclient;
	XMappingEvent xmapping;
	XErrorEvent xerror;
	XKeymapEvent xkeymap;
	long pad[24];
} XEvent;







typedef struct {
    short	lbearing;	
    short	rbearing;	
    short	width;		
    short	ascent;		
    short	descent;	
    unsigned short attributes;	
} XCharStruct;





typedef struct {
    Atom name;
    unsigned long card32;
} XFontProp;

typedef struct {
    XExtData	*ext_data;	
    Font        fid;            
    unsigned	direction;	
    unsigned	min_char_or_byte2;
    unsigned	max_char_or_byte2;
    unsigned	min_byte1;	
    unsigned	max_byte1;	
    int	all_chars_exist;
    unsigned	default_char;	
    int         n_properties;   
    XFontProp	*properties;	
    XCharStruct	min_bounds;	
    XCharStruct	max_bounds;	
    XCharStruct	*per_char;	
    int		ascent;		
    int		descent;	
} XFontStruct;




typedef struct {
    char *chars;		
    int nchars;			
    int delta;			
    Font font;			
} XTextItem;

typedef struct {		
    unsigned char byte1;
    unsigned char byte2;
} XChar2b;

typedef struct {
    XChar2b *chars;		
    int nchars;			
    int delta;			
    Font font;			
} XTextItem16;


typedef union { Display *display;
		GC gc;
		Visual *visual;
		Screen *screen;
		ScreenFormat *pixmap_format;
		XFontStruct *font; } XEDataObject;

typedef struct {
    XRectangle      max_ink_extent;
    XRectangle      max_logical_extent;
} XFontSetExtents;

typedef struct _XFontSet *XFontSet;

typedef struct {
    char           *chars;
    int             nchars;
    int             delta;
    XFontSet        font_set;
} XmbTextItem;

typedef struct {
    wchar_t        *chars;
    int             nchars;
    int             delta;
    XFontSet        font_set;
} XwcTextItem;

typedef void (*XIMProc)();

typedef struct _XIM *XIM;
typedef struct _XIC *XIC;

typedef unsigned long XIMStyle;

typedef struct {
    unsigned short count_styles;
    XIMStyle *supported_styles;
} XIMStyles;
















































typedef XPointer XVaNestedList;


typedef struct {
    XPointer client_data;
    XIMProc callback;
} XIMCallback;

typedef unsigned long XIMFeedback;








typedef struct _XIMText {
    unsigned short length;
    XIMFeedback *feedback;
    int encoding_is_wchar; 
    union {
	char *multi_byte;
	wchar_t *wide_char;
    } string; 
} XIMText;

typedef struct _XIMPreeditDrawCallbackStruct {
    int caret;		
    int chg_first;	
    int chg_length;	
    XIMText *text;
} XIMPreeditDrawCallbackStruct;

typedef enum {
    XIMForwardChar, XIMBackwardChar,
    XIMForwardWord, XIMBackwardWord,
    XIMCaretUp, XIMCaretDown,
    XIMNextLine, XIMPreviousLine,
    XIMLineStart, XIMLineEnd, 
    XIMAbsolutePosition,
    XIMDontChange
} XIMCaretDirection;

typedef enum {
    XIMIsInvisible,	 
    XIMIsPrimary,	
    XIMIsSecondary	
} XIMCaretStyle;

typedef struct _XIMPreeditCaretCallbackStruct {
    int position;		 
    XIMCaretDirection direction; 
    XIMCaretStyle style;	 
} XIMPreeditCaretCallbackStruct;

typedef enum {
    XIMTextType,
    XIMBitmapType
} XIMStatusDataType;
	
typedef struct _XIMStatusDrawCallbackStruct {
    XIMStatusDataType type;
    union {
	XIMText *text;
	Pixmap  bitmap;
    } data;
} XIMStatusDrawCallbackStruct;



extern XFontStruct *XLoadQueryFont(

);

extern XFontStruct *XQueryFont(

);


extern XTimeCoord *XGetMotionEvents(

);

extern XModifierKeymap *XDeleteModifiermapEntry(

);

extern XModifierKeymap	*XGetModifierMapping(

);

extern XModifierKeymap	*XInsertModifiermapEntry(

);

extern XModifierKeymap *XNewModifiermap(

);

extern XImage *XCreateImage(

);
extern XImage *XGetImage(

);
extern XImage *XGetSubImage(

);





extern Display *XOpenDisplay(

);

extern void XrmInitialize(

);

extern char *XFetchBytes(

);
extern char *XFetchBuffer(

);
extern char *XGetAtomName(

);
extern char *XGetDefault(

);
extern char *XDisplayName(

);
extern char *XKeysymToString(

);

extern int (*XSynchronize(

))();
extern int (*XSetAfterFunction(

))();
extern Atom XInternAtom(

);
extern Colormap XCopyColormapAndFree(

);
extern Colormap XCreateColormap(

);
extern Cursor XCreatePixmapCursor(

);
extern Cursor XCreateGlyphCursor(

);
extern Cursor XCreateFontCursor(

);
extern Font XLoadFont(

);
extern GC XCreateGC(

);
extern GContext XGContextFromGC(

);
extern Pixmap XCreatePixmap(

);
extern Pixmap XCreateBitmapFromData(

);
extern Pixmap XCreatePixmapFromBitmapData(

);
extern Window XCreateSimpleWindow(

);
extern Window XGetSelectionOwner(

);
extern Window XCreateWindow(

); 
extern Colormap *XListInstalledColormaps(

);
extern char **XListFonts(

);
extern char **XListFontsWithInfo(

);
extern char **XGetFontPath(

);
extern char **XListExtensions(

);
extern Atom *XListProperties(

);
extern XHostAddress *XListHosts(

);
extern KeySym XKeycodeToKeysym(

);
extern KeySym XLookupKeysym(

);
extern KeySym *XGetKeyboardMapping(

);
extern KeySym XStringToKeysym(

);
extern long XMaxRequestSize(

);
extern char *XResourceManagerString(

);
extern char *XScreenResourceString(

);
extern unsigned long XDisplayMotionBufferSize(

);
extern VisualID XVisualIDFromVisual(

);



extern XExtCodes *XInitExtension(

);

extern XExtCodes *XAddExtension(

);
extern XExtData *XFindOnExtensionList(

);
extern XExtData **XEHeadOfExtensionList(

);


extern Window XRootWindow(

);
extern Window XDefaultRootWindow(

);
extern Window XRootWindowOfScreen(

);
extern Visual *XDefaultVisual(

);
extern Visual *XDefaultVisualOfScreen(

);
extern GC XDefaultGC(

);
extern GC XDefaultGCOfScreen(

);
extern unsigned long XBlackPixel(

);
extern unsigned long XWhitePixel(

);
extern unsigned long XAllPlanes(

);
extern unsigned long XBlackPixelOfScreen(

);
extern unsigned long XWhitePixelOfScreen(

);
extern unsigned long XNextRequest(

);
extern unsigned long XLastKnownRequestProcessed(

);
extern char *XServerVendor(

);
extern char *XDisplayString(

);
extern Colormap XDefaultColormap(

);
extern Colormap XDefaultColormapOfScreen(

);
extern Display *XDisplayOfScreen(

);
extern Screen *XScreenOfDisplay(

);
extern Screen *XDefaultScreenOfDisplay(

);
extern long XEventMaskOfScreen(

);

extern int XScreenNumberOfScreen(

);

typedef int (*XErrorHandler) (	    

);

extern XErrorHandler XSetErrorHandler (

);


typedef int (*XIOErrorHandler) (    

);

extern XIOErrorHandler XSetIOErrorHandler (

);


extern XPixmapFormatValues *XListPixmapFormats(

);
extern int *XListDepths(

);



extern int XReconfigureWMWindow(

);

extern int XGetWMProtocols(

);
extern int XSetWMProtocols(

);
extern int XIconifyWindow(

);
extern int XWithdrawWindow(

);
extern int XGetCommand(

);
extern int XGetWMColormapWindows(

);
extern int XSetWMColormapWindows(

);
extern void XFreeStringList(

);
extern XSetTransientForHint(

);



extern XActivateScreenSaver(

);

extern XAddHost(

);

extern XAddHosts(

);

extern XAddToExtensionList(

);

extern XAddToSaveSet(

);

extern int XAllocColor(

);

extern int XAllocColorCells(

);

extern int XAllocColorPlanes(

);

extern int XAllocNamedColor(

);

extern XAllowEvents(

);

extern XAutoRepeatOff(

);

extern XAutoRepeatOn(

);

extern XBell(

);

extern int XBitmapBitOrder(

);

extern int XBitmapPad(

);

extern int XBitmapUnit(

);

extern int XCellsOfScreen(

);

extern XChangeActivePointerGrab(

);

extern XChangeGC(

);

extern XChangeKeyboardControl(

);

extern XChangeKeyboardMapping(

);

extern XChangePointerControl(

);

extern XChangeProperty(

);

extern XChangeSaveSet(

);

extern XChangeWindowAttributes(

);

extern int XCheckIfEvent(

);

extern int XCheckMaskEvent(

);

extern int XCheckTypedEvent(

);

extern int XCheckTypedWindowEvent(

);

extern int XCheckWindowEvent(

);

extern XCirculateSubwindows(

);

extern XCirculateSubwindowsDown(

);

extern XCirculateSubwindowsUp(

);

extern XClearArea(

);

extern XClearWindow(

);

extern XCloseDisplay(

);

extern XConfigureWindow(

);

extern int XConnectionNumber(

);

extern XConvertSelection(

);

extern XCopyArea(

);

extern XCopyGC(

);

extern XCopyPlane(

);

extern int XDefaultDepth(

);

extern int XDefaultDepthOfScreen(

);

extern int XDefaultScreen(

);

extern XDefineCursor(

);

extern XDeleteProperty(

);

extern XDestroyWindow(

);

extern XDestroySubwindows(

);

extern int XDoesBackingStore(

);

extern int XDoesSaveUnders(

);

extern XDisableAccessControl(

);


extern int XDisplayCells(

);

extern int XDisplayHeight(

);

extern int XDisplayHeightMM(

);

extern XDisplayKeycodes(

);

extern int XDisplayPlanes(

);

extern int XDisplayWidth(

);

extern int XDisplayWidthMM(

);

extern XDrawArc(

);

extern XDrawArcs(

);

extern XDrawImageString(

);

extern XDrawImageString16(

);

extern XDrawLine(

);

extern XDrawLines(

);

extern XDrawPoint(

);

extern XDrawPoints(

);

extern XDrawRectangle(

);

extern XDrawRectangles(

);

extern XDrawSegments(

);

extern XDrawString(

);

extern XDrawString16(

);

extern XDrawText(

);

extern XDrawText16(

);

extern XEnableAccessControl(

);

extern int XEventsQueued(

);

extern int XFetchName(

);

extern XFillArc(

);

extern XFillArcs(

);

extern XFillPolygon(

);

extern XFillRectangle(

);

extern XFillRectangles(

);

extern XFlush(

);

extern XForceScreenSaver(

);

extern XFree(

);

extern XFreeColormap(

);

extern XFreeColors(

);

extern XFreeCursor(

);

extern XFreeExtensionList(

);

extern XFreeFont(

);

extern XFreeFontInfo(

);

extern XFreeFontNames(

);

extern XFreeFontPath(

);

extern XFreeGC(

);

extern XFreeModifiermap(

);

extern XFreePixmap(

);

extern int XGeometry(

);

extern XGetErrorDatabaseText(

);

extern XGetErrorText(

);

extern int XGetFontProperty(

);

extern int XGetGCValues(

);

extern int XGetGeometry(

);

extern int XGetIconName(

);

extern XGetInputFocus(

);

extern XGetKeyboardControl(

);

extern XGetPointerControl(

);

extern int XGetPointerMapping(

);

extern XGetScreenSaver(

);

extern int XGetTransientForHint(

);

extern int XGetWindowProperty(

);

extern int XGetWindowAttributes(

);

extern XGrabButton(

);

extern XGrabKey(

);

extern int XGrabKeyboard(

);

extern int XGrabPointer(

);

extern XGrabServer(

);

extern int XHeightMMOfScreen(

);

extern int XHeightOfScreen(

);

extern XIfEvent(

);

extern int XImageByteOrder(

);

extern XInstallColormap(

);

extern KeyCode XKeysymToKeycode(

);

extern XKillClient(

);

extern unsigned long XLastKnownRequestProcessed(

);

extern int XLookupColor(

);

extern XLowerWindow(

);

extern XMapRaised(

);

extern XMapSubwindows(

);

extern XMapWindow(

);

extern XMaskEvent(

);

extern int XMaxCmapsOfScreen(

);

extern int XMinCmapsOfScreen(

);

extern XMoveResizeWindow(

);

extern XMoveWindow(

);

extern XNextEvent(

);

extern XNoOp(

);

extern int XParseColor(

);

extern int XParseGeometry(

);

extern XPeekEvent(

);

extern XPeekIfEvent(

);

extern int XPending(

);

extern int XPlanesOfScreen(

);

extern int XProtocolRevision(

);

extern int XProtocolVersion(

);


extern XPutBackEvent(

);

extern XPutImage(

);

extern int XQLength(

);

extern int XQueryBestCursor(

);

extern int XQueryBestSize(

);

extern int XQueryBestStipple(

);

extern int XQueryBestTile(

);

extern XQueryColor(

);

extern XQueryColors(

);

extern int XQueryExtension(

);

extern XQueryKeymap(

);

extern int XQueryPointer(

);

extern XQueryTextExtents(

);

extern XQueryTextExtents16(

);

extern int XQueryTree(

);

extern XRaiseWindow(

);

extern int XReadBitmapFile(

);

extern XRebindKeysym(

);

extern XRecolorCursor(

);

extern XRefreshKeyboardMapping(

);

extern XRemoveFromSaveSet(

);

extern XRemoveHost(

);

extern XRemoveHosts(

);

extern XReparentWindow(

);

extern XResetScreenSaver(

);

extern XResizeWindow(

);

extern XRestackWindows(

);

extern XRotateBuffers(

);

extern XRotateWindowProperties(

);

extern int XScreenCount(

);

extern XSelectInput(

);

extern int XSendEvent(

);

extern XSetAccessControl(

);

extern XSetArcMode(

);

extern XSetBackground(

);

extern XSetClipMask(

);

extern XSetClipOrigin(

);

extern XSetClipRectangles(

);

extern XSetCloseDownMode(

);

extern XSetCommand(

);

extern XSetDashes(

);

extern XSetFillRule(

);

extern XSetFillStyle(

);

extern XSetFont(

);

extern XSetFontPath(

);

extern XSetForeground(

);

extern XSetFunction(

);

extern XSetGraphicsExposures(

);

extern XSetIconName(

);

extern XSetInputFocus(

);

extern XSetLineAttributes(

);

extern int XSetModifierMapping(

);

extern XSetPlaneMask(

);

extern int XSetPointerMapping(

);

extern XSetScreenSaver(

);

extern XSetSelectionOwner(

);

extern XSetState(

);

extern XSetStipple(

);

extern XSetSubwindowMode(

);

extern XSetTSOrigin(

);

extern XSetTile(

);

extern XSetWindowBackground(

);

extern XSetWindowBackgroundPixmap(

);

extern XSetWindowBorder(

);

extern XSetWindowBorderPixmap(

);

extern XSetWindowBorderWidth(

);

extern XSetWindowColormap(

);

extern XStoreBuffer(

);

extern XStoreBytes(

);

extern XStoreColor(

);

extern XStoreColors(

);

extern XStoreName(

);

extern XStoreNamedColor(

);

extern XSync(

);

extern XTextExtents(

);

extern XTextExtents16(

);

extern int XTextWidth(

);

extern int XTextWidth16(

);

extern int XTranslateCoordinates(

);

extern XUndefineCursor(

);

extern XUngrabButton(

);

extern XUngrabKey(

);

extern XUngrabKeyboard(

);

extern XUngrabPointer(

);

extern XUngrabServer(

);

extern XUninstallColormap(

);

extern XUnloadFont(

);

extern XUnmapSubwindows(

);

extern XUnmapWindow(

);

extern int XVendorRelease(

);

extern XWarpPointer(

);

extern int XWidthMMOfScreen(

);

extern int XWidthOfScreen(

);

extern XWindowEvent(

);

extern int XWriteBitmapFile(

);

extern int XSupportsLocale(

);

extern char *XSetLocaleModifiers(

);

extern XFontSet XCreateFontSet(

);

extern void XFreeFontSet(

);

extern void XFreeStringList(

);

extern int XFontsOfFontSet(

);

extern char *XBaseFontNameListOfFontSet(

);

extern char *XLocaleOfFontSet(

);

extern XFontSetExtents *XExtentsOfFontSet(

);

extern int XmbTextEscapement(

);

extern int XwcTextEscapement(

);

extern int XmbTextExtents(

);

extern int XwcTextExtents(

);

extern int XmbTextPerCharExtents(

);

extern int XwcTextPerCharExtents(

);

extern void XmbDrawText(

);

extern void XwcDrawText(

);

extern void XmbDrawString(

);

extern void XwcDrawString(

);

extern void XmbDrawImageString(

);

extern void XwcDrawImageString(

);

extern XIM XOpenIM(

);

extern int XCloseIM(

);

extern char *XGetIMValues(

);

extern Display *XDisplayOfIM(

);

extern char *XLocaleOfIM(

);

extern XIC XCreateIC(

);

extern void XDestroyIC(

);

extern void XSetICFocus(

);

extern void XUnsetICFocus(

);

extern wchar_t *XwcResetIC(

);

extern char *XmbResetIC(

);

extern char *XSetICValues(

);

extern char *XGetICValues(

);

extern XIM XIMOfIC(

);

extern void XRegisterFilter(

);

extern void XUnregisterFilter(

);

extern int XFilterEvent(

);

extern int XmbLookupString(

);

extern int XwcLookupString(

);

extern XVaNestedList XVaCreateNestedList(

);




























































































































































































typedef long           INT32;
typedef short          INT16;

typedef char           INT8;


typedef unsigned long CARD32;
typedef unsigned short CARD16;
typedef unsigned char  CARD8;

typedef unsigned long		BITS32;
typedef unsigned short		BITS16;
typedef unsigned char		BYTE;

typedef unsigned char            BOOL;























  


























































typedef struct _xSegment {
    INT16 x1 , y1 , x2 , y2 ;
} xSegment;



typedef struct _xPoint {
	INT16		x , y ;
} xPoint;

typedef struct _xRectangle {
    INT16 x , y ;
    CARD16  width , height ;
} xRectangle;



typedef struct _xArc {
    INT16 x , y ;
    CARD16   width , height ;
    INT16   angle1 , angle2 ;
} xArc;


























































































































































































typedef CARD16 KeyButMask;






typedef struct {
    CARD8	byteOrder;
    BYTE	pad;
    CARD16	majorVersion , minorVersion ;
    CARD16	nbytesAuthProto ;	
    CARD16	nbytesAuthString ;	
    CARD16	pad2;
} xConnClientPrefix;

typedef struct {
    BOOL           success;
    BYTE           lengthReason; 
    CARD16         majorVersion , 
                   minorVersion ;
    CARD16         length ;  
} xConnSetupPrefix;


typedef struct {
    CARD32         release ;
    CARD32         ridBase , 
                   ridMask ;
    CARD32         motionBufferSize ;
    CARD16         nbytesVendor ;  
    CARD16         maxRequestSize ;
    CARD8          numRoots;          
    CARD8          numFormats;        
    CARD8          imageByteOrder;        
    CARD8          bitmapBitOrder;        
    CARD8          bitmapScanlineUnit,     
                   bitmapScanlinePad;     
    CARD8	   minKeyCode, maxKeyCode;
    CARD32	   pad2 ;
} xConnSetup;

typedef struct {
    CARD8          depth;
    CARD8          bitsPerPixel;
    CARD8          scanLinePad;
    CARD8          pad1;
    CARD32	   pad2 ;
} xPixmapFormat;



typedef struct {
    CARD8 	depth;
    CARD8 	pad1;
    CARD16	nVisuals ;  
    CARD32	pad2 ;
    } xDepth;

typedef struct {
    CARD32 visualID ;

    CARD8 class;

    CARD8 bitsPerRGB;
    CARD16 colormapEntries ;
    CARD32 redMask , greenMask , blueMask ;
    CARD32 pad ;
    } xVisualType;

typedef struct {
    CARD32         windowId ;
    CARD32       defaultColormap ;
    CARD32         whitePixel , blackPixel ;
    CARD32         currentInputMask ;   
    CARD16         pixWidth , pixHeight ;
    CARD16         mmWidth , mmHeight ;
    CARD16         minInstalledMaps , maxInstalledMaps ;
    CARD32       rootVisualID ;
    CARD8          backingStore;
    BOOL           saveUnders;
    CARD8          rootDepth;
    CARD8          nDepths;  
} xWindowRoot;









typedef struct {
    CARD32 time ;
    INT16 x , y ;
} xTimecoord;

typedef struct {
    CARD8 family;
    BYTE pad;
    CARD16 length ;
} xHostEntry;

typedef struct {
    INT16 leftSideBearing ,
	  rightSideBearing ,
	  characterWidth ,
	  ascent ,
	  descent ;
    CARD16 attributes ;
} xCharInfo;

typedef struct {
    CARD32 name ;
    CARD32 value ;
} xFontProp;




typedef struct {           
    CARD8 len;	

    INT8 delta;
} xTextElt;


typedef struct {        
    CARD32 pixel ;
    CARD16 red , green , blue ;
    CARD8 flags;  
    CARD8 pad;
} xColorItem;


typedef struct {
    CARD16 red , green , blue , pad ;
} xrgb;

typedef CARD8 KEYCODE;

















typedef struct {
    BYTE type;                  
    BYTE errorCode;
    CARD16 sequenceNumber ;       
    CARD32 resourceID ;
    CARD16 minorCode ;
    CARD8 majorCode;
    BYTE pad1;
    CARD32 pad3 ;
    CARD32 pad4 ;
    CARD32 pad5 ;
    CARD32 pad6 ;
    CARD32 pad7 ;
} xError;


























typedef struct {
	CARD8 reqType;
	CARD8 data;            
	CARD16 length ;         

} xReq;








typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 id ;  
    } xResourceReq;

typedef struct {
    CARD8 reqType;
    CARD8 depth;
    CARD16 length ;
    CARD32 wid , parent ;
    INT16 x , y ;
    CARD16 width , height , borderWidth ;  

    CARD16 class ;

    CARD32 visual ;
    CARD32 mask ;
} xCreateWindowReq;

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 window ;
    CARD32 valueMask ; 
} xChangeWindowAttributesReq;

typedef struct {
    CARD8 reqType;
    BYTE mode;
    CARD16 length ;
    CARD32 window ;
} xChangeSaveSetReq;

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 window , parent ;
    INT16 x , y ;
} xReparentWindowReq;

typedef struct {
    CARD8 reqType;
    CARD8 pad;
    CARD16 length ;
    CARD32 window ;
    CARD16 mask ;
    CARD16 pad2 ;
} xConfigureWindowReq;

typedef struct {
    CARD8 reqType;
    CARD8 direction;
    CARD16 length ;
    CARD32 window ;
} xCirculateWindowReq;

typedef struct {    
    CARD8 reqType;
    BOOL onlyIfExists;
    CARD16 length ;
    CARD16 nbytes  ;    
    CARD16 pad ;
} xInternAtomReq;

typedef struct {
    CARD8 reqType;
    CARD8 mode;
    CARD16 length ;
    CARD32 window ;
    CARD32 property , type ;
    CARD8 format;
    BYTE pad[3];
    CARD32 nUnits ;     
} xChangePropertyReq;

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 window ;
    CARD32 property ;
} xDeletePropertyReq;

typedef struct {
    CARD8 reqType;

    BOOL delete;

    CARD16 length ;
    CARD32 window ;
    CARD32 property , type ;
    CARD32 longOffset ;
    CARD32 longLength ;
} xGetPropertyReq;
 
typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 window ;
    CARD32 selection ;
    CARD32 time ;
} xSetSelectionOwnerReq;

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 requestor ;
    CARD32 selection , target , property ;
    CARD32 time ;
    } xConvertSelectionReq;



typedef struct {
    CARD8 reqType;
    BOOL ownerEvents;
    CARD16 length ;
    CARD32 grabWindow ;
    CARD16 eventMask ;
    BYTE pointerMode, keyboardMode;
    CARD32 confineTo ;
    CARD32 cursor ;
    CARD32 time ;
} xGrabPointerReq;

typedef struct {
    CARD8 reqType;
    BOOL ownerEvents;
    CARD16 length ;
    CARD32 grabWindow ;
    CARD16 eventMask ;
    BYTE pointerMode, keyboardMode;
    CARD32 confineTo ;
    CARD32 cursor ;
    CARD8 button;
    BYTE pad;
    CARD16 modifiers ;
} xGrabButtonReq;

typedef struct {
    CARD8 reqType;
    CARD8 button;
    CARD16 length ;
    CARD32 grabWindow ;
    CARD16 modifiers ;
    CARD16 pad ;
} xUngrabButtonReq;

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 cursor ;
    CARD32 time ;
    CARD16 eventMask ;
    CARD16 pad2 ;
} xChangeActivePointerGrabReq;

typedef struct {
    CARD8 reqType;
    BOOL ownerEvents;
    CARD16 length ;
    CARD32 grabWindow ;
    CARD32 time ;
    BYTE pointerMode, keyboardMode;  
    CARD16 pad ;
} xGrabKeyboardReq;

typedef struct {
    CARD8 reqType;
    BOOL ownerEvents;
    CARD16 length ;
    CARD32 grabWindow ;
    CARD16 modifiers ;
    CARD8 key;
    BYTE pointerMode, keyboardMode;  
    BYTE pad1, pad2, pad3;
} xGrabKeyReq;

typedef struct {
    CARD8 reqType;
    CARD8 key;
    CARD16 length ;
    CARD32 grabWindow ;
    CARD16 modifiers ;
    CARD16 pad ;
} xUngrabKeyReq;

typedef struct {
    CARD8 reqType;
    CARD8 mode;
    CARD16 length ;
    CARD32 time ;
} xAllowEventsReq;

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 window ;
    CARD32 start , stop ;
} xGetMotionEventsReq;

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 srcWid , dstWid ;
    INT16 srcX , srcY ;
} xTranslateCoordsReq;

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 srcWid , dstWid ;
    INT16 srcX , srcY ;
    CARD16 srcWidth , srcHeight ;
    INT16 dstX , dstY ;
} xWarpPointerReq;

typedef struct {
    CARD8 reqType;
    CARD8 revertTo;
    CARD16 length ;
    CARD32 focus ;
    CARD32 time ;
} xSetInputFocusReq;

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 fid ;
    CARD16 nbytes ;
    BYTE pad1, pad2;	
} xOpenFontReq;

typedef struct {
    CARD8 reqType;
    BOOL oddLength;
    CARD16 length ;
    CARD32 fid ;
    } xQueryTextExtentsReq;

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD16 maxNames ;
    CARD16 nbytes ;  
} xListFontsReq;

typedef xListFontsReq xListFontsWithInfoReq;

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD16 nFonts ;
    BYTE pad1, pad2;	
} xSetFontPathReq;

typedef struct {
    CARD8 reqType;
    CARD8 depth;
    CARD16 length ;
    CARD32 pid ;
    CARD32 drawable ;
    CARD16 width , height ;
} xCreatePixmapReq;

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 gc ;
    CARD32 drawable ;
    CARD32 mask ;
} xCreateGCReq;

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 gc ;
    CARD32 mask ;
} xChangeGCReq;    

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 srcGC , dstGC ;
    CARD32 mask ;
} xCopyGCReq;    

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 gc ;
    CARD16 dashOffset ;
    CARD16 nDashes ;        
} xSetDashesReq;    

typedef struct {
    CARD8 reqType;
    BYTE ordering;
    CARD16 length ;
    CARD32 gc ;
    INT16 xOrigin , yOrigin ;
} xSetClipRectanglesReq;    

typedef struct {
    CARD8 reqType;
    BOOL exposures;
    CARD16 length ;
    CARD32 window ;
    INT16 x , y ;
    CARD16 width , height ;
} xClearAreaReq;

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 srcDrawable , dstDrawable ;
    CARD32 gc ;
    INT16 srcX , srcY , dstX , dstY ;
    CARD16 width , height ;
} xCopyAreaReq;    

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 srcDrawable , dstDrawable ;
    CARD32 gc ;
    INT16 srcX , srcY , dstX , dstY ;
    CARD16 width , height ;
    CARD32 bitPlane ;
} xCopyPlaneReq;    

typedef struct {
    CARD8 reqType;
    BYTE coordMode;
    CARD16 length ;
    CARD32 drawable ;
    CARD32 gc ;
} xPolyPointReq;    

typedef xPolyPointReq xPolyLineReq;  



typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 drawable ;
    CARD32 gc ;
} xPolySegmentReq;    

typedef xPolySegmentReq xPolyArcReq;
typedef xPolySegmentReq xPolyRectangleReq;
typedef xPolySegmentReq xPolyFillRectangleReq;
typedef xPolySegmentReq xPolyFillArcReq;

typedef struct _FillPolyReq {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 drawable ;
    CARD32 gc ;
    BYTE shape;
    BYTE coordMode;
    CARD16 pad1 ;
} xFillPolyReq;    


typedef struct _PutImageReq {
    CARD8 reqType;
    CARD8 format;
    CARD16 length ;
    CARD32 drawable ;
    CARD32 gc ;
    CARD16 width , height ;
    INT16 dstX , dstY ;
    CARD8 leftPad;
    CARD8 depth;
    CARD16 pad ;
} xPutImageReq;    

typedef struct {
    CARD8 reqType;
    CARD8 format;
    CARD16 length ;
    CARD32 drawable ;
    INT16 x , y ;
    CARD16 width , height ;
    CARD32 planeMask ;
} xGetImageReq;    



typedef struct {
    CARD8 reqType;
    CARD8 pad;
    CARD16 length ;
    CARD32 drawable ;
    CARD32 gc ;
    INT16 x , y ;		
} xPolyTextReq;    

typedef xPolyTextReq xPolyText8Req;
typedef xPolyTextReq xPolyText16Req;

typedef struct {
    CARD8 reqType;
    BYTE nChars;
    CARD16 length ;
    CARD32 drawable ;
    CARD32 gc ;
    INT16 x , y ;
} xImageTextReq;    

typedef xImageTextReq xImageText8Req;
typedef xImageTextReq xImageText16Req;

typedef struct {
    CARD8 reqType;
    BYTE alloc;
    CARD16 length ;
    CARD32 mid ;
    CARD32 window ;
    CARD32 visual ;
} xCreateColormapReq;    

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 mid ;
    CARD32 srcCmap ;
} xCopyColormapAndFreeReq;    

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 cmap ;
    CARD16 red , green , blue ;
    CARD16 pad2 ;
} xAllocColorReq;    

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 cmap ;
    CARD16 nbytes ;  
    BYTE pad1, pad2;
} xAllocNamedColorReq;    

typedef struct {
    CARD8 reqType;
    BOOL contiguous;
    CARD16 length ;
    CARD32 cmap ;
    CARD16 colors , planes ;
} xAllocColorCellsReq;    

typedef struct {
    CARD8 reqType;
    BOOL contiguous;
    CARD16 length ;
    CARD32 cmap ;
    CARD16 colors , red , green , blue ;
} xAllocColorPlanesReq;    

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 cmap ;
    CARD32 planeMask ;
} xFreeColorsReq;    

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 cmap ;
} xStoreColorsReq;    

typedef struct {
    CARD8 reqType;
    CARD8 flags;   
    CARD16 length ;
    CARD32 cmap ;
    CARD32 pixel ;
    CARD16 nbytes ;  
    BYTE pad1, pad2;
    } xStoreNamedColorReq;

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 cmap ;
} xQueryColorsReq;    

typedef struct {    
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 cmap ;
    CARD16 nbytes ;  
    BYTE pad1, pad2;
} xLookupColorReq;    

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 cid ;
    CARD32 source , mask ;
    CARD16 foreRed , foreGreen , foreBlue ;
    CARD16 backRed , backGreen , backBlue ;
    CARD16 x , y ;
} xCreateCursorReq;    

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 cid ;
    CARD32 source , mask ;
    CARD16 sourceChar , maskChar ;
    CARD16 foreRed , foreGreen , foreBlue ;
    CARD16 backRed , backGreen , backBlue ;
} xCreateGlyphCursorReq;    

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 cursor ;
    CARD16 foreRed , foreGreen , foreBlue ;
    CARD16 backRed , backGreen , backBlue ;
} xRecolorCursorReq;    

typedef struct {
    CARD8 reqType;

    CARD8 class;

    CARD16 length ;
    CARD32 drawable ;
    CARD16 width , height ;
} xQueryBestSizeReq;    

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD16 nbytes ;  
    BYTE pad1, pad2;
} xQueryExtensionReq;

typedef struct {
    CARD8   reqType;
    CARD8   numKeyPerModifier;
    CARD16  length ;
} xSetModifierMappingReq;

typedef struct {
    CARD8 reqType;
    CARD8 nElts;  
    CARD16 length ;
} xSetPointerMappingReq;

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD8 firstKeyCode;
    CARD8 count;
    CARD16 pad1 ;
} xGetKeyboardMappingReq;    

typedef struct {
    CARD8 reqType;
    CARD8 keyCodes;
    CARD16 length ;
    CARD8 firstKeyCode;
    CARD8 keySymsPerKeyCode;
    CARD16 pad1 ;
} xChangeKeyboardMappingReq;

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 mask ;
} xChangeKeyboardControlReq;    

typedef struct {
    CARD8 reqType;
    INT8 percent;  
    CARD16 length ;
} xBellReq;    

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    INT16 accelNum , accelDenum ;
    INT16 threshold ;             
    BOOL doAccel, doThresh;
} xChangePointerControlReq;    

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    INT16 timeout , interval ;
    BYTE preferBlank, allowExpose;  
    CARD16 pad2 ;
} xSetScreenSaverReq;    

typedef struct {
    CARD8 reqType;
    BYTE mode;
    CARD16 length ;
    CARD8 hostFamily;
    BYTE pad;
    CARD16 hostLength ;
} xChangeHostsReq;    

typedef struct {
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    } xListHostsReq;

typedef struct {
    CARD8 reqType;
    BYTE mode;
    CARD16 length ;
    } xChangeModeReq;

typedef xChangeModeReq xSetAccessControlReq;
typedef xChangeModeReq xSetCloseDownModeReq;
typedef xChangeModeReq xForceScreenSaverReq;

typedef struct { 
    CARD8 reqType;
    BYTE pad;
    CARD16 length ;
    CARD32 window ;
    CARD16 nAtoms ;
    INT16 nPositions ;
    } xRotatePropertiesReq;
    




















































































































































































































	













	













	


















































extern int errno;






















































void bcopy();
void bzero();
int bcmp();






































































extern unsigned char _ctype_[];


extern int	abort();
extern int	abs();
extern double	atof();
extern int	atoi();
extern long int	atol();
extern char *	bsearch(
);
extern char *	calloc();
extern int	exit();
extern int	free();
extern char *	getenv();
extern char *	malloc();
extern int	qsort(
);
extern int	rand();
extern char *	realloc();
extern int	srand();

extern int    mbtowc();
extern int    wctomb();
extern size_t mbstowcs();
extern size_t wcstombs();




































extern int errno;			







































































































































































































































































typedef struct _XFreeFuncs {
    void (*atoms)();		
    int (*modifiermap)();	
    void (*key_bindings)();	
    void (*context_db)();	
    void (*defaultCCCs)();	
    void (*clientCmaps)();	
    void (*im_filters)();	
} _XFreeFuncRec;




typedef struct _XExten {	
	struct _XExten *next;	
	XExtCodes codes;	
	int (*create_GC)();	
	int (*copy_GC)();	
	int (*flush_GC)();	
	int (*free_GC)();	
	int (*create_Font)();	
	int (*free_Font)();	
	int (*close_display)();	
	int (*error)();		
        char *(*error_string)();  
	char *name;		
	void (*error_values)(); 
} _XExtension;







extern int _XError();			
extern int _XIOError();			
extern int (*_XIOErrorFunction)();	
extern int (*_XErrorFunction)();	
extern void _XEatData();		
extern char *_XAllocScratch();		
extern Visual *_XVIDtoVisual();		
extern unsigned long _XSetLastRequestRead();	
extern int _XGetHostname();		
extern Screen *_XScreenOfWindow ();	

extern int (*XESetCreateGC(

))(

);

extern int (*XESetCopyGC(

))(

);

extern int (*XESetFlushGC(

))(

);

extern int (*XESetFreeGC(

))(

);

extern int (*XESetCreateFont(

))(

);

extern int (*XESetFreeFont(

))(

); 

extern int (*XESetCloseDisplay(

))(

);

extern int (*XESetError(

))(

);

extern char* (*XESetErrorString(

))(

);

extern void (*XESetPrintErrorValues (

))(

);

extern int (*XESetWireToEvent(

))(

);

extern int (*XESetEventToWire(

))(

);

extern int (*XESetWireToError(

))(

);







































				






















































extern char *Xpermalloc(

);







typedef int     XrmQuark, *XrmQuarkList;


typedef char *XrmString;



extern XrmQuark XrmStringToQuark(

);

extern XrmQuark XrmPermStringToQuark(

);


extern XrmString XrmQuarkToString(

);

extern XrmQuark XrmUniqueQuark(

);










typedef enum {XrmBindTightly, XrmBindLoosely} XrmBinding, *XrmBindingList;

extern void XrmStringToQuarkList(

);

extern void XrmStringToBindingQuarkList(

);







typedef XrmQuark     XrmName;
typedef XrmQuarkList XrmNameList;




typedef XrmQuark     XrmClass;
typedef XrmQuarkList XrmClassList;












typedef XrmQuark     XrmRepresentation;



typedef struct {
    unsigned int    size;
    XPointer	    addr;
} XrmValue, *XrmValuePtr;








typedef struct _XrmHashBucketRec *XrmHashBucket;
typedef XrmHashBucket *XrmHashTable;
typedef XrmHashTable XrmSearchList[];
typedef struct _XrmHashBucketRec *XrmDatabase;


extern void XrmDestroyDatabase(

);

extern void XrmQPutResource(

);

extern void XrmPutResource(

);

extern void XrmQPutStringResource(

);

extern void XrmPutStringResource(

);

extern void XrmPutLineResource(

);

extern  XrmQGetResource(

);

extern int XrmGetResource(

);

extern int XrmQGetSearchList(

);

extern int XrmQGetSearchResource(

);







extern void XrmSetDatabase(

);

extern XrmDatabase XrmGetDatabase(

);

extern XrmDatabase XrmGetFileDatabase(

);

extern int XrmCombineFileDatabase(

);

extern XrmDatabase XrmGetStringDatabase(

);

extern void XrmPutFileDatabase(

);

extern void XrmMergeDatabases(

);

extern void XrmCombineDatabase(

);




extern int XrmEnumerateDatabase(

);

extern char *XrmLocaleOfDatabase(

);








typedef enum {
    XrmoptionNoArg,	
    XrmoptionIsArg,     
    XrmoptionStickyArg, 
    XrmoptionSepArg,    
    XrmoptionResArg,	
    XrmoptionSkipArg,   
    XrmoptionSkipLine,  
    XrmoptionSkipNArgs	

} XrmOptionKind;

typedef struct {
    char	    *option;	    
    char	    *specifier;     
    XrmOptionKind   argKind;	    
    XPointer	    value;	    
} XrmOptionDescRec, *XrmOptionDescList;


extern void XrmParseCommand(

);









typedef struct _XIMFilter {
    struct _XIMFilter *next;
    Window window;
    unsigned long event_mask;
    int nonmaskable;
    int (*filter)(

	);
    XPointer client_data;
} XFilterEventRec, *XFilterEventList;







typedef struct {
    char    *name;
    XPointer value;
} XIMArg;



























typedef struct {
    XrmQuark		xrm_name;		
    int			xrm_size;		
    long		xrm_offset;		
    unsigned short 	mode;			
    int			mask;			
} XIMrmResource, *XIMrmResourceList;

typedef struct {
    char		*resource_name;		
    int			resource_size;		
    long		resource_offset;	
    unsigned short 	mode;			
    int			mask;			
} XIMResource, *XIMResourceList;

typedef struct {
    XIMCallback start;
    XIMCallback done;
    XIMCallback draw;
    XIMCallback caret;
} ICCallbacks;





typedef struct {
    XRectangle		area;
    XRectangle		area_needed;
    XPoint		spot_location;
    Colormap		colormap;
    Atom		std_colormap;
    unsigned long	foreground;
    unsigned long	background;
    Pixmap		background_pixmap;
    XFontSet            fontset;
    int	       		line_space;
    Cursor		cursor;
    ICCallbacks		callbacks;
} ICAttributes, *ICAttributesPtr;

typedef struct _XLCd *XLCd; 





typedef struct {
  char* (*map_modifiers)(		

	);
    XFontSet (*create_fontset)(

	);
    XIM (*open_im)(

	);
} XLCdMethodsRec, *XLCdMethods;


typedef struct {
    char*		name;			
    char*		modifiers;		
} XLCdCoreRec, *XLCdCore;


typedef struct _XLCd {
    XLCdMethods		methods;		
    XLCdCoreRec		core;			
} XLCdRec;










typedef struct {
    

    void (*free)(

	);
    
    

    int (*mb_escapement)(

	);
    int (*mb_extents)(

	);
    int (*mb_extents_per_char)(

	);
    int (*mb_draw_string)(

	);
    void (*mb_draw_image_string)(

	);

    

    int (*wc_escapement)(

	);
    int (*wc_extents)(

	);
    int (*wc_extents_per_char)(

	);
    int (*wc_draw_string)(

	);
    void (*wc_draw_image_string)(

	);
} XFontSetMethodsRec, *XFontSetMethods;






typedef struct {
    XLCd		lcd;			
    char *		base_name_list;     	
    int            	num_of_fonts;       	
    char **		font_name_list;     	
    XFontStruct **	font_struct_list;   	
    XFontSetExtents  	font_set_extents;   	
    char *		default_string;     	
} XFontSetCoreRec, *XFontSetCore;






typedef struct _XFontSet {
    XFontSetMethods	methods;		
    XFontSetCoreRec	core;			
} XFontSetRec;











typedef struct {
    int (*close)(

	);
    char* (*get_values)(

	);
    XIC (*create_ic)(

	);
} XIMMethodsRec, *XIMMethods;




typedef struct {
    XLCd		lcd;			
    XIC			ic_chain;		

    Display *		display;               	
    XrmDatabase 	rdb;
    char *		res_name;
    char *		res_class;

    XIMrmResourceList	ic_resources;		
    unsigned int	ic_num_resources;
    XIMrmResourceList	ic_attr_resources;	
    unsigned int	ic_num_attr_resources;
} XIMCoreRec, *XIMCore;







typedef struct _XIM {
    XIMMethods		methods;		
    XIMCoreRec		core;			
} XIMRec;










 
typedef struct {
    void (*destroy)(

	);
    void (*set_focus)(

	);
    void (*unset_focus)(

	);
    char* (*set_values)(

	);
    char* (*get_values)(

	);
    char* (*mb_reset)(

	);
    wchar_t* (*wc_reset)(

	);
    int (*mb_lookup_string)(

	);
    int (*wc_lookup_string)(

	);
} XICMethodsRec, *XICMethods;






typedef struct {
    XIM			im;			
    XIC			next;			

    Window		client_window;		
						
    XIMStyle		input_style;		
    Window		focus_window;		
    XrmDatabase		res_database;		
    char *		string_database;	
    XIMCallback		geometry_callback;	
    int			preedit_state;		
    unsigned long	filter_events;		

    ICAttributes	preedit_attr;		
    ICAttributes	status_attr;		
} XICCoreRec, *XICCore;






typedef struct _XIC {
    XICMethods		methods;		
    XICCoreRec		core;			
} XICRec;





typedef struct {
    void (*mbinit)(

	);
    char (*mbchar)(

	);
    void (*mbfinish)(

	);
    char* (*lcname)(

	);
    void (*destroy)(

	);
} XrmMethodsRec, *XrmMethods;




typedef XLCd (*XLCdLoadProc)(

);



extern XLCd _XlcCurrentLC(

);

extern int _XlcValidModSyntax(

);

extern char *_XlcDefaultMapModifiers(

);

extern void _XIMCompileResourceList(

);

extern void _XCopyToArg(

);

extern char ** _XParseBaseFontNameList(

);

extern XrmMethods _XrmInitParseInfo(

);























extern	struct	_iobuf {
	int	_cnt;
	unsigned char *_ptr;
	unsigned char *_base;
	int	_bufsiz;
	short	_flag;
	char	_file;		
} _iob[];




































extern struct _iobuf	*fopen();
extern struct _iobuf	*fdopen();
extern struct _iobuf	*freopen();
extern struct _iobuf	*popen();
extern struct _iobuf	*tmpfile();
extern long	ftell();
extern char	*fgets();
extern char	*gets();
extern char	*sprintf();
extern char	*ctermid();
extern char	*cuserid();
extern char	*tempnam();
extern char	*tmpnam();


























extern int		fgetwc(), fputwc(), ungetwc();
extern wchar_t		*getws(), *fgetws();
extern int		putws(), fputws();







extern wchar_t
	*wscpy(),
	*wsncpy(),
	*wscat(),
	*wsncat(),
	*wschr(),
	*wsrchr(),
	*wspbrk(),
	*wstok();

extern int
	wscmp(),
	wsncmp(),
	wslen(),
	wsspn(),
	wscspn();

extern char
	*wstostr();

extern wchar_t
	*strtows();







extern wchar_t	*wsdup();
extern int	wscol();
extern double	wstod();
extern long	wstol();









































































				



























































typedef struct CodesetInfo {
    char    **charsets;
    int	    mask;
    int	    value;
    int     mapping_mask;
    int	    width;
} CodesetInfo, PlaneInfo;

typedef struct LocaleInfo {
    struct LocaleInfo   *next;
    char                *locale;
    CodesetInfo		*codesets[4];
    PlaneInfo		*planes[4];
} LocaleInfo;


typedef struct {
    XFontSetMethods     methods;        
    XFontSetCoreRec     core;           
    LocaleInfo	    *locale_info;
    XFontStruct	    *font_structs[4 + 4]; 
    char	    *font_names[4];
    XFontSetExtents  font_set_extents;
    unsigned	    direction;
    unsigned	    status;
} XFontSetStruct;


typedef enum {
    FontNameRegistry,
    FontNameVersion,
    FontNameSuffix,
    FontNameEnd,
    FontNameError
} XLFD_STATE;

extern char 		*strstr(); 
extern char		*CharToFontEncoding();
extern int		FindCodeset();
extern int		FindSubStringLength();
extern short    	FindFontForCharset();
extern XFontStruct    	*LoadFontForCharset();
extern int		WildCardInFontName();
extern wchar_t		*_XmbTowcBuffer();
extern int		XwcDrawOrDrawImageString();

extern int		XwcTextEscapement();
extern int		XFreeFontNames();
extern void		XwcDrawString();
extern void		XwcDrawImageString();
extern int		XwcTextExtents();
extern char**		XListFonts();

extern void    	_XSunFreeFontSet();
extern int 	_XSunmbTextEscapement();
extern int    	_XSunmbTextExtents();
extern int    	_XSunmbTextPerCharExtents();
extern void    	_XSunmbDrawString();
extern void    	_XSunmbDrawImageString();
extern int    	_XSunwcTextEscapement();
extern int    	_XSunwcTextExtents();
extern int   	_XSunwcTextPerCharExtents();
extern void    	_XSunwcDrawString();
extern void    	_XSunwcDrawImageString();
extern XFontSet      _XSunCreateFontSet();


LocaleInfo    *locale_head = 0;

static char * 	MakePtSizeZero();
static char ** 	ParseBaseFontNameList();
void XFreeStringList ();

extern int	XFreeFontNames();
extern char**   XListFonts();
extern void    	_XSunFreeFontSet();
extern int 	_XSunmbTextEscapement();
extern int    	_XSunmbTextExtents();
extern int    	_XSunmbTextPerCharExtents();
extern void    	_XSunmbDrawString();
extern void    	_XSunmbDrawImageString();
extern int    	_XSunwcTextEscapement();
extern int    	_XSunwcTextExtents();
extern int   	_XSunwcTextPerCharExtents();
extern void    	_XSunwcDrawString();
extern void    	_XSunwcDrawImageString();

static XFontSetMethodsRec fs_methods = {
    _XSunFreeFontSet,
    _XSunmbTextEscapement,
    _XSunmbTextExtents,
    _XSunmbTextPerCharExtents,
    _XSunmbDrawString,
    _XSunmbDrawImageString,
    _XSunwcTextEscapement,
    _XSunwcTextExtents,
    _XSunwcTextPerCharExtents,
    _XSunwcDrawString,
    _XSunwcDrawImageString
};












extern XFontSet
_XSunCreateFontSet(lcd, dpy, base_font_name_list, font_list, count,
           missing_charset_list, missing_charset_count)
    XLCd 	   lcd;
    Display	   *dpy;
    char           *base_font_name_list;
    char	    **font_list;
    int		    count;
    char          ***missing_charset_list;
    int            *missing_charset_count;
{
    char	    **charsets;
    int	    	    code_set;
    XFontSetStruct   *fs_info = 0;
    XFontStruct	    *font_info = 0;
    char            *locale = setlocale(1, (char *)0);
    LocaleInfo      *locale_info, *FindLocaleInfo();
    char	    *missing_list[4];
    int		    fixed_width;
    short	    font_found = 0;
    char	    *font_name;

    *missing_charset_list = 0;
    *missing_charset_count = 0;
    
    missing_list[0] = (char *)0;
    

    if ((locale_info = FindLocaleInfo(locale)) == 0) {
	fprintf((&_iob[2]), 
	    "XCreateFontSet(): locale-specific information not found\n");
	return(0);
    }

    for (code_set = 0; code_set <= 3; code_set++) {
	
	if (locale_info->codesets[code_set] == 0) 
	    continue;

	
	if ((strcmp(locale, "tchinese") == 0) && (code_set == 2)) {
	   for (plane = 0; plane < 4; plane++) {
		
		charsets = locale_info->planes[plane]->charsets;
		font_info = 0;
		fixed_width = 0;
		for ( ; *charsets && !font_info; charsets++) 
		    font_info = FindFontForCharset(dpy, *charsets, base_font_list, 
					 &fixed_width);
	
		if (font_info != 0) {
		    if (fs_info == 0) {
			fs_info = (XFontSetStruct *)calloc(1, sizeof(XFontSetStruct));
			fs_info->locale_info = locale_info;
		    }
		    fs_info->font_structs[4 + plane] = font_info;
		    if (fixed_width == 1)
			(fs_info)->status |= (1 << ( code_set));
		}  else {
		    



		    missing_list[count++] = *locale_info->planes[plane]->charsets;
		}
	   }
	}
	else {
		
		charsets = locale_info->codesets[code_set]->charsets;
		font_info = 0;
		fixed_width = 0;
		for ( ; *charsets && !font_info; charsets++) 
		    font_info = FindFontForCharset(dpy, *charsets, base_font_list, 
					 &fixed_width);
	
		if (font_info != 0) {
		    if (fs_info == 0) {
			fs_info = (XFontSetStruct *)calloc(1, sizeof(XFontSetStruct));
			fs_info->locale_info = locale_info;
		    }
		    fs_info->font_structs[code_set] = font_info;
		    if (fixed_width == 1)
			(fs_info)->status |= (1 << ( code_set));
		}  else {
		    



		    missing_list[count++] = *locale_info->codesets[code_set]->charsets;
		}
	}
    }
    

    if (fs_info) {
        fs_info->methods = (XFontSetMethods) &fs_methods;
	fs_info->core.lcd = lcd;
	fs_info->core.base_name_list = base_font_name_list;
    	fs_info->core.num_of_fonts = code_set;
    	





    	fs_info->core.font_name_list = fs_info->font_names;
    	fs_info->core.font_struct_list = fs_info->font_structs;
    	
    }

    

    if (missing_list[0] != 0) {

	*missing_charset_list = (char **)malloc((count + 1) * sizeof(char *));
	(*missing_charset_list)[count] = 0;
	*missing_charset_count = count;
	for( --count; count >= 0; --count)
	    (*missing_charset_list)[count] = missing_list[count];
    }

    if (fs_info != 0) {
	SetFontsetInfo(fs_info);
    }
	    
    return((XFontSet)fs_info);
}


static char **
ParseBaseFontNameList(base_font_name_list)
    char	* base_font_name_list;
{
    char	*temp_ptr_1, *temp_ptr_2;
    char	**return_ptr; 
    int		i = 0, count = 0; 
    
    
    temp_ptr_1 = base_font_name_list;
    if (temp_ptr_1)
        count = 1;	
    while (temp_ptr_1) {
        temp_ptr_1 = (char *)index(temp_ptr_1, ',');
        if (temp_ptr_1) {
            count++;
            temp_ptr_1++;
        } else
            break; 
    }
    
    if (count == 0)
        return((char **)0);
        
    return_ptr =  (char **)malloc((sizeof(char *) * (count + 1)));
    
    temp_ptr_1 = base_font_name_list;
    for (i = 0; i < count; i++) {
        char	*temp;
        int	char_count;
        
        if (i < (count-1)) {
            temp_ptr_2 = (char *)index(temp_ptr_1, ',');            
            temp = temp_ptr_2 + 1;  
        } else {
            temp_ptr_2 = (char *)index(temp_ptr_1,0);
            char_count =  (int)(temp_ptr_2  - temp_ptr_1);
        }            
        
        char_count =  (int)(temp_ptr_2  - temp_ptr_1) + 1;
         
         
         while (isspace(*(temp_ptr_2 - 1))) {
                temp_ptr_2--;
                char_count--;
         }
            
         return_ptr[i] = (char *)malloc((char_count));
         strncpy(return_ptr[i], temp_ptr_1, char_count - 1);
         return_ptr[i][char_count - 1] = 0;            
         temp_ptr_1 = temp;
    }
    return_ptr[i] = (char *) 0;
    return(return_ptr);
}




void
SetCodeSetInfoC(info, codeset)
    CodesetInfo    *info;
    int		   codeset;
{
    switch(codeset) {
	case 0:
	    info->charsets = (char **)malloc(5 * sizeof(char *));
	    info->charsets[0] = "iso8859-1";
	    info->charsets[1] = "jisx0201.1976-0";
	    info->charsets[2] = "sunolcursor-1";
	    info->charsets[3] = "sunolglyph-1";
	    info->charsets[4] = 0;
	    info->mask = 0x8080;
	    info->value = 0x0000;
	    info->mapping_mask = 0x007f;
	    info->width = 1;
	    break;

	case 1: 
	    info->charsets = (char **)malloc(4 * sizeof(char *));
	    info->charsets[0] = "iso8859-1";
	    info->charsets[1] = "sunolcursor-1";
	    info->charsets[2] = "sunolglyph-1";
	    info->charsets[3] = 0;
            info->mask = 0x8080;
   	    info->value = 0x8080;
            info->mapping_mask = 0x00ff;
	    info->width = 1;
            break;

	default:
 	    break;
    }
}

void
SetCodeSetInfoJapanese(info, codeset)
    CodesetInfo    *info;
    int		   codeset;
{
    switch(codeset) {
	case 0:
	    info->charsets = (char **)malloc(5 * sizeof(char *));
	    info->charsets[0] = "iso8859-1";
	    info->charsets[1] = "jisx0201.1976-0";
	    info->charsets[2] = "sunolcursor-1";
	    info->charsets[3] = "sunolglyph-1";
	    info->charsets[4] = 0;
	    info->mask = 0x8080;
	    info->value = 0x0000;
	    info->mapping_mask = 0x007f;
	    info->width = 1;
	    break;

	case 1: 
	    info->charsets = (char **)malloc(2 * sizeof(char *));
	    info->charsets[0] = "jisx0208.1983-0";
	    info->charsets[1] = 0;
            info->mask = 0x8080;
   	    info->value = 0x8080;
            info->mapping_mask = 0x7f7f;
	    info->width = 2;
            break;

    	case 2:
	    info->charsets = (char **)malloc(2 * sizeof(char *));
	    info->charsets[0] = "jisx0201.1976-0";
	    info->charsets[1] = 0;
            info->mask = 0x8080;
            info->value = 0x0080;
            info->mapping_mask = 0x00ff;
	    info->width = 2;
            break;

	default:
	    break;
    }
}

void
SetCodeSetInfoKorean(info, codeset)
    CodesetInfo    *info;
    int		   codeset;
{
    switch(codeset) {
	case 0:
	    info->charsets = (char **)malloc(5 * sizeof(char *));
	    info->charsets[0] = "iso8859-1";
	    info->charsets[1] = "ksc5636-0";
	    info->charsets[2] = "sunolcursor-1";
	    info->charsets[3] = "sunolglyph-1";
	    info->charsets[4] = 0;
	    info->mask = 0x8080;
	    info->value = 0x0000;
	    info->mapping_mask = 0x007f;
	    info->width = 1;
	    break;

	case 1: 
	    info->charsets = (char **)malloc(2 * sizeof(char *));
	    info->charsets[0] = "ksc5601.1987-0";
	    info->charsets[1] = 0;
            info->mask = 0x8080;
   	    info->value = 0x8080;
            info->mapping_mask = 0x7f7f;
	    info->width = 2;
            break;

	default:
	    break;
    }
}

void
SetCodeSetInfoChinese(info, codeset)
    CodesetInfo    *info;
    int		   codeset;
{
    switch(codeset) {
	case 0:
	    info->charsets = (char **)malloc(5 * sizeof(char *));
	    info->charsets[0] = "iso8859-1";
	    info->charsets[1] = "gb2312.1980-0";
	    info->charsets[2] = "sunolcursor-1";
	    info->charsets[3] = "sunolglyph-1";
	    info->charsets[4] = 0;
	    info->mask = 0x8080;
	    info->value = 0x0000;
	    info->mapping_mask = 0x007f;
	    info->width = 1;
	    break;

	case 1: 
	    info->charsets = (char **)malloc(2 * sizeof(char *));
	    info->charsets[0] = "gb2312.1980-1";
	    info->charsets[1] = 0;
            info->mask = 0x8080;
   	    info->value = 0x8080;
            info->mapping_mask = 0x7f7f;
	    info->width = 2;
            break;

	default:
	    break;
    }
}

void
SetCodeSetInfoTchinese(info, codeset, locale_info)
    CodesetInfo    *info;
    int		   codeset;
    LocaleInfo      *locale_info;
{

    PlaneInfo	*p_info;

    
    switch(codeset) {
	case 0:
	    info->charsets = (char **)malloc(5 * sizeof(char *));
	    info->charsets[0] = "iso8859-1";
	    info->charsets[1] = "cns11643-0";
	    info->charsets[2] = "sunolcursor-1";
	    info->charsets[3] = "sunolglyph-1";
	    info->charsets[4] = 0;
	    info->mask = 0xff000000;
	    info->value = 0x00000000;
	    info->mapping_mask = 0x007f;
	    info->width = 1;
	    break;

	case 1: 
	    info->charsets = (char **)malloc(2 * sizeof(char *));
	    info->charsets[0] = "cns11643-1";
	    info->charsets[1] = 0;
            info->mask = 0x60000000;
   	    info->value = 0x60000000;
            info->mapping_mask = 0x3fff;
	    info->width = 4;
            break;

    	case 2:
	    info->charsets = (char **)malloc(1 * sizeof(char *));
	    info->charsets[0] = 0;
            info->mask = 0;
            info->value = 0;
            info->mapping_mask = 0;
	    info->width = 4;

	    
	    p_info = locale_info->planes[0];
	    p_info->charsets = (char **)malloc(2 * sizeof(char *));
	    p_info->charsets[0] = "cns11643-2";
	    p_info->charsets[1] = 0;
            p_info->mask = 0x601fc000;
            p_info->value = 0x20088000;
            p_info->mapping_mask = 0x3fff;
	    p_info->width = 4;

	    
	    p_info = locale_info->planes[1];
	    p_info->charsets = (char **)malloc(2 * sizeof(char *));
	    p_info->charsets[0] = "cns11643-14";
	    p_info->charsets[1] = 0;
            p_info->mask = 0x601fc000;
            p_info->value = 0x200b8000;
            p_info->mapping_mask = 0x3fff;
	    p_info->width = 4;

	    
	    p_info = locale_info->planes[2];
	    p_info->charsets = (char **)malloc(2 * sizeof(char *));
	    p_info->charsets[0] = "cns11643-15";
	    p_info->charsets[1] = 0;
            p_info->mask = 0x601fc000;
            p_info->value = 0x200bc000;
            p_info->mapping_mask = 0x3fff;
	    p_info->width = 4;

	    
	    p_info = locale_info->planes[3];
	    p_info->charsets = (char **)malloc(2 * sizeof(char *));
	    p_info->charsets[0] = "cns11643-16";
	    p_info->charsets[1] = 0;
            p_info->mask = 0x601fc000;
            p_info->value = 0x200c0000;
            p_info->mapping_mask = 0x3fff;
	    p_info->width = 4;
            break;

	default:
	    break;
    }
}    


LocaleInfo *
FindLocaleInfo(locale)
    char    *locale;
{
    LocaleInfo      *locale_info = 0;

    for (locale_info = locale_head; locale_info != 0; 
	    locale_info = locale_info->next)
	if (strcmp(locale_info->locale, locale) == 0)
	    break;

    if (locale_info == 0) {
	locale_info = (LocaleInfo *)calloc(1, sizeof(LocaleInfo));
	locale_info->next = locale_head;
	locale_head = locale_info;

	locale_info->locale = strdup(locale);

	if (strcmp(locale, "C") == 0) {
	    locale_info->codesets[0] = 
		(CodesetInfo *)calloc(1, sizeof(CodesetInfo));
	    SetCodeSetInfo(locale_info->codesets[0], 0);
	} else if (strcmp(locale, "japanese") == 0) { 
	    register int i;

	    for (i = 0; i <= 2; i++) {
		locale_info->codesets[i] = 
		    (CodesetInfo *)malloc(sizeof(CodesetInfo));
		SetCodeSetInfo(locale_info->codesets[i], i); 
	    }
	} else if (strncmp(locale, "korean", 6) == 0) { 
	    register int i;

	    for (i = 0; i <= 1; i++) {
		locale_info->codesets[i] = 
		    (CodesetInfo *)malloc(sizeof(CodesetInfo));
		SetCodeSetInfoKorean(locale_info->codesets[i], i); 
	    }
	} else if (strcmp(locale, "chinese") == 0) { 
	    register int i;

	    for (i = 0; i <= 1; i++) {
		locale_info->codesets[i] = 
		    (CodesetInfo *)malloc(sizeof(CodesetInfo));
		SetCodeSetInfoChinese(locale_info->codesets[i], i); 
	    }
	} else if (strcmp(locale, "tchinese") == 0) {
	    register int i;

	    
	    for (i = 0; i <4; i++)
		locale_info->planes[i] = 
		    (PlaneInfo *)malloc(sizeof(PlaneInfo));
		
	    for (i = 0; i <= 2; i++) {
		locale_info->codesets[i] = 
		    (CodesetInfo *)malloc(sizeof(CodesetInfo));
		SetCodeSetInfoTchinese(locale_info->codesets[i], i, locale_info); 
	    }
	} else {
	    fprintf((&_iob[2]), "No locale info for locale %s\n", locale);
  	}
    }
	     
    return(locale_info);
}

SetFontsetInfo(fs_info)
    XFontSetStruct	*fs_info;
{
    SetFontSetBounds(fs_info);
    SetFontSetDirection(fs_info);
}

SetFontSetDirection(fs_info)
    XFontSetStruct      *fs_info;
{
    register int    i, FLTR = 0, FRTL = 0;
    XFontStruct     **font_structs = fs_info->font_structs;


    for (i = 0 ; i < (4 + 4); i++) {
	if (font_structs[i] == 0)
	    continue;
	if (font_structs[i]->direction == 0)
	    FLTR++;
	if (font_structs[i]->direction == 1)             
	    FRTL++; 
    }

    fs_info->direction = (FLTR > FRTL) ? 0 : 1;
}


static 
SetFontSetBounds(fs_info)
    XFontSetStruct	*fs_info;
{
    register int    i;
    XCharStruct	    min_bounds, max_bounds;
    XFontStruct	    **font_structs = fs_info->font_structs;
    short	    ascent, descent;

    for (i = 0 ; i <= (3 + 4); i++) 
	if (font_structs[i] != 0) {
            min_bounds = font_structs[i]->min_bounds;
            max_bounds = font_structs[i]->max_bounds;
            ascent = font_structs[i]->ascent;
            descent = font_structs[i]->descent;
	    break;
	}

    for (++i ; i <= (3 + 4); i++) {
	if (font_structs[i] == 0)
	    continue;

	min_bounds.lbearing = 
	    (((min_bounds.lbearing) < ( font_structs[i]->min_bounds.lbearing)) ? (min_bounds.lbearing) : ( font_structs[i]->min_bounds.lbearing));
	min_bounds.rbearing = 
	    (((min_bounds.rbearing) < ( font_structs[i]->min_bounds.rbearing)) ? (min_bounds.rbearing) : ( font_structs[i]->min_bounds.rbearing));
	min_bounds.ascent = 
	    (((min_bounds.ascent) < ( font_structs[i]->min_bounds.ascent)) ? (min_bounds.ascent) : ( font_structs[i]->min_bounds.ascent));
	min_bounds.descent = 
	    (((min_bounds.descent) < ( font_structs[i]->min_bounds.descent)) ? (min_bounds.descent) : ( font_structs[i]->min_bounds.descent));

	max_bounds.lbearing = 
	    (((max_bounds.lbearing) > ( font_structs[i]->max_bounds.lbearing)) ? (max_bounds.lbearing) : ( font_structs[i]->max_bounds.lbearing));
	max_bounds.rbearing = 
	    (((max_bounds.rbearing) > ( font_structs[i]->max_bounds.rbearing)) ? (max_bounds.rbearing) : ( font_structs[i]->max_bounds.rbearing));
	max_bounds.ascent = 
	    (((max_bounds.ascent) > ( font_structs[i]->max_bounds.ascent)) ? (max_bounds.ascent) : ( font_structs[i]->max_bounds.ascent));
	max_bounds.descent = 
	    (((max_bounds.descent) > ( font_structs[i]->max_bounds.descent)) ? (max_bounds.descent) : ( font_structs[i]->max_bounds.descent));

	ascent = (((ascent) > ( font_structs[i]->ascent)) ? (ascent) : ( font_structs[i]->ascent));
	descent = (((descent) > ( font_structs[i]->descent)) ? (descent) : ( font_structs[i]->descent));
    }

    



    fs_info->core.font_set_extents.max_ink_extent.x =
    fs_info->font_set_extents.max_ink_extent.x = min_bounds.lbearing;
    fs_info->core.font_set_extents.max_ink_extent.y =
    fs_info->font_set_extents.max_ink_extent.y = - max_bounds.ascent;
    fs_info->core.font_set_extents.max_ink_extent.width =
    fs_info->font_set_extents.max_ink_extent.width =
            max_bounds.rbearing - min_bounds.lbearing;
    fs_info->core.font_set_extents.max_ink_extent.height =       
    fs_info->font_set_extents.max_ink_extent.height =
            max_bounds.ascent + max_bounds.descent;
    



    fs_info->core.font_set_extents.max_logical_extent.x = 
    fs_info->font_set_extents.max_logical_extent.x = 0;
    fs_info->core.font_set_extents.max_logical_extent.y =
    fs_info->font_set_extents.max_logical_extent.y =
            - ascent;
    fs_info->core.font_set_extents.max_logical_extent.width =       
    fs_info->font_set_extents.max_logical_extent.width =
            max_bounds.width;
    fs_info->core.font_set_extents.max_logical_extent.height =       
    fs_info->font_set_extents.max_logical_extent.height =
            ascent + descent;

}

void
XFreeMissingCharsets(list)
    char **list;
{
    if (list == 0)
        return;

    



    free(list);
}

extern void
_XSunFreeFontSet(dpy, font_set)
    Display     *dpy;
    XFontSet    font_set;
{
    XFontSetStruct    *fs_info = (XFontSetStruct *)font_set;
    register int      i;

    if (font_set == 0)
        return;

    



    for (i = 0; i <= (3 + 4); i++)
        if (fs_info->font_structs[i] != 0)
            XFreeFont(dpy, fs_info->font_structs[i]);
    free(fs_info);
}

static short
FindFontForCharset(dpy, charset, base_font_list, font_info, font_name, 
	fixed_width)
    Display	*dpy;
    char	*charset;
    char	**base_font_list;
    XFontStruct **font_info;
    char	**font_name;
    int		*fixed_width;
{
    char	    **tmp_list;
    char	    **font_list;
    char	    tmp_name[256];
    int	  	    count;
    register int    i = 0;
    short	    font_found = 0;

    











    *font_info = (XFontStruct *)0;
    *font_name = (char *)0;

    for(tmp_list = base_font_list; *tmp_list ; tmp_list++) {
	
	if (WildCardInFontName(*tmp_list) != 1) 
	    *font_info = LoadFontForCharset(dpy, charset, *tmp_list);
	if (*font_info != 0) {
	    font_found = 1;
	    *font_name = *tmp_list;
	    *fixed_width = IsFontFixedWidth(*tmp_list);
	    break;
	}
    }


    




    for(tmp_list = base_font_list; *tmp_list && !font_found; tmp_list++) {
	if (WildCardInFontName(*tmp_list) != 1) 
	    continue;


	font_list = XListFonts(dpy, *tmp_list, 100, &count);


	for (i = 0; i < count; i++) {
	    *font_info = LoadFontForCharset(dpy, charset, font_list[i]);
	    if (*font_info != 0) {
		font_found = 1; 
		*font_name = *tmp_list;
		*fixed_width = IsFontFixedWidth(font_list[i]);
		break;
	    }
	}
	if (count > 0)
	    XFreeFontNames(font_list);
    }

    if (*font_info == 0) {
    	for(tmp_list = base_font_list; *tmp_list && !font_found; tmp_list++) {
            if (MakePtSizeZero(*tmp_list,tmp_name) == 0)
		continue;

	    count = 0;

            font_list = XListFonts(dpy, tmp_name, 100, &count);


            for (i = 0; i < count; i++) 
		if (strstr(font_list[i], charset)) {

		    *font_info = XLoadQueryFont(dpy, *tmp_list);

            	    if (*font_info != 0) {
			font_found = 1;  
			*font_name = *tmp_list;
                        *fixed_width = IsFontFixedWidth(font_list[i]);
                        break;
            	    }
		}
            if (count > 0)
            	XFreeFontNames(font_list);

    	}
    }

    return(font_found);
}

static char *
MakePtSizeZero(name, newname)
    char    *name;
    char    *newname;
{
    char    *oldtmp=name;
    char    *tmp=name;
    int	    i, count;

    bzero(newname,256);
    strncat(newname,"-",1);
    tmp++;
    oldtmp++;

    for (i=0; i<7; i++) {
	if ((tmp = (char *)index(oldtmp, '-')) == 0)
	    return(0);
	count = tmp - oldtmp;
	strncat(newname,oldtmp,count);
    	strncat(newname,"-",1);
        tmp++;
	oldtmp = tmp;
    }

    if (strncmp(tmp,"*",1)==0) {
	strcpy(newname,name);
	return(newname);
    }

    tmp = (char *)index(oldtmp, '-');
    strncat(newname,"0",1);
    strcat(newname,tmp);
    return(newname);
}

static XFontStruct *
LoadFontForCharset(dpy, charset, font_name)
    Display     *dpy; 
    char        *charset;
    char	*font_name;
{
    XFontStruct      *font_info = 0;

    if (strstr(font_name, charset) != 0) 

	font_info = XLoadQueryFont(dpy, font_name);


    return(font_info);
}

static int
WildCardInFontName(name)
    char    *name;
{
    if ((char *)index(name, '*') || (char *)index(name, '?'))
	return(1);
    else 
	return(0);
}








