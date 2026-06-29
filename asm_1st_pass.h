/* Note : Header guard prevents only the same header file from being included multiple times
          within a single translation unit, not across different ones.
          Each tranlation unit translated separetly, unrelated to other units   */

#if !defined(asm_1st_pass_h)
#define asm_1st_pass_h

/* #ifndef uint
typedef unsigned int uint;
#endif */



/* define macro : */

#define err_check(COND, ERR_NUM, RETURN_PARAM)                            \
       if (COND)                                                          \
       {                                                                  \
              print_err(global_err_list_head, ERR_NUM, line_num, str, "");    \
              return RETURN_PARAM;                                        \
       }

#define err_check1(COND, ERR_NUM, STR_PARAM, RETURN_PARAM )                  \
       if (COND)                                                             \
       {                                                                     \
              print_err(global_err_list_head, ERR_NUM, line_num, STR_PARAM,"");  \
              return RETURN_PARAM;                                           \
       }




/* this file defines types for first assembler pass

  Following data structures wiil be used for first and second assembler passes :
    1. labels table : binary search tree
           labels table will be binary search linked list sorted in alphabetical order
           according to label name. Each element in BST will be struct.
    2. raw_data for assembly instructions code preparatiion : linked list based on starting adress
           of each instruction.
    3. raw data for assembly data code preparation: linked list based on starting address
           of each instruction.

*/

typedef struct asm_1st_pass_status
{

       uint one_word_in_line : 1;
       uint valid_label_before_cmd : 1;

} asm_1st_pass_status;

enum label_types
{
       ERR_ONLY_LABEL_FOUND = -2,
       INVALID_LABEL,
       VALID_CMD,
       VALID_LABEL,
       FILE_FINISHED
};

#endif /* #if !defined(asm_1st_pass_h)    */

/*  Function : asm_1st_pass
         First assembler pass, analyses .am file and prepares data for
         second assembler pass.

    Parameters:
         no implicit parameters.
         but at finish of asm_1st_pass
         following data structures are prepared :
            1.  data_blk_item : linked list to store information of data directives.
                This linked list is arranged in oeder of data directives.
                We access every element in this linked list through 2 global pointers :
                            global_d_head
                            global_d_tail
            2.  i_item :  linked list to store information of CPU instructions .
                This linked list is arranged according to order of CPU commands.
                We acceess every element in this linked list through 2 global pointers :
                            global_i_head
                            global_i_tail
            3.  label_item : binary search tree of labels.
                This tree is arranged according to labels names keys.
                We access every element in this tree through following global pointer :
                            global_labels_table_ptr
    The above data structures will be used as input to second assembly pass.

    return

        returns errors number.
*/
int asm_1st_pass(char * am_fname, int *cpu_instr_mem_size, int *cpu_data_mem_size);
/*
  Function check_cmd_label:
    checks the first word of command, if it is valid label.
    Error will be reported in following cases ;
        - If it is valid label (i.e. differ from keyword/command and macro names)
          BUT without ':';
        - If it is valid label (ends with ':') alone, i.e. end of string
          appears immediately after it or there are white characters after it.
        - If the label ends with ':', but there are following errors:
            1. the label is too long
            2. the label not starts with alphabet char
            3. the label has not alphanum chars after first char.
            4. the label has name identical to command/keyword
            5. the label has name identical to macro name
  Parameters :
        str : pointer to pointer to begin of string to be analyzed
        line_nume : number of code line that is analyzed
        label_name : pointer to pointer to label name string.
  Return :
       -2 (ERR_ONLY_LABEL_FOUND) - only label is found in string
       -1 (INVALID_LABEL) - invalid label
        0 (VALID_CMD) - the first word does not end with ':', and appears in keywords list
        1 (VALID_LABEL) - valid label
        2 (FILE_FINISHED) - received last empty string

  In case of valid label parameter str will point at function finish on
        ' ' character after the label to allow further analyse of
        command string.
*/
int check_cmd_label(char **str, int line_num, char **lable_name);

/* function analyze_cmd :
        Checks command parameter within string
        If it found, returns keywords_item_ptr pointer to keywords_item
        that includes requirements for the command.

        Checks for several errors :
            - if given word is not command
            - if command requires at least 1 parameter, but EOS find in place of it.

        Returns
                - NULL if command is invalid, or error detected
                - pointer to keyword table item , that contains keyword features.

        In case of valid command that ends with ' ' delimeter and has at lest one parameter
        sets *str pointer to ' ' character after a command.

        In case of valid command that has no parameters (ends with whites and EOS or EOS),
        does not change the pointer *str position.

        In addition this function prepares instruction linked list and data linked list,
        and start to fill them for commands that need no parameters.
        Only valid commands will be inserted to instruction linked list and
        data linked list !
*/
keywords_item_ptr analyze_cmd(char **str, int line_num, int instr_cnt, int data_cnt,
                              i_item_ptr *i_item, data_blk_item_ptr *d_item);

