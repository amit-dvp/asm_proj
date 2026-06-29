/* cpu instructions linked list will
   store data necessary to build
   assembly code and validate cpu instructions.

   Every item in this linked list will contain
   data assotiated wihth one assembly cpu command.

   The items in the linked list will be added in the 
   same order as CPU instructions lines read. 

   To allow get out linked list items in the order of their arrival
   (from the head to tail) we will use pointer to head of linked list.
   Every new item will be added to the end (tail) of list.
   To simplify addition of the new items, pointer to tail (end) of 
   linked list will be used.
*/

#if !defined(mngr_cpu_instr_lnk_lst_types_h)

#define mngr_cpu_instr_lnk_lst_types_h

/* #ifndef uint
typedef unsigned int uint;
#endif */

/* struct describes features of parameter of type matrix*/
typedef struct mat_validity
{
    uint mat_label_syntax_valid : 1;
    uint mat_label_defined : 1;
    uint brackets_valid : 1;
    uint reg1_name_valid : 1;
    uint reg2_name_valid : 1;
} mat_validity;

/* struct describes parameter of type matrix */
typedef struct mat_param
{
    char *mat_label;
    int reg1_num;
    int reg2_num;
    mat_validity validity;
} mat_param;

/* struct describes label parameter of command */
typedef struct label_validity {
    uint label_param_syntax_valid : 1;
    uint label_length_valid : 1;
    uint label_addr_defined : 1;       /* label appears in labels table and has address */
} label_validity;

/* label parameter */
typedef struct label_param {
    char * label_name;
    int label_addr;
    label_validity validity;
} label_param;

typedef struct num_validity{
    uint num_is_valid : 1;
} num_validity;

/* number parameter */
typedef struct num_param
{
    int num;
    num_validity validity;
} num_param;

typedef struct reg_validity
{   uint reg_syntax_is_valid : 1;
    uint reg_num_is_valid : 1;
} reg_validity;

/* reg parameter */
typedef struct reg_param
{
    int reg_num;                  /* register number */
    reg_validity validity;
} reg_param;

/* defines optional types of parameter 1 */
typedef struct param_type {
    uint param_is_number : 1;   /* addressing type of 2-nd param is 0 (immediate    )*/
    uint param_is_label : 1;    /* addressing type of 1-st param is 1 (direct       )*/
    uint param_is_matrx : 1;    /* addressing type of 1-st param is 2 (matrix access) */
    uint param_is_reg_name : 1; /* addressing type of 1-st param is 3 (register     ) */
    uint no_param : 1;          /* set in case when parameter is not  defined */
} param_type;

typedef union params_union
{                  
    num_param num; /* only one of types is used in one command parameter */
    label_param lbl;
    mat_param mat;
    reg_param reg;
    char *no_param; /* will be set to NULL in case of no param. or param. not defined*/
} p_union;

typedef struct param_info {
    param_type param_type;
    p_union params_union;
} param_info;


typedef struct code_word {   /* this struct explains code word */
    uint exist : 1;          /* set 1 if code word exist  */
    uint E_R_A : 2;          /* E R A data */
    uint codeval : 10;       /* decimal coded 10 bit binary code word */
    uint ic_addr : 16;       /* address of code word (including spare bits)*/
} code_word;

typedef struct code_info {
    code_word i_wrd1; /* 1-st word : command's title (bin2dec value) */
    code_word i_wrd2; /* optional : 2-nd word of command (bin2dec value) */
    code_word i_wrd3; /* optional : 3-rd word of command (bin2dec value) */
    code_word i_wrd4; /* optional : 4-th word of command (bin2dec value)*/
    code_word i_wrd5; /* optional : 5-th word of command (bin2dec value)*/
} code_info;

typedef struct cpu_cmd_status
{
    uint cmd_label_found : 1; /* the cpu command has label: */
    uint params_num : 3;
    uint no_params_found : 1; /* no params found (flag) */
    uint param1_found : 1;    /* if first parameter is found */
    uint param2_found : 1;    /* if second param is found */
    uint dest_addrsng : 2; /* addressing of destination */
    uint src_addrsng : 2;  /* source addressing */
    uint opcode : 4;       /* is one of CPU commands name */
} cpu_cmd_status;



/* pointer to cpu_i_item */
typedef struct cpu_instr_ll_item * i_item_ptr;


/* following struct describes cpu instruction linked list item */
typedef struct cpu_instr_ll_item
{
    int start_addr;               /* start address of cpu command in instructions memory, must be 0...255 */
    char *cmd_name;                /* must match one of cpu commands in keywords list */
    int size_in_wrds;              /* data size of cpu command (in words : 1,2,3,4 or 5 */
    uint is_valid;                 /* instruction validity summary */
    char* label_name;              /* cpu istruction label name */
    cpu_cmd_status status;         /* status: bit fields of one 32-bit number */
    param_info param1;             /* parameter 1 description */
    param_info param2;             /* parameter 2 description */
    code_info code;                /* code data */
    i_item_ptr next; /* pointer to next item */
} i_item;

#endif /* #if !defined(mngr_cpu_instr_lnk_lst_types_h) 
        */
