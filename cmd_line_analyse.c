#include "assembler.h"
/*

 Types of assembly lines:

    1. comment      : [white chars]';'[any chars]'\n'
    2. empty string : [white chars]'\n'
    3. macro        : [white chars]"mcro"(must ' ')[white chars]MACRO_NAME_MAX30_CHARS[white chars]'\n'
    4. macro finish : [white chars]"mcroend"[white chars]'\n'
    5. CPU command  :
        1. CPU instruction with 2 parameters:
            [white chars][label_max_30chars:(must ' ')][white chars]cpu_command(must ' ')\
            [white chars]PARAM1[white chars],[white chars]PARAM2[white chars]'\n'
        2. CPU instruction with 1 parameter:
            [white chars][label_max30_chars:(must ' ')][white chars]cpu_command(must ' ')\
            [white chars]PARAM1[white chars]'\n'
        3. CPU instruction with 0 paramters:
            [white chars][label_max30_chars:(must ' ')][white chars]cpu_command[white chars]'\n'
    6. .data directive:
            [white chars][label_max30_chars:(must ' ')][white chars].data(must ' ')[white chars]\
            NUM1[white chars],[white chars]NUM2[white chars],...,[white chars]NUM_N[white chars]'\n'
    6. .string directive:
            [white chars][label_max30_chars:(must ' ')][white chars].string(must ' ')[white chars]\
            "string"[white chars]'\n'
    7. .mat directive
            [white chars][label_max30_chars:(must ' ')][white chars].mat(must ' ')[white chars]\
            [[white chars]NUM[white chars]][[white chars]NUM[white chars]][white chars]NUM1[white chars],\
            [white chars]NUM2[white chars],[white chars]NUM3[white chars]'\n'
                .mat [2][2] 1, 2, 3, 4
            Note: if defined less numbers - they will be set to 0
                  if defined too many numbers - error will be reported. 

*/

/* Read line from file opened for read.
   Store input line in *s up to line_lim characters number limit (not including '\n' or EOF)
   or up to '\n' of EOF (not including '\n' and EOF) if received less characters than line_lim.

   Skips all remainder characters from line_lim to '\n' or EOF.

   Parameters:
      fd_in    : pointer to input file
      s        : string array to store the line
      line_lim : line size limit (not including '\n' of EOF)
   Return:
          1) length of string (not including '\n' and '\0')
          2) -1 (NORM_LENGTH_AND_EOF)    string has normal length and finished with EOF
          3) -2 (TOO_MANY_CHARS_AND_ENTR) string has too many characters and finished with '\n'
          4) -3 (TOO_MANY_CHARS_AND_EOF) string has too many characters and finished with EOF
*/
int readline(FILE * fd, char *s, int line_lim)
{
    int c = 'a';
    int index = 0;


    /* Maximum line length in input file is 80 characters (not including '\n') (p.25)  */    
    /* Note that char_num starts from 0 so it can reach 79 maximum */
    int char_num = 0; /* number of characters in line */
    
    for (char_num = 0; (c = fgetc(fd)) != '\n' && c != EOF; ++char_num)
        if (char_num < line_lim)
            s[index = char_num] = c;

    if (c == '\n' && char_num != 0)
    {
        s[++index] = '\0';
        if (char_num > line_lim-1){ /* 0..79 = end of string , 80 =  '\0' */
            return TOO_MANY_CHARS_AND_ENTR;
        }    
    } else if  (c == '\n' && index ==0){
        s[index] = '\0';
        return 0;
    }

    if (c == EOF && char_num != 0)
    {
        s[++index] = '\0';
        if (char_num > line_lim-1){ /* 0..79 = end of string , 80 = '\0' */
            return TOO_MANY_CHARS_AND_EOF;
        }    
        else
            return NORM_LENGTH_AND_EOF;

    }else if  (c == EOF && index ==0){
        s[index] = '\0';
        return -1;
    }
   
    return index-1;
}

/*
        5) CPU command
            optionally contains white chars at the begin of string
            after it optionally has label+':'+' '
            after it optionally contains white chars
            after it contains CPU_command_keyword+' '
        6) data directive
            optionally contains white chars at the begin of string
            after it optionally has label+':'+' '
            after it optionally contains white chars
            after it contains data_directive+' '

    The function returns:
        1 - if the line is comment
        2 - if the line is empty

        3 - if the line has valid "mcro" command start
        and returns pointer to ' ' after "mcro" command

        4 - if the line has valid CPU command start
            returns pointer to label name
            returns pointer to CPU command name
            returns pointer to ' ' after

        (int line_num)
*/

