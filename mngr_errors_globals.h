/* Note : Header guard prevents only the same header file from being included multiple times
          within a single translation unit, not across different ones.
          Each tranlation unit translated separetly, unrelated to other units   */

#if !defined(errors_mngr_globals_h)

#define errors_mngr_globals_h

/* define global constants */
char * const PRE_ASM_ERR = "pre-assembler error";
char * const DYN_ALC_ERR = "dynamic allocation error";
char * const FILE_MNG_ERR = "file manage error";
char * const ASM_1_ERR = "assembler 1st pass error "; 

#endif