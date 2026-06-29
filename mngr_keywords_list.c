
/* this code prepares requiremnents for assembly code*/

#include "assembler.h"

/*
     Binary search tree will be created to store the list of all keywords in 
     efficient way for future processing.

     Each node of this tree will contain pointer to keyword (string constants).

     Binary search tree arrangement allows to check fastly if
     there is some word in it and to get ordered list of values ("in order").

     Binary search tree arrangement will also allow to use
     merge algoritm (that is a part of merge-sort algorithm) with some adaptation
     to check efficiently if there is ANY SAME word inside two different binary trees
     (for example, macros BST and keywords BST ), here is the algorithm :

          1. Compare words in sorted order ("in order") from both Binary Search Trees.
          2. If word in one binary search tree is less than word in second tree, 
             take the next word from this tree and compare with current word in another tree. 
          3. If two words are the same, report the case (found equal items).
          4. If words if any binary search trees are finished, the check is finished. 
*/



/* Function keywords_bst_create : creates dynamically allocated binary search tree 
     and returns pointer to top element of it. 
   
   Note : the creation of BST is done in 2 stages : 
      1. Creation of temporary unsorted internal (automatic) array with items contents.
         this array will disappear after finish of function run. 
      2. Create binary search tree by recursive find place for each new node,
         dynamically creation of the node and copy array's values into new node's parameters. 
         Return pointer to top of binary tree.
*/   
keywords_item_ptr keywords_bst_create()
{

   /*
       Keywords table will be stored firstly in temporary internal array
       (that is automatic variable) and after it array's values
       will be used to create dynamic binary search tree. 

       Note: we recognize between Data directive with '.' at the begin of word
             (for ex. ".data") and Data directive witout '.' (for example "data")
             as forbidden value for label or macro names.  

       Here is definition of keywords array:
   */

   int keywrd_arr_length;
   int i;
   keywords_item_ptr keyword_bst_top; /* pointer to top item of keywords binary serch tree */

   keywords_array keywrd_arr = {
       
       /* CPU commands with 2 operands */
       {"mov", 0xEF0221u},
       {"cmp", 0xFF1221u},
       {"add", 0xEF2221u},
       {"sub", 0xEF3221u},
       {"lea", 0xE64221u},
      
       /* CPU commands with 1 operand */
       {"clr", 0xE05111u},
       {"not", 0xE06111u},
       {"inc", 0xE07111u},
       {"dec", 0xE08111u},
       {"jmp", 0xE09111u},
       {"bne", 0xE0A111u},
       {"jsr", 0xE0B111u},
       {"red", 0xE0C111u},
       {"prn", 0xF0D111u},
       
       /* CPU commands without operands */
       {"rts", 0x00E001u},
       {"stop", 0x00F001u},
       
       /* Data directives */
       {".data", 0x000F12u},
       {".string", 0x000112u},
       {".mat", 0x000F12u},
       {".entry", 0x000112u},
       {".extern", 0x000112u},

       /* Macro definition commands*/
       {"mcro", 0x000118u},
       {"mcroend", 0x000008},

       /* fobidden names */
       {"data", 0x000008u},
       {"string", 0x000008u},
       {"mat", 0x000008u},
       {"entry", 0x000008u},
       {"extern", 0x000008u},

       /* Registers names */
       {"r0", 0x000004u},
       {"r1", 0x000004u},
       {"r2", 0x000004u},
       {"r3", 0x000004u},
       {"r4", 0x000004u},
       {"r5", 0x000004u},
       {"r6", 0x000004u},
       {"r7", 0x000004u}
       
       }; /* finish keywrd_arr initialization */

   keywrd_arr_length = sizeof(keywrd_arr)/sizeof(keywrd_arr[0]);

   /* add array's items to binary search tree, 
      Each time the search for place of new item will start from the top of binary tree.
   */
   for (i = 0; i < keywrd_arr_length; i++){
      if (i==0)
         keyword_bst_top = add_item_to_bst(NULL, keywrd_arr, keywrd_arr_length, 0);
      else
         add_item_to_bst(keyword_bst_top, keywrd_arr, keywrd_arr_length, i);
   }

   return keyword_bst_top; /* return pointer to binary search tree top */
}

