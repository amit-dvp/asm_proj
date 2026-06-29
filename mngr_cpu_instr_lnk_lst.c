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

   Returns pointer to new item. 

*/


#include "assembler.h"

extern error_def_item_ptr global_err_list_head;

/* function add_cpu_instr_item :
       creates the new item
       adds the new updated item to the end of linked
       list.xz
    updates tail and head, if needed.
    returns pointer to the new item

    Note : the new item is inttialized to default values only.
*/
i_item_ptr add_cpu_instr_item(i_item_ptr *head, i_item_ptr *tail)
{

    i_item_ptr prev;
    i_item_ptr new_item;

    /* allocate memory for new item */
    new_item = (i_item_ptr)malloc(sizeof(i_item));
    if (!new_item)
    {
        print_err(global_err_list_head, 84, -1, "", "");
        release_all_dynamic_memory_and_files();
        exit(1);
    }

    reset_i_item (&new_item);



    if (!(*head)) /* set first element is list */
        *tail = *head = new_item;
    else
    { /* insert new data to end of list and update tail pointer */
        /* connect previous item to new data */
        prev = *tail;
        prev->next = new_item;
        /* update tail to point new data */
        *tail = new_item;
    }

    return new_item;
}



/*

  function reset_inst_item  
        sets default values to all instruction item data parameters
    parameter :
        item : pointer to pointer to instruction item

 */

 void reset_i_item (i_item_ptr *item){

        /* initialize item with default values */
        (*item)->start_addr = 0;
        (*item)->cmd_name = NULL;
        (*item)->size_in_wrds = 0;
        (*item)->is_valid = 0;
        (*item)->next = NULL;

        /* init status */
        (*item)->status.cmd_label_found = 0;
        (*item)->status.params_num = 0;
        (*item)->status.no_params_found = 0;
        (*item)->status.param1_found = 0;
        (*item)->status.param2_found = 0;
        (*item)->status.dest_addrsng = 0;
        (*item)->status.src_addrsng = 0;
        (*item)->status.opcode = 0;

        /* param1 init */
        (*item)->param1.param_type.no_param = 1;
        (*item)->param1.param_type.param_is_number = 0;
        (*item)->param1.param_type.param_is_reg_name = 0;
        (*item)->param1.param_type.param_is_matrx = 0;
        (*item)->param1.param_type.param_is_label = 0;
        (*item)->param1.params_union.no_param = NULL;
        (*item)->param1.params_union.num.num = 0;
        (*item)->param1.params_union.num.validity.num_is_valid = 0;
        (*item)->param1.params_union.lbl.label_addr = 0;
        (*item)->param1.params_union.lbl.label_name = NULL;
        (*item)->param1.params_union.lbl.validity.label_addr_defined = 0;
        (*item)->param1.params_union.lbl.validity.label_length_valid = 0;
        (*item)->param1.params_union.lbl.validity.label_param_syntax_valid = 0;
        (*item)->param1.params_union.mat.mat_label = NULL;
        (*item)->param1.params_union.mat.reg1_num = 0;
        (*item)->param1.params_union.mat.reg2_num = 0;
        (*item)->param1.params_union.mat.validity.brackets_valid = 0;
        (*item)->param1.params_union.mat.validity.mat_label_defined = 0;
        (*item)->param1.params_union.mat.validity.mat_label_syntax_valid = 0;
        (*item)->param1.params_union.mat.validity.reg1_name_valid = 0;
        (*item)->param1.params_union.mat.validity.reg2_name_valid = 0;
        (*item)->param1.params_union.reg.reg_num = 0;
        (*item)->param1.params_union.reg.validity.reg_num_is_valid = 0;
        (*item)->param1.params_union.reg.validity.reg_syntax_is_valid = 0;

        /* param2 init */
        (*item)->param2.param_type.no_param = 1;
        (*item)->param2.param_type.param_is_number = 0;
        (*item)->param2.param_type.param_is_reg_name = 0;
        (*item)->param2.param_type.param_is_matrx = 0;
        (*item)->param2.param_type.param_is_label = 0;
        (*item)->param2.params_union.no_param = NULL;
        (*item)->param2.params_union.num.num = 0;
        (*item)->param2.params_union.num.validity.num_is_valid = 0;
        (*item)->param2.params_union.lbl.label_addr = 0;
        (*item)->param2.params_union.lbl.label_name = NULL;
        (*item)->param2.params_union.lbl.validity.label_addr_defined = 0;
        (*item)->param2.params_union.lbl.validity.label_length_valid = 0;
        (*item)->param2.params_union.lbl.validity.label_param_syntax_valid = 0;
        (*item)->param2.params_union.mat.mat_label = NULL;
        (*item)->param2.params_union.mat.reg1_num = 0;
        (*item)->param2.params_union.mat.reg2_num = 0;
        (*item)->param2.params_union.mat.validity.brackets_valid = 0;
        (*item)->param2.params_union.mat.validity.mat_label_defined = 0;
        (*item)->param2.params_union.mat.validity.mat_label_syntax_valid = 0;
        (*item)->param2.params_union.mat.validity.reg1_name_valid = 0;
        (*item)->param2.params_union.mat.validity.reg2_name_valid = 0;
        (*item)->param2.params_union.reg.reg_num = 0;
        (*item)->param2.params_union.reg.validity.reg_num_is_valid = 0;
        (*item)->param2.params_union.reg.validity.reg_syntax_is_valid = 0;

        /* code data */
        (*item)->code.i_wrd1.codeval = 0;
        (*item)->code.i_wrd1.E_R_A = 0;
        (*item)->code.i_wrd1.exist = 0;
        (*item)->code.i_wrd1.ic_addr = 0;
        (*item)->code.i_wrd2.codeval = 0;
        (*item)->code.i_wrd2.E_R_A = 0;
        (*item)->code.i_wrd2.exist = 0;
        (*item)->code.i_wrd2.ic_addr = 0;
        (*item)->code.i_wrd3.codeval = 0;
        (*item)->code.i_wrd3.E_R_A = 0;
        (*item)->code.i_wrd3.exist = 0;
        (*item)->code.i_wrd3.ic_addr = 0;
        (*item)->code.i_wrd4.codeval = 0;
        (*item)->code.i_wrd4.E_R_A = 0;
        (*item)->code.i_wrd4.exist = 0;
        (*item)->code.i_wrd4.ic_addr = 0;
        (*item)->code.i_wrd5.codeval = 0;
        (*item)->code.i_wrd5.E_R_A = 0;
        (*item)->code.i_wrd5.exist = 0;
        (*item)->code.i_wrd5.ic_addr = 0;


     

 }









