#include "assembler.h"

/*
 This file contains function to free heap dynamic allocated data and release files.

   a) All pointers to heap memory will be stored in global memory to allow 
      memory release from any point of program , for example in case of 
      exit on memory allocation problem or in case of new source file
      processing.

   b) The most of data stored in heap is arranged as linked data structure 
      to allow data release through small number of global pointers.


   c) To release files from any part of program, we will store files pointers 
      as global variables; Every file pointer that is already released 
      will be set to NULL. Every unused file pointer will be also set to NULL.
      This is done to allow release of only already opened files. 


 We use following structures in project that are allocated in heap:

    1.  data_blk_item : linked list to store information of data directives.
           This linked list is arranged in oeder of data directives.
           We access every element in this linked list through 2 global pointers :
                     global_d_head
                     global_d_tail
           Every data_blk_item has assotiated with it linked list of data that
           can be accessed through following pointers:
                     data_blk_item.num_head
                     data_blk_item.num_tail
           Also every data_blk_item can store pointers to string objects (char *) allocated in heap:
                      data_blk_item.label_name
                      data_blk_item.directive_name

    2.  i_item :  linked list to store information of CPU instructions .
           This linked list is arranged according to order of CPU commands.
           We acceess every element in this linked list through 2 global pointers :
                     global_i_head
                     global_i_tail
           Also every i_item can store following pointers to string objects (char *) in heap:
                     i_item.cmd_name
                     i_item.label_name

    3.  label_item : binary search tree of labels.
           This tree is arranged according to labels names keys.
           We access every element in this tree through following global pointer :
                     global_labels_table_ptr
           Also every label_item can store pointer to string object (char *) in heap:
                     label_item.label_name

    4.  keywords_item : binary search tree of keywords.
           Also stores requirements assotiated with each keyword.
           This tree is arranged according to keywords names keys.
           We acess every element in this tree through global pointer :
                     global_keyword_table_ptr
           Each tree node can contain pointer to keyword string in heap:
                     keywords_item.keyword

    5.  macro_table_entry : macros binary search tree.
           This tree is arranged according to macros names keys.
           We acess every element in this tree through global pointer :
                     global_macro_table_ptr
           Each macro table entry can contain pointer to linked list 
           of macro lines assotiated with it:
                     macro_table_entry.line_list_head
           Each macro table entry can contain pointer to macro name:
                     macro_table_entry.macro_name

    6.  error_def_item : errors definitions linked list
          This tree is arranged according to error ID numbers keys.
          We access every elemnet in this linked list through
                     global_err_list_head
          Each error_def_item can contain pointer to following strings in heap:
                     error_def_item.error_type
                     error_def_item.error_description           

*/

/******************* global pointers definition ******************************* */

error_def_item_ptr global_err_list_head = NULL;      /* global pointer to pointer to linked list of errord definitions */
keywords_item_ptr global_keyword_table_ptr = NULL;   /* global pointer to top of keywords BST (Binary Search Tree) */
macro_table_entry_ptr global_macro_table_ptr = NULL; /* global pointer to top of macros BST (Binary Search Tree) */

i_item_ptr global_i_head = NULL; /* global head of instruction items linked list */
i_item_ptr global_i_tail = NULL; /* global tail of instruction items linked list */

data_blk_item_ptr global_d_head = NULL; /* global head of data items linked list */
data_blk_item_ptr global_d_tail = NULL; /* global tail of data items linked list */

lable_item_ptr global_labels_table_ptr = NULL; /* global pointer to labels binary search tree */

/* global pointer to full filename with .as extention*/
char * global_full_fname_as = NULL;
/* global pointer to full filename with .am extention */
char * global_full_fname_am = NULL;
/* global pointer to full filename with .ext extention */
char * global_full_fname_ext = NULL;
/* global pointer to full filename with .ent extention */
char * global_full_fname_ent = NULL;
/* global pointer to full filename with .ob extention */
char * global_full_fname_ob = NULL;
/* global pointer to full filename with .log extention */
char * global_full_fname_log = NULL;
/* global pointer to full filename with .tmp extention */
char * global_full_fname_tmp = NULL;

FILE * global_fin = NULL;    /* global pointer to input file    */

FILE * global_fout_1 = NULL;  /* global pointer to output file 1 */
FILE * global_fout_2 = NULL;  /* global pointer to output file 2 */


