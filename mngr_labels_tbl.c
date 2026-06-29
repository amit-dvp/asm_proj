/* labels table is data structure used for first and second assembly pass

    labels table : Binary Search Tree
           labels table will be binary search tree with label name key.
           BST is arranged according to alphabetic order.
           Each element in BST is struct.
*/

#include "assembler.h"

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

lable_item_ptr add_labels_tree(lable_item_ptr parent, char * label_name )
{
    int compare_res;

    /* stop condition (and basic action) */
    if (parent == NULL)
    { /* if parent node does not exist, create it */
        parent = label_node_alloc(label_name);
        return parent;
    }

    /* recursion increment and recursion call */
    if ((compare_res = strncmp(label_name, parent->label_name,MAX_MACRO_AND_LABEL_LENGTH)) == 0) /* label_name already
                                                                     in parent node */
        ;  /* do nothing */
    else if (compare_res < 0)      /* if less than parent , continue with left subtree */
        parent->left = add_labels_tree(parent->left, label_name);
    else
        parent->right = add_labels_tree(parent->right, label_name);
    return parent;
}


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
lable_item_ptr label_node_alloc(char *label_name)
{
    lable_item_ptr new_node;
    /* dynamically create new tree_node object */
    new_node = (lable_item_ptr)malloc(sizeof(label_item));

    /* set new tree node parameters */
    /* copy label name to dynamically
       allocated array and assign pointer to it*/
    /* heap_string_duplicate is defined in mngr_macro_tbl.c 
       it copies string from stack array to heap memory.
    */
    new_node->label_name = heap_string_duplicate_safe(label_name,MAX_MACRO_AND_LABEL_LENGTH);
    /* set default values of features that 
       must be updated after it */
    new_node->label_address = TO_BE_DEFINED;    /* -5 */
    new_node-> features.points_to_cpu_cmd = 0;
    new_node-> features.is_cpu_cmd_param = 0;
    new_node-> features.points_to_data = 0;
    new_node-> features.points_to_str = 0;
    new_node-> features.points_to_mat = 0;
    new_node-> features.is_extrn = 0;
    new_node-> features.is_entry = 0;
    /* sets left and right to default (NULL) */
    new_node->left = NULL;
    new_node->right = NULL;

    /* return pointer to new tree node */
    return new_node;
}

/* Print label table tree node information
 */
void label_node_print(lable_item_ptr label_table)
{
    if (label_table == NULL)
    {
        printf("label table node is empty\n");
    } else {
        printf("label table node data:\n");
        printf("\tlabel_name: %s\n", label_table->label_name);
        printf("\tlabel_address: %d\n", label_table->label_address);
        printf("\tpoints_to_cpu_cmd: %u\n", label_table->features.points_to_cpu_cmd);
        printf("\tis_cpu_cmd_param: %u\n", label_table->features.is_cpu_cmd_param);
        printf("\tpoints_to_data: %u\n", label_table->features.points_to_data);
        printf("\tpoints_to_str: %u\n", label_table->features.points_to_str);
        printf("\tpoints_to_mat: %u\n", label_table->features.points_to_mat);
        printf("\tis_extrn: %u\n", label_table->features.is_extrn);
        printf("\tis_entry: %u\n", label_table->features.is_entry);
    }
}

/* Print label table tree node information
 */
void f_label_node_print(FILE * fd, lable_item_ptr label_table)
{
    if (label_table == NULL)
    {
        fprintf(fd, "label table node is empty\n");
    }
    else
    {
        fprintf(fd, "label table node data:\n");
        fprintf(fd, "\tlabel_name: %s\n", label_table->label_name);
        fprintf(fd, "\tlabel_address: %d\n", label_table->label_address);
        fprintf(fd, "\tpoints_to_cpu_cmd: %u\n", label_table->features.points_to_cpu_cmd);
        fprintf(fd, "\tis_cpu_cmd_param: %u\n", label_table->features.is_cpu_cmd_param);
        fprintf(fd, "\tpoints_to_data: %u\n", label_table->features.points_to_data);
        fprintf(fd, "\tpoints_to_str: %u\n", label_table->features.points_to_str);
        fprintf(fd, "\tpoints_to_mat: %u\n", label_table->features.points_to_mat);
        fprintf(fd, "\tis_extrn: %u\n", label_table->features.is_extrn);
        fprintf(fd, "\tis_entry: %u\n", label_table->features.is_entry);
    }
}

