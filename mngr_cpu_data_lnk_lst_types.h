


/* We will store CPU data inside linked list. 
   Every element in linked list will store 
   information necessary to create data code 
   and validate data directive related to some code line in input file.

   The new data will be added to the end of linked list.
   The read out of linked list will be done from the start of linked list.
   
   To simplify insertion of new items (at the tail) and read out of 
   the data (from the head) we will manage 2 pointers: 
   pointer to head of the list and pointer to tail of the list 
   (similarly to Queues implementation). 

   Every item in linked list will be processed in the 
   same order as data directives text processing. 
   
   After we'll know the last address of CPU instuctions,
   we will add offset address to all items adress data 
   to put data definitions after CPU code. 


   Note that .entry and .external commands dont 
   generate directly data lines in the code. 
   They only declare the status of labels (if the label is exported or imported).
   that is related mostly to labels table features.

   So we will prepare structure for .data/.string/.mat commands
   that related to data storage in the data memory.
 
*/

#if !defined(mngr_cpu_data_lnk_lst_types_h)
#define mngr_cpu_data_lnk_lst_types_h

typedef struct num_lnk_lst_item * num_lnk_lst_ptr;

/* #ifndef uint
typedef unsigned int uint;
#endif */

/* data linked list stores numbers according to their arrival value
   each data will be added to end of list
*/
typedef struct num_lnk_lst_item
{

    int num;
    num_lnk_lst_ptr next;

} num_lnk_lst_item;

typedef struct directive_flags
{
    uint is_data_dot_directive : 1;
    uint is_str_dot_directive : 1;
    uint is_mat_dot_directive : 1;
    uint is_valid_directive : 1; /* directive validity summary */
    uint err_has_invalid_label : 1;
    uint err_has_no_operands : 1;
    uint error_in_parameters : 1;
} directive_flags;

typedef struct data_blk_item * data_blk_item_ptr;

typedef struct data_blk_item
{

    int start_addr; 
    int size_in_words;
    uint is_valid; 
    char * label_name;
    char * directive_name;
    directive_flags flags;
    num_lnk_lst_ptr num_head; /*  pointer to head of linked list of int numbers  */
    num_lnk_lst_ptr num_tail; /*  pointer to tail of linked list of int numbers                      
                                          .data directive data is series of signed integer numbers
                                                up to 10 bit.
                                          .mat directive data is also series of signed numbers
                                                (matrix elements stored row by row data)
                                          .string directive data is also
                                                series of integer numbers, where
                                                each number is ascii code of string
                                                and the last number is zero. */
    data_blk_item_ptr  next;
} data_blk_item;




#endif /* #if !defined(mngr_cpu_data_lnk_lst_types_h) */        