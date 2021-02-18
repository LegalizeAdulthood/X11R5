/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

static char *doload_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/class/machdep/apollo_68k/RCS/doload.c,v 1.3 1991/09/12 17:00:43 bobg Exp $";

/* 
	Replacement for doload.c for the Apollo dynamic loader.
 */

#include <stdio.h>
#include <apollo/base.h>
#include <apollo/loader.h>
#ifndef loader_$kg_function
# define loader_$kg_function loader_$kg_define_pad1
#endif
#ifndef loader_$kg_symbol
# define loader_$kg_symbol -1
#endif
#include <apollo/error.h>

int doload_trace=0;

/*
 * Symbols to be loaded into KGT
 */
extern int class_Error;
extern char *class_RoutineStruct;
extern class_IsTypeByName();
extern class_IsType();
extern class_NewObject();
extern class_Load();
extern class_EnterInfo();
 
static struct kgt_sym
{
        char *kgt_name;
        long kgt_addr;
        loader_$kg_define_opts kgt_opt;
} kgt_syms[] =
{
        "class_Error",         (long)&class_Error,         0,
        "class_RoutineStruct", (long)&class_RoutineStruct, 0,
        "class_IsTypeByName",  (long)&class_IsTypeByName,  loader_$kg_function,
        "class_IsType",        (long)&class_IsType,        loader_$kg_function,
        "class_NewObject",     (long)&class_NewObject,     loader_$kg_function,
        "class_Load",          (long)&class_Load,          loader_$kg_function,
        "class_EnterInfo",     (long)&class_EnterInfo,     loader_$kg_function,
};

#define N_KGT_SYMS (sizeof(kgt_syms)/sizeof(struct kgt_sym))
        
/*
 * error_fmt - format Domain error text
 */
static char *
error_fmt( st )
        status_$t st;
{
        error_$string_t ss, mn, tx;
        short ssl = (short)sizeof(ss), mnl = (short)sizeof(mn), txl = (short)sizeof(tx);
        
        error_$get_text( st, ss, &ssl, mn, &mnl, tx, &txl );
        ss[ssl] = mn[mnl] = tx[txl] = '\0';
        return(tx);
}

/*
 * doload - loads a package into the process's address space. Returns Entry point address
 *          or NULL if error.
 */
char *doload(inFD, name, bp, lenP, path )
int inFD;			/* open fd for package file */
char *name;			/* name of package being loaded */
char **bp;			/* base address of package */
long *lenP; /* Not used by ap_doload */
char *path;			/* Pathname of package being loaded */
{
        loader_$handle_t handle;
        char *EntryPoint = NULL;
        status_$t st;
        loader_$opts opts = loader_$copy_proc | loader_$load_only_post_sr9_5 | loader_$install;
        long sym_addr;
        loader_$kg_lookup_opts lookup_opts = loader_$kg_data_only;
        loader_$sym_flags sym_type;
        int sym_sect = loader_$kg_symbol;
        int k;
        
    	     *lenP = 0;
        /*
         * Put class_Error and class_RoutineStruct into KGT
         */
        for ( k = 0; k < N_KGT_SYMS; k++ )
        {
                if ( (sym_type = loader_$kg_lookup( 
                                                    kgt_syms[k].kgt_name,
                                                   (short)strlen(kgt_syms[k].kgt_name),
                                                   sym_sect,
                                                   lookup_opts,
                                                   (void *)&sym_addr )) == 0 )
                {
                        /*
                         * Not found, insert it
                         */
                        if ( doload_trace )
                          printf( " %s: defining symbol: %s = 0x%.8x\n", name, kgt_syms[k].kgt_name, kgt_syms[k].kgt_addr );
                        
                        loader_$kg_define( 
                                           kgt_syms[k].kgt_name,
                                          (short)strlen(kgt_syms[k].kgt_name),
                                          (void *)kgt_syms[k].kgt_addr,
                                          sym_sect,
                                          kgt_syms[k].kgt_opt,
                                          &st );
                        if ( st.all != status_$ok )
                        {
                                fprintf( stderr,
                                        "doload: Error defining symbol \"%s\" for package \"%s\" - %s\n",
                                        kgt_syms[k].kgt_name, path, error_fmt( st ) );
                                return NULL;
                        }
                }
                else
                  if ( doload_trace )
                    printf( " %s: found symbol %s = 0x%.8x(0x%.4x)\n", name, kgt_syms[k].kgt_name, sym_addr, sym_type );
        }

        /*
         * Load package
         */
        loader_$load( name, 
                               (short)strlen( name ), 
                               opts, 
                               &handle,
                              &st );
        if ( st.all != status_$ok )
        {
                fprintf( stderr, "doload: Error loading package \"%s\" - %s\n", path, error_fmt(st) );
                return NULL;
        }
        
        /*
         * Return entry point
         */
        EntryPoint = loader_$lookup_start_addr( handle, &st );
        if ( st.all != status_$ok )
        {
                fprintf( stderr, "doload: Error finding entry point of package \"%s\" - %08x\n", path, st.all );
                return NULL;
        }
        if ( doload_trace )
          printf(" %s: entry = 0x%.8x\n", name, EntryPoint);
        
        return( EntryPoint );
}
