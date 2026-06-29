/* This code contains functions to create manage macro table  */

/* Because we don't know in advance the number of macros,
   the Macro table is arranged as binary search tree (see par.6.5 , Kernigan and Ritchie),
   Each tree entry (struct) stores macro's name as sorting key.
   Each entry is and inserted in the tree by string compare of macro names. 

   Also each tree entry stores pointer to head of linked list of lines assotiated with 
   its macro name. Each node in linked list of lines stores the pointer to one 
   line string inside macro.
*/

#include "assembler.h"

/* Recursive function add_macro_tree: adds a node with macro_name inside the binary search tree.
    Finds place to insert the node in the tree and inserts it.
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

    Important Note : it is IMPORTANT to set parent be NULL at first call to fucntion,
                     undefined garbage value of parent pointer will cause to segmentation fault
                     (in better case...)
*/
macro_table_entry_ptr add_macro_tree(macro_table_entry_ptr parent, char *macro_name)
{
        int compare_res;

        /* stop condition (and basic action) */
        if (parent == NULL){        /* if parent node does not exist, create it */
            parent = tree_node_alloc(macro_name); 
            return parent;
        }

        /* recursion increment and recursion call */
        if ((compare_res = strncmp(macro_name,parent->macro_name,MAX_MACRO_AND_LABEL_LENGTH)) == 0) /* macro_name already 
                                                                        in parent node */
            ; /* do nothing */
        else if (compare_res < 0) /* if less than parent , continue with left subtree */
            parent->left = add_macro_tree(parent->left, macro_name);
        else
            parent->right = add_macro_tree(parent->right, macro_name);

        return parent;
}

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
int add_line_to_table_entry(macro_table_entry_ptr table_ptr, char *macro_name, char *macro_line)
{
    macro_table_entry_ptr table_entry_ptr = NULL;
    table_entry_ptr = macro_entry_search(table_ptr, macro_name); /* search for entry that match
                                                                    macro name                 */
    if (table_entry_ptr == NULL)
        return -1;
    else
    {
        add_macro_line(table_entry_ptr, macro_line);
        return 1;
    }
}




/* Function add_macro_line : adds given macro line to end of macro lines linked
   list assotiated with macro table entry.

   Parameters:
    table_entry_ptr : pointer to some macro table entry ;
    macro_line      : pointer to macro line string that we want to add to
                      above macro table entry.
*/
void add_macro_line(macro_table_entry_ptr table_entry_ptr, char *macro_line)
{
    macro_line_node_ptr new_line_node_ptr = NULL;
    macro_line_node_ptr line_node_ptr = NULL;
    line_node_ptr = table_entry_ptr->line_list_head;
    /* create new line node to be added to lines nodes linked list */
    if ((new_line_node_ptr = (macro_line_node_ptr)malloc(sizeof(macro_line_node))) == NULL)
    {
        fprintf(stderr, "Error in add_macro_line: Can't allocate memory for macro_line_node");
        release_all_dynamic_memory_and_files();
        exit(1);
    };
    /* set parameters of the new node */
    /* add line heap COPY to macro line node !*/
    new_line_node_ptr->line = heap_string_duplicate_safe(macro_line,MAX_STR_LEN+5);
    new_line_node_ptr->next_line_node = NULL;
    new_line_node_ptr->prev_line_node = NULL;

    if (table_entry_ptr->line_list_head == NULL)
    { /* if linked list does not exist
         add new node as first entry in lines list*/
        (table_entry_ptr->line_list_head) = new_line_node_ptr;
    }
    else
    {                                            /* add line to last entry in lines list  */
        while (line_node_ptr->next_line_node == NULL) /* search for last line in the list */
            line_node_ptr->next_line_node = new_line_node_ptr; /* connect new node to last node */
            new_line_node_ptr->prev_line_node = line_node_ptr;
            line_node_ptr = line_node_ptr->next_line_node;
    }
}




/* Recursive function macro_entry_search :
      Return:  
               - pointer to node that match macro_name value
               - NULL if the node does not exist in the tree
      Parameters:
               macro_table : pointer to macro table tree top node
               macro_name  : pointer to macro name string
*/
macro_table_entry_ptr macro_entry_search(macro_table_entry_ptr macro_table, char *macro_name)
{

    macro_table_entry_ptr global_macro_table_ptr = NULL;
    int str_cmp_res;

    if (macro_table == NULL) /* macro_name parameter does not match any node in tree */
        return NULL;
    else if ((str_cmp_res = strncmp(macro_name, macro_table->macro_name,MAX_MACRO_AND_LABEL_LENGTH)) == 0)
        global_macro_table_ptr = macro_table; /* macro name match the node */
    else if (str_cmp_res < 0)
        global_macro_table_ptr = macro_entry_search(macro_table->left, macro_name);
    else /* str_cmp_res > 0 */
        global_macro_table_ptr = macro_entry_search(macro_table->right, macro_name);

    return global_macro_table_ptr;
}




