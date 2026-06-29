#if !defined(mngr_labels_tbl_types_h)

#define mngr_labels_tbl_types_h

/* labels table is data structure used for first and second assembly pass

    1. labels table : Binary Search Tree
           labels table will be binary search tree with label name key.
           BST is arranged according to alphabetic order. 
           Each element in BST is struct.

*/

typedef struct label_features
{

    uint points_to_cpu_cmd : 1; /* label points to CPU instr. */
    uint is_cpu_cmd_param : 1;  /* appears as CPU command parameter */
    uint points_to_data : 1;    /* label points to .data */
    uint points_to_str : 1;     /* label points to .string */
    uint points_to_mat : 1;     /* label points to .mat    */
    uint is_extrn : 1;          /* defined in outer file */
    uint is_entry : 1;          /* can be used by other files */

} label_features;

typedef struct label_item * lable_item_ptr;

    typedef struct label_item
{

    char * label_name;
    int label_address;
    label_features features;
    lable_item_ptr left;
    lable_item_ptr right;

} label_item;

#endif


/*

   function  convert_feture_to_num

        converts features struct to unsigned int number;

            points_to_cpu_cmd : bit 0
            is_cpu_cmd_param  : bit 1
            points_to_data    : bit 2
            points_to_str     : bit 3
            points_to_mat     : bit 4
            is_extrn          : bit 5
            is_entry          : bit 6

*/


uint convert_feature_to_num(label_features feature);