/* Recursive function add_item_to_bst: 
      adds a node with keyword parameters from k_arr[index] array element
      to the binary search tree.
   Finds place to insert the node with keyword recursievly in the tree and inserts it
   if the node does not exist yet */
keywords_item_ptr add_item_to_bst(keywords_item_ptr parent, keywords_array k_arr, int k_arr_length, int index){
   int compare_result;

   if (index >= k_arr_length)
      return parent;

   if (parent == NULL)
   {
      /* allocate dynamically memory for parent keyword item */
      parent = k_item_alloc(k_arr, index);
      return parent; /* stop and return parent node after its creation */
   }

   /* compare keyword in parent and in array (with incremented index)*/
   compare_result = strncmp((k_arr + index)->keyword, parent->keyword,MAX_MACRO_AND_LABEL_LENGTH);
   if (compare_result == 0)
      ; /* do nothing , the key already in tree */
   else if (compare_result < 0)
   {
      /* if parent exists and the word is less than parent's word */
      parent->left = add_item_to_bst(parent->left, k_arr, k_arr_length, index);

   }
   else
   {
      /* if parent exists and the word is larger than parent's word */
      parent->right = add_item_to_bst(parent->right, k_arr, k_arr_length, index);
   }
   return parent;
}

/* Function k_item_alloc : 
    dynamic allocation of memory for keywords struct item
    and initialization of it based on value of keywords array k_arr[index]
    
    Return: 
      pointer to node of binary search tree.

*/
keywords_item_ptr k_item_alloc(keywords_array k_arr, int index)
{
   keywords_item_ptr parent = NULL;
   uint key_feature = 0;
   char * heap_key_param_name = NULL;

   /* allocate dynamically memory for parent keyword item */
   parent = (keywords_item_ptr)malloc(sizeof(keywords_item));
   if (parent == NULL)
   {
      printf("Error : Can not allocate memory for keyword item");
      release_all_dynamic_memory_and_files();
      exit(1);
   }
   /* init keyword item 
      by copy keyword item to heap */
   heap_key_param_name = heap_string_duplicate_safe((k_arr + index)->keyword,MAX_MACRO_AND_LABEL_LENGTH);

   parent->keyword = heap_key_param_name;
   parent->left = NULL;
   parent->right = NULL;

   key_feature = (k_arr + index)->k_item_feature;
   
   /* Store the data in bitfield structure : */
   num_to_feature(key_feature, &(parent->feature));

   return parent;

} 



/* Recursive print of keywords table tree
   The print is performed "in order" (Left , Root, Right) - alphabetic order of keywords.
   The keywords table is printed in following order:
      Firstly appear keyword name
      After it appear keyword fetaures hexadecimal number.

   Parameter:
      keyword_table: pointer to the top node of binary search tree.
*/
void keywords_table_print(keywords_item_ptr keywords_table)
{
   if (keywords_table != NULL)
   {

      keywords_table_print(keywords_table->left);

      /* in order father print : */
      /* print keyword name */
      printf("%s \n", keywords_table->keyword );
      /* print assotiated feature value */
      printf("   0x%08X\n", feature_to_num( &(keywords_table->feature)));

      keywords_table_print(keywords_table->right);
   }
}

/* Recursive function to check if the given word is inside binary search tree
      Return:
               - pointer to node that match keyword value
               - NULL if the node does not exist in the tree
      Parameters:
               keywords_table : pointer to macro table tree top node
               keyword_name     : pointer to macro name string
*/
keywords_item_ptr keywords_table_search(keywords_item_ptr keywords_table, char *keyword_name)
{

      keywords_item_ptr keyword_item_p = NULL;
      int str_cmp_res = 0;

      if (keywords_table == NULL) /* keyword_name parameter does not match any node in tree */
         return NULL;
      else if ((str_cmp_res = strncmp(keyword_name, keywords_table->keyword,MAX_MACRO_AND_LABEL_LENGTH)) == 0)
         keyword_item_p = keywords_table; /* keyword name match the node */
      else if (str_cmp_res < 0)
         keyword_item_p = keywords_table_search(keywords_table->left, keyword_name);
      else /* str_cmp_res > 0 */
         keyword_item_p = keywords_table_search(keywords_table->right, keyword_name);

      return keyword_item_p;   
}

