#include "assembler.h"
#include "mngr_errors_globals.h"

/* Function errors_list_create : 
    inserts errors definititions to dynamic linked list.
    Receives pointer to pointer to dynamic linked list
    and updates contents of pointer to dynamic linked list.
*/
void errors_list_create(error_def_item_ptr *list_h)
{

    /* list_h = NULL; */


    add_err_2_lst(list_h, 1   , PRE_ASM_ERR, "code line has more than 80 characters");
    add_err_2_lst(list_h, 2   , PRE_ASM_ERR, "\"mcro\" command has more than one parameter");
    add_err_2_lst(list_h, 3   , PRE_ASM_ERR, "\"mcro\" command has no parameters");
    add_err_2_lst(list_h, 4   , PRE_ASM_ERR, "\"mcro\" command parameter has illegal symbols");
    add_err_2_lst(list_h, 5   , PRE_ASM_ERR, "\"mcroend\" command has redundant characters after it");
    add_err_2_lst(list_h, 6   , PRE_ASM_ERR, "only macro's name must appear\n\tat macro substitution start");
    add_err_2_lst(list_h, 7   , PRE_ASM_ERR, "name of macro starts with illegal character");
    add_err_2_lst(list_h, 8   , PRE_ASM_ERR, "macro's name has more than 30 characters");
    add_err_2_lst(list_h, 9   , PRE_ASM_ERR, "macro's name is identical to keyword name");
    add_err_2_lst(list_h, 10  , PRE_ASM_ERR, "macro's substitution label\n\thas redundunt "
                                             "characters after it");

    add_err_2_lst(list_h, 50  , FILE_MNG_ERR, "could not remove file:");

    add_err_2_lst(list_h, 80   , DYN_ALC_ERR, "dynamic memory allocation fault in function add_extn_to_fname");
    add_err_2_lst(list_h, 81   , DYN_ALC_ERR, "dynamic memory allocation error.");
    add_err_2_lst(list_h, 82   , DYN_ALC_ERR, "dynamic memory allocation error in function add_num_to_lnk_lst");
    add_err_2_lst(list_h, 83   , DYN_ALC_ERR, "dynamic memory allocation error in function add_new_data_blk_item");
    add_err_2_lst(list_h, 84   , DYN_ALC_ERR, "dynamic memory allocation error in function add_cpu_instr_item");

    add_err_2_lst(list_h, 150  , ASM_1_ERR  , "label's length is more than 30 characters");
    add_err_2_lst(list_h, 151  , ASM_1_ERR  , "only label in command line");
    add_err_2_lst(list_h, 152  , ASM_1_ERR  , "label's name starts with illegal character ");
    add_err_2_lst(list_h, 153  , ASM_1_ERR  , "label's name has illegal character ");
    add_err_2_lst(list_h, 154  , ASM_1_ERR  , "label's name is same as macro name ");
    add_err_2_lst(list_h, 155  , ASM_1_ERR  , "label's name is same as command's (or keyword's) name ");
    add_err_2_lst(list_h, 156  , ASM_1_ERR  , "label's name differs from macro/keyword, \n\tbut does not ends with ':' ");
    add_err_2_lst(list_h, 157  , ASM_1_ERR  , "label alone, the label ends with ':', \n\tbut no command found ");

    add_err_2_lst(list_h, 159  , ASM_1_ERR  , "command name is not valid ");
    add_err_2_lst(list_h, 160  , ASM_1_ERR  , "the command requires at least one operand ");
    add_err_2_lst(list_h, 161  , ASM_1_ERR  , "the command got too much parameters (extra text in params) ");
    add_err_2_lst(list_h, 162  , ASM_1_ERR  , "1st parameter: signed number after # symbol \n\tmust be in range -128...127");
    add_err_2_lst(list_h, 163  , ASM_1_ERR  , "1st parameter: must int number after # symbol ");
    add_err_2_lst(list_h, 164  , ASM_1_ERR  , "1st parameter: label of command equals command name ");
    add_err_2_lst(list_h, 165  , ASM_1_ERR  , "1st parameter: label of command equals macro name ");
    add_err_2_lst(list_h, 166  , ASM_1_ERR  , "1st parameter: label of command starts with non alfabet char ");
    add_err_2_lst(list_h, 167  , ASM_1_ERR  , "1st parameter: label of command has non alphanum char ");
    add_err_2_lst(list_h, 168  , ASM_1_ERR  , "1st parameter: label of command is too long ");
    add_err_2_lst(list_h, 169  , ASM_1_ERR  , "did not find ',' character after the 1st parameter ");
    add_err_2_lst(list_h, 170  , ASM_1_ERR  , "the second operand in command did not found ");
    add_err_2_lst(list_h, 171  , ASM_1_ERR  , "syntax problem in 2nd command parameter ");
    add_err_2_lst(list_h, 172  , ASM_1_ERR  , "2nd command parameter does not exist");
    add_err_2_lst(list_h, 173  , ASM_1_ERR  , "2nd parameter: signed number after # symbol \n\tmust be in range -128...127");
    add_err_2_lst(list_h, 174  , ASM_1_ERR  , "2nd parameter: must int number after # symbol  ");
    add_err_2_lst(list_h, 175  , ASM_1_ERR  , "2nd parameter: label of command equals command name ");
    add_err_2_lst(list_h, 176  , ASM_1_ERR  , "2nd parameter: label of command equals macro name ");
    add_err_2_lst(list_h, 177  , ASM_1_ERR  , "2nd parameter: label of command starts with non alfabet char ");
    add_err_2_lst(list_h, 178  , ASM_1_ERR  , "2nd parameter: label of command has non alphanum char ");
    add_err_2_lst(list_h, 179  , ASM_1_ERR  , "2nd parameter: label of command is too long ");
    add_err_2_lst(list_h, 180  , ASM_1_ERR  , "the given command must have no parameters");



    add_err_2_lst(list_h, 200  , ASM_1_ERR  , "expected only one parameter, but matrix call \n\tparam has ',' after 1st param. ");
    add_err_2_lst(list_h, 201  , ASM_1_ERR  , "1st parameter: white char between label and '[' in matrix call ");
    add_err_2_lst(list_h, 202  , ASM_1_ERR  , "1st parameter: extra text within matrix param definition ");
    add_err_2_lst(list_h, 203  , ASM_1_ERR  , "1st parameter: label in matrix call equals to command name ");
    add_err_2_lst(list_h, 204  , ASM_1_ERR  , "1st parameter: label in matrix call equals to macro name ");
    add_err_2_lst(list_h, 205  , ASM_1_ERR  , "1st parameter: label in matrix call has non alphabet first char ");
    add_err_2_lst(list_h, 206  , ASM_1_ERR  , "1st parameter: label in matrix call \n\thas non alphanum char after 1st char ");
    add_err_2_lst(list_h, 207  , ASM_1_ERR  , "1st parameter: label in matrix call is too long ");
    add_err_2_lst(list_h, 208  , ASM_1_ERR  , "1st parameter: no label in matrix call ");
    add_err_2_lst(list_h, 209  , ASM_1_ERR  , "1st parameter: paranthesis problem in matrix call \n\twhite char appear between ']' and '[' \n\tor no '[' or no ']' ");
    add_err_2_lst(list_h, 210  , ASM_1_ERR  , "1st parameter: extra text between matrix call and delimiter ");
    add_err_2_lst(list_h, 211  , ASM_1_ERR  , "1st parameter: reg1 text error in matrix call ");
    add_err_2_lst(list_h, 212  , ASM_1_ERR  , "1st parameter: reg2 text error in matrix call ");
    add_err_2_lst(list_h, 213  , ASM_1_ERR  , "1st parameter: matrix definition syntax error in matrix call ");

    add_err_2_lst(list_h, 221  , ASM_1_ERR  , "2nd parameter: white char between label and '[' in matrix call ");
    add_err_2_lst(list_h, 222  , ASM_1_ERR  , "2nd parameter: extra text within matrix param definition ");
    add_err_2_lst(list_h, 223  , ASM_1_ERR  , "2nd parameter: label in matrix call equals to command name ");
    add_err_2_lst(list_h, 224  , ASM_1_ERR  , "2nd parameter: label in matrix call equals to macro name ");
    add_err_2_lst(list_h, 225  , ASM_1_ERR  , "2nd parameter: label in matrix call has non alphabet first char ");
    add_err_2_lst(list_h, 226  , ASM_1_ERR  , "2nd parameter: label in matrix call \n\thas non alphanum char after 1st char ");
    add_err_2_lst(list_h, 227  , ASM_1_ERR  , "2nd parameter: label in matrix call is too long ");
    add_err_2_lst(list_h, 228  , ASM_1_ERR  , "2nd parameter: no label in matrix call ");
    add_err_2_lst(list_h, 229  , ASM_1_ERR  , "2nd parameter: paranthesis problem in matrix call \n\twhite char appear between ']' and '[' \n\tor no '[' or no ']' ");
    add_err_2_lst(list_h, 230  , ASM_1_ERR  , "2nd parameter: extra text between matrix call and delimiter ");
    add_err_2_lst(list_h, 231  , ASM_1_ERR  , "2nd parameter: reg1 text error in matrix call ");
    add_err_2_lst(list_h, 232  , ASM_1_ERR  , "2nd parameter: reg2 text error in matrix call ");
    add_err_2_lst(list_h, 233  , ASM_1_ERR  , "2nd parameter: matrix definition syntax error in matrix call ");
    add_err_2_lst(list_h, 234  , ASM_1_ERR  , "2nd parameter: found ',' character after matrix call ");

    add_err_2_lst(list_h, 300  , ASM_1_ERR  , "1st parameter: label param (direct addressing method) \n\tis forbidden for given command type ");
    add_err_2_lst(list_h, 301  , ASM_1_ERR  , "1st parameter: matrix param.(matrix access addressing method) \n\tis forbidden for given command type ");
    add_err_2_lst(list_h, 302  , ASM_1_ERR  , "1st parameter: number param.(immediate addressing method) \n\tis forbidden for given command type ");
    add_err_2_lst(list_h, 303  , ASM_1_ERR  , "1st parameter: register param(direct register addressing method) \n\tis forbidden for given command type ");
    
    add_err_2_lst(list_h, 304  , ASM_1_ERR  , "2nd parameter: label param (direct addressing method) \n\tis forbidden for given command type ");
    add_err_2_lst(list_h, 305  , ASM_1_ERR  , "2nd parameter: matrix param.(matrix access addressing method) \n\tis forbidden for given command type ");
    add_err_2_lst(list_h, 306  , ASM_1_ERR  , "2nd parameter: number param.(immediate addressing method) \n\tis forbidden for given command type ");
    add_err_2_lst(list_h, 307  , ASM_1_ERR  , "2nd parameter: register param(direct register addressing method) \n\tis forbidden for given command type ");

    add_err_2_lst(list_h, 400  , ASM_1_ERR  , ".data directive: did not found white char after .data command ");
    add_err_2_lst(list_h, 401  , ASM_1_ERR  , ".data directive: directive has no parameters ");
    add_err_2_lst(list_h, 402  , ASM_1_ERR  , ".data directive: two data prameters not separated with ',' delimiter ");
    add_err_2_lst(list_h, 403  , ASM_1_ERR  , ".data directive: found ',' whithout data before it ");
    add_err_2_lst(list_h, 404  , ASM_1_ERR  , ".data directive: syntax error, expect integer data");
    add_err_2_lst(list_h, 405  , ASM_1_ERR  , ".data directive: parameter value must be within 10 bit limits -512 ... 511 ");
    add_err_2_lst(list_h, 406  , ASM_1_ERR  , ".data directive: extra ',' character at end of data list ");

    add_err_2_lst(list_h, 450  , ASM_1_ERR  , ".string directive: did not found white char after .string command ");
    add_err_2_lst(list_h, 451  , ASM_1_ERR  , ".string directive: directive has no parameters ");
    add_err_2_lst(list_h, 452  , ASM_1_ERR  , ".string directive: found redundant character ");
    add_err_2_lst(list_h, 453  , ASM_1_ERR  , ".string directive: \" must appear at start of string ");
    add_err_2_lst(list_h, 454  , ASM_1_ERR  , ".string directive: \" must appear at end of string  ");
    add_err_2_lst(list_h, 455  , ASM_1_ERR  , ".string directive: illegal character with ASCII code \n\tnot in valid range 32...126  ");

    add_err_2_lst(list_h, 470  , ASM_1_ERR  , ".mat directive: did not find white char after .mat command  ");
    add_err_2_lst(list_h, 471  , ASM_1_ERR  , ".mat directive: did not find '[' character after .mat command  ");
    add_err_2_lst(list_h, 472  , ASM_1_ERR  , ".mat directive: find more than one text after '[' and before ']' ");
    add_err_2_lst(list_h, 473  , ASM_1_ERR  , ".mat directive: did not fine text before ']'  ");
    add_err_2_lst(list_h, 474  , ASM_1_ERR  , ".mat directive: got text but not found ']' after '[' ");
    add_err_2_lst(list_h, 475  , ASM_1_ERR  , ".mat directive: did not find ']' character ");
    add_err_2_lst(list_h, 476  , ASM_1_ERR  , ".mat directive: the number of rows is not valid integer number ");
    add_err_2_lst(list_h, 477  , ASM_1_ERR  , ".mat directive: internal ']' does not close to internal '[' ");
    add_err_2_lst(list_h, 478  , ASM_1_ERR  , ".mat directive: did not find column number definition after '[' ");
    add_err_2_lst(list_h, 479  , ASM_1_ERR  , ".mat directive: did not find ']' after column number definition ");
    add_err_2_lst(list_h, 480  , ASM_1_ERR  , ".mat directive: did not find second ']' ");
    add_err_2_lst(list_h, 481  , ASM_1_ERR  , ".mat directive: the number of columns is not valid integer number ");
    add_err_2_lst(list_h, 482  , ASM_1_ERR  , ".mat directive: got extra text before ',' delimiter ");
    add_err_2_lst(list_h, 483  , ASM_1_ERR  , ".mat directive: eror in parameter syntax, it is not int number ");
    add_err_2_lst(list_h, 484  , ASM_1_ERR  , ".mat directive: got extra delimiter ',' after ',' delimiter ");
    add_err_2_lst(list_h, 485  , ASM_1_ERR  , ".mat directive: found some extra text (may be too much parameters) ");

    add_err_2_lst(list_h, 500  , ASM_1_ERR  , "label error: other command has the same label\n\t(with different address value) ");
    add_err_2_lst(list_h, 501  , ASM_1_ERR  , ".extern directive: expect at least one white char after the directive name ");
    add_err_2_lst(list_h, 502  , ASM_1_ERR  , ".extern directive: did not find label definition after the directive name");
    add_err_2_lst(list_h, 503  , ASM_1_ERR  , ".extern directive: extra text after the directive name ");
    add_err_2_lst(list_h, 504  , ASM_1_ERR  , ".extern directive: label name equals command or register name ");
    add_err_2_lst(list_h, 505  , ASM_1_ERR  , ".extern directive: label name equals macro name ");
    add_err_2_lst(list_h, 506  , ASM_1_ERR  , ".extern directive: label is too long ");
    add_err_2_lst(list_h, 507  , ASM_1_ERR  , ".extern directive: label has non alphanum char. after the first char ");
    add_err_2_lst(list_h, 508  , ASM_1_ERR  , ".extern directive: first char of label is non alphabet character ");

    add_err_2_lst(list_h, 511  , ASM_1_ERR  , ".entry directive: expect at least one white char after the directive name ");
    add_err_2_lst(list_h, 512  , ASM_1_ERR  , ".entry directive: did not find label definition after the directive name");
    add_err_2_lst(list_h, 513  , ASM_1_ERR  , ".entry directive: extra text after the directive name ");
    add_err_2_lst(list_h, 514  , ASM_1_ERR  , ".entry directive: label name equals command or register name ");
    add_err_2_lst(list_h, 515  , ASM_1_ERR  , ".entry directive: label name equals macro name ");
    add_err_2_lst(list_h, 516  , ASM_1_ERR  , ".entry directive: label is too long ");
    add_err_2_lst(list_h, 517  , ASM_1_ERR  , ".entry directive: label has non alphanum char. after the first char ");
    add_err_2_lst(list_h, 518  , ASM_1_ERR  , ".entry directive: first char of label is non alphabet character ");



}

