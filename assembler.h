
/* Note : Header guard prevents only the same header file from being included multiple times 
          within a single translation unit, not across different ones. 
          Each tranlation unit translated separetly, unrelated to other units   */

#if !defined(assembler_h)
#define assembler_h

#ifndef uint
typedef unsigned int uint;
#endif

#include <stdlib.h> /* for malloc and free */
#include <stdio.h>  /* NULL , printf, scanf etc. definition */
#include <string.h> /* strcmp, etc.*/
#include <ctype.h>  /* isspace, etc. */
#include "mngr_labels_tbl_types.h"
#include "mngr_macro_tbl_types.h"
#include "mngr_keywords_list_types.h"
#include "cmd_line_analyse_types.h"
#include "consts.h"
#include "mngr_errors_types.h"
#include "mngr_cpu_data_lnk_lst_types.h"
#include "mngr_cpu_instr_lnk_lst_types.h"
#include "asm_1st_pass.h"
#include "asm_2nd_pass.h"

/* global pointers */
extern error_def_item_ptr global_err_list_head;   /* global pointer to pointer to linked list of errord definitions */
extern keywords_item_ptr global_keyword_table_ptr;   /* global pointer to store top of keywords BST */
extern macro_table_entry_ptr global_macro_table_ptr; /* global pointer to top of macros BST (Binary Search Tree) */
extern data_blk_item_ptr global_d_item_ptr ;

extern i_item_ptr global_i_head; /* global head of instruction items linked list */
extern i_item_ptr global_i_tail;  /* global tail of instruction items linked list */

extern data_blk_item_ptr global_d_head; /* global head of data items linked list */                   
extern data_blk_item_ptr global_d_tail; /* global tail of data items linked list */

/* global pointer to CPU data linked list head */
extern data_blk_item_ptr head;
/* global pointer to CPU data linked list tail */
extern data_blk_item_ptr tail;

extern lable_item_ptr global_labels_table_ptr; /* global pointer to labels binary search tree */

extern char *global_gen_str_pointer;  /* gen. purposer global string pointer to string object in heap */

/* file extentions global constants */
extern char * const ASM_EXTN;       /* const pointer to input assembly file extention             */
extern char * const PRE_ASM_EXTN;   /* const pointer to extention of result of pre-assembel (macros expansion) */
extern char * const OBJ_EXTN;       /* const pointer to extention of object file */
extern char * const EXTERNALS_EXTN; /* const pointer to extention of extenals file */
extern char * const ENTRY_EXTN;     /* const pointer to extention of entries file  */

extern char * const MCRO_CMD ;       /* const pointer to mcro command */
extern char * const MCROEND_CMD ;    /* const pointer to mcro end command */

/* global pointer to full filename with .as extention*/
extern char *global_full_fname_as;
/* global pointer to full filename with .am extention */
extern char *global_full_fname_am;
/* global pointer to full filename with .ext extention */
extern char *global_full_fname_ext;
/* global pointer to full filename with .ent extention */
extern char *global_full_fname_ent;
/* global pointer to full filename with .ob extention */
extern char *global_full_fname_ob;
/* global pointer to full filename with .log extention */
extern char *global_full_fname_log;
/* global pointer to full filename with .tmp extention */
extern char *global_full_fname_tmp;

extern FILE *global_fin ; /* global pointer to input file    */
extern FILE *global_fout_1 ; /* global pointer to output file 1 */
extern FILE *global_fout_2 ; /* global pointer to output file 2 */

#define DEBUG_PRINT_EN 0 /* Set to 0 to disable debug prints */
#define NO_ARGS 1

#if DEBUG_PRINT_EN
   #define debug(X) printf("DEBUG: "); X /* put printf string as argument */
#else
   #define debug(X) ;   /* do nothing */
#endif


#endif /* #if !defined(assembler_h) */               


/* 
    function print_log_file
        prints log file with following information:
            - cpu instructions data 
            - cpu data 
            - labels table
*/
void print_log_file();





     /*---------------------------------------------*/
    /* Macro table management functions prototypes */
    /*---------------------------------------------*/

    /* Recursive function addtree: adds a node with macro_name inside the binary search tree.
        Based on addtree function in Karnigan and Ritchie , page 141.

        Return:
            Unchanged pointer to parent node that equals to its parameter
            (macro_table_entry_ptr parent) if the node exists (when parent pointer in not NULL).
            or new pointer to new dynamically allocated parent node if the the parent node
            does not exist (when parent pointer equals to NULL).
        Parameters:
            parent : top node of binary search tree
            macro_name : macro_name string to be inserted into binary search tree.

        Example of call to function:
            parent = addtree (parent, "macro_name");

        Important Note : it is IMPORTANT to set parent be NULL at first call to fucntion
                         undefined garbage value of parent pointer will cause to segmentation fault
                         (in better case...)
    */
    macro_table_entry_ptr add_macro_tree(macro_table_entry_ptr parent, char *macro_name);