/* Recursive function is_keywrd_in_table : cheks is there is
   node in table that match given keyword name text (or any given text).
      Return:  - 0 if name does not exist in keyword table tree
               - 1 if name exists in macro keyword tree
      Parameters:
               keyword_table : pointer to keyword table tree top node
               keyword_name  : pointer to keyword name string
*/
int is_keyword_in_table(keywords_item_ptr keywords_table, char * keyword_name)
{

   int str_cmp_res;

   if (keywords_table == NULL) /* keyword_name parameter does not match any node in tree */
      return FALSE;
   else if ((str_cmp_res = strncmp(keyword_name, keywords_table->keyword,MAX_MACRO_AND_LABEL_LENGTH)) == 0)
      return TRUE; /* macro name match the node */
   else if (str_cmp_res < 0)
      return is_keyword_in_table(keywords_table->left, keyword_name);
   else /* str_cmp_res > 0 */
      return is_keyword_in_table(keywords_table->right, keyword_name);
}






/* Function num_to_feature takes unsigned number and stores it as 
   features struct */
void num_to_feature(uint num, keyword_features * feature){

   /* Store the data in bitfield structure : */

   /* 1-st 4 bits group*/
   feature->is_cpu_instruction = (num & is_cpu_instruction_mask) >> 0;
   feature->is_data_directive = (num & is_data_directive_mask) >> 1;
   feature->is_register_name = (num & is_register_name_mask) >> 2;
   feature->is_reserved_word = (num & is_reserved_word_mask) >> 3;
   /* 2-nd 4 bits group */
   feature->min_num_of_operands = (num & min_num_of_operands_mask) >> 4;
   /* 3-rd 4 bits group */
   feature->max_num_of_operands = (num & max_num_of_operands_mask) >> 8;
   /* 4-th 4 bits group */
   feature->opcode = (num & opcode_mask) >> 12;
   /* 5-th 4 bits group */
   feature->src_addr_mthd_0 = (num & src_addr_mthd_0_mask) >> 16;
   feature->src_addr_mthd_1 = (num & src_addr_mthd_1_mask) >> 17;
   feature->src_addr_mthd_2 = (num & src_addr_mthd_2_mask) >> 18;
   feature->src_addr_mthd_3 = (num & src_addr_mthd_3_mask) >> 19;
   /* 6-th 4 bits group */
   feature->dest_addr_mthd_0 = (num & dest_addr_mthd_0_mask) >> 20;
   feature->dest_addr_mthd_1 = (num & dest_addr_mthd_1_mask) >> 21;
   feature->dest_addr_mthd_2 = (num & dest_addr_mthd_2_mask) >> 22;
   feature->dest_addr_mthd_3 = (num & dest_addr_mthd_3_mask) >> 23;
}

/* Function featue_to_num  
   takes feature struct and prepares number from it */

int feature_to_num(keyword_features *feature)
{  uint num;
   num = ((uint)(feature->is_cpu_instruction) << 0) +
         ((uint)(feature->is_data_directive) << 1) +
         ((uint)(feature->is_register_name) << 2) +
         ((uint)(feature->is_reserved_word) << 3) +
         ((uint)(feature->min_num_of_operands) << 4) +
         ((uint)(feature->max_num_of_operands) << 8) +
         ((uint)(feature->opcode) << 12) +
         ((uint)(feature->src_addr_mthd_0) << 16) +
         ((uint)(feature->src_addr_mthd_1) << 17) +
         ((uint)(feature->src_addr_mthd_2) << 18) +
         ((uint)(feature->src_addr_mthd_3) << 19) +
         ((uint)(feature->src_addr_mthd_0) << 20) +
         ((uint)(feature->src_addr_mthd_1) << 21) +
         ((uint)(feature->src_addr_mthd_2) << 22) +
         ((uint)(feature->src_addr_mthd_3) << 23) ;

   return num;
}   
