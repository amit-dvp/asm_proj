/*
   This header defines types for Macro Table implementation.

*/


/* Because we don't know in advance the number of macros,
   the Macro Table is arranged as binary search tree (see par.6.5 , Kernigan and Ritchie),
   Each tree entry (struct) stores macro's name as sorting key.
   Each entry is and inserted in the tree by string compare of macro names.

   Also each tree entry stores pointer to head of linked list of lines assotiated with
   its macro name. Each node in linked list of lines stores the pointer to one
   line string inside macro.
*/

/* Note : Header guard prevents only the same header file from being included multiple times
          within a single translation unit, not across different ones.
          Each tranlation unit translated separetly, unrelated to other units   */

#if !defined(macro_table_h)
#define macro_table_h


/* pointer to linked list item macro_line_item that stores one macro line.
    The pointer can be defined before definition of struct macro_line_item. */
typedef struct macro_line_node *macro_line_node_ptr;

/* macro_line_item item for linked list for storage of lines of macro
    each line of the same macro is stored in linked list */
typedef struct macro_line_node
{
    macro_line_node_ptr next_line_node; /* pointer to next line */
    macro_line_node_ptr prev_line_node; /*pointer to prev. line node */
    char *line;                         /* pointer to line string of given macro */
} macro_line_node;

typedef struct macro_table_entry * macro_table_entry_ptr; /* pointer can be defined before
                                                             struct itself */

/* each macro table entry has macro name key
    it also store ponter to head of lines linked list assotiated with macro.
*/
typedef struct macro_table_entry
{
    char *macro_name;
    macro_line_node_ptr line_list_head; /* pointer to head of lines list */
    macro_table_entry_ptr left;         /* pointer to left child in binary search tree  */
    macro_table_entry_ptr right;        /* pointer to right child in binary search tree */

} macro_table_entry;

#endif /* #if !defined(macro_table_h) */