/* Function add_line_to_table_entry: add line to node in tree that
   match given given macro name.

   Return:
       1 if node with given macro name appears in macros table
      -1 If node with given macro name does not appear in macro table

   Parameters:
       table_ptr  : pointer to top of binary search tree table of macros entries
       macro_name : pointer to macro name string
       macro_line : pointer to macro line string
*/
int add_line_to_table_entry(macro_table_entry_ptr table_ptr, char *macro_name, char *macro_line);

/* Function add_macro_line : adds given macro line to end of macro lines linked
   list assotiated with macro table entry.

   Parameters:
    table_entry_ptr : pointer to some macro table entry ;
    macro_line      : pointer to macro line string that we want to add to
                      above macro table entry.
*/
void add_macro_line(macro_table_entry_ptr table_entry_ptr, char *macro_line);

/* Recursive function macro_entry_search :
      Return:
               - pointer to node that match macro_name value
               - NULL if the node does not exist in the tree
      Parameters:
               macro_table : pointer to macro table tree top node
               macro_name  : pointer to macro name string
*/
macro_table_entry_ptr macro_entry_search(macro_table_entry_ptr macro_table, char *macro_name);

/* Recursive function is_macro_in_table : cheks is there is
   node in table that match given macro name text.
      Return:  - 0 if macro does not exist in macro table tree
               - 1 if macro exists in macro table tree
      Parameters:
               macro_table : pointer to macro table tree top node
               macro_name  : pointer to macro name string
*/
int is_macro_in_table(macro_table_entry_ptr macro_table, char *macro_name);

/* Recursive print of macro names in binary search tree
   in alphabetical order ("in order").

   Parameter:
        macro_table: pointer to the top node of binary search tree.
*/
void macros_names_print(macro_table_entry_ptr macro_table);

/* Recursive print of macro table tree
   The print is performed "in order" - alphabetic order of macro names.
   The macro table is printed in following order:
        Firstly appear macro name
        After it appear macro body lines assotiated with macro name.

   Parameter:
        macro_table: pointer to the top node of binary search tree.
*/
void macros_table_print(macro_table_entry_ptr macro_table);

/* Function tree_node_alloc: allocates new macro tree node
   and return pointer value to it.
   Parameter:
      macro_name : pointer to macro name string to be stored in new macro tree node.
   Return :
      pointer to new allocated macro tree node.
   Based on talloc function in Kernigan and Ritchie, page 142.
*/
macro_table_entry_ptr tree_node_alloc(char *macro_name);




/* Safe copy of any string to dynamically allocated char array.
   Limits copy length.
   Returns pointer to this array.

   It can be used to copy string created in temporay stack memory into
   heap memory.

   Based on strdup function in Karnigan and Ritchie , page.143
*/
char *heap_string_duplicate_safe(char *str, int length_limit);



/* To free a memory of macro binary search tree,
   we will use "post-order" (Go Left , Go Right, Free Root) traversal.
   This ensures that we free each root node AFTER its children have been freed,
   avoiding access to already freed memory.
*/
void free_macro_binary_search_tree(macro_table_entry_ptr root);


/*---------------------------------------------*/
/* Assembler requirements function prototypes  */
/*---------------------------------------------*/

/* Recursive function add_item_to_bst:
      adds a node with keyword parameters from k_arr[index] array element
      inside the binary search tree.
   Finds place to insert the node recursievly in the tree and inserts it
   when the node does not exist yet */
keywords_item_ptr add_item_to_bst(keywords_item_ptr parent,
                    keywords_array k_arr, int k_arr_length, int index);


/* Function keywords_bst_create : creates dynamically allocated binary search tree
     and returns pointer to top element of it.

   Note : the creation of BST is done in 2 stages :
      1. Creation of temporary unsorted internal (automatic) array with items contents.
         this array will disappear after finish of function run.
      2. Create binary search tree by recursive find place for each new node,
         dynamically creation of the node and copy array's values into new node's parameters.
         Return pointer to top of binary tree.
*/
keywords_item_ptr keywords_bst_create();


/* Function k_item_alloc :
    dynamic allocation of memory for keywords struct item
    and initialization of it based on value of keywords array k_arr[index]

    Return:
      pointer to node of binary search tree.
*/
keywords_item_ptr k_item_alloc(keywords_array k_arr, int index);


/* Recursive print of keywords table tree
    The print is performed "in order" - alphabetic order of keywords.
    The keywords table is printed in following order:
        Firstly appear keyword name
        After it appear keyword fetaures hexadecimal number.

    Parameter:
        keyword_table: pointer to the top node of binary search tree.
*/
void keywords_table_print(keywords_item_ptr keywords_table);