/* Recursive function is_macro_in_table : cheks is there is
   node in table that match given macro name text (or any given text).
      Return:  - 0 if macro does not exist in macro table tree
               - 1 if macro exists in macro table tree
      Parameters:
               macro_table : pointer to macro table tree top node
               macro_name  : pointer to macro name string
*/
int is_macro_in_table(macro_table_entry_ptr macro_table, char *macro_name)
{

    int str_cmp_res;

    if (macro_table == NULL) /* macro_name parameter does not match any node in tree */
        return FALSE;
    else if ((str_cmp_res = strncmp(macro_name, macro_table->macro_name,MAX_MACRO_AND_LABEL_LENGTH)) == 0)
        return TRUE; /* macro name match the node */
    else if (str_cmp_res < 0)
        return is_macro_in_table(macro_table->left, macro_name);
    else /* str_cmp_res > 0 */
        return is_macro_in_table(macro_table->right, macro_name);
}




/* Recursive print of macro names in binary search tree 
   in alphabetical order ("in order").

   Parameter: 
        macro_table: pointer to the top node of binary search tree.
*/
void macros_names_print(macro_table_entry_ptr macro_table)
{

    if (macro_table != NULL)
    {
        macros_names_print(macro_table->left);
        printf("%s \n", macro_table->macro_name);
        macros_names_print(macro_table->right);
    }
}



/* Recursive print of macro table tree
   The print is performed "in order" (Go Left, Print Root, Go Right) - alphabetic order of macro names.
   The macro table is printed in following order:
        Firstly appear macro name
        After it appear macro body lines assotiated with macro name.

   Parameter:
        macro_table: pointer to the top node of binary search tree.
*/
void macros_table_print(macro_table_entry_ptr macro_table)
{
    macro_line_node_ptr line_node_ptr;

    if (macro_table != NULL)
    {
        
        macros_table_print(macro_table->left);

        /* print macro name */
        printf("%s \n", macro_table->macro_name);
        /* print assotiated linked list of lines */
        line_node_ptr = macro_table->line_list_head;
        while (line_node_ptr != NULL)
        {
            printf("   %s\n", line_node_ptr->line);
            line_node_ptr = line_node_ptr->next_line_node;
        }
        macros_table_print(macro_table->right);
    }
}



/* Function tree_node_alloc: allocates new macro tree node 
   and return pointer value to it.
   Parameter:
      macro_name : pointer to macro name string to be stored in new macro tree node.
   Return : 
      pointer to new allocated macro tree node.
   Based on talloc function in Kernigan and Ritchie, page 142.
*/
macro_table_entry_ptr tree_node_alloc(char * macro_name)
{
        macro_table_entry_ptr new_tree_node_ptr;
        /* dynamically create new tree_node object */
        new_tree_node_ptr = (macro_table_entry_ptr)malloc(sizeof(macro_table_entry));
        
        /* set new tree node parameters */
                 /* copy macro name to dynamically
                    allocated array and assign pointer to it*/
        new_tree_node_ptr->macro_name = heap_string_duplicate_safe(macro_name,MAX_MACRO_AND_LABEL_LENGTH);
        new_tree_node_ptr->line_list_head = NULL;
        new_tree_node_ptr->left = NULL;
        new_tree_node_ptr->right = NULL;

        /* return pointer to new tree node */
        return new_tree_node_ptr;
}




/* Safe copy of any string to dynamically allocated char array.
    Limits copy length !
   Returns pointer to this array.

   It can be used to copy string created in temporay stack memory into
   heap memory.

   Based on strdup function in Karnigan and Ritchie , page.143
*/
char* heap_string_duplicate_safe(char *str, int length_limit)
{
    char *p = NULL; /* pointer to character variable */

    p = (char *)malloc(sizeof(char)*length_limit+1); /* + 1 because of '\0' */
    if (p==NULL){
        fprintf(ERR_STREAM, "Error in allocation memory for string %s", str);
        release_all_dynamic_memory_and_files();
        exit(1);
    } else {
        strncpy(p,str,length_limit);
    }
    return p;
}