/* insert new item to linked list of errors definitions
   according to its error_number value,
   and return pointer to first item in the list

   Note: The function UPDATES value of pointer to list head.
         So we MUST pass the function pointer to <pointer to list head>
         to be able use the returned value in calling function.
*/
void add_err_2_lst(error_def_item_ptr *list_head,
                   int error_number, char *error_type, char *error_desription)
{

    error_def_item_ptr new;
    error_def_item_ptr p1 , p2;
    char * heap_err_descr_name;
    char * heap_err_type_name;
    /* create linked list item */
    new = (error_def_item_ptr)malloc(sizeof(error_def_item));
    if (!new) {
        fprintf(ERR_STREAM,"Error: can not allocate memory"
                           "for error definition items list\n");
        release_all_dynamic_memory_and_files();
        exit(1);
    }
    /* init new error definition item 
       copy error desription name and error type name to heap */
    heap_err_descr_name = heap_string_duplicate_safe(error_desription,MAX_ERR_DESCR_LEN);
    heap_err_type_name = heap_string_duplicate_safe(error_type,MAX_ERR_TYPE_LEN);
    new->error_desription = heap_err_descr_name;
    new->error_number = error_number;
    new->error_type = heap_err_type_name;
    new->next = NULL;
    new->prev = NULL;

    p1 = *list_head;
    /* increment p1 to point to list member 
       that we want to add new item before it.

       p2 will point to previous member.
    */
    while((p1)&&(p1->error_number < error_number)){
        p2 = p1;          /* prev item */
        p1 = p1->next;    /* next item */
    }

    /* isert to empty list or
       update list head value if 
       new item must be inserted after it.
     */
    if (p1 == *list_head)
    {
        *list_head = new;  /* update list head to point new item*/
        new->next = p1;    /* update new item to point the same item 
                              pointed by list head before update */
        new->prev = NULL;                      
    } else {
        p2->next = new; /* update prev item to point the new item*/
        new->next = p1; /* update new item to point next item    */
        new->prev = p2;
    }
}


/* print errors linked list item according to its number 
  here list_h is pointer to first item in the list.
*/
void print_err(error_def_item_ptr h, int item_num, int line_num, char* line_str, char* user_dscrpt){

    while((h)&&(h->error_number!=item_num))
        h = h->next;

    if (!h) /* h = NULL*/
        return;            /* given error number does not appear in errors list */
    else   /*h->error_number==item_num*/
        if (line_num>0){
            fprintf(ERR_STREAM, "Error in line %d:\n\t%s: %s\n", line_num, h->error_desription, user_dscrpt);
            fprintf(ERR_STREAM,"\t|%s|\n\n", line_str);
        } else {
            fprintf(ERR_STREAM, "Error : %s: %s\n", h->error_desription, user_dscrpt);
            fprintf(ERR_STREAM,"\t%s\n\n", line_str);
        }
}