/* Recursive function is_keywrd_in_table : cheks is there is
   node in table that match given keyword name text (or any given text).
      Return:  - 0 if name does not exist in keyword table tree
               - 1 if name exists in macro keyword tree
      Parameters:
               keyword_table : pointer to keyword table tree top node
               keyword_name  : pointer to keyword name string
*/
int is_keyword_in_table(keywords_item_ptr keywords_table, char *keyword_name);

    /* To free a memory of keywords binary search tree,
        we will use "post-order" (Go Left , Go Right, Free Root) traversal.
        This ensures that we free each node after its children have been freed,
        avoiding access to already freed memory.
    */
    void free_keys_binary_search_tree(keywords_item_ptr root);

/* Function featue_to_num
   takes feature struct and prepares number from it */

int feature_to_num(keyword_features *feature);

/* Function num_to_feature takes unsigned number and stores it as
   features struct */
void num_to_feature(unsigned int num, keyword_features *feature);

/* Recursive function to check if the given word is inside binary search tree
      Return:
               - pointer to node that match keyword value
               - NULL if the node does not exist in the tree
      Parameters:
               keywords_table : pointer to macro table tree top node
               keyword_name     : pointer to macro name string
*/
keywords_item_ptr keywords_table_search(keywords_item_ptr keywords_table, char *keyword_name);



/*--------------------------*/
/* Errors manager functions */
/*--------------------------*/

/* Function errors_list_create :
    inserts errors definititions to dynamic linked list.
    Receives pointer to pointer to dynamic linked list
    and updates contents of pointer to dynamic linked list.
*/
void errors_list_create(error_def_item_ptr *list_h);

/* insert new item to linked list of errors definitions
   according to its error_number value,
   and return pointer to first item in the list

   Note: The function UPDATES value of pointer to list head.
         So we MUST pass the function pointer to <pointer to list head>
         to be able use the returned value in calling function.
*/
void add_err_2_lst(error_def_item_ptr *list_head,
                   int error_number, char *error_type, char *error_desription);

/* print errors linked list item according to its number
  here list_h is pointer to first item in the list.
*/
void print_err(error_def_item_ptr h, int item_num, int line_num, char *line_str, char *user_dscrpt);



    /*---------------------------------*/
    /*  pre_asm task functions         */
    /*---------------------------------*/

/*
  Function pre_asm_first_stage:
  performs first stage of pre-assembler task (check only).
  It gets asm file name (from command line) without extention .as
  Scans input file for
    validity of macro definitions (mcro command),
    validity of macro name within macro definition
    validity of macro definition finish (mcroend command).
    validity of macro substitution command (i.e. macro name only).
  Also checks length of every line (must be less of 80).

  Valid macro names will be added into macros binary search tree entries
  for future use in second pre-assembler stage and first stage's macro
  substitution commands check. Macro substitution commands must
  contain single word with macro name only.
  In this stage we detect error if macro definition appears as first word
  but there are additional words/non_white_chars after it.
  In case of any error detection this tree memory will be released (freed).



  Any error in mcro or macroend lines or in any line length will be reported.

  In case of any error, the second pre-assembly stage will not start.
  The function returns number of errors.
*/
int pre_asm_first_stage(char * file_name);

/* Function check_name:
   checks that the name has illegal characters
   returns
    0 (VALID_NAME)   - the name is valid
    1 (NOT_ALPHABET) - the first char is not aphabetic (small of uppercase letter)
    2 (NOT_ALPHANUM) - the remain chars are not alphanumeric
*/
int check_name(char *name);

/*
  Function pre_asm_second_stage :

    Performs second stage of pre-assembler second task, i.e.
    macros expansion and .am file preparation.
    Based on finite state machine.

    Macro substutution commands will be checked in second scan stage only in sense that
    we check macro's name appearance only in first word of line and
    check that additional words does not appear.

    At the second stage pre-assembler prepares an expanded .am file.
    It passes through already validated macro definitions and macro end and stores
    the macro names and the macro substitution lines in linked list inside binary search
    tree binded with appropriate macro names.

    Macro definition, macro body and macro end lines will not be copied into
    .am file.

    Each line with single word that match macro's name in macros table will cause to insertion
    of apropriate binded macro lines into .am file.

    Comment lines and empty lines will not be copied into .am file.

 */
int pre_asm_second_stage(char * file_name_as , char * file_name_am);

/* Function macro_def_detect:
      Detects if the given command line is mcro command.

      This function assumes that the command line syntax is valid,
      including macro's name (can be used in second stage of pre-assembler).

      Returns 1 (MCRO_CMD_FOUND) if it is mcro command line,
              0 (MCRO_CMD_NOT_FOUND) it is not mcro command line.

      Also returns macro_name that appears after mcro command.
*/
int macro_def_detect(char *cmd_line, char *macro_name);

/* Function macro_end_def_detect:
      Checks if the given line has mcroend command.
      Assumes that the file is already checked in first pre-assembler scan
      stage and the line with mcroend command is valid.

      Returns 1 (MCROEND_CMD_FOUND) if it is mcro command line,
              0 (MCROEND_CMD_NOT_FOUND) it is not mcro command line.

*/
int macro_end_def_detect(char *cmd_line);

