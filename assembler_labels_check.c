#include "assembler.h"
#include "assembler_globals.h"

int main(void)
{   uint res, res1;
    int res2 = 0;
    label_features label_feture, label_feature1;
    lable_item_ptr labels_table_ptr = NULL;


    reset_feature(&label_feture);
    label_feture.is_cpu_cmd_param = 1;
    label_feture.is_extrn = 0;
    label_feture.points_to_cpu_cmd = 1;
    label_feture.points_to_data = 0;
    res = convert_feature_to_num(label_feture);
    reset_feature(&label_feature1);
    convert_num_to_feature(&label_feature1,res);

    res1 = check_feature_validity(label_feture, "dudu");

    labels_table_ptr = add_labels_tree(labels_table_ptr, "bahmas");
    labels_table_ptr = add_labels_tree(labels_table_ptr, "shizaf");
    labels_table_ptr = add_labels_tree(labels_table_ptr, "kayak");
    labels_table_ptr = add_labels_tree(labels_table_ptr, "apch-hi");

    res2 = add_label_feature(labels_table_ptr, "shizaf", label_feture, 25);
    reset_feature(&label_feature1);
    label_feature1.points_to_mat = 1;
    res2 = add_label_feature(labels_table_ptr, "shizaf", label_feature1, 25);
    res2 = add_label_feature(labels_table_ptr, "bardak", label_feature1, TO_BE_DEFINED);
    reset_feature(&label_feature1);
    label_feature1.is_entry = 1;
    label_feature1.is_extrn = 1;
    label_feature1.points_to_cpu_cmd = 1;
    res2 = add_label_feature(labels_table_ptr, "bardak", label_feature1, 25);
    res2 = add_label_feature(labels_table_ptr, "bardak", label_feature1, 25);

    label_table_print(labels_table_ptr);

    free_label_binary_search_tree(labels_table_ptr);

    return 0;
}





