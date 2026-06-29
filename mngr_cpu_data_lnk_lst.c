#include "assembler.h"

extern error_def_item_ptr global_err_list_head;


/* We will store CPU data inside linked list.
   Every element in this linked list will store
   information necessary to create data code
   and validate data directive.

   The new data will be added to the end of linked list.
   The read out of linked list will be done at order as
   data inserted, i.e. from the start of linked list.
   To simplify insertion of new item (at the tail) and read out of
   the data (from the head) we will manage 2 pointers:
   pointer to head of the list and pointer to tail of the list
   (similarly to as it is done in Queues).

   Every item in this linked list will be processed in the
   same order as data directives processing.

   Every item in CPU data linked list will have pointers to head and tail
   of internal linked list of integer numbers ("data payload"):
        .data directive data payload is series of signed integer numbers.
        .mat directive data payload is also series of signed numbers
            that are matrix elements stored row by row. 
        .string directive data is also series of integer numbers, 
            while each number stores the ascii code of one char 
            and the last number in series is zero.


   After we'll know the last address of CPU instuctions,
   we will add this offset address to updte data definitions to appear after
   CPU instructions in the code.


   Note that .entry and .external commands dont
   generate directly data lines in the code.
   They only declare the status of labels (if the label is exported or imported).
   that is related mostly to labels table features.

   So we will prepare structure for .data/.string/.mat commands
   that related to data storage in the data memory.

*/

/* function add_data_blk_item :
       creates the new item
       adds the new updated item to the end of linked 
       list. 
    updates tail and head, if needed.
    returns pointer to the new item

    Note : the new item is inttialized to default values only.
*/
data_blk_item_ptr add_new_data_blk_item(data_blk_item_ptr * head, data_blk_item_ptr * tail)
{
    data_blk_item_ptr prev = NULL;   /* pointer to previous last data item */
    /* create the new data item */
    data_blk_item_ptr new_data = NULL;
    new_data = (data_blk_item_ptr)malloc(sizeof(data_blk_item));
    if (!new_data){
        print_err(global_err_list_head, 83, -1, "", "");
        release_all_dynamic_memory_and_files();
        exit(1);
    }
    /* set parameters of new item to init value :*/
    new_data->start_addr = 0;
    new_data->size_in_words = 0;
    new_data->label_name = NULL;
    new_data->directive_name = NULL;
    new_data->flags.is_data_dot_directive=0;
    new_data->flags.is_str_dot_directive=0;
    new_data->flags.is_mat_dot_directive=0;
    new_data->flags.is_valid_directive=0;
    new_data->flags.err_has_invalid_label=0;
    new_data->flags.err_has_no_operands=0;
    new_data->flags.error_in_parameters=0;
    new_data->num_head = NULL;
    new_data->num_tail = NULL;
    new_data->next = NULL;

    if (!(*head))       /* set first element is list */
        *tail = *head = new_data;
    else {    /* insert new data to end of list and update tail pointer */
        /* connect previous item to new data */
        prev = *tail;
        prev->next = new_data;
        /* update tail to point new data */
        *tail = new_data;
    }

    return new_data;
}








/* function add_item_to_num_lnk_lst
        adds num to end of internal data linked list

    We will change head pointer and tail pointers NULL to point first item in case
          of empty list, so we use double pointer for them.  
    Returns pointer to new_item       
*/
num_lnk_lst_ptr add_item_to_num_lnk_lst(num_lnk_lst_ptr *head, num_lnk_lst_ptr *tail, int num)
{
    num_lnk_lst_ptr prev;      /* pointer to previous tail item (or NULL) */
    num_lnk_lst_ptr new_data = NULL;
    /* create new item struct object */
    new_data = (num_lnk_lst_ptr)malloc(sizeof(num_lnk_lst_item));
    if (!new_data){
        print_err(global_err_list_head, 82, -1, "", "");
        release_all_dynamic_memory_and_files();
        exit(1);
    }
    /* assign new item parameters */
    new_data->num = num;
    new_data->next = NULL;
    /* connect new data item to list */
    if ( !(*head)) /* set first element is list */
        *tail = *head = new_data;
    else
    { /* insert new data to end of list and update tail pointer */
        /* connect previous item to new data */
        prev = *tail;
        prev->next = new_data;
        /* update tail to point new data */
        *tail = new_data;
    }
    return new_data;
}