/* Function macro_subst_detect:
      Checks if the given line has macro substitution command.
      We suppose that all cases where the first word is macro subst command
      are already checked in first stage.

      Returns 1 (MCRO_SUBST_FOUND) if it is mcro substitution command line,
              0 (MCRO_SUBST_NOT_FOUND) it is not mcro substitution command line.

              Also returns macro name in case when MCRO_SUBST_FOUND.
*/
int macro_subst_detect(char *cmd_line, char *macro_name);

/* Function comment_line_detect:
    Checks if the given line is comment line.
    Return:
        0 (IS_NOT_COMMENT_LINE) if it is not comment line
        1 (IS_COMMENT_LINE) if it is comment line
*/
int comment_line_detect(char *cmd_line);

/* Function empty_line_detect:
    Checks if the given line has empty line:
        - starts with EOS
        - contains only white characters and EOS

        Note: we already ommited '\n' and EOF while read the line.

    Return:
        0 (IS_NOT_EMPTY_LINE) if it is not empty line
        1 (IS_EMPTY_LINE) if it is empty line
*/
int empty_line_detect(char *cmd_line);






    /*------------------------------------*/
    /*     mngr_cpu_data_lnk_lst          */
    /*------------------------------------*/

/* function add_data_blk_item :
       creates the new item
       adds the new updated item to the end of linked
       list.
    updates tail and head, if needed.
    returns pointer to the new item

    Note : the new item is inttialized to default values only.
*/
data_blk_item_ptr add_new_data_blk_item(data_blk_item_ptr *head, data_blk_item_ptr *tail);

/* function print_data_blk_item :
    Prints data of data block item
    Parameter : pointer to data block item
*/
void print_data_blk_item(data_blk_item_ptr data_item);

/* function print_data_blk_item :
    Prints data of data block item
    Parameter : pointer to data block item
*/
void f_print_data_blk_item(FILE *fd, data_blk_item_ptr data_item);

/* function print_d_items_list

    prints all data items list info

*/
void print_d_items_list(data_blk_item_ptr head, data_blk_item_ptr tail);

/* function f_print_d_items_list

    prints all data items list info to file

*/
void f_print_d_items_list(FILE *fd, data_blk_item_ptr head, data_blk_item_ptr tail);

/* function add_item_to_num_lnk_lst
        adds num to end of internal data linked list

    We will change head pointer and tail pointers NULL to point first item in case
          of empty list, so we use double pointer for them.
    Returns pointer to new_item
*/
num_lnk_lst_ptr add_item_to_num_lnk_lst(num_lnk_lst_ptr *head, num_lnk_lst_ptr *tail, int num);

/* this function prints num list */
void print_num_lnk_lst(num_lnk_lst_ptr head, num_lnk_lst_ptr tail);

/* this function prints num list to file*/
void f_print_num_lnk_lst(FILE *fd, num_lnk_lst_ptr head, num_lnk_lst_ptr tail);




    /*----------------------------------------*/
    /*    mngr_cpu_instr_lnk_lst              */
    /*----------------------------------------*/

    /* function add_cpu_instr_item :
           creates the new item
           adds the new updated item to the end of linked
           list.
        updates tail and head, if needed.
        returns pointer to the new item
        Note : the new item is inttialized to default values only.
    */
    i_item_ptr add_cpu_instr_item(i_item_ptr *head, i_item_ptr *tail);

/* function print_i_items_list 

    prints all instruction items list info

*/
void print_i_items_list(i_item_ptr head, i_item_ptr tail);



/* function print_i_items_list

    prints all instruction items list info to file

*/
void f_print_i_items_list(FILE * fd , i_item_ptr head, i_item_ptr tail);



/* function print_cpu_i_item :
    Prints data of CPU instruction item in linked list
    Parameter : pointer to CPU instruction item in linked list
*/
void print_cpu_i_item(i_item_ptr i_item);



/* function print_cpu_i_item :
    Prints data of CPU instruction item in linked list
    Parameter : pointer to CPU instruction item in linked list
*/
void f_print_cpu_i_item(FILE * fd, i_item_ptr i_item);


/*

  function reset_inst_item  
        sets default values to all instruction item data parameters
    parameter :
        item : pointer to pointer to instruction item

 */

 void reset_i_item (i_item_ptr *item);


/*-------------------------------------------*/
/*               mngr_labels_tbl             */
/*-------------------------------------------*/