char * global_gen_str_pointer = NULL; /* gen. purposer global string pointer to string object in heap */

/*****************  function to release all dynamic allocated  memory and files *********************************/

/*
       function all_dynamic_memory_free
           releases all dynamic allocated memory
*/
void release_all_dynamic_memory_and_files()
{

               partial_dynamic_memory_free(global_d_head,global_d_tail,
                                    global_i_head, global_i_tail,
                                    global_labels_table_ptr,
                                    global_macro_table_ptr,
                                    global_full_fname_as,
                                    global_full_fname_am,
                                    global_full_fname_ext,
                                    global_full_fname_ent,
                                    global_full_fname_ob,
                                    global_full_fname_log, 
                                    global_full_fname_tmp,
                                    global_gen_str_pointer);
                                    
       remainder_dynamic_memory_free();
}

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
                              macro_table_entry_ptr macro_table_ptr,
                              char * full_fname_as,
                              char * full_fname_am,
                              char * full_fname_ext,
                              char * full_fname_ent,
                              char * full_fname_ob ,
                              char * full_fname_log,
                              char * full_fname_tmp,
                              char * gen_str_pointer )
{

        /* free directives data linked list */
        free_data_blk_list(&d_head, &d_tail); 



       
       /* free CPU instructions data linked list*/
       free_i_items_lst(&i_head, &i_tail);


       
       /* free labels table binary search tree */
       free_label_binary_search_tree(labels_table_ptr);
       
       /* free macros binary search tree */
       free_macro_binary_search_tree(macro_table_ptr);



       /* release pointer to full filename with .as extention */
       if (full_fname_as)
       {
              free(full_fname_as);
       }
       /* release pointer to full filename with .am extention */ 
       if (full_fname_am)
       {
              free(global_full_fname_am);
       }
       /* release pointer to full filename with .ext extention */
       if (full_fname_ext)
       {
              free(global_full_fname_ext);
       }
       /* release pointer to full filename with .ent extention */ 
       if (global_full_fname_ent)
       {
              free(full_fname_ent);
       }
       /* release pointer to full filename with .ob extention */ 
       if (global_full_fname_ob)
       {
              free(full_fname_ob);
       }
       /* release pointer to full filename with .log extention */
       if (global_full_fname_log)
       {
              free(full_fname_log);
       }
       /* release pointer to full filename with .tmp extention */ 
       if (global_full_fname_tmp)
       {
              free(full_fname_tmp);
       }

       /* release general pointer to str */
       if (gen_str_pointer)
       {
              free(global_gen_str_pointer);
              gen_str_pointer = NULL;
       }

            global_d_head = NULL ;
            global_d_tail = NULL; 
            global_i_head = NULL; 
            global_i_tail = NULL; 
            global_labels_table_ptr = NULL;
            global_macro_table_ptr  = NULL;
            global_full_fname_as    = NULL;
            global_full_fname_am    = NULL;
            global_full_fname_ext   = NULL;
            global_full_fname_ent   = NULL;
            global_full_fname_ob    = NULL;
            global_full_fname_log   = NULL; 
            global_full_fname_tmp   = NULL;
            global_gen_str_pointer  = NULL;


       /* release input and output file pointers  */
/*        if (global_fin)
       {
              fclose(global_fin);
              global_fin = NULL;
       }

       if (global_fout_1)
       {
              fclose(global_fout_1);
              global_fout_1 = NULL;
       }

       if (global_fout_2)
       {
              fclose(global_fout_2);
              global_fout_2 = NULL;
       } */
}

/*
       function remainder_dynamic_memory_free
              releases dynamic memory remainder
              including (used for next file processing):
                     keywords table binary search tree
                     errors linked list



*/
void remainder_dynamic_memory_free()
{

       /* free keywords table binary search tree */
       free_keys_binary_search_tree(global_keyword_table_ptr);
       global_keyword_table_ptr = NULL;
       free_errors_list(global_err_list_head);
       global_err_list_head = NULL;

}

