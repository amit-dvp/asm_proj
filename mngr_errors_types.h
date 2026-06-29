/* Note : Header guard prevents only the same header file from being included multiple times
          within a single translation unit, not across different ones.
          Each tranlation unit translated separetly, unrelated to other units   */

#if !defined(errors_mngr_types_h)

#define errors_mngr_types_h

/* errors definitions will be defined temporary in local array and 
   passed to dynamic linked list */


/* following struct will store error definition */
typedef struct error_def {
    int error_number;
    char * error_type;
    char * error_desription;
} error_def;


/* define array of error_handlers */
typedef error_def errors_arr[];



/* define pointer to error_def_item */
typedef struct error_def_item * error_def_item_ptr;

/* following struct will store list of possible errors */
typedef struct error_def_item
{
    int error_number;
    char *error_type;
    char *error_desription;
    error_def_item_ptr next;
    error_def_item_ptr prev;

} error_def_item;

#endif /* #if !defined(errors_mngr_types_h) */


/* define global constants */
extern  char * const PRE_ASM_ERR ;
extern  char * const DYN_ALC_ERR ;
extern  char * const ASM_1_ERR   ;

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