/* The function pre_asm_analyser :

    checks if the line is :
        1) comment:
            starts with ';'
        2) empty  :
            optionally contains white chars at the begin of string and
            and ends with '\n'
        3) macro command:
            optionally contains white chars at the begin of string
            and starts with "mcro" + ' '
        4) macro finish:
            optionally contains white chars at the begin of string
            and starts with "mcroend" and
            optionally contains white chars after "mcroend"


*/

/* The function gen_text_find:

    Finds continuous text from start of text fragment within string
    and up to given delimiter char.

    The function gets pointer to fragment start inside string.
    It is pointer to pointer to allow the function to increment its value
    in order to return pointer to next delimiter char if it exists.

    If delimiter does not exist, or if its value is '\0', the function returns the unchanged
    pointer to fragment start (in case when the search reach EOS(End Of String)
    or reach second txt block before delimiter of EOS).


    The coninuous text can start immediately or to be separated
    from the fragment start position with only white characters
    (' ' and '\t' , including fragment start itself).

    Note : if delimiter is white char (' ' or '\t'), it will be skipped and
            the search for continuous text will be continued.

    Only single text fragment allowed to appear between
    start position and delimiter.

    The retuns values match to the following situations:

        1 : (GOT_TXT_AND_DLMTR) means we found continuous TeXT, and after it
                DeLiMiTeR i.e. continuous text placed between fragment's start and
                delimiter. The text may be separated from fragment start and
                from delimiter with white chars:

                sdssds    ,         |   sdssds        ,         |           sdssds,
            ^             ^         |   ^             ^         |   ^             ^
          f. start        delimiter | f. start        delimiter | f. start        delimiter

        2 : (GOT_WHITES_AND_DELMTR) means we found only WHITe charS
            between fragment start and DeLiMiTeR (text does not appear):

                              ,
                ^             ^
              f. start        delimiter

        3 : (GOT_TXT_AND_EOS) means no delimeter found or delimiter equals EOS and we got
                continuous TeXT between fragment's start and End Of String (EOS is '\0').
                The text may be separated from fragment start and
                from end of string with white chars:

                    sdssds    '\0'    |  sdssds        '\0'   |          sdssds'\0'
                ^             ^       |  ^             ^      |  ^             ^
              f. start        EOS     | f. start       EOS    | f. start       EOS

        4 : (GOT_WHITES_AND_EOS) means that we got empty string. No delimiter found or it equals
                EOS. Only WHiTe charS appear from
                fragment start and up to End Of String without
                continuous text.

                              '\0'
                ^             ^
             f. start        EOS

        5 : (GOT_EOS_AT_START) End Of String appears immediately AT fragment START:

                '\0'
                ^
              EOS at f. start

        6 : (GOT_DLMTR_AT_START) DeLiMiTeR is found immediately at fragment's START:
            Note : if delimiter is white char, it will be skipped and
            the search for continuous text will be continued.

                ,
                ^
              delimiter at fragment start

        7 : (GOT_SECOND_TXT_BLK) SECOND TeXT block separated with whith white char
            is found starting from fragment start and before delimeter or EOS:

                          '\0'      |                 '\0'      |                  '\0'
                sd sds    ,         |   sd   sds      ,         |   sd         sds,
            ^             ^         |   ^             ^         |   ^             ^
          f. start        delimiter | f. start        delimiter | f. start        delimiter
                          or EOS    |                 or EOS    |                 or EOS


*/
int gen_text_find(char **str, char dlmtr, char *txt){ 

int i;

char *s = *str;     /* assign to pointer s --> pointer to string str*/

if (*s == EOS)      /* found EOS at fragment start */
    return GOT_EOS_AT_START;

if (*s == dlmtr && (!isspace(dlmtr)) )   /* found NON EOS and non white delimiter at fragment start */
    return GOT_DLMTR_AT_START;

/************ skip optional white chars between first delimiter and word *********/
while (isspace(*s) && (*s != EOS))
    s++;

if (*s == EOS)        /* empty string with only while chars */
    return GOT_WHITES_AND_EOS;

if (*s == dlmtr)
{             /* only white chars before delimiter  */
    *str = s; /* update fragment pointer*/
    return GOT_WHITES_AND_DELMTR;
}

/* store the txt word */
for (i = 0; !isspace(*s) && (*s != dlmtr) && (*s != EOS); s++, i++)
    txt[i] = *s;

/*********** after the word stored : ******************/
txt[i] = EOS;         /* close the text string with '\0' */

if (*s == EOS)        /* found EOS after end of word */
    return GOT_TXT_AND_EOS;

if (*s == dlmtr)
{             /* found non EOS delimiter after end of word */
    *str = s; /* update fragment pointer*/
    return GOT_TXT_AND_DLMTR;
}

/********* skip white characters after the word : **********/
while (isspace(*s) && (*s != EOS))
    s++;

if (*s == EOS)        /* found EOS find after skip whites */
    return GOT_TXT_AND_EOS;

if (*s == dlmtr){     /* found non EOS delimiter after end of word */
    *str = s;         /* update fragment pointer*/
    return GOT_TXT_AND_DLMTR;
}


if (*s != dlmtr)            /* another word started  */
    return GOT_SECOND_TXT_BLK;

return 0;
}