/* function print_data_blk_item :
    Prints data of data block item
    Parameter : pointer to data block item
*/
void print_data_blk_item(data_blk_item_ptr data_item)
{
        printf("CPU data item : \n");
        printf("\tstart addr: %d\n", data_item->start_addr);
        printf("\tsize in words: %d\n", data_item->size_in_words);
        printf("\tdirective name: %s\n",data_item->directive_name);
        printf("\tlabel name : %s\n", data_item->label_name);
        printf("\tflags : \n");
        printf("\t\tis_data_dot_directive : %u\n", data_item->flags.is_data_dot_directive);
        printf("\t\tis_str_dot_directive: %u\n", data_item->flags.is_str_dot_directive);
        printf("\t\tis_mat_dot_directive: %u\n", data_item->flags.is_mat_dot_directive);
        printf("\t\tis_valid_directive: %u\n", data_item->flags.is_valid_directive);
        printf("\t\terr_has_invalid_label: %u\n", data_item->flags.err_has_invalid_label);
        printf("\t\terr_has_no_operands: %u\n", data_item->flags.err_has_no_operands);
        printf("\t\terror_in_parameters: %u\n ", data_item->flags.error_in_parameters);
        printf("\tdata payload values: \n\t\t");
        print_num_lnk_lst(data_item->num_head, data_item->num_tail);
}

/* function print_data_blk_item :
    Prints data of data block item
    Parameter : pointer to data block item
*/
void f_print_data_blk_item(FILE * fd, data_blk_item_ptr data_item)
{
    fprintf(fd, "\nCPU data item : \n");
    fprintf(fd, "\tstart addr: %d\n", data_item->start_addr);
    fprintf(fd, "\tsize in words: %d\n", data_item->size_in_words);
    fprintf(fd, "\tdirective name: %s\n", data_item->directive_name);
    fprintf(fd, "\tlabel name : %s\n", data_item->label_name);
    fprintf(fd, "\tflags : \n");
    fprintf(fd, "\t\tis_data_dot_directive : %u\n", data_item->flags.is_data_dot_directive);
    fprintf(fd, "\t\tis_str_dot_directive: %u\n", data_item->flags.is_str_dot_directive);
    fprintf(fd, "\t\tis_mat_dot_directive: %u\n", data_item->flags.is_mat_dot_directive);
    fprintf(fd, "\t\tis_valid_directive: %u\n", data_item->flags.is_valid_directive);
    fprintf(fd, "\t\terr_has_invalid_label: %u\n", data_item->flags.err_has_invalid_label);
    fprintf(fd, "\t\terr_has_no_operands: %u\n", data_item->flags.err_has_no_operands);
    fprintf(fd, "\t\terror_in_parameters: %u\n ", data_item->flags.error_in_parameters);
    fprintf(fd, "\tdata payload values: \n\t\t");
    f_print_num_lnk_lst(fd, data_item->num_head, data_item->num_tail);
}

/* function print_d_items_list

    prints all data items list info

*/
void print_d_items_list(data_blk_item_ptr head, data_blk_item_ptr tail)
{
    int start_flag = 1;
    data_blk_item_ptr node = head; /* pointer to list node */
    
    /* increment head and free previous head */
    do
    {
        if (start_flag == 0)
            node = node->next;
        start_flag = 0;
        print_data_blk_item(node);
    } while (node != tail);
}

/* function f_print_d_items_list

    prints all data items list info to file

*/
void f_print_d_items_list(FILE * fd, data_blk_item_ptr head, data_blk_item_ptr tail)
{
    int start_flag = 1;
    data_blk_item_ptr node = head; /* pointer to list node */

    /* increment head and free previous head */
    do
    {
        if (start_flag == 0)
            node = node->next;
        start_flag = 0;
        f_print_data_blk_item(fd, node);
    } while (node != tail);
}




/* this function prints num list */
void print_num_lnk_lst(num_lnk_lst_ptr head, num_lnk_lst_ptr tail)
{
    num_lnk_lst_ptr tmp = head ;
    while (tmp)
    {
        
        printf("%d ", tmp-> num);
        tmp = tmp->next;
    } 
    
    puts("");
}

/* this function prints num list to file*/
void f_print_num_lnk_lst(FILE * fd, num_lnk_lst_ptr head, num_lnk_lst_ptr tail)
{
    num_lnk_lst_ptr tmp = head;
    while (tmp)
    {

        fprintf(fd, "%d ", tmp->num);
        tmp = tmp->next;
    }

    puts("");
}