/*
       function full_file_names_release
           releases dynamic allocated memory
           of full file names only.

*/
void full_file_names_release()
{


       /* release pointer to full filename with .as extention*/
       if (global_full_fname_as)
       {
              free(global_full_fname_as);
              global_full_fname_as = NULL;
       }
       /* release pointer to full filename with .am extention */
       if (global_full_fname_am)
       {
              free(global_full_fname_am);
              global_full_fname_am = NULL;
       }
       /* release pointer to full filename with .ext extention */
       if (global_full_fname_ext)
       {
              free(global_full_fname_ext);
              global_full_fname_ext = NULL;
       }
       /* release pointer to full filename with .ent extention */
       if (global_full_fname_ent)
       {
              free(global_full_fname_ent);
              global_full_fname_ent = NULL;
       }
       /* release pointer to full filename with .ob extention */
       if (global_full_fname_ob)
       {
              free(global_full_fname_ob);
              global_full_fname_ob = NULL;
       }
       /* release pointer to full filename with .log extention */
       if (global_full_fname_log)
       {
              free(global_full_fname_log);
              global_full_fname_log = NULL;
       }
       /* release pointer to full filename with .tmp extention */
       if (global_full_fname_tmp)
       {
              free(global_full_fname_tmp);
              global_full_fname_tmp = NULL;
       }

       /* release general pointer to str */
       if (global_gen_str_pointer)
       {
              free(global_gen_str_pointer);
              global_gen_str_pointer = NULL;
       }

       /* release input and output file pointers  */
       if (global_fin)
       {
              fclose(global_fin);
              global_fin = NULL;
       }

       if (global_fout_1)
       {
              fclose(global_fout_1);
              global_fout_1 = NULL;
       }

       if (global_fout_2)
       {
              fclose(global_fout_2);
              global_fout_2 = NULL;
       }       

}

/*************************  generate global full file names ************************/
/*
 function global_full_file_names_gen
              allocates storage for file names in heap and assignes pointer to them
              to global pointers
*/
void global_full_file_names_gen (char * file_name){
    
    /*FILE *only_try_to_open;* check file */

       global_full_fname_as  = add_extn_to_fname(file_name, ".as");
       global_full_fname_am  = add_extn_to_fname(file_name, ".am");
       global_full_fname_ext = add_extn_to_fname(file_name, ".ext");
       global_full_fname_ent = add_extn_to_fname(file_name, ".ent");
       global_full_fname_ob  = add_extn_to_fname(file_name, ".ob");
       global_full_fname_log = add_extn_to_fname(file_name, ".log");
       global_full_fname_tmp = add_extn_to_fname(file_name, ".tmp");

        /* check if input file really exists 
       only_try_to_open = fopen(global_full_fname_as, "r");
       if (!only_try_to_open){
            printf("Error: input file %s can not be opened for read\n", global_full_fname_as);
            printf("Exitting...\n\n");
            remainder_dynamic_memory_free();
            full_file_names_release();
            exit(1);
       }
       fclose(only_try_to_open);
       only_try_to_open = NULL; */

}

/* Function add_extn_to_fname:
   creates full filename by
   adition of extention to file name */
char * add_extn_to_fname(char *fname, char *const ext)
{

       char *full_fname;
       
       full_fname = (char *)malloc(sizeof(char) * MAX_MACRO_AND_LABEL_LENGTH + 3);
       if (!full_fname)
       {
              print_err(global_err_list_head, 1, -1, "Exitting...", "");
              release_all_dynamic_memory_and_files();
              exit(1);
       }

       strncpy(full_fname, fname,MAX_MACRO_AND_LABEL_LENGTH);
       strncat(full_fname, ext,MAX_MACRO_AND_LABEL_LENGTH);

       return full_fname;
}



/****************  free dynamic memory of data block linked list *************************/