/* Recursive function add_labels_tree:
    adds a node with label_name inside the Binary Search Tree.
    Finds place to insert the node in the tree and inserts it.
    Based on addtree function in Karnigan and Ritchie , page 141.

    Return:
        Unchanged pointer to parent node that equals to its parameter
        (lable_item_ptr parent) if the node exists (when parent pointer in not NULL).
        or new pointer to new dynamically allocated parent node if the the parent node
        does not exist (when parent pointer equals to NULL).

    Parameters:
        parent : top node of binary search tree
        label_name : label_name string to be inserted into binary search tree.
    Example of call to function:
        parent = addtree (parent, "label_name");

    Important Note : it is IMPORTANT to set parent be NULL at first call to function,
                     undefined garbage value of parent pointer will cause to segmentation fault
                     (in better case...)
*/

lable_item_ptr add_labels_tree(lable_item_ptr parent, char *label_name);

/* Function label_node_alloc: allocates new label tree node
   and return pointer value to it.
   Sets features to default values, that must be updated in the
   future.

   Parameter:
      label_name : pointer to label name string to be stored in new label tree node.
   Return :
      pointer to new allocated label tree node.
   Based on talloc function in Kernigan and Ritchie, page 142.
*/
lable_item_ptr label_node_alloc(char *label_name);



/* Print label table tree node information
 */
void label_node_print(lable_item_ptr label_table);

/* Print label table tree node information
 */
void f_label_node_print(FILE *fd, lable_item_ptr label_table);



/* Recursive print of label table binary search tree
   The print is performed "in order" (Go Left, Print Root, Go Right) - alphabetic order of label names.
   The label table is printed in following order:
        - left tree
        - parent
        - right tree

   Parameter:
        label_table: pointer to the top node of binary search tree.
*/
void label_table_print(lable_item_ptr label_table);

/* Recursive function label_entry_search :
      Return:
               - pointer to node that match label_name value
               - NULL if the node does not exist in the tree
      Parameters:
               label_table : pointer to label table tree top node
               label_name  : pointer to label name string
*/
lable_item_ptr label_entry_search(lable_item_ptr label_table, char *label_name);


/* Recursive print of label table binary search tree to file
   The print is performed "in order" (Go Left, Print Root, Go Right) - alphabetic order of label names.
   The label table is printed in following order:
        - left tree
        - parent
        - right tree

   Parameter:
        label_table: pointer to the top node of binary search tree.
*/
void f_label_table_print( FILE * fd, lable_item_ptr label_table);


/* Recursive function label_entry_search :
      Return:
               - pointer to node that match label_name value
               - NULL if the node does not exist in the tree
      Parameters:
               label_table : pointer to label table tree top node
               label_name  : pointer to label name string
*/
lable_item_ptr label_entry_search(lable_item_ptr label_table, char * label_name);





/* Recursive function is_label_in_table : cheks is there is
   node in table that match given label name text.
      Return:  - 0 if label does not exist in label table tree
               - 1 if label exists in label table tree
      Parameters:
               label_table : pointer to label table tree top node
               label_name  : pointer to label name string
*/
int is_label_in_table(lable_item_ptr label_table, char *label_name);

/*  function : add_label_feature

    This function will add new label with new feature to binary search tree
    if the label does not exist in it.

    If the label already exists in the tree, it will perform bitwise OR between
    any two bits in old and new features to update the old one
    In this way what is already set - will remain set and if the same label is defined
    in different commands we will detect it.

    The function also updates label's address if is not defined yet (i.s. has value TO_BE_DEFINED,
    that is -5. But if the label's address is already defined and new label value is not TO_BE_DEIFNED
    the function will return error ( because of "try to change already defined address").
    If parameters value is TO_BE_DEFINED it will not change the adress value.

    Parameters :
        label_name                 - pointer to label's name string

        new_features               - new features to be OR- ed with previous.
        label_address              - will be set only if value of old address is -5.

    return :
        ERR_LABEL_ADDR_EXISTS (0) - fault
        LABEL_ADDR_UPDATED    (1) - valid

*/
int add_label_feature(lable_item_ptr *parent_node, char *label_name, label_features new_features, int label_address);

/* function check_feature_validity

    The function checks if feature is valid.
    If not, the appropriate error will be generated.

    following rules will be checked :
        1. the same label can not be defined twice in the same file,
            i.e. no of following parameters can get value 1 simultaneously :
                -   points_to_data
                -   points_to_cpu_cmd
                -   points_to_str
                -   points_to_mat
                -   is_extern
        2. if is_extern = 1 , also is_entry can not get value 1.
        3. if is_entry = 1 , also is_extern can not get value 1
        4. if label is defined as cpu_command_param it must be also defined in ONE of following
                -   points_to_data
                -   points_to_cpu_cmd
                -   points_to_str
                -   points_to_mat
                -   is_extern

        Note1: At the finish of first asseembly stage we will check that all labels that have
              is_cpu_cmd_param = 1 also have 1 in one of following :
                -   points_to_data
                -   points_to_cpu_cmd
                -   points_to_str
                -   points_to_mat
                -   is_extern

        return :
          NO_ERR   ( 0) - label is valid
          ERR      ( 1) - label is invalid
*/
int check_feature_validity(label_features feature, char *label_name);