/************ following functions are for asm 1st stage pass *******************/

/*
  Function is_label :

    Analyses if the given string is label before the command :
        1. checks if it ends with ':' (if chk_semicolon is set to 1)
        2. checks label's length < 30
        3. checks if the label starts form alphabetical char
        4. checks if the label has non alphanum chars or '_' between first char and ':'
           (if chk_semicolon is set to 1) or between first char and end of string
           (if chk_semicolon is not set to 1).
        5. if it is different from all keywords and all macro names.

    Parameters:
        str : pointer to string
        chk_colon :         if this parameter equals 1 will check that label ends with ':'
                            and continue check without ':' character at end.
                            Otherwise will not check for ':' and will check the whole word.

    Return (status):
            label_is_ok       : label is valid
            label_is_too_long : label name has more than 30 characters
            has_column_at_end  : ends with ":" character,
            no_column_at_end  : no ':' char at end of label (relevalnt if chk_colon is set)
            non_aphabet_start : first symbol is not alphabetical char
            has_non_alphanum  : has non alphanumerical character after first character
                                and before the last character.
            equals_cmd_name   : label name is same as command name
            equals_macro_name : label name is same as macro name
*/
void is_label(char *str, int chk_colon,is_cmd_lbl_stat_ptr status)
{
    int res;    /* result of validity checks */
    int str_len = 0 ; /* string length */
    char maybe_label[MAX_LINE_SIZE] = {'\0'}; /* array to store label text or command t */

    /* init status */
    status->label_is_ok = 0; 
    status->label_is_too_long = 0;
    status->has_column_at_end = 0;
    status-> no_column_at_end = 0;
    status->non_aphabet_start = 0;
    status->has_non_alphanum = 0;
    status->equals_cmd_name = 0;
    status->equals_macro_name = 0;

    debug(printf("run function is_cmd_label \n"));
    /* copy input string to maybe_label string for analyze*/
    strncpy(maybe_label,str,MAX_LINE_SIZE);
    /* now check if the last symbol in label is ':'*/
    str_len = strlen(maybe_label);
    /* if empty string */
    if(str_len == 0)
    {
        status->label_is_ok = 0;
        return;
    }
    /* check ':' does not appear at the end and it is no macro or keyword */
    if (str_len>0 && maybe_label[str_len - 1] != ':' && chk_colon == 1)
    {
            status->no_column_at_end = 1;
            status->label_is_ok = 0;
            return;
    }

    /* check ':' appear at the end */
    if (maybe_label[str_len - 1] == ':' && chk_colon == 1)
        status->has_column_at_end = 1;


    /* if chk_semicolon set to 1: set EOS in place of ':' character at the end of string
    to check remaining string  */
    if (chk_colon == 1)
        maybe_label[str_len - 1] = EOS;
    
    /* check string length*/
    if (str_len > MAX_MACRO_AND_LABEL_LENGTH)
    {
        status->label_is_too_long = 1;
        return;
    }

    /* check label's syntax */
    res = check_name(maybe_label);
    /* in case the first chart of label is not alphabetic */
    if (res == NOT_ALPHABET)
    {
        status->non_aphabet_start = 1;
        return;
    }
    /* in case other chars are not alphanumerical*/
    if (res == NOT_ALPHANUM)
    {
        status->has_non_alphanum = 1;
        return;
    }
    /* check that label's name does not appear in macro's table */
    if (is_macro_in_table(global_macro_table_ptr, maybe_label))
    {
        status->equals_macro_name = 1;
        return;
    }

    /* check that label's name does not appear in keywords table */
    if (is_keyword_in_table(global_keyword_table_ptr, maybe_label))
    {
        status->equals_cmd_name = 1;
        return;
    }

    status->label_is_ok = 1;

} /* function is_cmd_label */