/* function analyze_cpu_instr_params:

    - analyse cpu instruction parameters vs. cpu instruction requirements
    - update i_item fields, including its binary fields for code preparation.
    - calculate next instruction counter value
    parameters:
        full_str  - pointer to full line , for errors reports
        str       - pointer to pointer to parameters part of line only, for analysis
        cmd_reqs  - object that stor command requirements
        i_item    - pointer to pointer to instruction item object

    return:

        (-1) CMD_ERR       : in case of error
        ( 0) CMD_VALID     : the command is valid

*/
int analyze_cpu_instr_params(char *full_str, char **str, int line_num,
                             keywords_item_ptr cmd_reqs, i_item_ptr *i_item);

/* function report_matrix_param_errors
    parameters:
        mat_status : matrix status object
        line_num   : code's line number
        str        : code's string
        param_num  : number of parameter (1 or 2 , will change report contents accordingly)
    return:
     -1 (CMD_ERR) if there is a problem in matrix call
      0 (CMD_VALID) if matrix call is valid

*/
int report_matrix_param_errors(mat_param_status *mat_status, int line_num, char *str, int param_number);

/* function addrsn_check

    Checks if parameter's type addressing type is allowed.
    parameters :
        type : type of parameter object
        param_num : parameter number (1 for 1st and 2 for 2nd)
        cmd_reqs : given command requirements object
    return:
        1 (PARAM_ALLOWED)   - param. is allowed
        0 (PARAM_FORBIDDEN) - param. is forbidden
*/
int addrsn_check(param_type type, int param_num, keywords_item_ptr cmd_reqs);

/* function matrix_1st_param_define:

       define and check  matrix call parameter ( 1st parameter)
       parameters:
              i_item : instruction item object
              cmd_reqs : requirements for given command
              str : pointer to pointer to parameters string
              matrix_label_name : matrix label name
              reg1 : register 1 number
              reg2 : register 2 number
              line_num : line number
        return :
            CMD_ERR   (-1) : command is not valid
            CMD_VALID ( 1) : command is valid
*/
int matrix_1st_param_define(i_item_ptr *i_item, keywords_item_ptr cmd_reqs, char **str,
                            char *matrix_label_name, int reg1, int reg2, int line_num);

/* function matrix_2nd_param_define:

       define and check matrix call parameter ( 2nd parameter)
       parameters:
              i_item : instruction item object
              cmd_reqs : requirements for given command
              str : pointer to pointer to parameters string
              matrix_label_name : matrix label name
              reg1 : register 1 number
              reg2 : register 2 number
              line_num : line number
        return :
            CMD_ERR   (-1) : command is not valid
            CMD_VALID ( 1) : command is valid
*/
int matrix_2nd_param_define(i_item_ptr *i_item, keywords_item_ptr cmd_reqs, char **str,
                            char *matrix_label_name, int reg1, int reg2, int line_num);

/* function label_2nd_param_define:
       define and check label parameter ( 2nd parameter)
       parameters:
              i_item : instruction item object
              cmd_reqs : requirements for given command
              str : pointer to pointer to parameters string
              txt : label text
              line_num : line number
        return :
            CMD_ERR   (-1) : command is not valid
            CMD_VALID ( 1) : command is valid
*/
int label_2nd_param_define(i_item_ptr *i_item, keywords_item_ptr cmd_reqs,
                           char **str, char *txt, int line_num);

/* function label_1st_param_define:
       define and check label parameter (1st parameter)
       parameters:
              i_item : instruction item object
              cmd_reqs : requirements for given command
              str : pointer to pointer to parameters string
              txt : label text
              line_num : line number
        return :
            CMD_ERR   (-1) : command is not valid
            CMD_VALID ( 1) : command is valid
*/
int label_1st_param_define(i_item_ptr *i_item, keywords_item_ptr cmd_reqs, char **str, char *txt, int line_num);

/* function number_1st_param_define:

       define and check number parameter (1st parameter)
       parameters:
              i_item : instruction item object
              cmd_reqs : requirements for given command
              str : pointer to pointer to parameters string
              txt : label text
              param_num : parameter number
              line_num : line number
        return :
            CMD_ERR   (-1) : command is not valid
            CMD_VALID ( 1) : command is valid
*/
int number_1st_param_define(i_item_ptr *i_item, keywords_item_ptr cmd_reqs, char **str,
                            char *txt, int param_num, int line_num);

/* function number_2nd_param_define:

       define and check number parameter (2nd parameter)
       parameters:
              i_item : instruction item object
              cmd_reqs : requirements for given command
              str : pointer to pointer to parameters string
              txt : label text
              praram_num : parameter number
              line_num : line number
        return :
            CMD_ERR   (-1) : command is not valid
            CMD_VALID ( 1) : command is valid
*/
int number_2nd_param_define(i_item_ptr *i_item, keywords_item_ptr cmd_reqs, char **str,
                            char *txt, int param_num, int line_num);