/*
    recursive function label_table_features_check

        checks feature validity of all items in labels table
        The check is performed "in order" (in alpabetical order of labels)

        return : number of errors
*/

int label_table_features_check(lable_item_ptr label_table);

/*

   function  convert_feature_to_num

        converts features struct to unsigned int number;

            points_to_cpu_cmd : bit 0
            is_cpu_cmd_param  : bit 1
            points_to_data    : bit 2
            points_to_str     : bit 3
            points_to_mat     : bit 4
            is_extrn          : bit 5
            is_entry          : bit 6

*/
uint convert_feature_to_num(label_features feature);

/*
   function  convert_num_to_feature

        converts unsigned number to feature object ;

            points_to_cpu_cmd : bit 0
            is_cpu_cmd_param  : bit 1
            points_to_data    : bit 2
            points_to_str     : bit 3
            points_to_mat     : bit 4
            is_extrn          : bit 5
            is_entry          : bit 6

*/
void convert_num_to_feature(label_features *feature, int num);

/*
   function  reset_feature

        sets all feature bits to 0;

            points_to_cpu_cmd : bit 0
            is_cpu_cmd_param  : bit 1
            points_to_data    : bit 2
            points_to_str     : bit 3
            points_to_mat     : bit 4
            is_extrn          : bit 5
            is_entry          : bit 6

*/
void reset_feature(label_features *feature);



    /*-------------------------------------------------*/
    /*       cmd_line_analyse                          */
    /*-------------------------------------------------*/

    /* Read line from file opened for read.
       Store input line in *s up to line_lim characters number limit (not including '\n' or EOF)
       or up to '\n' of EOF (not including '\n' and EOF) if received less characters than line_lim.

       Skips all remainder characters from line_lim to '\n' or EOF.

       Parameters:
          fd_in    : pointer to input file
          s        : string array to store the line
          line_lim : line size limit (not including '\n' of EOF)
       Return:
              1) length of string (not including '\n' and '\0')
              2) -1 (NORM_LENGTH_AND_EOF)    string has normal length and finished with EOF
              3) -2 (TOO_MANY_CHARS_AND_ENTR) string has too many characters and finished with '\n'
              4) -3 (TOO_MANY_CHARS_AND_EOF) string has too many characters and finished with EOF
    */
    int readline(FILE * fd, char *s, int line_lim);

/* The function gen_text_find:

    Finds continuous text from start of text fragment within string
    and up to given delimiter char.

    The function gets pointer to fragment start inside string.
    It is pointer to pointer to allow the function to increment its value
    in order to return pointer to next delimiter char if it exists.

    If delimiter does not exist, or if its value is '\0', the function returns the unchanged
    pointer to fragment start (in case when the search reach EOS(End Of String)
    or reach second txt block before delimiter of EOS).


    The coninuous text can start immediately or to be separated
    from the fragment start position with only white characters
    (' ' and '\t' , including fragment start itself).

    Note : if delimiter is white char (' ' or '\t'), it will be skipped and
            the search for continuous text will be continued.

    Only single text fragment allowed to appear between
    start position and delimiter.

    The retuns values match to the following situations:

        1 : (GOT_TXT_AND_DLMTR) means we found continuous TeXT, and after it
                DeLiMiTeR i.e. continuous text placed between fragment's start and
                delimiter. The text may be separated from fragment start and
                from delimiter with white chars:

                sdssds    ,         |   sdssds        ,         |           sdssds,
            ^             ^         |   ^             ^         |   ^             ^
          f. start        delimiter | f. start        delimiter | f. start        delimiter

        2 : (GOT_WHITES_AND_DELMTR) means we found only WHITe charS
            between fragment start and DeLiMiTeR (text does not appear):

                              ,
                ^             ^
              f. start        delimiter

        3 : (GOT_TXT_AND_EOS) means no delimeter found or delimiter equals EOS and we got
                continuous TeXT between fragment's start and End Of String (EOS is '\0').
                The text may be separated from fragment start and
                from end of string with white chars:

                    sdssds    '\0'    |  sdssds        '\0'   |          sdssds'\0'
                ^             ^       |  ^             ^      |  ^             ^
              f. start        EOS     | f. start       EOS    | f. start       EOS

        4 : (GOT_WHITES_AND_EOS) means that we got empty string. No delimiter found or it equals
                EOS. Only WHiTe charS appear from
                fragment start and up to End Of String without
                continuous text.

                              '\0'
                ^             ^
             f. start        EOS

        5 : (GOT_EOS_AT_START) End Of String appears immediately AT fragment START:

                '\0'
                ^
              EOS at f. start

        6 : (GOT_DLMTR_AT_START) DeLiMiTeR is found immediately at fragment's START:
            Note : if delimiter is white char, it will be skipped and
            the search for continuous text will be continued.

                ,
                ^
              delimiter at fragment start

        7 : (GOT_SECOND_TXT_BLK) SECOND TeXT block separated with whith white char
            is found starting from fragment start and before delimeter or EOS:

                          '\0'      |                 '\0'      |                  '\0'
                sd sds    ,         |   sd   sds      ,         |   sd         sds,
            ^             ^         |   ^             ^         |   ^             ^
          f. start        delimiter | f. start        delimiter | f. start        delimiter
                          or EOS    |                 or EOS    |                 or EOS


*/
int gen_text_find(char **str, char dlmtr, char *txt);