/*
  Function is_cmd_name :

    Analyses if the given string is command name:
        1. checks if it appears as command (cpu_instruction or data directive) in keywords table
        2. if it is command returns command's features

    Parameters:
        str : pointer to string

    Return :
        feature - bit-fields struct with command features.
        0 - (NOT_INSTR) not instruction
        1 - (CPU_INSTR) is cpu istruction
        2 - (DATA_DIRECTIVE) is data directive
*/
int is_cmd_name(char *str, keywords_item_ptr * keyword_item)
{
    keywords_item_ptr keyword_item_p = NULL;

    /* init feature valriable */
    * keyword_item = NULL;

    /* search for kayword name in global keywords table */
    keyword_item_p = keywords_table_search(global_keyword_table_ptr, str);
    /* if pointer is NULL it is not command */
    if (keyword_item_p == NULL){
        *keyword_item = NULL;
        return NOT_INSTR;
    }
    /* check if input string appears in keywords table */
    if (keyword_item_p->feature.is_cpu_instruction == 1)
    {
        *keyword_item = keyword_item_p;
        return CPU_INSTR;
    }
    if (keyword_item_p->feature.is_data_directive == 1)
    {
        *keyword_item = keyword_item_p;
        return DATA_DIRECTIVE;
    }

    return NOT_INSTR;
    
} /* function is_cmd_name */

/* function is_reg_name
      Analyses string and checks that it is valid register name

      Return :
         -1 (NO_REG_NAME)    : if it is not register name
          0...7 : if it is r0 ... r7
*/
int is_reg_name (char * str){

    if (strncmp(str,"r0",MAX_MACRO_AND_LABEL_LENGTH) == 0)
        return R0_NAME;
    if (strncmp(str,"r1",MAX_MACRO_AND_LABEL_LENGTH) == 0)
        return R1_NAME;
    if (strncmp(str,"r2",MAX_MACRO_AND_LABEL_LENGTH) == 0)
        return R2_NAME;
    if (strncmp(str, "r3",MAX_MACRO_AND_LABEL_LENGTH) == 0)
        return R3_NAME;
    if (strncmp(str, "r4",MAX_MACRO_AND_LABEL_LENGTH) == 0)
        return R4_NAME;
    if (strncmp(str, "r5",MAX_MACRO_AND_LABEL_LENGTH) == 0)
        return R5_NAME;
    if (strncmp(str, "r6",MAX_MACRO_AND_LABEL_LENGTH) == 0)
        return R6_NAME;
    if (strncmp(str, "r7",MAX_MACRO_AND_LABEL_LENGTH) == 0)
        return R7_NAME;

    return NO_REG_NAME;
} /* function is_reg_name */

/*
    Function is_param_number
        check the string if it is valid number after '#' symbol.
        This number after must be in range -128...127 to match 8 bit
        signed number field in CPU memory (it must fit 8 bits 2..9).

    Return :
        num , return number if its value is valid;
        NUM_PARAM_LIM_FAULT(-3)    : starts with '#' and the number is integer but not in limits
        NUM_PARAM_SYNTAX_FAULT(-2) : starts with '#' but has syntax problem in number definition
        NOT_NUM_PARAM(-1)          : does not start with '#'
        PARAM_NUM_IS_VALID (0)     : starts with '#' and the parameter is valid

*/
int is_param_number(char *str, int * num)
{
    int res; /* result of check */
    char maybe_num[MAX_LINE_SIZE] = {'\0'}; /* array to store num text */
    char * str_p = NULL; /* pointer to next */
    *num = 0;
    if (*str == NUM_PARAM_START_CHAR)
    {   /* str_p can be char or '\0' */
        str_p = str + 1;
        /* if '#' does not appear alone (characters after '#'): */
        if(*str_p!=EOS){
            /* copy string starting from second char to meybe_num variable */
            strncpy(maybe_num , str_p,MAX_LINE_SIZE-2); 
            res = str2int(maybe_num,num);
            /* check number limits */
            if (res == VALID_NUM){
                if (*num >= MIN_INT_8_BIT_NUM && *num <= MAX_INT_8_BIT_NUM)
                    return PARAM_NUM_IS_VALID;
                if (*num < MIN_INT_8_BIT_NUM || *num > MAX_INT_8_BIT_NUM)
                    return NUM_PARAM_LIM_FAULT;
            }        
        }
        
        return NUM_PARAM_SYNTAX_FAULT;

    } else
        return NOT_NUM_PARAM;
}



