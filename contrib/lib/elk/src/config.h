#ifndef CONFIG_H
#define CONFIG_H
/* Machine/compiler dependencies
 */

#ifdef ibm032
#  define VFORK
#  define POINTER_CONSTANT_HIGH_BITS 0x10000000
#  define CAN_LOAD_OBJ
#    define XFLAG_BROKEN
#  define CAN_DUMP
#    define SEGMENT_SIZE      0x10000000
#    define FILE_TEXT_START   (N_TXTOFF(hdr))
#    define MEM_TEXT_START    0
#    define TEXT_LENGTH_ADJ   0
#  define TIME_H              <sys/time.h>
#endif

#ifdef mips
#  undef sony_news
#  define VFORK
#  define VPRINTF
#  define TIME_H              <sys/time.h>
#  define POINTER_CONSTANT_HIGH_BITS 0x800000
#  define ECOFF
#  define CAN_DUMP
#    define TEXT_START 0x400000
#    define DATA_START 0x800000
#  define CAN_LOAD_OBJ
#    define XFLAG_BROKEN
#endif

#ifdef is68k
#  define VFORK
#  define CAN_LOAD_OBJ
#  define CAN_DUMP
#    define SEGMENT_SIZE      0x20000
#    define FILE_TEXT_START   sizeof(struct exec)
#    define MEM_TEXT_START    sizeof(struct exec)
#    define TEXT_LENGTH_ADJ   sizeof(struct exec)
#  define TIME_H              <sys/time.h>
#endif

#ifdef sun
#  define VFORK
#  define VPRINTF
#  define DIRENT              /* Remove this for SunOS 3.4 */
#  define CAN_LOAD_OBJ
#    define XFLAG_BROKEN
#  define CAN_DUMP
#    define SEGMENT_SIZE      SEGSIZ
#    define FILE_TEXT_START   sizeof(struct exec)
#    define MEM_TEXT_START    (PAGSIZ+sizeof(struct exec))
#    define TEXT_LENGTH_ADJ   sizeof(struct exec)
#  define TIME_H              <sys/time.h>
#  define TERMIO              /* Remove this for SunOS 3.4 */
#endif

#ifdef vax
#  define VFORK
#  define CAN_LOAD_OBJ
#  define CAN_DUMP
#    define SEGMENT_SIZE      1024
#    define FILE_TEXT_START   1024
#    define MEM_TEXT_START    0
#    define TEXT_LENGTH_ADJ   0
#  define TIME_H              <sys/time.h>
#endif

#ifdef sony_news   /* Sony NEWS with 680x0 */
#  define VFORK
#  define CAN_LOAD_OBJ
#  define CAN_DUMP
#    define SEGMENT_SIZE      PAGSIZ
#    define FILE_TEXT_START   PAGSIZ
#    define MEM_TEXT_START    0
#    define TEXT_LENGTH_ADJ   0
#  define TIME_H              <sys/time.h>
#endif

#ifdef hp9000s300
#  define VFORK
#  define VPRINTF
#  define DIRENT              
#  define TERMIO
#  define STACK_SIZE          (1024*1024)
#  define TIME_H              <sys/time.h>
#  define SYSCONF
#  define FORGETS_SIGNAL_HANDLER
#  define random              rand
#  define srandom             srand
#  define bcopy(from,to,len)  memcpy(to,from,len)
#  define bzero(p,len)        memset(p,0,len)
#  define bcmp                memcmp
#  define index               strchr
#  define CAN_LOAD_OBJ
#  define CAN_DUMP
#    define SEGMENT_SIZE      EXEC_PAGESIZE
#    define PAGESIZE          EXEC_PAGESIZE
#    define FILE_TEXT_START   TEXT_OFFSET(hdr)
#    define MEM_TEXT_START    0
#    define TEXT_LENGTH_ADJ   0
#endif

#ifdef hp9000s800
#  define VFORK
#  define VPRINTF
#  define DIRENT              
#  define TERMIO
#  define STACK_SIZE          (1024*1024)
#  define TIME_H              <sys/time.h>
#  define POINTER_CONSTANT_HIGH_BITS 0x40000000
#  define ALIGN8
#  define SYSCONF
#  define FORGETS_SIGNAL_HANDLER
#  define random              rand
#  define srandom             srand
#  define bcopy(from,to,len)  memcpy(to,from,len)
#  define bzero(p,len)        memset(p,0,len)
#  define bcmp                memcmp
#  define index               strchr
#if 0
#  define CAN_LOAD_OBJ        /* /bin/ld is broken. */
#endif
#  define INIT_OBJECTS
#  define PORTABLE
#endif

#if defined(i386) && !defined(sequent)
#  define UNISTD
#  define DIRENT
#  define VPRINTF
#  define FCHMOD_BROKEN       /* It isn't there */
#  define USE_SIGNAL
#  define INIT_OBJECTS
#  define STACK_SIZE          (1024*512)
#  define random              rand
#  define srandom             srand
#  define MAX_OFILES          20
#  define bcopy(from,to,len)  memcpy(to,from,len)
#  define bzero(p,len)        memset(p,0,len)
#  define bcmp                memcmp
#  define index               strchr
#  define TIME_H              <time.h>
#  define CAN_DUMP
#  define COFF
#    define PAGESIZE          4096
#  define TERMIO
#  define FORGETS_SIGNAL_HANDLER
#endif

#ifdef sequent
#  define VFORK
#  define CAN_LOAD_OBJ
#  define USE_GNULD           /* sawmill!rjk reports that /bin/ld is broken */
#  define CAN_DUMP
#    define SEGMENT_SIZE      2048
#    define FILE_TEXT_START   0
#    define MEM_TEXT_START    2048
#    define TEXT_LENGTH_ADJ   sizeof(struct exec)
#  define TIME_H              <sys/time.h>
#endif

#ifdef NeXT
#  define VFORK
#  define VPRINTF
#  define MACH_O
#  define CAN_LOAD_OBJ
#  define TIME_H              <sys/time.h>
#endif

/* Dumping needs re-entrant continuations to correctly start
 * a dumped executable (see P_Dump):
 */
#if defined(PORTABLE) && defined(CAN_DUMP)
 #error "PORTABLE and CAN_DUMP are mutually exclusive options"
#endif

#ifndef MAXPATHLEN
#  define MAXPATHLEN 1024
#endif

#ifndef PORTABLE
#  ifdef __GNUC__
#    define alloca	__builtin_alloca
#  endif
#endif

#ifdef sparc
#  include "sparc.h"
#endif


/* Constant definitions
 */

#define HEAP_SIZE            512       /* in KBytes */

#define OBARRAY_SIZE         1009      /* symbol hash table size */

#define GLOBAL_GC_OBJ        100

#define AFTER_GC_FUNCS       32
#define BEFORE_GC_FUNCS      32

#define STACK_MARGIN         (48*1024)  /* approx. stack_start - stkbase */

#define HEAP_MARGIN          (HEAP_SIZE/10*1024)

#define MAX_SYMBOL_LEN       1024

#define MAX_STRING_LEN       1024

#define MAX_MAX_OPEN_FILES   64

#define STRING_GROW_SIZE     64

#define DEF_PRINT_DEPTH      20
#define DEF_PRINT_LEN        1000

#define FLONUM_FORMAT        "%.15g"

#define MAX_TYPE             128

#ifdef CAN_DUMP
#  define INITIAL_STK_OFFSET   (20*1024)       /* 2*NCARGS */
#endif

#endif