/*
  Function is_label :

    Analyses if the given string is label before the command :
        1. checks if it ends with ':' (if chk_semicolon is set to 1)
        2. checks label's length < 30
        3. checks if the label starts form alphabetical char
        4. checks if the label has non alphanum chars or '_' between first char and ':'
           (if chk_semicolon is set to 1) or between first char and end of string
           (if chk_semicolon is not set to 1).
        5. if it is different from all keywords and all macro names.

    Parameters:
        str : pointer to string
        chk_colon :         if this parameter equals 1 will check that label ends with ':'
                            and continue check without ':' character at end.
                            Otherwise will not check for ':' and will check the whole word.

    Return (status):
            label_is_ok       : label is valid
            label_is_too_long : label name has more than 30 characters
            has_column_at_end  : ends with ":" character,
            no_column_at_end  : no ':' char at end of label (relevalnt if chk_colon is set)
            non_aphabet_start : first symbol is not alphabetical char
            has_non_alphanum  : has non alphanumerical character after first character
                                and before the last character.
            equals_cmd_name   : label name is same as command name
            equals_macro_name : label name is same as macro name
*/
void is_label(char *str, int chk_colon,is_cmd_lbl_stat_ptr status);




/*
  Function is_cmd_name :

    Analyses if the given string is command name:
        1. checks if it appears as command (cpu_instruction or data directive) in keywords table
        2. if it is command returns command's features

    Parameters:
        str : pointer to string

    Return :
        feature - bit-fields struct with command features.
        0 - (NOT_INSTR) not instruction
        1 - (CPU_INSTR) is cpu istruction
        2 - (DATA_DIRECTIVE) is data directive
*/
int is_cmd_name(char *str, keywords_item_ptr * keyword_item);




/* function is_reg_name
      Analyses string and checks that it is valid register name

      Return :
         -1     : if it is not register name
          0...7 : if it is r0 ... r7
*/
int is_reg_name(char *str);

/*
    Function is_param_number
        check the string if it is valid number after '#' symbol.
        This number after must be in range -128...127 to match 8 bit
        signed number field in CPU memory (it must fit 8 bits 2..9).

    Return :
        num , return number if its value is valid;
        NUM_PARAM_LIM_FAULT(-2)    : if the number is integer but not in limits
        NUM_PARAM_SYNTAX_FAULT(-1) : if there is syntax problen in number definition
*/
int is_param_number(char *str, int *num);

/* convert num string to decimal number
    The number must be integer.
    Can include + or - at the start.
    Return :
        num : integer number value (in case of valid number)
        result :
            NOT_NUM        = 0 in case of non valid number (syntax problem)
            VALID_NUM      = 1 in case of valid number
    Note: does not check the limits of number !
*/
int str2int(char *str, int *num);



/* convert num string to decimal number
    The number must be integer.
    Can include + or - at the start.
    Return :
        num : integer number value (in case of valid number)
        result :
            NOT_NUM        = 0 in case of non valid number (syntax problem)
            VALID_NUM      = 1 in case of valid number
    Note: does not check the limits of number !
*/
int str2int(char *str, int * num);




/* Function is_matrix_param

        checks  if it is matrix call parameter in CPU command
        searches the text from current pointer's position and up to delimiter ','

        Matrix call parameter example:
                 label[ r1  ][r2      ]   ,
             ^                            ^
         pointer pos.                  delimiter

           1. The label name must be adjucent to '[' character.
           2. The label name maust be valid label name.
           3. White chars allowed to appear between text start (current text pointer position)
              and matrix definition start and between matrix definition finish and delimiter.
           4. White chars allowed between '[' and register name and
              between register name and ']'.
           5. register name must be valid i.e. r0 ... r7 only alowed.
           6. ']' and '['  must be adjucent.

        In case of first CPU command parameter the delimiter will be ','
        and the text pointer will be set to it at the finish of function run.
        In case of second CPU command param

        The function will update the pointer to point the delimiter after the matrix definition.
        In case

        Return :
                maybe_matrix_call               : the text contains at least '[' or ']' character;
                not_matrix_call                 : the text is not matrix call (no '[' or ']' chars)
                matrix_call_valid               : valid label found, valid register names and
                                                    valid syntax with ',' or '\0' at the end.
                reg1_name_err                   : reg1 name is not r0...r7
                reg2_name_err                   : reg2 name is not r0...r7
                label_not_exists                : label does not exist
                label_is_too_long               : label is too long 
                label_non_aphabet_start         : label does not start with alphabet char;
                label_has_non_alphanum          : label has not alphanum char after 1st char;
                label_equals_macro_name         : label equals matrix name;
                label_equals_cmd_name           : label equals command name;
                invalid_label_sntx_err          : label hassyntax error;
                err_white_btwn_lbl_and_prnth    : label has white char between it and '[';
                paranth_problem                 : must be no whites between ']' and '[';
                matrix_def_syntax_err           : matrix definition has syntax error;
                delimeter_after_m_call          : found delimetter after matrix call;
                EOS_after_matrix_call           : found EOS after matrix call;
                txt_between_mcall_and_dlmter    : extra text between matrix call and delimenter;
                err_extra_text                  : extra text in matrix call

*/

