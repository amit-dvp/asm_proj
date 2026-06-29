/* this file defines types for cmd_line_analyse.c translate unit*/
#if !defined(cmd_line_analyse_types_h)
#define cmd_line_analyse_types_h

/* #ifndef uint
typedef unsigned int uint;
#endif */

/* delimiters definition within command string */
#define COMMA_DLMTR  ','
#define SPACE_DLMTR  ' '
#define ENTER  '\n'
#define SKIP_DLMTR  '\0'
#define EOS '\0'        /* END OF STRING*/

enum str_fragment_analysis
{
    GOT_TXT_AND_DLMTR = 1,
    GOT_WHITES_AND_DELMTR,
    GOT_TXT_AND_EOS,
    GOT_WHITES_AND_EOS,
    GOT_EOS_AT_START,
    GOT_DLMTR_AT_START,
    GOT_SECOND_TXT_BLK
};


/************ following types are for asm 1st stage pass *******************/



typedef struct is_cmd_lbl_stat * is_cmd_lbl_stat_ptr ;

typedef struct is_cmd_lbl_stat
{   uint label_is_ok : 1;            /* label is valid */
    uint label_is_too_long : 1;      /* label lenth is too much */
    uint has_column_at_end : 1;      /* first char in label is not alphanumerical */
    uint no_column_at_end : 1;       /* no ':' at end of label (relevalnt if chk_colon is set)*/
    uint non_aphabet_start : 1;      /* first char in label is not alphanumerical */
    uint has_non_alphanum : 1;       /* has non alphanumeric char, between second char and ':' */  
    uint equals_macro_name : 1;      /* first char in label is not alphanumerical */
    uint equals_cmd_name : 1;        /* first char in label is not alphanumerical */
} is_cmd_lbl_stat;


typedef struct mat_param_status{
    uint maybe_matrix_call : 1;
    uint not_matrix_call : 1;
    uint matrix_call_valid : 1;
    uint reg1_name_err : 1;
    uint reg2_name_err : 1;
    uint label_not_exists : 1;
    uint label_is_too_long : 1;
    uint label_non_aphabet_start : 1;
    uint label_has_non_alphanum : 1;
    uint label_equals_macro_name : 1;
    uint label_equals_cmd_name : 1;
    uint invalid_label_sntx_err : 1;
    uint err_white_btwn_lbl_and_prnth : 1;
    uint paranth_problem : 1;
    uint matrix_def_syntax_err : 1;      
    uint delimeter_after_m_call : 1;     
    uint EOS_after_matrix_call : 1; 
    uint txt_between_mcall_and_dlmter : 1; 
    uint err_extra_text : 1;    
} mat_param_status;









#endif /* #if !defined(cmd_line_analyse_types_h) */