/* Recursive print of label table binary search tree
   The print is performed "in order" (Go Left, Print Root, Go Right) - alphabetic order of label names.
   The label table is printed in following order:
        - left tree
        - parent
        - right tree

   Parameter:
        label_table: pointer to the top node of binary search tree.
*/
void label_table_print(lable_item_ptr label_table)
{
    if (label_table == NULL){
        ;  /* do nothing */
    }else
    {
        label_table_print(label_table->left);
        label_node_print(label_table);
        label_table_print(label_table->right);
    }
}

/* Recursive print of label table binary search tree to file
   The print is performed "in order" (Go Left, Print Root, Go Right) - alphabetic order of label names.
   The label table is printed in following order:
        - left tree
        - parent
        - right tree

   Parameter:
        label_table: pointer to the top node of binary search tree.
*/
void f_label_table_print( FILE * fd, lable_item_ptr label_table)
{
    if (label_table == NULL)
    {
        ; /* do nothing */
    }
    else
    {
        f_label_table_print(fd, label_table->left);
        f_label_node_print(fd, label_table);
        f_label_table_print(fd, label_table->right);
    }
}

/* Recursive function label_entry_search :
      Return:
               - pointer to node that match label_name value
               - NULL if the node does not exist in the tree
      Parameters:
               label_table : pointer to label table tree top node
               label_name  : pointer to label name string
*/
lable_item_ptr label_entry_search(lable_item_ptr label_table, char * label_name)
{

    lable_item_ptr label_table_ptr = NULL;
    int str_cmp_res;

    if (label_table == NULL) /* label_name parameter does not match any node in tree */
        return NULL;
    else if ((str_cmp_res = strncmp(label_name, label_table->label_name,MAX_MACRO_AND_LABEL_LENGTH)) == 0)
        label_table_ptr = label_table; /* label name match the node */
    else if (str_cmp_res < 0)
        label_table_ptr = label_entry_search(label_table->left, label_name);
    else /* str_cmp_res > 0 */
        label_table_ptr = label_entry_search(label_table->right, label_name);

    return label_table_ptr;
}

/* Recursive function is_label_in_table : cheks is there is
   node in table that match given label name text.
      Return:  - 0 if label does not exist in label table tree
               - 1 if label exists in label table tree
      Parameters:
               label_table : pointer to label table tree top node
               label_name  : pointer to label name string
*/
int is_label_in_table(lable_item_ptr label_table, char * label_name)
{
    int str_cmp_res;

    if (label_table == NULL) /* label_name parameter does not match any node in tree */
        return FALSE;
    else if ((str_cmp_res = strncmp(label_name, label_table->label_name,MAX_MACRO_AND_LABEL_LENGTH)) == 0)
        return TRUE; /* label name match the node */
    else if (str_cmp_res < 0)
        return is_label_in_table(label_table->left, label_name);
    else /* str_cmp_res > 0 */
        return is_label_in_table(label_table->right, label_name);
}

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
int add_label_feature (lable_item_ptr * parent_node , char *label_name, label_features new_features, int label_address)
{
    int res;
    lable_item_ptr label_item = NULL;
    uint old_features_value;
    uint new_features_value;


    /* if binary search tree does not exist, or label does not exist in the tree 
       Note short circuiting for || !
     */
    if (!(*parent_node) || !(res = is_label_in_table(*parent_node, label_name)))
    {
        *parent_node = add_labels_tree(*parent_node, label_name);

    }

    /* search for label's item inside the tree */
    label_item = label_entry_search(*parent_node, label_name);


    /* if the label already appears in the tree */

    /* convert item's old feature values to number */
    old_features_value = convert_feature_to_num(label_item->features);

    /* convert new feature values to number */
    new_features_value = convert_feature_to_num(new_features);

    /* perform bitwise OR between features values */
    old_features_value |= new_features_value;

    convert_num_to_feature(&(label_item->features),old_features_value );

    /* check if address is not defined : */
    if (label_address != TO_BE_DEFINED){
        if (label_item->label_address == TO_BE_DEFINED)
            label_item->label_address = label_address;
        else /* if address already defined , dont change it */
            return ERR_LABEL_ADDR_EXISTS;
    }

    return LABEL_ADDR_UPDATED;
}

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
int check_feature_validity(label_features feature, char * label_name){
    
    uint res; /* number to check features logic*/
    int check_num  ; 
    /* convert feature to number : */
    res = convert_feature_to_num(feature);

    /* now check if any of following bits are 1 simultaneously :
        bit0 (points_to_cpu_cmd)
        bit2 (points_to_data)
        bit3 (points_to_str)
        bit4 (points_to_mat)
        bit5 (is_extern) */
    
    /* check_num will be 1 if and only if : only one of bits is 1 , 
       1 otherwise it will be more than 1 */
    check_num =   (int)((res>>0)&1) + (int)((res>>2)&1) + (int)((res>>3)&1) 
                + (int)((res>>4)&1) + (int)((res>>5)&1);

    if (check_num > 1){
            printf ("Error: the same label [%s] is defined several times:\n", label_name);
            if (feature.points_to_cpu_cmd) printf("\t> as label of CPU command\n");
            if (feature.points_to_data) printf("\t> as label of .data directive\n");
            if (feature.points_to_str) printf("\t> as abel of .string directive\n");
            if (feature.points_to_mat) printf("\t> as label of .mat directive\n");
            if (feature.is_extrn) printf("\t> as external label \n");
            puts("");

            return ERR;
    }       
           
    /* now check that .extern and .entry labels are not both 1 :  */
    if(feature.is_extrn == 1 && feature.is_entry == 1 ){
        printf("Error: the same label [%s] is set both in .extern :\n\n", label_name);
        return ERR;
    }
    /* if bit 1 (is_cpu_cmd_param) is set, check_num must have value 1 
       i.e. one of bits 0,2,3,4,5 must be 1.
    */
    if (feature.is_cpu_cmd_param && check_num != 1){
        printf("Error: the label [%s] appears as command parameter \n\t"
               "but its value is not defined. It must be defined\n\t" 
               "as CPU command label or .data or .string \n\t"
               "or .mat label or external label.\n\n", label_name);
        return ERR;
    }

    return NO_ERR;
}