/* convert num string to decimal number
    The number must be integer.
    Can include + or - at the start.
    Return :
        num : integer number value (in case of valid number)
        result :
            NOT_NUM        = 0 in case of non valid number (syntax problem)
            VALID_NUM      = 1 in case of valid number
    Note: does not check the limits of number !
*/
int str2int(char *str, int * num){
    int i=0;
    int sign = 1;
    *num = 0;
    /* check and store sign value: */
    if(*str == '-')
    {
        sign = -1;
        str++; 
    }
    else if (*str == '+')
    {
        str++;
    }
    /* composite number if it has digits */
    for (i=strlen(str);isdigit(*str)&&i;i--,str++){
        *num = (*num)*10 + (*str - '0');
    } 
    /* in case of non digits in string */
    if (!isdigit(*str) && i )
        return NOT_NUM;

    *num = (*num)*sign;
    return VALID_NUM;
}

/* Function is_matrix_param

        checks  if it is matrix call parameter in CPU command
        searches the text from current pointer's position and up to delimiter ','

        Matrix call parameter example:
                 label[ r1  ][r2      ]   ,
             ^                            ^
         pointer pos.                  delimiter

           1. The label name must be adjucent to '[' character.
           2. The label name maust be valid label name.
           3. White chars allowed to appear between text start (current text pointer position)
              and matrix definition start and between matrix definition finish and delimiter.
           4. White chars allowed between '[' and register name and
              between register name and ']'.
           5. register name must be valid i.e. r0 ... r7 only alowed.
           6. ']' and '['  must be adjucent.

        In case of first CPU command parameter the delimiter will be ','
        and the text pointer will be set to it at the finish of function run.
        In case of second CPU command param

        The function will update the pointer to point the delimiter after the matrix definition.
        In case

        Return :
                maybe_matrix_call               : the text contains at least '[' or ']' character;
                not_matrix_call                 : the text is not matrix call (no '[' or ']' chars)
                matrix_call_valid               : valid label found, valid register names and
                                                    valid syntax with ',' or '\0' at the end.
                reg1_name_err                   : reg1 name is not r0...r7
                reg2_name_err                   : reg2 name is not r0...r7
                label_not_exists                : label does not exist
                label_is_too_long               : label is too long 
                label_non_aphabet_start         : label does not start with alphabet char;
                label_has_non_alphanum          : label has not alphanum char after 1st char;
                label_equals_macro_name         : label equals matrix name;
                label_equals_cmd_name           : label equals command name;
                invalid_label_sntx_err          : label hassyntax error;
                err_white_btwn_lbl_and_prnth    : label has white char between it and '[';
                paranth_problem                 : must be no whites between ']' and '[';
                matrix_def_syntax_err           : matrix definition has syntax error;
                delimeter_after_m_call          : found delimetter after matrix call;
                EOS_after_matrix_call           : found EOS after matrix call;
                txt_between_mcall_and_dlmter    : extra text between matrix call and delimenter;
                err_extra_text                  : extra text in matrix call

*/

