/* assembly_lang_reqs will define assembly language requirements */

#if !defined(assembly_requirements_h)
#define assembly_requirements_h

/* #ifndef uint
typedef unsigned int uint;
#endif */

/* every assembly keyword (cpu instructions, data directives and register names ) 
   will be assotiated with its features definition struct */
typedef struct keyword_features{
   
    /* 1-st 4 bits group*/
    uint is_cpu_instruction : 1; /* the bit is set if it is CPU instruction */
    uint is_data_directive : 1;
    uint is_register_name : 1;
    uint is_reserved_word : 1; /* name of Data diective without "." */
    /* 2-nd 4 bits group */
    uint min_num_of_operands : 4; /* min number of operands */
    /* 3-rd 4 bits group */
    uint max_num_of_operands : 4; /* max number of operands (15 means any number) */
    /* 4-th 4 bits group */
    uint opcode : 4; /* opcode in case of cpu_instruction */
    /* 5-th 4 bits group */
    uint src_addr_mthd_0 : 1;      /* sourse addressing method 0 (immediate   ) can be used */
    uint src_addr_mthd_1 : 1;      /* sourse addressing method 1 (direct      ) can be used */
    uint src_addr_mthd_2 : 1;      /* sourse addressing method 2 (matrix acess) can be used */
    uint src_addr_mthd_3 : 1;      /* sourse addressing method 3 (register    ) can be used */
    /* 6-th 4 bits group */
    uint dest_addr_mthd_0 : 1;     /* dest. addressing method 0 (immediate   ) can be used */
    uint dest_addr_mthd_1 : 1;     /* dest. addressing method 1 (direct      ) can be used */
    uint dest_addr_mthd_2 : 1;     /* dest. addressing method 2 (matrix acess) can be used */
    uint dest_addr_mthd_3 : 1;     /* dest. addressing method 3 (register    ) can be used */

} keyword_features;

#define is_cpu_instruction_mask   0x00000001u
#define is_data_directive_mask    0x00000002u 
#define is_register_name_mask     0x00000004u
#define is_reserved_word_mask     0x00000008u
#define min_num_of_operands_mask  0x000000F0u
#define max_num_of_operands_mask  0x00000F00u
#define opcode_mask               0x0000F000u
#define src_addr_mthd_0_mask      0x00010000u
#define src_addr_mthd_1_mask      0x00020000u
#define src_addr_mthd_2_mask      0x00040000u
#define src_addr_mthd_3_mask      0x00080000u
#define dest_addr_mthd_0_mask     0x00100000u
#define dest_addr_mthd_1_mask     0x00200000u
#define dest_addr_mthd_2_mask     0x00400000u
#define dest_addr_mthd_3_mask     0x00800000u

/* each keyword will be bundeled with its keyword requirements */
typedef struct k_item {
    char * keyword;
    uint k_item_feature;
} k_item;

/*
    keywords table will be stored firstly as internal array
    (temporary automatic variable) and after it
    dynamic binary search tree from it to allow
    withdraw keywords sorted in alphabetical order.

    Here is definition of keywords array:
*/

typedef k_item keywords_array[];

/* pointer to keywords table */
typedef struct keywords_item *keywords_item_ptr;
typedef struct keyword_features *keyword_features_ptr;

    /* following struct object will be stored in heap */
typedef struct keywords_item
{
    char *keyword;
    keyword_features feature;
    keywords_item_ptr left;
    keywords_item_ptr right;
} keywords_item;

#endif /* #if !defined(assembly_requirements_h) */