/* function free_data_blk_list:
          release memory of data block list from head to tail.

          parameters: 
              head - pointer to pointer to head of linked list
              tail - pointer to pointer to tail of linked list
*/
void free_data_blk_list(data_blk_item_ptr *head, data_blk_item_ptr *tail)
{
       data_blk_item_ptr prev = NULL; /* pointer to previous first data item */

       if(*head == NULL)
              return;

       /* increment head and free previous head */
       while ((*tail) && (*head) && ((*tail) != (*head)))
       {
              prev = *head;      /* store pointer to previous head item */
              *head = prev->next; /* update head to point next value */

              /* release num link list within data block list if num_hed != NULL and num_tail != NULL*/
              if (prev->num_head)
                     free_num_lnk_lst(&(prev->num_head), &(prev->num_tail));

              /* release label_name if it is not NULL*/
                     if(prev->label_name)
                     {
                            free(prev->label_name);  
                            prev->label_name = NULL;    
                     }       
              /* release directive_name if it is not NULL */  
                     if(prev->directive_name)
                     {
                            free((void *)(prev->directive_name));
                            prev->directive_name = NULL;
                     }
                     prev->next = NULL;
              
              
                     if(prev)
                     {
                            free((void *)(prev)); /* release previous value */
                            prev = NULL;
                     } 

       }


              /* free the last item */
              if ((*tail) && (*head) && (*tail == *head))
              {

                     /*  release num link list within data block list */
                     if ((*tail)->num_head && (*tail)->num_tail)
                            free_num_lnk_lst(&((*tail)->num_head), &((*tail)->num_tail));
                     /* release label_name if it is not NULL */
                     if ((*tail)->label_name)
                     {
                            free((void *)((*tail)->label_name));
                            (*tail)->label_name = NULL;
                     }
                     /*  release directive_name if it is not NULL */ 
                     if ((*tail)->directive_name)
                     {
                            free((void *)((*tail)->directive_name));
                            (*tail)->directive_name = NULL;
                     }
                     if (*tail){
                            free((void *)(*tail));
                            *tail = NULL;
                            *head = NULL;
                     }        
              }




}





/* function free_num_lnk_lst:
      release memory of num link list associated with data linked list item.

          parameters:
              head - pointer to pointer to head of linked list
              tail - pointer to pointer to tail of linked list

*/
void free_num_lnk_lst(num_lnk_lst_ptr *head, num_lnk_lst_ptr *tail)
{

       num_lnk_lst_ptr prev; /* pointer to previous num item */
       /* increment head and free previous head item */
       while ((*tail) && (*head) && (*tail != *head))
       {
              prev = *head;      /* store pointer to previous head item */
              *head = prev->next; /* update head to point next value */
              prev->next = NULL;
              if(prev)
              {
                     free((void *)(prev)); /* free previous head item */
                     prev = NULL;
              }       
       }

       /* now free the last item */
       if ((*tail) && (*head) && (*tail == *head))
       {      (*head)->next = NULL;
              free((void *)(*head));
              *tail = *head = NULL;
       }
}

/****************  free dynamic memory of CPU instructions linked list *************************/

/* function free_i_items_lst:
      release memory of data block list
       parameters:
              head - pointer to pointer to head of linked list
              tail - pointer to pointer to tail of linked list
*/
void free_i_items_lst(i_item_ptr *head, i_item_ptr *tail)
{

       i_item_ptr prev; /* pointer to previous first data item */

       /* increment head and free previous head */
       while ((*tail) && (*head) && (*tail != *head))
       {
              prev = *head;      /* store pointer to previous head item */
              *head = prev->next; /* update head to point next value */
              /* release cmd_name pointer within i_item */
              if(prev->cmd_name)
              {
                     free((void *)(prev->cmd_name));
                     prev->cmd_name = NULL;
              }       
              /* release label_name pointer within i_item */
              if (prev->label_name)
              {
                     free((void *)(prev->label_name));
                     prev->label_name = NULL;
              }
              free((void *)(prev)); /* release previous value */
              prev = NULL;
       }
       /* free the last item */
       if ((*tail) && (*head) && (*tail == *head))
       {
              prev = *head;      /* store pointer to previous head item */
              /* release cmd_name pointer within i_item */
              if(prev->cmd_name)
              {
                     free((void *)(prev->cmd_name));
                     prev->cmd_name = NULL;
              }       
              /* release label_name pointer within i_item */
              if (prev->label_name)
              {
                     free((void *)(prev->label_name));
                     prev->label_name = NULL;
              }       
              
              free((void *)(prev)); /* release previous value */
              prev = NULL;      
      
       }

       *tail = *head = NULL;
}



/*******************  free memory of binary search tree of labels ************************/