/* function print_i_items_list 

    prints all instruction items list info

*/
void print_i_items_list(i_item_ptr head, i_item_ptr tail)
{
    int start_flag = 1;
    i_item_ptr node = head; /* pointer to list node */

    /* increment head and free previous head */
    do{
        if (start_flag == 0)
            node = node->next;
        start_flag = 0;    
        print_cpu_i_item(node);
    }while(node != tail);
    
}

/* function print_i_items_list

    prints all instruction items list info to file

*/
void f_print_i_items_list(FILE * fd , i_item_ptr head, i_item_ptr tail)
{
    int start_flag = 1;
    i_item_ptr node = head; /* pointer to list node */

    /* increment head and free previous head */
    do
    {
        if (start_flag == 0)
            node = node->next;
        start_flag = 0;
        f_print_cpu_i_item(fd , node);
    } while (node != tail);
}




/* function print_cpu_i_item :
    Prints data of CPU instruction item in linked list
    Parameter : pointer to CPU instruction item in linked list
*/
void print_cpu_i_item(i_item_ptr i_item)
{
    printf("--------------------------\n");
    printf ("CPU instruction item:\n");
    printf("\tstart_addr: %d\n", i_item->start_addr);
    printf("\tcmd_name: %s\n", i_item->cmd_name);
    printf("\tlabel_name: %s\n", i_item->label_name);
    printf("\tsize_in_words: %d\n", i_item->size_in_wrds);
    printf("\tis_valid: %d\n", i_item->is_valid);
    /* status */
    printf("\tStatus:\n");
    printf("\t\tcmd_label_found: %u\n", i_item->status.cmd_label_found);
    printf("\t\tparams_num: %u\n", i_item->status.params_num );
    printf("\t\tno_params_found: %u\n", i_item->status.no_params_found);
    printf("\t\tparam1_found: %u\n", i_item->status.param1_found);
    printf("\t\tparam2_found: %u\n", i_item->status.param2_found);
    printf("\t\tdest_addresing: %u\n", i_item->status.dest_addrsng);
    printf("\t\tsrc_addresing: %u\n", i_item->status.src_addrsng);
    printf("\t\topcode: %u\n", i_item->status.opcode);

    /* param1 */
    printf("\tParam1:\n");

    if (i_item->param1.param_type.no_param) /* if param 1 not exists */
        printf("\t\tparam1 is not defined \n");
    else    
    {    

        if (i_item->param1.param_type.param_is_number)
        {
            printf("\t\tparam_is_number: %u\n", i_item->param1.param_type.param_is_number);
            printf("\t\tnum: %d\n", i_item->param1.params_union.num.num);
            printf("\t\tnum_is_valid: %u\n", i_item->param1.params_union.num.validity.num_is_valid);
        }
        if (i_item->param1.param_type.param_is_label)
        {
            printf("\t\tparam_is_label: %u\n", i_item->param1.param_type.param_is_label);
            printf("\t\tlabel_name: %s\n", i_item->param1.params_union.lbl.label_name);
            printf("\t\tlabel_addr: %d\n", i_item->param1.params_union.lbl.label_addr);
            printf("\t\tlabel_addr_defined: %u\n", i_item->param1.params_union.lbl.validity.label_addr_defined);
            printf("\t\tlabel_length_valid: %u\n", i_item->param1.params_union.lbl.validity.label_length_valid);
            printf("\t\tlabel_param_syntax_valid: %u\n", i_item->param1.params_union.lbl.validity.label_param_syntax_valid);
        }
        if (i_item->param1.param_type.param_is_matrx)
        {
            printf("\t\tparam_is_matrx: %u\n", i_item->param1.param_type.param_is_matrx);
            printf("\t\tmat_label: %s\n", i_item->param1.params_union.mat.mat_label);
            printf("\t\treg1_num: %d\n", i_item->param1.params_union.mat.reg1_num);
            printf("\t\treg2_num: %d\n", i_item->param1.params_union.mat.reg2_num);
            printf("\t\tmat_label_defined: %d\n", i_item->param1.params_union.mat.validity.mat_label_defined);
            printf("\t\tmat_label_syntax_valid: %d\n", i_item->param1.params_union.mat.validity.mat_label_syntax_valid);
            printf("\t\tbrackets_valid: %d\n", i_item->param1.params_union.mat.validity.brackets_valid);
            printf("\t\treg1_name_valid: %d\n", i_item->param1.params_union.mat.validity.reg1_name_valid);
            printf("\t\treg2_name_valid: %d\n", i_item->param1.params_union.mat.validity.reg2_name_valid);
        }
        if (i_item->param1.param_type.param_is_reg_name)
        {
            printf("\t\tparam_is_reg_name: %u\n", i_item->param1.param_type.param_is_reg_name);
            printf("\t\treg_num: %u\n", i_item->param1.params_union.reg.reg_num);
            printf("\t\treg_num_is_valid: %u\n", i_item->param1.params_union.reg.validity.reg_num_is_valid);
            printf("\t\treg_syntax_is_valid: %u\n", i_item->param1.params_union.reg.validity.reg_syntax_is_valid);
        }

    }

    /* param2 */
    printf("\tParam2:\n");

    if (i_item->param2.param_type.no_param) /* if param 2 not exists */
        printf("\t\tparam2 is not defined \n");
    else    
    {    

        if (i_item->param2.param_type.param_is_number)
        {
            printf("\t\tparam_is_number: %u\n", i_item->param2.param_type.param_is_number);
            printf("\t\tnum: %d\n", i_item->param2.params_union.num.num);
            printf("\t\tnum_is_valid: %u\n", i_item->param2.params_union.num.validity.num_is_valid);
        }
        if (i_item->param2.param_type.param_is_label)
        {
            printf("\t\tparam_is_label: %u\n", i_item->param2.param_type.param_is_label);
            printf("\t\tlabel_name: %s\n", i_item->param2.params_union.lbl.label_name);
            printf("\t\tlabel_addr: %d\n", i_item->param2.params_union.lbl.label_addr);
            printf("\t\tlabel_addr_defined: %u\n", i_item->param2.params_union.lbl.validity.label_addr_defined);
            printf("\t\tlabel_length_valid: %u\n", i_item->param2.params_union.lbl.validity.label_length_valid);
            printf("\t\tlabel_param_syntax_valid: %u\n", i_item->param2.params_union.lbl.validity.label_param_syntax_valid);
        }
        if (i_item->param2.param_type.param_is_matrx)
        {
            printf("\t\tparam_is_matrx: %u\n", i_item->param2.param_type.param_is_matrx);
            printf("\t\tmat_label: %s\n", i_item->param2.params_union.mat.mat_label);
            printf("\t\treg1_num: %d\n", i_item->param2.params_union.mat.reg1_num);
            printf("\t\treg2_num: %d\n", i_item->param2.params_union.mat.reg2_num);
            printf("\t\tmat_label_defined: %d\n", i_item->param2.params_union.mat.validity.mat_label_defined);
            printf("\t\tmat_label_syntax_valid: %d\n", i_item->param2.params_union.mat.validity.mat_label_syntax_valid);
            printf("\t\tbrackets_valid: %d\n", i_item->param2.params_union.mat.validity.brackets_valid);
            printf("\t\treg1_name_valid: %d\n", i_item->param2.params_union.mat.validity.reg1_name_valid);
            printf("\t\treg2_name_valid: %d\n", i_item->param2.params_union.mat.validity.reg2_name_valid);
        }
        if (i_item->param2.param_type.param_is_reg_name)
        {
            printf("\t\tparam_is_reg_name: %u\n", i_item->param2.param_type.param_is_reg_name);
            printf("\t\treg_num: %u\n", i_item->param2.params_union.reg.reg_num);
            printf("\t\treg_num_is_valid: %u\n", i_item->param2.params_union.reg.validity.reg_num_is_valid);
            printf("\t\treg_syntax_is_valid: %u\n", i_item->param2.params_union.reg.validity.reg_syntax_is_valid);
        }
    }

    
    printf("\tCode data:\n"); /* code data */

    printf("\t\t code word 1: \n");
    printf("\t\t\texist: %u\n", i_item->code.i_wrd1.exist);
    printf("\t\t\tE R A: %u\n", i_item->code.i_wrd1.E_R_A);
    printf("\t\t\tcode value: 0X%X\n", i_item->code.i_wrd1.codeval);
    printf("\t\t\tIC addr : %d\n", i_item->code.i_wrd1.ic_addr);

    printf("\t\t code word 2: \n");
    printf("\t\t\texist: %u\n", i_item->code.i_wrd2.exist);
    printf("\t\t\tE R A: %u\n", i_item->code.i_wrd2.E_R_A);
    printf("\t\t\tcode value: 0X%X\n", i_item->code.i_wrd2.codeval);
    printf("\t\t\tIC addr : %d\n", i_item->code.i_wrd2.ic_addr);

    printf("\t\t code word 3: \n");
    printf("\t\t\texist: %u\n", i_item->code.i_wrd3.exist);
    printf("\t\t\tE R A: %u\n", i_item->code.i_wrd3.E_R_A);
    printf("\t\t\t code value: 0X%X\n", i_item->code.i_wrd3.codeval);
    printf("\t\t\t IC addr : %d\n", i_item->code.i_wrd3.ic_addr);

    printf("\t\t code word 4: \n");
    printf("\t\t\texist: %u\n", i_item->code.i_wrd4.exist);
    printf("\t\t\tE R A: %u\n", i_item->code.i_wrd4.E_R_A);
    printf("\t\t\tcode value: 0X%X\n", i_item->code.i_wrd4.codeval);
    printf("\t\t\tIC addr : %d\n", i_item->code.i_wrd4.ic_addr);

    printf("\t\t code word 5: \n");
    printf("\t\t\texist: %u\n", i_item->code.i_wrd5.exist);
    printf("\t\t\tE R A: %u\n", i_item->code.i_wrd5.E_R_A);
    printf("\t\t\tcode value: 0X%X\n", i_item->code.i_wrd5.codeval);
    printf("\t\t\tIC addr : %d\n", i_item->code.i_wrd5.ic_addr);
}

