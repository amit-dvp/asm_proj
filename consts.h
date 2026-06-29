/* Note : Header guard prevents only the same header file from being included multiple times
          within a single translation unit, not across different ones.
          Each tranlation unit translated separetly, unrelated to other units   */

#if !defined(consts_h1)
#define consts_h1

#define MAX_LINE_SIZE 80    /* maximum size of assembly code line */
#define MEM_BLK_LENGTH 256  /* length of memory block*/
#define MAX_STR_LEN (MAX_LINE_SIZE+1) /* maximum string length from 0..79, including '\n' and '\0' */
#define MAX_MACRO_AND_LABEL_LENGTH 30
#define ERR_STREAM stdout             /* stream for error reports, can be set also stderr */
#define MIN_INT_8_BIT_NUM -128      /* minimum int 8 bit num*/
#define MAX_INT_8_BIT_NUM  127       
#define INSTR_CNT_INIT_VAL 100
#define MAX_ERR_DESCR_LEN 150
#define MAX_ERR_TYPE_LEN 30
#define TO_BE_DEFINED -5  /* special negative value, means label's address will be defined */

#define ONE_WORD_SRC 1
#define ONE_WORD_DST 1
#define TWO_WORDS_SRC 2
#define TWO_WORDS_DST 2
#define CMD_WORD_SIZE 1
#define ONE_WORD_SRC_DST 1

enum boolvals
{
    FALSE,
    TRUE
};


#define NORM_LENGTH_AND_EOF -1          /* string has normal length and finished with EOF  */
#define TOO_MANY_CHARS_AND_ENTR -2      /* string has too many characters and finished with '\n'*/
#define TOO_MANY_CHARS_AND_EOF -3       /* string has too many characters and finished with EOF */

#define START_OF_COMMENT ';'

enum name_status
{
    VALID_NAME,
    NOT_ALPHABET,
    NOT_ALPHANUM
};

enum macro_cmd_find_status
{
    MCRO_CMD_NOT_FOUND,
        MCRO_CMD_FOUND
};

enum macroend_cmd_find_status
{
    MCROEND_CMD_NOT_FOUND,
    MCROEND_CMD_FOUND
};

typedef enum pre_asm_2_stage
{
    SEARCH_MACRO_CMD_OR_MACRO_SUBST,
    MACRO_CMD_DETECTED,
    STORE_LINES,
    DETECTED_MACRO_END,
    MACRO_SUBST
} pre_asm_2_stage_state;

enum macro_subst
{
    MCRO_SUBST_NOT_FOUND,
    MCRO_SUBST_FOUND
};

enum comment_lines_check
{
    IS_NOT_COMMENT_LINE,
    IS_COMMENT_LINE
};

enum empty_lines_check
{
    IS_NOT_EMPTY_LINE,
    IS_EMPTY_LINE
};

enum instr_find
{
    NOT_INSTR,          /* not instruction */
    CPU_INSTR,          /* cpu instruction */
    DATA_DIRECTIVE      /* data directive  */
};

enum str2dec_opts
{
    NOT_NUM,
    VALID_NUM
};

#define NUM_PARAM_START_CHAR '#'

enum num_param_features
{
    NUM_PARAM_LIM_FAULT = -3,
    NUM_PARAM_SYNTAX_FAULT,
    NOT_NUM_PARAM,
    PARAM_NUM_IS_VALID

};

enum regs_names
{
    NO_REG_NAME=-1,
    R0_NAME,    
    R1_NAME,  
    R2_NAME,
    R3_NAME,
    R4_NAME,
    R5_NAME,
    R6_NAME,
    R7_NAME 
};

enum params_check_res
{

    CMD_ERR = -1,
    CMD_VALID

};

enum data_check_res
{

    DATA_ERR = -1,
    DATA_VALID = 1

};

enum param_en
{
    PARAM_FORBIDDEN,
    PARAM_ALLOWED
};

enum add_label_feature_res
{

    ERR_LABEL_ADDR_EXISTS,
    LABEL_ADDR_UPDATED

};


enum label_column_chk {

NO_COLON_CHK,
COLON_CHK_EN

};


enum error_options{
    NO_ERR,
    ERR
};


enum are_val {

    ABSOLUTE_ARE,
    EXTERNAL_ARE,
    RELOCATABLE_ARE
};


/* file extentions global constants */
extern char *const ASM_EXTN;        /* input assembly file extention             */
extern char * const PRE_ASM_EXTN;   /* extention of result of pre-assembel (macros expansion) */
extern char * const OBJ_EXTN;       /* extention of object file */
extern char * const EXTERNALS_EXTN; /* extention of extenals file */
extern char * const ENTRY_EXTN;     /* extention of entries file  */





#endif /* #if !defined(consts_h) */