/* function register_1st_param_define:

       define and check register parameter (1st parameter)
       parameters:
              i_item : instruction item object
              cmd_reqs : requirements for given command
              str : pointer to pointer to parameters string
              txt : label text
              reg_num : register number
              line_num : line number
        return :
            CMD_ERR   (-1) : command is not valid
            CMD_VALID ( 1) : command is valid
*/

int register_1st_param_define(i_item_ptr *i_item, keywords_item_ptr cmd_reqs, char **str,
                              char *txt, int reg_num, int line_num);

/* function register_2nd_param_define:
       define and check register parameter (2nd parameter)
       parameters:
              i_item : instruction item object
              cmd_reqs : requirements for given command
              str : pointer to pointer to parameters string
              txt : label text
              reg_num : register number
              line_num : line number
        return :
            CMD_ERR   (-1) : command is not valid
            CMD_VALID ( 1) : command is valid
*/
int register_2nd_param_define(i_item_ptr *i_item, keywords_item_ptr cmd_reqs, char **str,
                              char *txt, int reg_num, int line_num);

/* function i_item_size_calc

        calculate command size in words (used to calculate next IC address for next command),
        including command word and params words.
        thew size depends on source and destination addressing methods, and
        also command definition word is included in it.

        return :
            command size in words in CPU memory.

*/
int i_item_size_calc(i_item_ptr i_item);

/*

    function data_directive_process:

         processes .data directive
         each new data takes one word in data memory

        return :
        DATA_ERR   (-1) : data params error
        DATA_VALID ( 0) : data valid

        Note:  .data directive must have at leat one paramter.
*/

int data_directive_process(char **str, int line_num, data_blk_item_ptr *d_item,
                           keywords_item_ptr cmd_reqs, int *data_cnt);

/*
   function init_label_status

        initializes label_status object
*/
void init_label_status(is_cmd_lbl_stat *label_status_p);

/*  function d_item_init

        creates and initializes data item object.
*/
void d_item_init(data_blk_item_ptr *d_item, keywords_item_ptr cmd_reqs, int data_cnt);

/*

        function string_directive_process:

             processes .string directive
             each new data takes one word in data memory

            return :
            DATA_ERR   (-1) : data params error
            DATA_VALID ( 0) : data valid

            Note:  .string directive must have exectly one paramter between " " .
                   only visible ASCII codes can be used, as codes from 32 to 126

*/

int string_directive_process(char **str, int line_num, data_blk_item_ptr *d_item,
                             keywords_item_ptr cmd_reqs, int *data_cnt);

/*

    function mat_directive_process:

         processes .mat directive that can have 0...inf parameters
         calculates matrix total size by mult. rows x cols
         analyzes and stores the numbers that appear in command.
         complement numbers with 0 , if number not defined in parameters list.

         If defined too many parameters - it is error.

         each new data takes one dditional word in data memory

            Note: after .mat directive must appear at least one white char.

        return :
        DATA_ERR   (-1) : data params error
        DATA_VALID ( 0) : data valid
*/
int mat_directive_process(char **str, int line_num, data_blk_item_ptr *d_item,
                          keywords_item_ptr cmd_reqs, int *data_cnt);

/*
    function extern_directive_process:

            extern directive does not generate any data (only specifies label's type).
            It defines only one label parameter as external label.

            This label will be checked and inserted in table of labels (binary search tree).
            This function does not check if the label defined as entry also.
            This will be checked later.
        return :
        DATA_ERR   (-1) : in case of any error detection
        DATA_VALID ( 0) : in case of valid processing
*/
int extern_directive_process(char **str, int line_num);

/*
    function entry_directive_process:

            entry directive does not generate any data (only specifies label's type).
            It defines only one label parameter as entry label.

            This label will be checked and inserted in table of labels (binary search tree).
            This function does not check if the label defined as external also.
            This will be checked later.

        return :
        DATA_ERR   (-1) : in case of any error detection
        DATA_VALID ( 0) : in case of valid processing
*/
int entry_directive_process(char **str, int line_num);

/* function upd_d_items_addr :

    add same offset value to all items addresses in linked list
    (will be done at finish of assembly 1st stage )

    parameters :
        head, tail : start and finish pointers of linked list
        addr_offset : address offset number that will be added to all
                      addr parameters of all items in the list.
    return :
        last program address
*/
int upd_d_items_addr(data_blk_item_ptr head, data_blk_item_ptr tail, int addr_offset);

/*
 function labels_table_data_addr_upd
    Update data addresses in labels table

    if the labele points to .data or to .str or to .mat
    add offset to its address value.

    The recursive update is performed "in order" (Go left, update parent, go right).

*/

void labels_table_data_addr_upd(lable_item_ptr label_table, int addr_offset);