/* function print_cpu_i_item :
    Prints data of CPU instruction item in linked list
    Parameter : pointer to CPU instruction item in linked list
*/
void f_print_cpu_i_item(FILE * fd, i_item_ptr i_item)
{
    fprintf(fd, "--------------------------\n");
    fprintf(fd, "CPU instruction item:\n");
    fprintf(fd, "\tstart_addr: %d\n", i_item->start_addr);
    fprintf(fd, "\tcmd_name: %s\n", i_item->cmd_name);
    fprintf(fd, "\tlabel_name: %s\n", i_item->label_name);
    fprintf(fd, "\tsize_in_words: %d\n", i_item->size_in_wrds);
    fprintf(fd, "\tis_valid: %d\n", i_item->is_valid);
    /* status */
    fprintf(fd, "\tStatus:\n");
    fprintf(fd, "\t\tcmd_label_found: %u\n", i_item->status.cmd_label_found);
    fprintf(fd, "\t\tparams_num: %u\n", i_item->status.params_num);
    fprintf(fd, "\t\tno_params_found: %u\n", i_item->status.no_params_found);
    fprintf(fd, "\t\tparam1_found: %u\n", i_item->status.param1_found);
    fprintf(fd, "\t\tparam2_found: %u\n", i_item->status.param2_found);
    fprintf(fd, "\t\tdest_addresing: %u\n", i_item->status.dest_addrsng);
    fprintf(fd, "\t\tsrc_addresing: %u\n", i_item->status.src_addrsng);
    fprintf(fd, "\t\topcode: %u\n", i_item->status.opcode);

    /* param1 */
    fprintf(fd, "\tParam1:\n");
    if (i_item->param1.param_type.no_param) /* if param 1 not exists */
        fprintf(fd, "\t\tparam1 is not defined \n");
    else

    {    

        if (i_item->param1.param_type.param_is_number)
        {
            fprintf(fd, "\t\tparam_is_number: %u\n", i_item->param1.param_type.param_is_number);
            fprintf(fd, "\t\tnum: %d\n", i_item->param1.params_union.num.num);
            fprintf(fd, "\t\tnum_is_valid: %u\n", i_item->param1.params_union.num.validity.num_is_valid);
        }
        if (i_item->param1.param_type.param_is_label)
        {
            fprintf(fd, "\t\tparam_is_label: %u\n", i_item->param1.param_type.param_is_label);
            fprintf(fd, "\t\tlabel_name: %s\n", i_item->param1.params_union.lbl.label_name);
            fprintf(fd, "\t\tlabel_addr: %d\n", i_item->param1.params_union.lbl.label_addr);
            fprintf(fd, "\t\tlabel_addr_defined: %u\n", i_item->param1.params_union.lbl.validity.label_addr_defined);
            fprintf(fd, "\t\tlabel_length_valid: %u\n", i_item->param1.params_union.lbl.validity.label_length_valid);
            fprintf(fd, "\t\tlabel_param_syntax_valid: %u\n", i_item->param1.params_union.lbl.validity.label_param_syntax_valid);
        }
        if (i_item->param1.param_type.param_is_matrx)
        {
            fprintf(fd, "\t\tparam_is_matrx: %u\n", i_item->param1.param_type.param_is_matrx);
            fprintf(fd, "\t\tmat_label: %s\n", i_item->param1.params_union.mat.mat_label);
            fprintf(fd, "\t\treg1_num: %d\n", i_item->param1.params_union.mat.reg1_num);
            fprintf(fd, "\t\treg2_num: %d\n", i_item->param1.params_union.mat.reg2_num);
            fprintf(fd, "\t\tmat_label_defined: %d\n", i_item->param1.params_union.mat.validity.mat_label_defined);
            fprintf(fd, "\t\tmat_label_syntax_valid: %d\n", i_item->param1.params_union.mat.validity.mat_label_syntax_valid);
            fprintf(fd, "\t\tbrackets_valid: %d\n", i_item->param1.params_union.mat.validity.brackets_valid);
            fprintf(fd, "\t\treg1_name_valid: %d\n", i_item->param1.params_union.mat.validity.reg1_name_valid);
            fprintf(fd, "\t\treg2_name_valid: %d\n", i_item->param1.params_union.mat.validity.reg2_name_valid);
        }
        if (i_item->param1.param_type.param_is_reg_name)
        {
            fprintf(fd, "\t\tparam_is_reg_name: %u\n", i_item->param1.param_type.param_is_reg_name);
            fprintf(fd, "\t\treg_num: %u\n", i_item->param1.params_union.reg.reg_num);
            fprintf(fd, "\t\treg_num_is_valid: %u\n", i_item->param1.params_union.reg.validity.reg_num_is_valid);
            fprintf(fd, "\t\treg_syntax_is_valid: %u\n", i_item->param1.params_union.reg.validity.reg_syntax_is_valid);
        }

    }

    /* param2 */
    fprintf(fd, "\tParam2:\n");

    if (i_item->param2.param_type.no_param) /* if param 2 not exists */
        fprintf(fd, "\t\tparam2 is not defined \n");
    else    
    {

        if (i_item->param2.param_type.param_is_number)
        {
            fprintf(fd, "\t\tparam_is_number: %u\n", i_item->param2.param_type.param_is_number);
            fprintf(fd, "\t\tnum: %d\n", i_item->param2.params_union.num.num);
            fprintf(fd, "\t\tnum_is_valid: %u\n", i_item->param2.params_union.num.validity.num_is_valid);
        }
        if (i_item->param2.param_type.param_is_label)
        {
            fprintf(fd, "\t\tparam_is_label: %u\n", i_item->param2.param_type.param_is_label);
            fprintf(fd, "\t\tlabel_name: %s\n", i_item->param2.params_union.lbl.label_name);
            fprintf(fd, "\t\tlabel_addr: %d\n", i_item->param2.params_union.lbl.label_addr);
            fprintf(fd, "\t\tlabel_addr_defined: %u\n", i_item->param2.params_union.lbl.validity.label_addr_defined);
            fprintf(fd, "\t\tlabel_length_valid: %u\n", i_item->param2.params_union.lbl.validity.label_length_valid);
            fprintf(fd, "\t\tlabel_param_syntax_valid: %u\n", i_item->param2.params_union.lbl.validity.label_param_syntax_valid);
        }
        if (i_item->param2.param_type.param_is_matrx)
        {
            fprintf(fd, "\t\tparam_is_matrx: %u\n", i_item->param2.param_type.param_is_matrx);
            fprintf(fd, "\t\tmat_label: %s\n", i_item->param2.params_union.mat.mat_label);
            fprintf(fd, "\t\treg1_num: %d\n", i_item->param2.params_union.mat.reg1_num);
            fprintf(fd, "\t\treg2_num: %d\n", i_item->param2.params_union.mat.reg2_num);
            fprintf(fd, "\t\tmat_label_defined: %d\n", i_item->param2.params_union.mat.validity.mat_label_defined);
            fprintf(fd, "\t\tmat_label_syntax_valid: %d\n", i_item->param2.params_union.mat.validity.mat_label_syntax_valid);
            fprintf(fd, "\t\tbrackets_valid: %d\n", i_item->param2.params_union.mat.validity.brackets_valid);
            fprintf(fd, "\t\treg1_name_valid: %d\n", i_item->param2.params_union.mat.validity.reg1_name_valid);
            fprintf(fd, "\t\treg2_name_valid: %d\n", i_item->param2.params_union.mat.validity.reg2_name_valid);
        }
        if (i_item->param2.param_type.param_is_reg_name)
        {
            fprintf(fd, "\t\tparam_is_reg_name: %u\n", i_item->param2.param_type.param_is_reg_name);
            fprintf(fd, "\t\treg_num: %u\n", i_item->param2.params_union.reg.reg_num);
            fprintf(fd, "\t\treg_num_is_valid: %u\n", i_item->param2.params_union.reg.validity.reg_num_is_valid);
            fprintf(fd, "\t\treg_syntax_is_valid: %u\n", i_item->param2.params_union.reg.validity.reg_syntax_is_valid);
        }

    }

    fprintf(fd, "\tCode data:\n"); /* code data */

    fprintf(fd, "\t\t code word 1: \n");
    fprintf(fd, "\t\t\texist: %u\n", i_item->code.i_wrd1.exist);
    fprintf(fd, "\t\t\tE R A: %u\n", i_item->code.i_wrd1.E_R_A);
    fprintf(fd, "\t\t\tcode value: 0X%X\n", i_item->code.i_wrd1.codeval);
    fprintf(fd, "\t\t\tIC addr : %d\n", i_item->code.i_wrd1.ic_addr);

    fprintf(fd, "\t\t code word 2: \n");
    fprintf(fd, "\t\t\texist: %u\n", i_item->code.i_wrd2.exist);
    fprintf(fd, "\t\t\tE R A: %u\n", i_item->code.i_wrd2.E_R_A);
    fprintf(fd, "\t\t\tcode value: 0X%X\n", i_item->code.i_wrd2.codeval);
    fprintf(fd, "\t\t\tIC addr : %d\n", i_item->code.i_wrd2.ic_addr);

    fprintf(fd, "\t\t code word 3: \n");
    fprintf(fd, "\t\t\texist: %u\n", i_item->code.i_wrd3.exist);
    fprintf(fd, "\t\t\tE R A: %u\n", i_item->code.i_wrd3.E_R_A);
    fprintf(fd, "\t\t\t code value: 0X%X\n", i_item->code.i_wrd3.codeval);
    fprintf(fd, "\t\t\t IC addr : %d\n", i_item->code.i_wrd3.ic_addr);

    fprintf(fd, "\t\t code word 4: \n");
    fprintf(fd, "\t\t\texist: %u\n", i_item->code.i_wrd4.exist);
    fprintf(fd, "\t\t\tE R A: %u\n", i_item->code.i_wrd4.E_R_A);
    fprintf(fd, "\t\t\tcode value: 0X%X\n", i_item->code.i_wrd4.codeval);
    fprintf(fd, "\t\t\tIC addr : %d\n", i_item->code.i_wrd4.ic_addr);

    fprintf(fd, "\t\t code word 5: \n");
    fprintf(fd, "\t\t\texist: %u\n", i_item->code.i_wrd5.exist);
    fprintf(fd, "\t\t\tE R A: %u\n", i_item->code.i_wrd5.E_R_A);
    fprintf(fd, "\t\t\tcode value: 0X%X\n", i_item->code.i_wrd5.codeval);
    fprintf(fd, "\t\t\tIC addr : %d\n", i_item->code.i_wrd5.ic_addr);
}