/*
    recursive function label_table_features_check

        checks feature validity of all items in labels table
        The check is performed "in order" (in alpabetical order of labels)

        return : number of errors
*/

int label_table_features_check(lable_item_ptr label_table){
    int left_result = INVALID_LABEL;
    int parent_result = INVALID_LABEL;
    int right_result = INVALID_LABEL;

    if (label_table == NULL)
    {
        return NO_ERR; /* stop condition */
    }
    else
    {
        left_result = label_table_features_check(label_table->left);
        parent_result = check_feature_validity(label_table->features,label_table->label_name);
        right_result = label_table_features_check(label_table->right);

        return left_result + parent_result + right_result;
    }
} /* label_table_features_check */



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
uint convert_feature_to_num(label_features feature)
{
    uint num = 0;
    if (feature.points_to_cpu_cmd == 1) /* set bit 0 */
        num += 1;
    if (feature.is_cpu_cmd_param == 1) /* set bit 1 */
        num += 2;
    if (feature.points_to_data == 1) /* set bit 2 */
        num += 4;
    if (feature.points_to_str == 1) /* set bit 3 */
        num += 8;
    if (feature.points_to_mat == 1) /* set bit 4 */
        num += 16;
    if (feature.is_extrn == 1) /* set bit 5 */
        num += 32;
    if (feature.is_entry == 1) /* set bit 6 */
        num += 64;

    return num;
} /* convert_feature_to_num */


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
void convert_num_to_feature(label_features * feature, int num)
{

    if ((num >> 0) & 1) /* check if bit 0 is 1 */
        feature->points_to_cpu_cmd = 1;
    if ((num >> 1) & 1) /* check if bit 1 is 1 */
        feature->is_cpu_cmd_param = 1;
    if ((num >> 2) & 1) /* check if bit 2 is 1 */
        feature->points_to_data = 1;
    if ((num >> 3) & 1) /* check if bit 3 is 1 */
        feature->points_to_str = 1;
    if ((num >> 4) & 1) /* check if bit 4 is 1 */
        feature->points_to_mat = 1;
    if ((num >> 5) & 1) /* check if bit 5 is 1 */
        feature->is_extrn = 1;
    if ((num >> 6) & 1) /* check if bit 6 is 1 */
        feature->is_entry = 1;

} /* convert_num_to_feature */


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
void reset_feature(label_features *feature)
{
    feature->points_to_cpu_cmd = 0;
    feature->is_cpu_cmd_param = 0;
    feature->points_to_data = 0;
    feature->points_to_str = 0; 
    feature->points_to_mat = 0; 
    feature->is_extrn = 0;
    feature->is_entry = 0;
}