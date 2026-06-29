/* Note : Header guard prevents only the same header file from being included multiple times
          within a single translation unit, not across different ones.
          Each tranlation unit translated separetly, unrelated to other units   */

#if !defined(assembler_globals_h)

#define assembler_globals_h

#include <stdio.h>
#include "mngr_errors_types.h"

/* file extentions global constants 
   these are global constant pointers to constant array of chars */
char * const ASM_EXTN = ".as";        /* const pointer to input assembly file extention             */
char * const PRE_ASM_EXTN = ".am";     /* const pointer to extention of result of pre-assembel (macros expansion) */
char * const OBJ_EXTN = ".ob";         /* const pointer to extention of object file */
char * const EXTERNALS_EXTN = ".ext";  /* const pointer to extention of extenals file */
char * const ENTRY_EXTN = ".ent";      /* const pointer to extention of entries file  */



char *const MCRO_CMD = "mcro";    /* const pointer to mcro command */
char *const MCROEND_CMD = "mcroend";  /* const pointer to mcro end command */


#endif