void is_matrix_param(char **str, char ** label_name, int * reg1, int * reg2, mat_param_status * status);

/*
   Reset_matrix_status
        parameter
            status: matrix status object

*/

void reset_matrix_status(mat_param_status *status);




/*-------------------------------------------------*/
/*       release dynamic memory and files          */
/*-------------------------------------------------*/

/*
       function all_dynamic_memory_free
           releases all dynamic allocated memory
*/
void release_all_dynamic_memory_and_files();

    /*
           function partial_dynamic_memory_free
               releases dynamic allocated memory
               not including (used for next file processing):
                  keywords table binary search tree
                  errors linked list

    */
void partial_dynamic_memory_free(data_blk_item_ptr d_head, data_blk_item_ptr d_tail,
                              i_item_ptr i_head, i_item_ptr i_tail,
                              lable_item_ptr labels_table_ptr, 
                              macro_table_entry_ptr macr_table_ptr,
                              char * full_fname_as,
                              char * full_fname_am,
                              char * full_fname_ext,
                              char * full_fname_ent,
                              char * full_fname_ob ,
                              char * full_fname_log,
                              char * full_name_tmp,
                              char * gen_str_pointer );

/*
       function remainder_dynamic_memory_free
              releases dynamic memory remainder
              including (used for next file processing):
                     keywords table binary search tree
                     errors linked list



*/
void remainder_dynamic_memory_free();

/*
       function full_file_names_release
           releases dynamic allocated memory
           of full file names only.

*/
void full_file_names_release();

    /*
     function global_full_file_names_gen
                  allocates storage for file names in heap and assignes pointer to them
                  to global pointers
    */
    void global_full_file_names_gen(char *file_name);

/* Function add_extn_to_fname:
   creates full filename by
   adition of extention to file name */
char *add_extn_to_fname(char *fname, char *const ext);

/* function free_data_blk_list:
          release memory of data block list

          parameters:
              head - pointer to pointer to head of linked list
              tail - pointer to pointer to tail of linked lis

*/
void free_data_blk_list(data_blk_item_ptr *head, data_blk_item_ptr *tail);

/* function free_num_lnk_lst:
      release memory of num link list associated with data linked list item.

    parameters:
              head - pointer to pointer to head of linked list
              tail - pointer to pointer to tail of linked lis
*/
void free_num_lnk_lst(num_lnk_lst_ptr *head, num_lnk_lst_ptr *tail);

/* function free_i_items_lst:
      release memory of data block list
       parameters:
              head - pointer to pointer to head of linked list
              tail - pointer to pointer to tail of linked list
*/
void free_i_items_lst(i_item_ptr *head, i_item_ptr *tail);

/* To free a memory of label binary search tree,
   we will use "post-order" (Go Left , Go Right, Free Root) traversal.
   This ensures that we free each root node AFTER its children have been freed,
   avoiding access to already freed memory.

   parameter :
       root : pointer to top item of BST
*/
void free_label_binary_search_tree(lable_item_ptr root);

/* To free a memory of keywords binary search tree,
   we will use "post-order" (Go Left , Go Right, Free Root) traversal.
   This ensures that we free each node after its children have been freed,
   avoiding access to already freed memory.


   parameter :
       root : pointer to top item of BST
*/
void free_keys_binary_search_tree(keywords_item_ptr root);

/************************ free macros table binary search tree  ********************/

/* To free a memory of macro binary search tree,
   we will use "post-order" (Go Left , Go Right, Free Root) traversal.
   This ensures that we free each root node AFTER its children have been freed,
   avoiding access to already freed memory.
   
   parameter :
       root : pointer to top item of BST
*/
void free_macro_binary_search_tree(macro_table_entry_ptr root);



/*  function free_errors_list

        releases dynamic linked list of errors

        parameter : 
            list_h : pointer to head of linked list
*/
void free_errors_list(error_def_item_ptr list_h);

/*

 function free_line_list_macro
       
       frees line linked list associated with given macro binary search item.
       parameter:
              root : pointer to top of macro table binary search tree

*/

void free_line_list (macro_table_entry_ptr root);