void is_matrix_param(char **str, char ** label_name, int * reg1, int * reg2, mat_param_status * status){
    is_cmd_lbl_stat label_status;
    
    int res, res1;
    char txt[MAX_LINE_SIZE] = {'\0'}; /* array to store num text */
    char * ptr ; /* internal temp pointer to string */
    
    /* reset status  : */
    reset_matrix_status(status);

    /* set internal pointer to be same as string pointer */
    ptr = *str;
    /* check if '[' symbol appears anywhere in string before ',' symbol or
       before '\0' symbol */
    res = gen_text_find(&ptr, ',' , txt);
    

    /* check if txt contain '[' ot ']' symbol */
    ptr = txt;
    while (*ptr != EOS && *ptr != COMMA_DLMTR && *ptr != '[' && *ptr != ']')
    { /*increment while not EOS or [ or ] */
        ptr++;
    }
    /* not_matrix_call : did not find '[' or ']' up to delimiter or '\0' */
    if (*ptr==EOS || *ptr==COMMA_DLMTR){
        status->not_matrix_call = 1;
        return;
    }    
    /* if exist '[' ot ']' - may be matrix call */
    if (*ptr == '[' || *ptr == ']')
        status->maybe_matrix_call = 1;
    
    /* set internal pointer again to be same as string pointer */
    ptr = * str;
    
    /* seek for valid label and '[' close to it */
    res = gen_text_find(&ptr, '[', txt);
    /* may be '[' does not exist or there is white char between 
       the label and '['
    */
    if (res != GOT_TXT_AND_DLMTR ){
        /* no label */
        if (res == GOT_DLMTR_AT_START ){
            status->label_not_exists = 1;
            status->matrix_def_syntax_err = 1;
            return;
        }    
        /* not '[' after label */
        if (res == GOT_TXT_AND_EOS){
            status-> paranth_problem = 1;
            status-> matrix_def_syntax_err = 1;
            return;
        }    
        /* also will appear in case of ' ' between label and '[' */
        if (res == GOT_SECOND_TXT_BLK){
            status->invalid_label_sntx_err = 1;
            status->err_extra_text = 1;
            status->matrix_def_syntax_err = 1;
            return;
        }
        /* anyway there is an error in matrix param definition */
        status->matrix_def_syntax_err = 1;
        return;
    } /* now we know that res == GOT_TXT_AND_DLMTR */

    /* now the pointer ptr points to delimiter.
       check the char before it (delimiter) , to verify if there exists white char
       between the label and delimeter    
    */
    ptr--;
    if ( *ptr==SPACE_DLMTR || *ptr == '\t'){
        status->err_white_btwn_lbl_and_prnth =1;
        status->matrix_def_syntax_err = 1;
        return;
    }
    
    /* now check that we find valid label text */
    is_label(txt, NO_COLON_CHK, &label_status);
    
    if (!label_status.label_is_ok){
        if (label_status.label_is_too_long){
            status->label_is_too_long = 1;
            status->matrix_def_syntax_err = 1;
            return;
        }    
        if (label_status.has_non_alphanum){
            status->label_has_non_alphanum = 1;
            status->matrix_def_syntax_err = 1;
            return;
        }    


        if(label_status.non_aphabet_start){
            status->label_non_aphabet_start = 1;
            status->matrix_def_syntax_err = 1;
            return;
        }    

        if(label_status.equals_cmd_name){
            status->label_equals_cmd_name = 1;
            status->matrix_def_syntax_err = 1;
            return;
        }    


        if(label_status.equals_macro_name){
            status->label_equals_macro_name = 1;
            status->matrix_def_syntax_err = 1;
            return;
        }
    }

    /* now we know that txt is valid label name.
        copy label name to heap */
    *label_name = heap_string_duplicate_safe(txt,MAX_MACRO_AND_LABEL_LENGTH+2);

    /* increment ptr two symbols forward
        to set pointer one char after '[' char */
    ptr += 2;

    /* check that pointer's value is not EOS */
    if (*ptr == EOS)
    {
        status->matrix_def_syntax_err = 1;
        return;
    }

    /* now search for reg1 value   r1   ][ r2]
                                  ^  
    */
    res = gen_text_find(&ptr, ']', txt);

    if (res != GOT_TXT_AND_DLMTR){
        /* no reg name */    
        if (res == GOT_WHITES_AND_DELMTR){
            status->reg1_name_err = 1;
            status->matrix_def_syntax_err = 1;
            return;
        }
        /* no ']' char : */
        if (res == GOT_TXT_AND_EOS){
            status->paranth_problem = 1;
            status->matrix_def_syntax_err = 1;
            return;
        }

        if (res == GOT_SECOND_TXT_BLK)
        {
            status->err_extra_text = 1;
            status->matrix_def_syntax_err = 1;
            return;
        }

        /* any other error */
            status->matrix_def_syntax_err = 1;
            return;
        }
        
        /* now we suppose that label and reg1 text and ']' after it exists */
        /* check reg1 text :*/

        res1 = is_reg_name(txt);

        if (res1 == NO_REG_NAME){
            status->reg1_name_err = 1;
            status->matrix_def_syntax_err = 1;
            return;
        }

        /* return reg1 register name */
        *reg1=res1;

        /* now the pointer is on ']' char */

        /* increment pointer and check that '[' char is adjucent to it */
        ptr ++;
        if (*ptr != '['){
            status->paranth_problem = 1;
            status->matrix_def_syntax_err = 1;
            return;
        }
        
        /* increment pointer after '[' */
        ptr ++;
        /* check if EOS is not received */
        if (*ptr == EOS ){
            status->paranth_problem = 1;
            status->matrix_def_syntax_err = 1;
            return;
        }

        /* now search second register text */
        res = gen_text_find(&ptr, ']', txt);

        /* if problem is found */
        if (res != GOT_TXT_AND_DLMTR)
        {
            /* no reg2 text */
            if (res == GOT_WHITES_AND_DELMTR){
                status->reg2_name_err = 1;
                status->matrix_def_syntax_err = 1;
                return;
            }

            if (res == GOT_TXT_AND_EOS ){
                status->paranth_problem = 1;
                status->matrix_def_syntax_err = 1;
                return;
            }

            if (res == GOT_SECOND_TXT_BLK){
                status->err_extra_text = 1;
                status->matrix_def_syntax_err = 1;
                return;
            }

            /* any other error */
            status->matrix_def_syntax_err = 1;
            return;
        }

        /* at this moment we found txt and ']' */
        /* check txt is valid register name    */

        res1 = is_reg_name(txt);

        if (res1 == NO_REG_NAME)
        {
            status->reg2_name_err = 1;
            status->matrix_def_syntax_err = 1;
            return;
        }

        /* return reg1 register name */
        *reg2 = res1; 

        /* increment pointer after second ']' */
        ptr ++;
        
        if (*ptr == EOS){
            status->EOS_after_matrix_call = 1;
            status->matrix_call_valid = 1;
            return;
        }
        
        
        /* now check what appears after ']' 
           if it is delimiter ',' or EOS        
        */
        res = gen_text_find(&ptr, ',', txt);

        /* find ',' delimiter */
        if (res == GOT_WHITES_AND_DELMTR || res == GOT_DLMTR_AT_START){
            status -> delimeter_after_m_call = 1;    /* i.e. ',' after matrix call */
            /*increment str to place of delimiter ',' */ 
            *str = ptr;
            status->matrix_call_valid = 1;
            return;
        }

        if (res == GOT_WHITES_AND_EOS || res == GOT_EOS_AT_START){
            status->EOS_after_matrix_call = 1;
            status->matrix_call_valid = 1;
            return;
        }
 
        /* check if there no additional text before delimeter or EOS */
        if ((res == GOT_TXT_AND_DLMTR) || (res == GOT_TXT_AND_EOS) || (res == GOT_SECOND_TXT_BLK)){
            status -> txt_between_mcall_and_dlmter = 1;
            status-> err_extra_text = 1;
            status -> matrix_def_syntax_err = 1;
            return;
        }

        status->matrix_call_valid = 1;

        return;
}

/*
   Reset_matrix_status
        parameter
            status: matrix status object

*/

void reset_matrix_status(mat_param_status * status){

    status->maybe_matrix_call = 0;
    status->not_matrix_call = 0;
    status->matrix_call_valid = 0;
    status->reg1_name_err = 0;
    status->reg2_name_err = 0;
    status->label_not_exists = 0;
    status->label_is_too_long = 0;
    status->label_non_aphabet_start = 0;
    status->label_has_non_alphanum = 0;
    status->label_equals_macro_name = 0;
    status->label_equals_cmd_name = 0;
    status->invalid_label_sntx_err = 0;
    status->err_white_btwn_lbl_and_prnth = 0;
    status->paranth_problem = 0;
    status->matrix_def_syntax_err = 0;
    status->delimeter_after_m_call = 0;
    status->EOS_after_matrix_call = 0;
    status->txt_between_mcall_and_dlmter = 0;
    status->err_extra_text = 0;

}