/* To free a memory of label binary search tree,
   we will use "post-order" (Go Left , Go Right, Free Root) traversal.
   This ensures that we free each root node AFTER its children have been freed,
   avoiding access to already freed memory.

   parameter : 
       root : pointer to top item of BST
*/
void free_label_binary_search_tree(lable_item_ptr root)
{
       if (root == NULL)
              return;
       /* left */
       free_label_binary_search_tree(root->left);
       /* right */
       free_label_binary_search_tree(root->right);
       /* before free root, free label_name pointer */
       if (root->label_name){
              free((void *)(root->label_name));
              root->label_name = NULL;
       }       
       /* free root */
       if (root){
              free((void *)root);
              root = NULL;
       }       
}

/************************ free memory of keywords binary search tree ********************/

/* To free a memory of keywords binary search tree,
   we will use "post-order" (Go Left , Go Right, Free Root) traversal.
   This ensures that we free each node after its children have been freed,
   avoiding access to already freed memory.
*/
void free_keys_binary_search_tree(keywords_item_ptr root)
{
       if (!root)
              return;
       /* left */
       free_keys_binary_search_tree(root->left);
       /* right */
       free_keys_binary_search_tree(root->right);
       /* root */
       /* before free root, free pointer to keyword assotiated with it : */
       if(root->keyword)
       {
              free((void *)(root->keyword));
              root->keyword=NULL;
       }
       if(root)
       {
              free(root);
              root = NULL;
       }
}

/************************ free macros table binary search tree  ********************/

/* To free a memory of macro binary search tree,
   we will use "post-order" (Go Left , Go Right, Free Root) traversal.
   This ensures that we free each root node AFTER its children have been freed,
   avoiding access to already freed memory.
*/
void free_macro_binary_search_tree(macro_table_entry_ptr root)
{
              if (!root)
                     return;
              /* left */
              if (root->left)
                     free_macro_binary_search_tree(root->left);
              /* right */
              if(root->right)
                     free_macro_binary_search_tree(root->right);

              /* before free root, free macro_name pointer */
              if (root->macro_name){
                     /* free line list assocoated with root */
                     free_line_list(root);
                     free((void *)(root->macro_name));
                     root->macro_name = NULL;       
              }       
              /* free root */
              if (root)
              {      /* free line list assocoated with root */
                     free((void*)(root));
                     root = NULL;
              }
}

/*

 function free_line_list_macro
       
       frees line linked list associated with given macro binary search item.
       parameter:
              root : pointer to top of macro table binary search tree

*/

void free_line_list (macro_table_entry_ptr root)
{


macro_line_node_ptr line_item = root->line_list_head;
macro_line_node_ptr prev_line_item = NULL;

if(line_item == NULL)
       return;

/* go ahead to search for last item :  */
while(line_item->next_line_node)
       line_item=line_item->next_line_node;
/* go back to free line items  */
while(line_item->prev_line_node) 
{
       if(line_item->prev_line_node){
              prev_line_item = line_item->prev_line_node;
              free(line_item);
              line_item = NULL;
              line_item = prev_line_item;
       } else {
              free(line_item);
              line_item = NULL;
       }
}       
}





/************************    free errors linked list     ********************/

/*  function free_errors_list

        releases dynamic linked list of errors
*/
void free_errors_list(error_def_item_ptr list_h)
{

       error_def_item_ptr list_item = list_h; 
       error_def_item_ptr prev_list_item = NULL;

       if (!list_h)
              return;

       /* search for last item */
       while (list_item->next != NULL)
       {
              prev_list_item = list_item;
              list_item = prev_list_item->next;
       }

       /* go reverse until we get head of list */
       while (list_item->prev != NULL)
       {
              prev_list_item = list_item->prev;
              /* disconnect next list item (last) from previous */
              prev_list_item->next = NULL;
              /* before release list item , release pointer to error_type and error_description */
              free((void *)(list_item->error_desription) );
              list_item->error_desription = NULL;
              free((void *)(list_item->error_type));
              list_item->error_type = NULL;
              /* release last list item */
              list_item->prev=NULL;
              
              free((void *)list_item);

              list_item = NULL;
              list_item = prev_list_item;
       }
       /* release the last item: */
       if (list_item)
       {
              /* before release list item , release pointer to error_type and error_description */
              free((void *)(list_item->error_desription) );
              list_item->error_desription = NULL;
              free((void *)(list_item->error_type));
              list_item->error_type = NULL;
              free((void *)list_item);
              list_item = NULL;
       }
}



/******************* finction to sel all global file names *****************************/


