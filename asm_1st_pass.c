#include "assembler.h"


/* At first pass of assembler the labels table will built
   and will prepared information for instruction memory and
   data memory fill (not including labels information that
   will be completed in second assembly stage).

   The program lines will be analysed and stored in
   cpu data linked list , if it is cpu command, or in
   cpu data linked list if it is data directive.

   Missed labels information will be signed to be
   accomplished in the 2nd assembly stage.

   In case of any error in the code line, its analysis will be stopped,
   and the error will be reported.

   If all lines will be valid, and no errors will be found 
   (including total memory overflow, etc.) files *.ent and *.ext
   will be generated.  

*/

/*  Function : asm_1st_pass
         First assembler pass, analyses .am file and prepares data for
         second assembler pass.

    Parameters:
         no implicit parameters.
         but at finish of asm_1st_pass
         following data structures are prepared :
            1.  data_blk_item : linked list to store information of data directives.
                This linked list is arranged in oeder of data directives.
                We access every element in this linked list through 2 global pointers :
                            global_d_head
                            global_d_tail
            2.  i_item :  linked list to store information of CPU instructions .
                This linked list is arranged according to order of CPU commands.
                We acceess every element in this linked list through 2 global pointers :
                            global_i_head
                            global_i_tail
            3.  label_item : binary search tree of labels.
                This tree is arranged according to labels names keys.
                We access every element in this tree through following global pointer :
                            global_labels_table_ptr
    The above data structures will be used as input to second assembly pass.

    Parameters (both used to return data from function):
            cpu_data_mem_size     :   CPU data memory size  
            cpu_instr_mem_size    :   CPU instr. memory size 

    return :
            returns errors number.        
*/
int asm_1st_pass(char * am_fname, int *cpu_instr_mem_size, int *cpu_data_mem_size)
{

    char *ptr = NULL;                        /* internal temp pointer to string */
    int res;                          /* temp result variable */
    int err_cnt = 0;         /* counter of errors */
    int readline_res;     /* result of read line function */
    char str[MAX_LINE_SIZE + 5] = {'\0'}; /* temp. array to store line */
    char * label_name = NULL; /* pointer to label name */
    is_cmd_lbl_stat label_status;  /* label status */
    int line_num=0;    /* counter of lines num */
    int instr_cnt = INSTR_CNT_INIT_VAL; /* instruction words counter  */
    int data_cnt = 0;  /* data words counter */
    int cpu_cmd_params_res ; /* result of analyze cpu command */
    int data_proc_res ;      /* result of data process */
    int string_proc_res ;    /* result of string process */
    int mat_proc_res ;       /* result of matrix process */
    int cmd_label_status;
    int extern_cmd_res = 0 ; /* result of check .extern command */
    int cpu_cmd_size = 0 ; /* size of cpu command in words , including command word and params words*/
    int add_lbl_res;     /* result of label addition */
    int entry_cmd_res ;  /* result od .entry directive analysis */
    int no_entry_no_extern_flag = 0; /* flag that signs command is not .enrtry and not .extern */
    int lbl_err_cnt = 0; /* labels errors counter */
    FILE * fd_as = NULL;  /* assembler file descriptor  */
    int fres = 0; /* result of fclose */ 

    keywords_item_ptr cmd_reqs = NULL;
    i_item_ptr i_item = NULL;         /* pointer to instruction item in linked list of instructions */
    data_blk_item_ptr d_item = NULL;  /* pointer to data item in linked list of data*/
    label_features label_feature;   /* label feature object */

    int prog_size = 0; /* program size */
    int cpu_instr_last_addr = 0; /* cpu instruction last addr*/
    int data_cnt_start_addr = 0; /* initial value of data cnt address */
    int prog_last_addr = 0;           /* last program address */

    fd_as = fopen(am_fname, "r");
    if(!fd_as){
        printf("Error: can not open %s file in read mode.\n", am_fname);
        printf("Exitting...\n\n");
        release_all_dynamic_memory_and_files();
        exit(1);
    }


    printf("\n---------------------------------------\n");
    printf(" Assembler:\n\t1-st stage is running.\n");
    printf("\tScan for errors in <%s> file.\n", am_fname);
    printf("---------------------------------------\n");

    do {

        i_item = NULL; /* init i_item, the update will be done if we get cpu command */
        d_item = NULL; /* init d_item, the update will be done if we get cpu data    */


        /* init label status */
        init_label_status(&label_status);

        /************     read new line    ***************************************/
        /* read input file line */
        readline_res = readline(fd_as, str, MAX_LINE_SIZE);
        line_num++;
        debug(printf("process line number %d\n", line_num));

        /******** check if the line starts with label ******/
        ptr = str;
        cmd_label_status = res = check_cmd_label(&ptr, line_num, &label_name);

        if(cmd_label_status == FILE_FINISHED)
            break; /* received last string in *.am file */

        if (res == ERR_ONLY_LABEL_FOUND || res == INVALID_LABEL){
            err_cnt++;
            continue; /* got to next line , dont analyse command */
        }    

        /********   check the command after the label   ****************************/
        /* if the first word is valid CMD return back ptr value
           if not , continue with current value of ptr    */
        if (res == VALID_CMD)
            ptr = str;
        
        /* we expect now to meet command */
        cmd_reqs = analyze_cmd(&ptr, line_num, instr_cnt, data_cnt, &i_item, &d_item);

        /* in case of non valid command */
        if (cmd_reqs == NULL){
            
            err_cnt++;
            continue; /* got to next line , dont analyse command */
        }
        /* if valid command name is received , add label data to i_item or d_item*/
        /* set no .entry and no .extern flag */
        no_entry_no_extern_flag = strncmp(cmd_reqs->keyword, ".extern",MAX_MACRO_AND_LABEL_LENGTH) && strncmp(cmd_reqs->keyword, ".entry",MAX_MACRO_AND_LABEL_LENGTH);

        if (cmd_reqs && i_item && cmd_label_status == 1 && label_name) /* if valid command received and it is cpu instruction */
            i_item->label_name = label_name;
        if (cmd_reqs && i_item && cmd_label_status == 0 && label_name) /* if valid command received and it is cpu instruction */
            i_item->label_name = NULL;

        if (cmd_reqs && d_item && cmd_label_status == 1 && no_entry_no_extern_flag && label_name)
            d_item->label_name = label_name;
        if (cmd_reqs && d_item && cmd_label_status == 0 && no_entry_no_extern_flag)
            d_item->label_name = NULL;

        /***********  analyse parameters vs. requirements **************/
        if(cmd_reqs->feature.is_cpu_instruction)
        {
            /************ analyze cpu instruction ***************/
            cpu_cmd_params_res = analyze_cpu_instr_params(str , & ptr, line_num, cmd_reqs, &i_item);
            if (cpu_cmd_params_res == CMD_VALID){
                if(cmd_label_status == VALID_LABEL)
                    i_item->status.cmd_label_found = 1;
                i_item->is_valid = 1;
                i_item->status.opcode = cmd_reqs->feature.opcode;
                /* calculate size of cpu command in words*/
                cpu_cmd_size = i_item_size_calc(i_item);
                i_item->size_in_wrds = cpu_cmd_size;
                
                /* add label only in case if command has label */
                if (cmd_label_status == VALID_LABEL)
                {
                    /* add cpu command label to labels table*/
                    reset_feature(&label_feature);
                    label_feature.points_to_cpu_cmd = 1;
                    add_lbl_res = add_label_feature(&global_labels_table_ptr, label_name, 
                                                label_feature, i_item->start_addr);
                    /* in case if labels address is already defined */
                    if (add_lbl_res == ERR_LABEL_ADDR_EXISTS)
                    {
                         print_err(global_err_list_head, 500, line_num, str, label_name);
                         i_item->is_valid = 0;
                         err_cnt++;
                        continue; /* got to next line , stop analyse command */
                    }        
                }
                /* finished add label */
                /* update instruction counter */
                instr_cnt += cpu_cmd_size;
            }    
            if (cpu_cmd_params_res == CMD_ERR){
                i_item->is_valid = 0;
                err_cnt++;
                continue; /* got to next line , stop analyse command */
            }    
        }

        /*********** analyze data directive for .data or .string or .mat **************/
        if (    no_entry_no_extern_flag && 
                cmd_reqs->feature.is_data_directive && (
                !strncmp(d_item->directive_name,".data",MAX_MACRO_AND_LABEL_LENGTH) || 
                !strncmp(d_item->directive_name,".string",MAX_MACRO_AND_LABEL_LENGTH) ||    
                !strncmp(d_item->directive_name, ".mat",MAX_MACRO_AND_LABEL_LENGTH))  
            )
        {   /* reset data process results values */ 
            data_proc_res = string_proc_res = mat_proc_res = 0;

            d_item_init(&d_item,cmd_reqs,data_cnt);

            if (!strncmp(d_item->directive_name,".data",MAX_MACRO_AND_LABEL_LENGTH))
                data_proc_res = data_directive_process(&ptr, line_num, &d_item, cmd_reqs, &data_cnt);

            if (!strncmp(d_item->directive_name,".string",MAX_MACRO_AND_LABEL_LENGTH))
                string_proc_res = string_directive_process(&ptr, line_num, &d_item, cmd_reqs, &data_cnt);

            if (!strncmp(d_item->directive_name, ".mat",MAX_MACRO_AND_LABEL_LENGTH))
                mat_proc_res = mat_directive_process(&ptr, line_num, &d_item, cmd_reqs, &data_cnt);

            /* in case of valid command with valid label: */
            if ((data_proc_res == DATA_VALID && cmd_label_status == VALID_LABEL) ||
                (string_proc_res == DATA_VALID && cmd_label_status == VALID_LABEL) ||
                (mat_proc_res == DATA_VALID && cmd_label_status == VALID_LABEL))
            {
                d_item->label_name = label_name;
                /* add data directive label to labels table */
                reset_feature(&label_feature);
                if (data_proc_res == DATA_VALID)
                    label_feature.points_to_data = 1;
                if (string_proc_res == DATA_VALID)
                    label_feature.points_to_str = 1;
                if (mat_proc_res == DATA_VALID)
                    label_feature.points_to_mat = 1;
                add_lbl_res = add_label_feature(&global_labels_table_ptr, label_name, label_feature, d_item->start_addr);
                /* in case of label's address already exist */
                if (add_lbl_res == ERR_LABEL_ADDR_EXISTS)
                {
                    print_err(global_err_list_head, 500, line_num, str, label_name);
                    d_item->is_valid = 0;
                    err_cnt++;
                    continue; /* got to next line , stop analyse command */
                }
                /* ^^^ finished add label ^^^ */

                data_cnt += d_item->size_in_words;
                d_item->flags.err_has_invalid_label = 0;
                d_item->flags.err_has_no_operands = 0;
                d_item->flags.error_in_parameters = 0;
                d_item->flags.is_valid_directive = 1;
                d_item->is_valid = 1;
            }

            /* in case of command without label: */
            if ((data_proc_res == DATA_VALID && cmd_label_status == VALID_CMD) ||
                (string_proc_res == DATA_VALID && cmd_label_status == VALID_CMD) ||
                (mat_proc_res == DATA_VALID && cmd_label_status == VALID_CMD))
            {
                d_item->label_name = NULL;
                data_cnt += d_item->size_in_words;
                d_item->flags.err_has_invalid_label = 0;
                d_item->flags.err_has_no_operands = 0;
                d_item->flags.error_in_parameters = 0;
                d_item->flags.is_valid_directive = 1;
                d_item->is_valid = 1;
            }

            if (data_proc_res == DATA_ERR || string_proc_res == DATA_ERR || mat_proc_res == DATA_ERR)
            {
                d_item->flags.error_in_parameters = 1;
                d_item->flags.is_valid_directive = 0;
                d_item->is_valid = 0;
                err_cnt++;
                continue; /* got to next line , stop analyse command */
            }    
        }
        /*********** analyze directive .external **************/
        if (cmd_reqs->feature.is_data_directive && !strncmp(cmd_reqs->keyword, ".extern",MAX_MACRO_AND_LABEL_LENGTH))
        {
            extern_cmd_res = extern_directive_process(&ptr, line_num);
            if (extern_cmd_res == DATA_ERR)
            {
            err_cnt++;
            continue; /* got to next line , stop analyse directive */
            }
        }

        /*********** analyze directive .entry **************/
        if (cmd_reqs->feature.is_data_directive && !strncmp(cmd_reqs->keyword, ".entry",MAX_MACRO_AND_LABEL_LENGTH))
        {   entry_cmd_res = entry_directive_process(&ptr, line_num);
            if (entry_cmd_res == DATA_ERR)
            {
                err_cnt++;
                continue; /* got to next line , stop analyse directive */
            }
        }

    } while (readline_res != NORM_LENGTH_AND_EOF);
    

    /* now check feature validity of labels table */
    lbl_err_cnt = label_table_features_check(global_labels_table_ptr);

    err_cnt += lbl_err_cnt;

    /* If no errors found, update data adresses to start closly after 
       CPU commands memory area.
     */
    if (!err_cnt){
        /* find the last item in the CPU commands linked list */
        data_cnt_start_addr = global_i_tail->start_addr + global_i_tail->size_in_wrds;
        /* size of instructions memory */
        cpu_instr_last_addr = data_cnt_start_addr - 1;
        /* update start data addresses values in data linked list 
           and get the last address value */
        prog_last_addr = upd_d_items_addr(global_d_head, global_d_tail, data_cnt_start_addr);
        /* size of data memory */
        prog_size = prog_last_addr - INSTR_CNT_INIT_VAL + 1;
        *cpu_instr_mem_size = cpu_instr_last_addr - INSTR_CNT_INIT_VAL + 1;
        *cpu_data_mem_size = prog_last_addr - data_cnt_start_addr +1;
        /* report error if last address is beyond max. mem address */

        labels_table_data_addr_upd(global_labels_table_ptr, data_cnt_start_addr);
    }


    /* print labels table for debug puposes */
    debug(printf("-------- PRINT CPU INSTR. ITEMS -----------------\n"));
    debug(print_i_items_list(global_i_head, global_i_tail));
    debug(printf("-------- PRINT DATA ITEMS -----------------\n"));
    debug(print_d_items_list(global_d_head, global_d_tail));
    debug(printf("-------- PRINT LABELS TABLE -----------------\n"));
    debug(label_table_print(global_labels_table_ptr));

    /* report error if last program address is beyond max. memory size */
    if (prog_last_addr > 256)
    {
        fprintf(ERR_STREAM, " Error : last program's address %d is"
                            "\n\tbeyond max. memory address 256 \n",
                prog_last_addr);
        fflush(ERR_STREAM);
        err_cnt++;
    }

    printf("----------------------------------------------------------\n");
    printf("1-st assembler stage run is finished.\n");
    if (err_cnt)
    {
        printf("\tFound %d errors,\n\tFiles generation for file <%s> is stopped. \n", err_cnt, am_fname);
        printf("\tPlease correct the input file <%s>.\n", global_full_fname_as);
        printf("----------------------------------------------------------\n");
    }
    else
    {
        printf("\tNo errors found in <%s> file.\n\n", am_fname);
        printf("\tProgram size............%d \n", prog_size);
        printf("\tInstr. memory size......%d \n", *cpu_instr_mem_size);
        printf("\tData memory size........%d \n", *cpu_data_mem_size);
        printf("\tStart memory address....%d \n", INSTR_CNT_INIT_VAL);
        printf("\tLast memory address.....%d \n", prog_last_addr);
        printf("----------------------------------------------------------\n");
    }


    fres = fclose(fd_as);
    if(fres)
        printf("Error : %s file did not closed successfully ", am_fname);
    else    
        fd_as = NULL;
    
    return err_cnt;

}

/*
  Function check_cmd_label:
    checks the first word of command, if it is valid label.
    Error will be reported in following cases ;
        - If it is valid label (i.e. differ from keyword/command and macro names)
          BUT without ':';
        - If it is valid label (ends with ':') alone, i.e. end of string
          appears immediately after it or there are white characters after it.
        - If the label ends with ':', but there are following errors:
            1. the label is too long
            2. the label not starts with alphabet char
            3. the label has not alphanum chars after first char.
            4. the label has name identical to command/keyword
            5. the label has name identical to macro name
  Parameters :
        str : pointer to pointer to begin of string to be analyzed
        line_nume : number of code line that is analyzed
        label_name : pointer to pointer to label name string.
  Return :
       -2 (ERR_ONLY_LABEL_FOUND) - only label is found in string
       -1 (INVALID_LABEL) - invalid label
        0 (VALID_CMD) - the first word does not end with ':', and appears in keywords list
        1 (VALID_LABEL) - valid label
        2 (FILE_FINISHED) - received last empty string

  In case of valid label parameter str will point at function finish on
        ' ' character after the label to allow further analyse of
        command string.
*/
int check_cmd_label(char **str, int line_num, char ** lable_name)
{

    is_cmd_lbl_stat label_status; /* label status */
    char txt[MAX_LINE_SIZE] = {'\0'}; /* array to store temp text */
    char txt1[MAX_LINE_SIZE] = {'\0'}; /* array to store temp text */
    char *ptr;                        /* internal temp pointer to string */
    char *ptr1;                       /* internal temp pointer to string */
    int res;                          /* temp result variable */
    int res1;                         /* temp result variable */
    char * heap_label_name;          /* will be used for label name in heap */
    keywords_item_ptr keyword_item ;  /* keyword item */

        /******** check if the line starts with label ******/

        ptr = *str;
    /* read first word up to ' ' delimiter */
    /* Note : all empty lines are removed at pre-assembler stage */
    res = gen_text_find(&ptr, ' ', txt);

    /* check if the text is label */
    is_label(txt, COLON_CHK_EN, &label_status);

    /* check if the line starts with command (no label) */
    if ((res == GOT_TXT_AND_EOS || res == GOT_TXT_AND_DLMTR) && label_status.no_column_at_end )
    {
        if ((res1 = is_cmd_name(txt, &keyword_item)) == CPU_INSTR || res1 == DATA_DIRECTIVE)
        {
            return VALID_CMD;
        }
    }

    /* if got text and EOS (i.e. did not get ' ' delimeter, but EOS immediately after
       the label )
       it means that text is alone.
       if it is not command/keyword or macro name,
       it is label alone, so it is error.
     */
    if (res == GOT_TXT_AND_EOS)
    {
        if (label_status.equals_cmd_name == 0 &&
            label_status.equals_macro_name == 0)
        {
            print_err(global_err_list_head, 157, line_num, *str, "");
            return ERR_ONLY_LABEL_FOUND;
        }
    }

    /* empty string can be only last string , because pre-assembly removes all other strings*/
    if (res == GOT_EOS_AT_START || res == GOT_WHITES_AND_EOS)
    {
        return FILE_FINISHED;
    }

    /* label has ':' at the end, but it is invalid */
    if ((label_status.label_is_ok == 0) && label_status.has_column_at_end)
    {
        /* label is too long */
        if (label_status.label_is_too_long)
        {
            print_err(global_err_list_head, 150, line_num, *str, "");
            return INVALID_LABEL;
        }
        /* label equals keyword/command name*/
        if (label_status.equals_cmd_name)
        {
            print_err(global_err_list_head, 155, line_num, *str, "");
            return INVALID_LABEL;
        }
        /* label equals macro name */
        if (label_status.equals_macro_name)
        {
            print_err(global_err_list_head, 154, line_num, *str, "");
            return INVALID_LABEL;
        }

        /* label does not start with alphabet char */
        if (label_status.non_aphabet_start)
        {
            print_err(global_err_list_head, 152, line_num, *str, "");
            return INVALID_LABEL;
        }

        /* label has non alphanum char (from 2nd char)*/
        if (label_status.has_non_alphanum)
        {
            print_err(global_err_list_head, 153, line_num, *str, "");
            return INVALID_LABEL;
        }
    }

    /* check following error : it is valid label
        (differs from macro or command name) but without ':' at the end
        The command label must have ':' at the end.
        It can not be first word in the command without ':' at the end
        and not command name.
    */
    if (label_status.no_column_at_end)
    {
        /* check if the text is label */
        is_label(txt, NO_COLON_CHK, &label_status);
        /* command can not start with valid label without ':' at the end: */
        print_err(global_err_list_head, 156, line_num, *str, "");
        return INVALID_LABEL;
    }

    /* If after delimiter ' ' (=space) thare are only white chars and EOS.
        This is also an error.
    */

    ptr1 = ptr;
    /* check if there are only white char after the text is label */
    res1 = gen_text_find(&ptr1, EOS, txt1);
    if (res1 == GOT_WHITES_AND_EOS || res1 == GOT_WHITES_AND_DELMTR ||
        res1 == GOT_DLMTR_AT_START || res1 == GOT_EOS_AT_START)
    {
        print_err(global_err_list_head, 157, line_num, *str, "");
        return ERR_ONLY_LABEL_FOUND;
    }

    
    /* ------------ VALID LABEL IS FOUND ----------------*/
    /* copy label name to heap and return its pointer : */
    
    /* before the storage, clear ':' at end of string : */
    txt[strlen(txt)-1] = EOS;
    /* now we know that txt is valid label name.
       copy label name to heap */
    heap_label_name = heap_string_duplicate_safe(txt,MAX_MACRO_AND_LABEL_LENGTH+2);
    *lable_name = heap_label_name;
    /* set *str to point ' ' character after the label*/
    *str = ptr;
    return VALID_LABEL;


} /* function check_cmd_label*/

/* function analyze_cmd :
        Checks command parameter within string
        If it found, returns keywords_item_ptr pointer to keywords_item 
        that includes requirements for the command.

        Checks for several errors : 
            - if given word is not command
            - if command requires at least 1 parameter, but EOS find in place of it.

        Returns
                - NULL if command is invalid, or error detected
                - pointer to keyword table item , that contains keyword features.

        In case of valid command that ends with ' ' delimeter and has at lest one parameter 
        sets *str pointer to ' ' character after a command.
        
        In case of valid command that has no parameters (ends with whites and EOS or EOS), 
        does not change the pointer *str position. 

        In addition this function prepares instruction linked list and data linked list,
        and start to fill them for commands that need no parameters. 
        Only valid commands will be inserted to instruction linked list and 
        data linked list !    
*/
keywords_item_ptr analyze_cmd(char **str, int line_num, int instr_cnt, int data_cnt,
                              i_item_ptr *i_item, data_blk_item_ptr *d_item)
{
    int res=0;
    int res1=0;
    int res2=0;
    char txt[MAX_LINE_SIZE] = {'\0'};
    char txt1[MAX_LINE_SIZE] = {'\0'};
    char *ptr,*ptr1;
    char * heap_cmd_name; /* pointer to command name array in heap */
    int no_entry_no_extern_flag = 0; /* flag that signs command is not .enrtry and not .extern */
    keywords_item_ptr cmd_requrements = NULL;
    ptr = *str;
    res = gen_text_find(&ptr, ' ', txt);
    /* if the text is found : analyse the command */
    if (res == GOT_TXT_AND_DLMTR || res == GOT_TXT_AND_EOS)
        res1 = is_cmd_name(txt, &cmd_requrements);
    
    /* error : we expected command name */
    if (res1 == NOT_INSTR){
        print_err(global_err_list_head, 159, line_num, *str, txt);
        return NULL;
    }
    /* now we know that txt is one of command names. 
       store the comand name in list of 
       copy command name to heap */
    heap_cmd_name = heap_string_duplicate_safe(txt,MAX_MACRO_AND_LABEL_LENGTH+2);
    
    /* fill command name and instruction counter data to linked list items */
    if (res1==CPU_INSTR){
        /* add new item to cpu instr linked list */
        *i_item = add_cpu_instr_item(&global_i_head, &global_i_tail);
        (*i_item)->start_addr = instr_cnt;
        (*i_item)->cmd_name = heap_cmd_name;  
    }
    
    /* set no .entry and no .extern flag */
    no_entry_no_extern_flag = (strncmp(cmd_requrements->keyword,".extern",MAX_MACRO_AND_LABEL_LENGTH)!=0) 
        && (strncmp(cmd_requrements->keyword,".entry",MAX_MACRO_AND_LABEL_LENGTH)!=0);

    if (res1 == DATA_DIRECTIVE && no_entry_no_extern_flag)
    {
        /* add new item to data linked list */
        *d_item = add_new_data_blk_item(&global_d_head, &global_d_tail);
        (*d_item)->start_addr = data_cnt;
        (*d_item)->directive_name = heap_cmd_name;
    }

    /* check what text appears after the command */
    ptr1 = ptr;
    res2 = gen_text_find(&ptr1, EOS, txt1);

    /* Report error in case when command received alone
        but we expect at least 1 operand 
        Check if only whites and EOS follow after the command 
        or there is EOS immediately after the command */
    if (cmd_requrements->feature.min_num_of_operands > 0
        && (res2 == GOT_WHITES_AND_EOS ||res == GOT_TXT_AND_EOS)) {
            if (res1 == CPU_INSTR)
                (*i_item)->is_valid = 0;
            if (res1 == DATA_DIRECTIVE && no_entry_no_extern_flag)
                (*d_item)->is_valid = 0;
            print_err(global_err_list_head, 160, line_num, *str, "");
            return NULL;
    }

    /* in case of 0 parameters 
       and found text after command report error 
    */

    if ((cmd_requrements->feature.min_num_of_operands == 0) && (cmd_requrements->feature.max_num_of_operands == 0) &&
        (res2 == GOT_TXT_AND_EOS || res2 == GOT_TXT_AND_DLMTR ||
         res2 == GOT_SECOND_TXT_BLK)  )
    {
        if (res1 == CPU_INSTR)
            (*i_item)->is_valid = 0;
        if (res1 == DATA_DIRECTIVE && no_entry_no_extern_flag)
            (*d_item)->is_valid = 0;
        print_err(global_err_list_head, 180, line_num, *str, "");
        return NULL;

    }


    /* in case of valid command with no parameters,
       that ends with EOS : dont set the str pointer
       after the command */
    if ((cmd_requrements->feature.min_num_of_operands == 0)
        &&(cmd_requrements->feature.max_num_of_operands == 0)
        && (res2 == GOT_EOS_AT_START || res2 == GOT_WHITES_AND_EOS || res == GOT_TXT_AND_EOS) 
        && (res1 == CPU_INSTR)) {
        /* it is cpu instruction check that really needs no parameters: */
            (*i_item)->status.params_num = 0;
            (*i_item)->is_valid = 1;
            (*i_item)->size_in_wrds = 1;
        }    
        

    /* in case of command that needs parameters (will be analysed in the future) 
        set pointer the next ' ' character after 
        the command keyword. 
    */
    if (cmd_requrements->feature.min_num_of_operands > 0 
        && (res == GOT_TXT_AND_DLMTR)) {
        /* set pointer the next ' ' character after the command keyword*/
        *str = ptr;
        return cmd_requrements;
    }

    return cmd_requrements;
}

/* function analyze_cpu_instr_params:

    - analyse cpu instruction parameters vs. cpu instruction requirements
    - update i_item fields, including its binary fields for code preparation.
    - calculate next instruction counter value
    parameters: 
        full_str  - pointer to full line , for errors reports
        str       - pointer to pointer to parameters part of line only, for analysis
        cmd_reqs  - object that stor command requirements
        i_item    - pointer to pointer to instruction item object           
    
    return: 

        (-1) CMD_ERR       : in case of error
        ( 0) CMD_VALID     : the command is valid 

*/
int analyze_cpu_instr_params(char * full_str, char **str, int line_num, 
                    keywords_item_ptr cmd_reqs, i_item_ptr *i_item) 
{
    char * ptr, *ptr2; /* temp pointer to part of string (ptr2 is used for 2nd param) */
    char txt[MAX_LINE_SIZE] = {'\0'}; /* temp char array */
    int expected_cmd_params_num; /* expected command params number */
    int res[6]={0};   /* temp result of function  */
    int reg_num = 0;
    int i = 0;            /* index*/
    int param_num;    /* variable to store parameter number value*/
    is_cmd_lbl_stat label_status; /* object to store label status */
    char * matrix_label_name; 
    int reg1, reg2; /* value of reg1 and reg2 in matrix operand*/
    mat_param_status mat_status; /* status object of optional matrix parameter */
    /* store requred number of command params */
    int param1_found_flag = 0; /* flag to sign that 1st param is found */ 
    int define_res ; /* result of define function */
    
    
    expected_cmd_params_num = cmd_reqs->feature.min_num_of_operands;
    /* reset matrix status object  : */
    reset_matrix_status(&mat_status);

    ptr = *str;

    /**************    analyze first parameter  *****************************/
    /*------------------------------------------------------------------
           check if 1st parameter is matrix call parameter
    -------------------------------------------------------------------*/
    is_matrix_param(&ptr, &matrix_label_name, &reg1, &reg2, &mat_status);
    /* check if parameter has '[' or ']' character, i.e. may be matrix call */
    if (mat_status.not_matrix_call == 0)
    {
        res[1]=report_matrix_param_errors(&mat_status,line_num,full_str,1);
        if (res[1] == CMD_ERR )
          return CMD_ERR;
    
        /* err: expected one parameter only , but find delimiter ',' after it */
        err_check1(mat_status.delimeter_after_m_call && expected_cmd_params_num == 1,
                   200, full_str, CMD_ERR)

            /* the parameter is matrix call. fill 1st parameter's data if expect 2 parameters */
            if (res[1] == CMD_VALID && expected_cmd_params_num == 2)
        {
            define_res = matrix_1st_param_define(i_item, cmd_reqs, str,
                                    matrix_label_name, reg1, reg2, line_num);
            if (define_res==CMD_ERR)
                return CMD_ERR;
            if (define_res == CMD_VALID)
                param1_found_flag = 1; /* set flag and continue to check 2nd parameter */
        }

        /* if expected cmd params num == 1 , store result in destiantion (according to table on p.35) */
        if (res[1] == CMD_VALID && expected_cmd_params_num == 1)
        {
            define_res = matrix_2nd_param_define(i_item, cmd_reqs, str,
                                                 matrix_label_name, reg1, reg2, line_num);
            if (define_res == CMD_ERR) 
                return CMD_ERR;
            else    
                return CMD_VALID;
        }
    }


    if (param1_found_flag == 0){
        /* if it is not matix, return pointer to previous position */
        ptr = *str;

        /* if num of params is 1 analyze first cpu instruction parameter with delimiter EOS */
        if (expected_cmd_params_num == 1 )
            res[2] = gen_text_find(&ptr, EOS, txt);
        /* if num of params is 2 analyze first cpu instruction parameter with delimiter ',' */
        if (expected_cmd_params_num == 2 )
            res[2] = gen_text_find(&ptr, ',', txt);

        /* if there is more than one text block before delimeter :*/
        err_check1(res[2] == GOT_SECOND_TXT_BLK, 161,full_str, CMD_ERR)
        /* too few parameters */
        err_check1(res[2] == GOT_EOS_AT_START || res[2] == GOT_WHITES_AND_EOS ||
                            res[2] == GOT_WHITES_AND_DELMTR || 
                            res[2] == GOT_DLMTR_AT_START,160, full_str,CMD_ERR)

    
        /* if first parameter is endeed found, analyze it */
        if (res[2] == GOT_TXT_AND_EOS || res[2] == GOT_TXT_AND_DLMTR)
        {
            /*---------------------------------------------------------------
                 check if 1st parameter is register name
            ---------------------------------------------------------------*/
            /* if it starts with r it can be register name or label */
            reg_num = res[3] = is_reg_name(txt);
            /* parameter is register name. 
               fill 1st parameter's data if expect 2 parameters */
            if (res[3] != NO_REG_NAME && expected_cmd_params_num == 2)
            {
                define_res = register_1st_param_define(i_item, cmd_reqs, str, txt, reg_num, line_num);
                if (define_res == CMD_ERR)
                    return CMD_ERR;
                else
                    param1_found_flag = 1; /* set flag and continue to check 2nd parameter */
            }
            
            /* if expected cmd params num == 1 , store result in destiantion (according to table on p.35) */
            if (res[3] != NO_REG_NAME && expected_cmd_params_num == 1)
            {

                define_res = register_2nd_param_define(i_item, cmd_reqs, str, txt, reg_num, line_num);

                if (define_res == CMD_ERR)
                    return CMD_ERR;
                else
                    return CMD_VALID;
            }
        
            /* check for param_number if flag == 0*/
            if (param1_found_flag == 0)
            {
                /*---------------------------------------------------------------
                   check if 1st parameter is #number parameter (if starts with #)
                ---------------------------------------------------------------*/
                res[4] = is_param_number(txt, &param_num);
                /* if parameter starts with '#' */
                if (res[4] != NOT_NUM_PARAM && param1_found_flag == 0)
                {
                    /* parameter number limits fault */
                    err_check1(res[4] == NUM_PARAM_LIM_FAULT, 162,full_str, CMD_ERR)
                    /* parameter syntax fault */
                    err_check1(res[4] == NUM_PARAM_SYNTAX_FAULT, 163,full_str, CMD_ERR)
                    
                    /* the parameter is #number fill 1st parameter's data if expect 2 parameters */                    
                    if (res[4] == PARAM_NUM_IS_VALID && expected_cmd_params_num == 2)
                    {
                        define_res = number_1st_param_define(i_item, cmd_reqs, str, txt, param_num, line_num);
                        if(define_res == CMD_ERR)
                            return CMD_ERR;
                        else    
                            /* set flag and continue to check 2nd parameter */
                            param1_found_flag = 1;
                    }

                    /* if expected cmd params num == 1 : store result in destiantion (according to table on p.35) */
                    if (res[4] == PARAM_NUM_IS_VALID && expected_cmd_params_num == 1)
                    {
                        /* treat number second param */
                        define_res = number_2nd_param_define(i_item, cmd_reqs, str, txt, param_num, line_num);
                        if (define_res == CMD_ERR)
                            return CMD_ERR;
                        else    
                            return CMD_VALID;
                    }

                }

                if (param1_found_flag == 0)
                {
                    /*---------------------------------------------------------------
                     check if 1st parameter is label (match label
                     definition). Note : this label may be defined or not defined
                    ---------------------------------------------------------------*/
                    is_label(txt, NO_COLON_CHK, &label_status);
                    /* verify that txt does not start with '#' */
                    if (*txt != NUM_PARAM_START_CHAR && param1_found_flag == 0)
                    {
                        /* err: label that equals to command name  */
                        err_check1(label_status.equals_cmd_name, 164,full_str, CMD_ERR)
                        /* err: label that equals macro name */
                        err_check1(label_status.equals_macro_name, 165,full_str, CMD_ERR)
                        /* err: label with non alphabet start character */
                        err_check1(label_status.non_aphabet_start, 166,full_str, CMD_ERR)
                        /* err: label with non alphanum char after the 1st character */
                        err_check1(label_status.has_non_alphanum, 167,full_str, CMD_ERR)
                        /* err: label is too long */
                        err_check1(label_status.label_is_too_long, 168,full_str, CMD_ERR)
                        /* store valid label value */
                        
                        /* the parameter is labell, fill 1st parameter's data if expect 2 parameters */                        
                        if(label_status.label_is_ok == 1 && expected_cmd_params_num == 2)
                        {
                            define_res = label_1st_param_define(i_item, cmd_reqs, str, txt, line_num);
                            if (define_res == CMD_ERR)
                                return CMD_ERR;
                            else
                                /* continue to check 2nd parameter */
                                param1_found_flag = 1;
                        } /* if(label_status.label_is_ok == 1) */

                        /* the parameter is labell, fill 2nd parameter's data if expect 1 parameter */
                        if (label_status.label_is_ok == 1 && expected_cmd_params_num == 1)
                        {
                            define_res = label_2nd_param_define(i_item, cmd_reqs, str, txt, line_num);
                            if (define_res == CMD_ERR)
                                return CMD_ERR;
                            else    
                                return CMD_VALID;
                        }

                    } /* if (*txt != NUM_PARAM_START_CHAR && param1_found_flag == 0) */
                } /* param1_found_flag == 0*/
            } /* param1_found_flag == 0*/
        } /*if (res[2] == GOT_TXT_AND_EOS || res[2] == GOT_TXT_AND_DLMTR)*/
    } /* if (param1_found_flag == 0) */

    /************   analyze second parameter *************************/

    if (expected_cmd_params_num == 2)
    {

        /* this case should not appear, but it is safe guard */
        if (*ptr != ','){
            err_check1(res[4] == NUM_PARAM_SYNTAX_FAULT, 169, full_str,CMD_ERR)
            return CMD_ERR;
        }

        /* increment both pointers to next char after ',' */  
        ptr2 = ++ptr;

        /* reset res array : */
        for (i = 0; i < (sizeof(res)/sizeof(res[1])); i++)
            res[i] = 0;
        /* reset matrix status object  : */
        reset_matrix_status(&mat_status);

        /*------------------------------------------------------------------
                check if the 2nd parameter is matrix call parameter
        -------------------------------------------------------------------*/
        is_matrix_param(&ptr2, &matrix_label_name, &reg1, &reg2, &mat_status);
        /* check if parameter has '[' or ']' character, i.e. may be matrix call */
        if (mat_status.not_matrix_call == 0)
        {

            res[1] = report_matrix_param_errors(&mat_status, line_num, *str,2);
            if (res[1] == CMD_ERR)
                return CMD_ERR;

            /* fill matrix call parameter's data */
            if (res[1] == CMD_VALID)
            {
                define_res = matrix_2nd_param_define(i_item, cmd_reqs, str,
                                                      matrix_label_name, reg1, reg2, line_num);
                if (define_res == CMD_ERR)
                    return CMD_ERR;
                else    
                    return CMD_VALID;
            }
        } /* 2nd param : if (mat_status.not_matrix_call == 0) */
        
        /* return ptr2 to previous position (i.e. before matrix check) */
        ptr2 = ptr;
        /* analyze 2nd cpu instruction parameter with delimiter EOS */
        res[2] = gen_text_find(&ptr2, EOS, txt);
        /* if there is more than one text block before EOS */
        err_check1(res[2] == GOT_SECOND_TXT_BLK, 171, full_str,CMD_ERR)
        /* no second parameter */
        err_check1(res[2] == GOT_EOS_AT_START || res[2] == GOT_WHITES_AND_EOS ||
                            res[2] == GOT_WHITES_AND_DELMTR || 
                            res[2] == GOT_DLMTR_AT_START,172, full_str, CMD_ERR)
        
        /* if 2nd parameter is endeed found, analyze it */
        if (res[2] == GOT_TXT_AND_EOS || res[2] == GOT_TXT_AND_DLMTR)
        {

            /*---------------------------------------------------------------
                 check if 2nd parameter is register name
            ---------------------------------------------------------------*/
            /* if it starts with r it can be register name or label */
            reg_num = res[3] = is_reg_name(txt);
            /* parameter is register name */

            if (res[3] != NO_REG_NAME)
            {
                define_res = register_2nd_param_define(i_item, cmd_reqs, str, txt, reg_num, line_num);
                if (define_res == CMD_ERR)
                    return CMD_ERR;
                else
                    return CMD_VALID;

            } /* if (res[3] != NO_REG_NAME) */
            
            /*---------------------------------------------------------------
               check if 2nd parameter is #number parameter (if starts with #)
            ---------------------------------------------------------------*/
            res[4] = is_param_number(txt, &param_num);
            /* if parameter starts with '#' */
            if (res[4] != NOT_NUM_PARAM )
            {
                /* parameter number limits fault */
                err_check1(res[4] == NUM_PARAM_LIM_FAULT, 173, full_str, CMD_ERR)
                /* parameter syntax fault */
                err_check1(res[4] == NUM_PARAM_SYNTAX_FAULT, 174,full_str, CMD_ERR)
                /* "if" can be ommited here but added for clarity (the last option) */
                if (res[4] == PARAM_NUM_IS_VALID)
                {
                    /* treat number second param */
                    define_res = number_2nd_param_define(i_item, cmd_reqs, str, txt, param_num, line_num);
                    if (define_res == CMD_ERR)
                        return CMD_ERR;
                    else
                        return CMD_VALID;
                }
            } /* if (res[4] != NOT_NUM_PARAM */

            /*---------------------------------------------------------------
             check if 2nd parameter is label (match label
             definition). Note : this label may be defined or not defined
            ---------------------------------------------------------------*/
            is_label(txt, NO_COLON_CHK, &label_status);
            /* verify that txt does not start with '#' */
            if (*txt != NUM_PARAM_START_CHAR )
            {
                /* err: label that equals to command name  */
                err_check1(label_status.equals_cmd_name, 175, full_str, CMD_ERR)
                /* err: label that equals macro name */
                err_check1(label_status.equals_macro_name, 176, full_str, CMD_ERR)
                /* err: label with non alphabet start character */
                err_check1(label_status.non_aphabet_start, 177, full_str, CMD_ERR)
                /* err: label with non alphanum char after the 1st character */
                err_check1(label_status.has_non_alphanum, 178, full_str, CMD_ERR)
                /* err: label is too long */
                err_check1(label_status.label_is_too_long, 179, full_str, CMD_ERR)
                /* store valid label value */
                if (label_status.label_is_ok == 1)
                {

                    define_res = label_2nd_param_define(i_item, cmd_reqs, str, txt, line_num);
                    if (define_res == CMD_ERR)
                        return CMD_ERR;
                    else
                        return CMD_VALID;

                } /* if(label_status.label_is_ok == 1) */

            } /* if (*txt != NUM_PARAM_START_CHAR ) */
        } /* if (res[2] == GOT_TXT_AND_EOS || res[2] == GOT_TXT_AND_DLMTR)*/
    } /* if (expected_cmd_params_num == 2) */

    return CMD_VALID;
}


/* function report_matrix_param_errors
    parameters:
        mat_status : matrix status object 
        line_num   : code's line number
        str        : code's string
        param_num  : number of parameter (1 or 2 , will change report contents accordingly)
    return:
     -1 (CMD_ERR) if there is a problem in matrix call
      0 (CMD_VALID) if matrix call is valid

*/
int report_matrix_param_errors(mat_param_status * mat_status, int line_num, char * str , int param_number)
{
    /* if matrix call contain at least '[' or ']' symbols */
    if (mat_status->maybe_matrix_call)
    {
        if (param_number == 1)
        {
            /* err: white char between label and paranth */
            err_check1(mat_status->err_white_btwn_lbl_and_prnth, 201, str, CMD_ERR)
            /* extra text in matrix call */
            err_check1(mat_status->err_extra_text, 202, str, CMD_ERR)
            /* label in matrix call equals to command name */
            err_check1(mat_status->label_equals_cmd_name, 203, str, CMD_ERR)
            /* label in matrix call equals to macro name */
            err_check1(mat_status->label_equals_macro_name, 204, str, CMD_ERR)
            /* label in matrix call has not alphabet first char */
            err_check1(mat_status->label_has_non_alphanum, 205, str, CMD_ERR)
            /* label in matrix call has not alphanum after first char */
            err_check1(mat_status->label_has_non_alphanum, 206, str, CMD_ERR)
            /* label in matrix call is too long */
            err_check1(mat_status->label_is_too_long, 207, str, CMD_ERR)
            /* no label in matrix call */
            err_check1(mat_status->label_not_exists, 208, str, CMD_ERR)
            /* paranthesis problem in matrix call */
            err_check1(mat_status->paranth_problem, 209, str, CMD_ERR)
            /* text between matrix call and delimiter */
            err_check1(mat_status->txt_between_mcall_and_dlmter, 210, str, CMD_ERR)
            /* reg1 name error in matrix call */
            err_check1(mat_status->reg1_name_err, 211, str, CMD_ERR)
            /* reg2 name error in matrix call */
            err_check1(mat_status->reg2_name_err, 212, str, CMD_ERR)
            /* matrix definition syntax error */
            err_check1(mat_status->matrix_def_syntax_err, 213, str, CMD_ERR)
        }

        if (param_number == 2)
        {   
            /* for 2nd parameter delimiter after mat call is forbidden */
            err_check1(mat_status->delimeter_after_m_call, 234, str, CMD_ERR)
            /* err: white char between label and paranth */
            err_check1(mat_status->err_white_btwn_lbl_and_prnth, 221, str, CMD_ERR)
            /* extra text in matrix call */
            err_check1(mat_status->err_extra_text, 222, str, CMD_ERR)
            /* label in matrix call equals to command name */
            err_check1(mat_status->label_equals_cmd_name, 223, str, CMD_ERR)
            /* label in matrix call equals to macro name */
            err_check1(mat_status->label_equals_macro_name, 224, str, CMD_ERR)
            /* label in matrix call has not alphabet first char */
            err_check1(mat_status->label_has_non_alphanum, 225, str, CMD_ERR)
            /* label in matrix call has not alphanum after first char */
            err_check1(mat_status->label_has_non_alphanum, 226, str, CMD_ERR)
            /* label in matrix call is too long */
            err_check1(mat_status->label_is_too_long, 227, str, CMD_ERR)
            /* no label in matrix call */
            err_check1(mat_status->label_not_exists, 228, str, CMD_ERR)
            /* paranthesis problem in matrix call */
            err_check1(mat_status->paranth_problem, 229, str, CMD_ERR)
            /* text between matrix call and delimiter */
            err_check1(mat_status->txt_between_mcall_and_dlmter, 230, str, CMD_ERR)
            /* reg1 name error in matrix call */
            err_check1(mat_status->reg1_name_err, 231, str, CMD_ERR)
            /* reg2 name error in matrix call */
            err_check1(mat_status->reg2_name_err, 232, str, CMD_ERR)
            /* matrix definition syntax error */
            err_check1(mat_status->matrix_def_syntax_err, 233, str, CMD_ERR)
        }
    } 

    return CMD_VALID;

}   

 

/* function addrsn_check 

    Checks if parameter's type addressing type is allowed.
    parameters : 
        type : type of parameter object
        param_num : parameter number (1 for 1st and 2 for 2nd)
        cmd_reqs : given command requirements object 
    return: 
        1 (PARAM_ALLOWED)   - param. is allowed 
        0 (PARAM_FORBIDDEN) - param. is forbidden    
*/
int addrsn_check(param_type type, int param_num, keywords_item_ptr cmd_reqs){

    if (param_num == 1){
        if (type.param_is_number==1)
            if (cmd_reqs->feature.src_addr_mthd_0==1)
                return PARAM_ALLOWED;

        if (type.param_is_label == 1)
            if (cmd_reqs->feature.src_addr_mthd_1 == 1)
                return PARAM_ALLOWED;

        if (type.param_is_matrx == 1)
            if (cmd_reqs->feature.src_addr_mthd_2 == 1)
                return PARAM_ALLOWED;

        if (type.param_is_reg_name == 1)
            if (cmd_reqs->feature.src_addr_mthd_3 == 1)
                return PARAM_ALLOWED;
    }

    if (param_num == 2)
    {
        if (type.param_is_number == 1)
            if (cmd_reqs->feature.dest_addr_mthd_0 == 1)
                return PARAM_ALLOWED;

        if (type.param_is_label == 1)
            if (cmd_reqs->feature.dest_addr_mthd_1 == 1)
                return PARAM_ALLOWED;

        if (type.param_is_matrx == 1)
            if (cmd_reqs->feature.dest_addr_mthd_2 == 1)
                return PARAM_ALLOWED;

        if (type.param_is_reg_name == 1)
            if (cmd_reqs->feature.dest_addr_mthd_3 == 1)
                return PARAM_ALLOWED;
    }


    return PARAM_FORBIDDEN;
}

/* function matrix_1st_param_define:

       define and check  matrix call parameter ( 1st parameter)
       parameters:
              i_item : instruction item object
              cmd_reqs : requirements for given command
              str : pointer to pointer to parameters string
              matrix_label_name : matrix label name
              reg1 : register 1 number
              reg2 : register 2 number
              line_num : line number
        return :
            CMD_ERR   (-1) : command is not valid
            CMD_VALID ( 1) : command is valid
*/
int matrix_1st_param_define(i_item_ptr *i_item, keywords_item_ptr cmd_reqs, char **str, 
                             char *matrix_label_name, int reg1, int reg2, int line_num){
    
    param_type type; /* parameter type object */
    int param_chk;   /* parameter addressing check result */

    type.param_is_label = 0;
    type.param_is_matrx = 1;
    type.param_is_number = 0;
    type.param_is_reg_name = 0;

    /* check 1st parameter addressing type for given command */
    param_chk = addrsn_check(type, 1, cmd_reqs);

    /* report err and return if 1st param addressing error occures */
    err_check1(param_chk == PARAM_FORBIDDEN, 301, *str, CMD_ERR)

    /* addressing is valid :*/
    (*i_item)->param1.param_type.param_is_label = 0;
    (*i_item)->param1.param_type.param_is_matrx = 1;
    (*i_item)->param1.param_type.param_is_number = 0;
    (*i_item)->param1.param_type.param_is_reg_name = 0;
    (*i_item)->param1.param_type.no_param = 0;

  
    (*i_item)->param1.params_union.mat.mat_label = matrix_label_name;
    (*i_item)->param1.params_union.mat.reg1_num = reg1;
    (*i_item)->param1.params_union.mat.reg2_num = reg2;
    (*i_item)->param1.params_union.mat.validity.brackets_valid = 1;
    (*i_item)->param1.params_union.mat.validity.mat_label_defined = 1;
    (*i_item)->param1.params_union.mat.validity.mat_label_syntax_valid = 1;
    (*i_item)->param1.params_union.mat.validity.reg1_name_valid = 1;
    (*i_item)->param1.params_union.mat.validity.reg2_name_valid = 1;

    (*i_item)->status.params_num += 1;
    (*i_item)->status.param1_found = 1;
    (*i_item)->status.src_addrsng = 2; /* matrix */

    return CMD_VALID;
}

/* function matrix_2nd_param_define:

       define and check matrix call parameter ( 2nd parameter)
       parameters:
              i_item : instruction item object
              cmd_reqs : requirements for given command
              str : pointer to pointer to parameters string
              matrix_label_name : matrix label name
              reg1 : register 1 number
              reg2 : register 2 number
              line_num : line number
        return :
            CMD_ERR   (-1) : command is not valid
            CMD_VALID ( 1) : command is valid
*/
int matrix_2nd_param_define(i_item_ptr *i_item, keywords_item_ptr cmd_reqs, char **str,
                            char *matrix_label_name, int reg1, int reg2, int line_num){

    param_type type; /* parameter type object */
    int param_chk;   /* parameter addressing check result */

    type.param_is_label = 0;
    type.param_is_matrx = 1;
    type.param_is_number = 0;
    type.param_is_reg_name = 0;

    /* check 2nd parameter addressing type for given command */
    param_chk = addrsn_check(type, 2, cmd_reqs);

    /* report err and return if 2nd param addressing error occures */
    err_check1(param_chk == PARAM_FORBIDDEN,305, *str, CMD_ERR)

    /* addressing is valid :*/
    (*i_item)->param2.param_type.param_is_label = 0;
    (*i_item)->param2.param_type.param_is_matrx = 1;
    (*i_item)->param2.param_type.param_is_number = 0;
    (*i_item)->param2.param_type.param_is_reg_name = 0;
    (*i_item)->param2.param_type.no_param = 0;

    (*i_item)->param2.params_union.mat.mat_label = matrix_label_name;
    (*i_item)->param2.params_union.mat.reg1_num = reg1;
    (*i_item)->param2.params_union.mat.reg2_num = reg2;
    (*i_item)->param2.params_union.mat.validity.brackets_valid = 1;
    (*i_item)->param2.params_union.mat.validity.mat_label_defined = 1;
    (*i_item)->param2.params_union.mat.validity.mat_label_syntax_valid = 1;
    (*i_item)->param2.params_union.mat.validity.reg1_name_valid = 1;
    (*i_item)->param2.params_union.mat.validity.reg2_name_valid = 1;
    (*i_item)->status.params_num += 1;
    (*i_item)->status.param2_found = 1;
    (*i_item)->status.dest_addrsng = 2; /* matrix */

    return CMD_VALID;
}

/* function label_2nd_param_define:
       define and check label parameter ( 2nd parameter)
       parameters:
              i_item : instruction item object
              cmd_reqs : requirements for given command
              str : pointer to pointer to parameters string
              txt : label text
              line_num : line number
        return :
            CMD_ERR   (-1) : command is not valid
            CMD_VALID ( 1) : command is valid
*/
int label_2nd_param_define(i_item_ptr *i_item, keywords_item_ptr cmd_reqs, 
                           char **str, char *txt, int line_num)
{

       char *heap_label_param_name; /* pointer to label parameter in cpu command */
       param_type type;             /* parameter type object */
       int param_chk;               /* parameter addressing check result */
       label_features label_feature; /* label feature object */

       type.param_is_label = 1;
       type.param_is_matrx = 0;
       type.param_is_number = 0;
       type.param_is_reg_name = 0;

       /* check 1st parameter addressing type for given command */
       param_chk = addrsn_check(type, 2, cmd_reqs);

       /* report err and return if 1st param addressing error occures */
       err_check1(param_chk == PARAM_FORBIDDEN, 304, *str, CMD_ERR)

        /* addressing is valid :*/
       (*i_item)->param2.param_type.param_is_label = 1;
       (*i_item)->param2.param_type.param_is_matrx = 0;
       (*i_item)->param2.param_type.param_is_number = 0;
       (*i_item)->param2.param_type.param_is_reg_name = 0;
       (*i_item)->param2.param_type.no_param = 0;

       (*i_item)->param2.params_union.lbl.label_addr = TO_BE_DEFINED;
       /* copy label name to heap and return its pointer : */
       /* now we know that txt is valid label name.
       copy label name to heap */
       heap_label_param_name = heap_string_duplicate_safe(txt,MAX_MACRO_AND_LABEL_LENGTH+2);
       (*i_item)->param2.params_union.lbl.label_name = heap_label_param_name;
       (*i_item)->param2.params_union.lbl.validity.label_addr_defined = 0;
       (*i_item)->param2.params_union.lbl.validity.label_param_syntax_valid = 1;
       (*i_item)->param2.params_union.lbl.validity.label_length_valid = 1;
       (*i_item)->status.params_num += 1;
       (*i_item)->status.param2_found = 1;
       (*i_item)->status.dest_addrsng = 1; /* label */

       /* add label parameter name to labels table , sign it as parameter only */
       reset_feature(&label_feature);
       label_feature.is_cpu_cmd_param = 1;
       /* because it is parameter - no new address for label will be defined !
          so address value is TO_BE_DEFINED
       */
       add_label_feature(&global_labels_table_ptr, heap_label_param_name,
                                       label_feature, TO_BE_DEFINED);

       return CMD_VALID;
}

/* function label_1st_param_define:
       define and check label parameter (1st parameter)
       parameters:
              i_item : instruction item object
              cmd_reqs : requirements for given command
              str : pointer to pointer to parameters string
              txt : label text
              line_num : line number
        return :
            CMD_ERR   (-1) : command is not valid
            CMD_VALID ( 1) : command is valid
*/
int label_1st_param_define(i_item_ptr *i_item, keywords_item_ptr cmd_reqs, char **str, char *txt, int line_num){
       
       char *heap_label_param_name; /* pointer to label parameter in cpu command */
       param_type type;             /* parameter type object */
       int param_chk;               /* parameter addressing check result */
       label_features label_feature; /* label feature object */

       type.param_is_label = 1;
       type.param_is_matrx = 0;
       type.param_is_number = 0;
       type.param_is_reg_name = 0;

       /* check 1st parameter addressing type for given command */
       param_chk = addrsn_check(type, 1, cmd_reqs);

       /* report err and return if 1st param addressing error occures */
       err_check1(param_chk == PARAM_FORBIDDEN, 300, *str, CMD_ERR)

       /* addressing is valid :*/
       (*i_item)->param1.param_type.param_is_label = 1;
       (*i_item)->param1.param_type.param_is_matrx = 0;
       (*i_item)->param1.param_type.param_is_number = 0;
       (*i_item)->param1.param_type.param_is_reg_name = 0;
       (*i_item)->param1.param_type.no_param = 0;
       (*i_item)->param1.params_union.lbl.label_addr = TO_BE_DEFINED;
       /* copy label name to heap and return its pointer : */
       /* now we know that txt is valid label name.
       copy label name to heap */
       heap_label_param_name = heap_string_duplicate_safe(txt,MAX_MACRO_AND_LABEL_LENGTH+2);
       (*i_item)->param1.params_union.lbl.label_name = heap_label_param_name;
       (*i_item)->param1.params_union.lbl.validity.label_addr_defined = 0;
       (*i_item)->param1.params_union.lbl.validity.label_param_syntax_valid = 1;
       (*i_item)->param1.params_union.lbl.validity.label_length_valid = 1;
       (*i_item)->status.params_num += 1;
       (*i_item)->status.param1_found = 1;
       (*i_item)->status.src_addrsng = 1; /* label */

       /* add label parameter name to labels table , sign it as parameter only */
       reset_feature(&label_feature);
       label_feature.is_cpu_cmd_param = 1;
       /* because it is parameter - no new address for label will be defined ! 
          so address value is TO_BE_DEFINED 
       */
       add_label_feature(&global_labels_table_ptr, heap_label_param_name,
                                       label_feature, TO_BE_DEFINED);


       return CMD_VALID;
}

/* function number_1st_param_define:

       define and check number parameter (1st parameter)
       parameters:
              i_item : instruction item object
              cmd_reqs : requirements for given command
              str : pointer to pointer to parameters string
              txt : label text
              param_num : parameter number
              line_num : line number
        return :
            CMD_ERR   (-1) : command is not valid
            CMD_VALID ( 1) : command is valid
*/
int number_1st_param_define(i_item_ptr *i_item, keywords_item_ptr cmd_reqs, char **str, 
                            char *txt, int param_num, int line_num){
    int param_chk; /* parameter addressing check result */
    param_type type; /* parameter type object */

    type.param_is_label = 0;
    type.param_is_matrx = 0;
    type.param_is_number = 1;
    type.param_is_reg_name = 0;

    /* check 1st parameter addressing type for given command */
    param_chk = addrsn_check(type, 1, cmd_reqs);

    /* report err and return if 1st param addressing error occures */
    err_check1(param_chk == PARAM_FORBIDDEN, 302, *str, CMD_ERR)

    /* addressing is valid :*/
    (*i_item)->param1.param_type.param_is_label = 0;
    (*i_item)->param1.param_type.param_is_matrx = 0;
    (*i_item)->param1.param_type.param_is_number = 1;
    (*i_item)->param1.param_type.param_is_reg_name = 0;
    (*i_item)->param1.param_type.no_param = 0;

    (*i_item)->param1.params_union.num.num = param_num;
    (*i_item)->param1.params_union.num.validity.num_is_valid = 1;
    (*i_item)->status.params_num += 1;
    (*i_item)->status.param1_found = 1;
    (*i_item)->status.src_addrsng = 0; /* int number */

    return CMD_VALID;
}

/* function number_2nd_param_define:

       define and check number parameter (2nd parameter)
       parameters:
              i_item : instruction item object
              cmd_reqs : requirements for given command
              str : pointer to pointer to parameters string
              txt : label text
              praram_num : parameter number
              line_num : line number
        return :
            CMD_ERR   (-1) : command is not valid
            CMD_VALID ( 1) : command is valid
*/
int number_2nd_param_define(i_item_ptr *i_item, keywords_item_ptr cmd_reqs, char **str,
                            char *txt, int param_num, int line_num){
    int param_chk;   /* parameter addressing check result */
    param_type type; /* parameter type object */
    type.param_is_label = 0;
    type.param_is_matrx = 0;
    type.param_is_number = 1;
    type.param_is_reg_name = 0;                  /* check 1st parameter addressing type for given command */
    param_chk = addrsn_check(type, 2, cmd_reqs); /* report err and return if 1st param addressing error occures */
    if (param_chk == PARAM_FORBIDDEN)
    {
        print_err(global_err_list_head, 306, line_num, *str, "");
        return CMD_ERR;
    } /* addressing is valid :*/
    (*i_item)->param2.param_type.param_is_label = 0;
    (*i_item)->param2.param_type.param_is_matrx = 0;
    (*i_item)->param2.param_type.param_is_number = 1;
    (*i_item)->param2.param_type.param_is_reg_name = 0;
    (*i_item)->param2.param_type.no_param = 0;

    (*i_item)->param2.params_union.num.num = param_num;
    (*i_item)->param2.params_union.num.validity.num_is_valid = 1;
    (*i_item)->status.params_num +=1;
    (*i_item)->status.param2_found = 1;
    (*i_item)->status.dest_addrsng = 0; /* int number */

    return CMD_VALID;

}

/* function register_1st_param_define:

       define and check register parameter (1st parameter)
       parameters:
              i_item : instruction item object
              cmd_reqs : requirements for given command
              str : pointer to pointer to parameters string
              txt : label text
              reg_num : register number
              line_num : line number
        return :
            CMD_ERR   (-1) : command is not valid
            CMD_VALID ( 1) : command is valid
*/

int register_1st_param_define(i_item_ptr *i_item, keywords_item_ptr cmd_reqs, char **str,
                            char *txt, int reg_num, int line_num){
    
    int param_chk;   /* parameter addressing check result */
    param_type type; /* parameter type object */

    type.param_is_label = 0;
    type.param_is_matrx = 0;
    type.param_is_number = 0;
    type.param_is_reg_name = 1;

    /* check 1st parameter addressing type for given command */
    param_chk = addrsn_check(type, 1, cmd_reqs);

    /* report err and return if 1st param addressing error occures */
    err_check1(param_chk == PARAM_FORBIDDEN, 303, *str, CMD_ERR)

    /* store type of parameter */
    (*i_item)->param1.param_type.param_is_label = 0;
    (*i_item)->param1.param_type.param_is_matrx = 0;
    (*i_item)->param1.param_type.param_is_number = 0;
    (*i_item)->param1.param_type.param_is_reg_name = 1;
    (*i_item)->param1.param_type.no_param = 0;

    (*i_item)->param1.params_union.reg.reg_num = reg_num; /* store register name number*/
    (*i_item)->param1.params_union.reg.validity.reg_num_is_valid = 1;
    (*i_item)->param1.params_union.reg.validity.reg_syntax_is_valid = 1;
    (*i_item)->status.params_num += 1;
    (*i_item)->status.param1_found = 1;
    (*i_item)->status.src_addrsng = 3; /* register */

    return CMD_VALID;

}

/* function register_2nd_param_define:
       define and check register parameter (2nd parameter)
       parameters:
              i_item : instruction item object
              cmd_reqs : requirements for given command
              str : pointer to pointer to parameters string
              txt : label text
              reg_num : register number
              line_num : line number
        return : 
            CMD_ERR   (-1) : command is not valid
            CMD_VALID ( 1) : command is valid
*/
int register_2nd_param_define(i_item_ptr *i_item, keywords_item_ptr cmd_reqs, char **str,
                              char *txt, int reg_num, int line_num){
    
    int param_chk;   /* parameter addressing check result */
    param_type type; /* parameter type object */

    type.param_is_label = 0;
    type.param_is_matrx = 0;
    type.param_is_number = 0;
    type.param_is_reg_name = 1;                  /* check 2nd parameter addressing type for given command */
    param_chk = addrsn_check(type, 2, cmd_reqs); /* report err and return if 2nd param addressing error occures */
    err_check1(param_chk == PARAM_FORBIDDEN, 307, *str, CMD_ERR)(*i_item)->param2.param_type.param_is_label = 0;
    (*i_item)->param2.param_type.param_is_matrx = 0;
    (*i_item)->param2.param_type.param_is_number = 0;
    (*i_item)->param2.param_type.param_is_label = 0;
    (*i_item)->param2.param_type.param_is_reg_name = 1;
    (*i_item)->param2.param_type.no_param = 0; 

    (*i_item)->param2.params_union.reg.reg_num = reg_num; /* store register name number*/
    (*i_item)->param2.params_union.reg.validity.reg_num_is_valid = 1;
    (*i_item)->param2.params_union.reg.validity.reg_syntax_is_valid = 1;
    (*i_item)->status.params_num += 1;
    (*i_item)->status.param2_found = 1;
    (*i_item)->status.dest_addrsng = 3; /* register */

    return CMD_VALID;

}

/* function i_item_size_calc

        calculate command size in words (used to calculate next IC address for next command),
        including command word and params words.
        thew size depends on source and destination addressing methods, and
        also command definition word is included in it.

        return : 
            command size in words in CPU memory.

*/
int i_item_size_calc(i_item_ptr i_item){

    if (i_item->is_valid){

        if (i_item->status.params_num==2){

            if (i_item->status.src_addrsng==0){
                if (i_item->status.dest_addrsng == 0)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + ONE_WORD_DST ;

                if (i_item->status.dest_addrsng == 1)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + ONE_WORD_DST ;

                if (i_item->status.dest_addrsng == 2)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + TWO_WORDS_DST;

                if (i_item->status.dest_addrsng == 3)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + ONE_WORD_DST;
            }

            if (i_item->status.src_addrsng == 1)
            {
                if (i_item->status.dest_addrsng == 0)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + ONE_WORD_DST;

                if (i_item->status.dest_addrsng == 1)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + ONE_WORD_DST;

                if (i_item->status.dest_addrsng == 2)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + TWO_WORDS_DST;

                if (i_item->status.dest_addrsng == 3)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + ONE_WORD_DST;
            }

            if (i_item->status.src_addrsng == 2)
            {
                if (i_item->status.dest_addrsng == 0)
                    return CMD_WORD_SIZE + TWO_WORDS_SRC + ONE_WORD_DST;

                if (i_item->status.dest_addrsng == 1)
                    return CMD_WORD_SIZE + TWO_WORDS_SRC + ONE_WORD_DST;

                if (i_item->status.dest_addrsng == 2)
                    return CMD_WORD_SIZE + TWO_WORDS_SRC + TWO_WORDS_DST;

                if (i_item->status.dest_addrsng == 3)
                    return CMD_WORD_SIZE + TWO_WORDS_SRC + ONE_WORD_DST;
            }

            if (i_item->status.src_addrsng == 3)
            {
                if (i_item->status.dest_addrsng == 0)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + ONE_WORD_DST;

                if (i_item->status.dest_addrsng == 1)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + ONE_WORD_DST;

                if (i_item->status.dest_addrsng == 2)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + TWO_WORDS_DST;

                if (i_item->status.dest_addrsng == 3)  /* !!!! special case !!!!*/
                    return CMD_WORD_SIZE + ONE_WORD_SRC_DST;
            }
        }
        /* if number of params is 1, only dest param is used */
        if (i_item->status.params_num == 1)
        {

            if (i_item->status.dest_addrsng == 0)
                return CMD_WORD_SIZE + ONE_WORD_DST;

            if (i_item->status.dest_addrsng == 1)
                return CMD_WORD_SIZE + ONE_WORD_DST;

            if (i_item->status.dest_addrsng == 2)
                return CMD_WORD_SIZE + TWO_WORDS_DST;

            if (i_item->status.dest_addrsng == 3)
                return CMD_WORD_SIZE + ONE_WORD_DST;
        }
        /* in case of no parameters */
        if (i_item->status.params_num == 0)
        {
                return CMD_WORD_SIZE ;
        }

    } /* if (i_item->is_valid)*/

    return 0;
} /* i_item_size_calc*/

/*

    function data_directive_process:

         processes .data directive
         each new data takes one word in data memory

        return : 
        DATA_ERR   (-1) : data params error
        DATA_VALID ( 0) : data valid

        Note:  .data directive must have at leat one paramter. 
*/

int data_directive_process(char **str, int line_num, data_blk_item_ptr *d_item, 
                            keywords_item_ptr cmd_reqs, int * data_cnt)
{
    int text_srch_res; /* result of text find */
    int str2int_res ; /* result of convert string to integer */ 
    int dec_val = 0 ; /* decimal data value , must check limits of 10 bit: -512 to 511 */
    char * ptr ; /* pointer to string */
    char txt[MAX_LINE_SIZE] = {'\0'}; /* temp char array */

    (*d_item)->flags.is_str_dot_directive = 1;
    ptr = *str;
    /* check if there is at least one white char after the .data command */
    err_check1(((*ptr != ' ')&&(*ptr!= '\t')), 400, *str, CMD_ERR);
    /* increment pointer by one char */
    ptr++;
    /* if next pointer points to EOS it is command without parameters */
    err_check1(*ptr == EOS, 401, *str, CMD_ERR);
    /* if next pointer points to ',' it is command without parameters */
    err_check1(*ptr == EOS, 403, *str, CMD_ERR);


    (*d_item)->start_addr = *data_cnt;
    (*d_item)->directive_name = heap_string_duplicate_safe(cmd_reqs->keyword,MAX_MACRO_AND_LABEL_LENGTH);
    (*d_item)->size_in_words = 0;
    (*d_item)->flags.is_mat_dot_directive = 0;
    (*d_item)->flags.is_data_dot_directive = 1;
    (*d_item)->flags.is_str_dot_directive = 0;
    (*d_item)->flags.err_has_invalid_label = 0;
    (*d_item)->flags.err_has_no_operands = 0;
    (*d_item)->flags.error_in_parameters = 0;
    (*d_item)->flags.is_valid_directive = 1;

    do
    {

        /* search for parameter with ',' delimiter */
        text_srch_res = gen_text_find(&ptr, ',', txt);
        /* increment pointer by one char */
        ptr++;

        /* report error in case of two text blocks not separated with ',' */
        err_check1(text_srch_res == GOT_SECOND_TXT_BLK, 402, *str, DATA_ERR)

        /* report error in case of  ',' delimiter without data */
        err_check1(text_srch_res == GOT_WHITES_AND_DELMTR ||
                    text_srch_res == GOT_DLMTR_AT_START, 403, *str, DATA_ERR)


        /* report error in case of  ',' delimiter without data after it */
        err_check1( text_srch_res == GOT_WHITES_AND_EOS ||
                    text_srch_res == GOT_EOS_AT_START  , 406, *str, DATA_ERR)


        if (text_srch_res == GOT_TXT_AND_DLMTR || text_srch_res == GOT_TXT_AND_EOS){

            /* convert txt to 10 bit number (decimal) */
            str2int_res = str2int(txt, &dec_val);
            /* report error in case when data has syntax error */
            err_check1(str2int_res == 0, 404, *str, DATA_ERR)
            /* check that decimal value is in limits -512 ... 511 */
            err_check1(dec_val < -512 || dec_val > 511, 405, *str, DATA_ERR)
            /* now add the decimal number to list of data */
            add_item_to_num_lnk_lst(&((*d_item)->num_head), &((*d_item)->num_tail), dec_val);
            (*d_item)->size_in_words += 1;
        }

    }
    while (text_srch_res == GOT_TXT_AND_DLMTR);

    (*d_item)->is_valid = 1;

    return DATA_VALID;
}



/* 
   function init_label_status

        initializes label_status object
*/
void init_label_status(is_cmd_lbl_stat *label_status_p)
{

    /* init label status */
    label_status_p->label_is_ok = 0;
    label_status_p->label_is_too_long = 0;
    label_status_p->has_column_at_end = 0;
    label_status_p->non_aphabet_start = 0;
    label_status_p->has_non_alphanum = 0;
    label_status_p->equals_cmd_name = 0;
    label_status_p->equals_macro_name = 0;
}

/*  function d_item_init

        creates and initializes data item object.
*/
void d_item_init(data_blk_item_ptr *d_item, keywords_item_ptr cmd_reqs,int data_cnt){

    
    (*d_item)->directive_name = 
        heap_string_duplicate_safe(cmd_reqs->keyword,MAX_MACRO_AND_LABEL_LENGTH);
    

    (*d_item)->start_addr = data_cnt;
    (*d_item)->size_in_words = 0;
    (*d_item)->label_name = NULL;
    (*d_item)->flags.err_has_invalid_label = 0;
    (*d_item)->flags.err_has_no_operands = 0;
    (*d_item)->flags.error_in_parameters = 0;
    (*d_item)->flags.is_data_dot_directive = 0;
    (*d_item)->flags.is_mat_dot_directive = 0;
    (*d_item)->flags.is_str_dot_directive = 0;
    (*d_item)->flags.is_valid_directive = 0;
    (*d_item)->num_head = NULL;
    (*d_item)->num_tail = NULL;
    (*d_item)->next = NULL;


} /* d_item_init */

/*

        function string_directive_process:

             processes .string directive
             each new data takes one word in data memory

            return :
            DATA_ERR   (-1) : data params error
            DATA_VALID ( 0) : data valid

            Note:  .string directive must have exectly one paramter between " " .
                   only visible ASCII codes can be used, as codes from 32 to 126

*/

int string_directive_process(char **str, int line_num, data_blk_item_ptr *d_item,
                                 keywords_item_ptr cmd_reqs, int *data_cnt)
{

    char *ptr; /* pointer to string */
    char txt[MAX_LINE_SIZE] = {'\0'}; /* temp char array */
    int text_srch_res; /* text search result */
    int i ; /* index */
    int data_err_flag = 0 ; /* data error flag */
    int dec_val; /* decimal value of ascii char */

    (*d_item)->flags.is_str_dot_directive = 1;
    ptr = *str;
    /* check if there is at least one white char after the .string command */
    err_check1(((*ptr != ' ') && (*ptr != '\t')), 450, *str, DATA_ERR);
    /* add new item to cpu data linked list */
    


    /* search for text parameter with EOS delimiter */
    text_srch_res = gen_text_find(&ptr, EOS, txt);

    /* error : no text */
    err_check1(text_srch_res == GOT_WHITES_AND_DELMTR ||
                   text_srch_res == GOT_WHITES_AND_EOS, 451, *str, DATA_ERR);
    /* error : extra text */
    err_check1(text_srch_res == GOT_SECOND_TXT_BLK , 452, *str, DATA_ERR);

    if (text_srch_res == GOT_TXT_AND_EOS || text_srch_res == GOT_TXT_AND_DLMTR){
        
        /* first character in string must be '\"'  */
        err_check1(txt[0] != '\"', 453, *str, DATA_ERR);
        err_check1(txt[strlen(txt) - 1] != '\"', 454, *str, DATA_ERR);

        /* now we have string from txt[1] and up to txt[strlen(txt) - 2]*/
        for (i = 1; i < strlen(txt) - 1 ; i++)
        {
            if (txt[i]<32 || txt[i]>126)
            {   /* check that ASCII codes values in string are in valid range */
                print_err(global_err_list_head, 455, line_num, *str, "");
                data_err_flag = 1;
                break;
            }

            dec_val = txt[i];
            add_item_to_num_lnk_lst(&((*d_item)->num_head), &((*d_item)->num_tail), dec_val);
            (*d_item)->size_in_words += 1;
        };

        if (data_err_flag == 1){
            (*d_item)->flags.error_in_parameters = 1;
            (*d_item)->is_valid= 0;
            return DATA_ERR;
        } else {
            /* add value '\0' to the list */    
            add_item_to_num_lnk_lst(&((*d_item)->num_head), &((*d_item)->num_tail), 0);
            (*d_item)->size_in_words+=1;
            (*d_item)->is_valid = 1;
            (*d_item)->flags.is_mat_dot_directive = 0;
            (*d_item)->flags.is_data_dot_directive = 0;
            (*d_item)->flags.is_str_dot_directive = 1;
            (*d_item)->flags.err_has_invalid_label = 0;
            (*d_item)->flags.err_has_no_operands = 0;
            (*d_item)->flags.error_in_parameters = 0;
            (*d_item)->flags.is_valid_directive = 1;
            return DATA_VALID;
        } /* if (data_err_flag == 1)*/
    } /* if (text_srch_res == GOT_TXT_AND_EOS || text_srch_res == GOT_TXT_AND_DLMTR) */

    return DATA_VALID;

} /* string_directive_process*/

/*

    function mat_directive_process:

         processes .mat directive that can have 0...inf parameters
         calculates matrix total size by mult. rows x cols
         analyzes and stores the numbers that appear in command.
         complement numbers with 0 , if number not defined in parameters list.

         If defined too many parameters - it is error.

         each new data takes one dditional word in data memory
            
            Note: after .mat directive must appear at least one white char. 
                  
        return :
        DATA_ERR   (-1) : data params error
        DATA_VALID ( 0) : data valid
*/
int mat_directive_process(char **str, int line_num, data_blk_item_ptr *d_item,
                      keywords_item_ptr cmd_reqs, int *data_cnt)
{

    char *ptr;                        /* pointer to string */
    char txt[MAX_LINE_SIZE] = {'\0'}; /* temp char array */
    int text_srch_res;                /* text search result */
    int rows ;                        /* rows number */
    int rows_res ;                    /* result of rows translate from str to int*/
    int cols ;                        /* columns number */
    int cols_res ;                    /* result of columns translate from str to int */
    int data_words_num;               /* number of data words */
    int i ;                           /* index */
    int data_ended_in_str = 0 ;       /* signs that data is finished in string */
    int mat_data;                     /* variable to store matrix data*/ 
    int mat_data_res ;                /* result od mat_data translate from str to int */

    (*d_item)->flags.is_mat_dot_directive = 1;
    ptr = *str;
    /* check if there is at least one white char after the .mat command */
    err_check1(((*ptr != ' ') && (*ptr != '\t')), 470, *str, DATA_ERR);

    /* now search for '[' */
    text_srch_res = gen_text_find(&ptr, '[', txt);

    /* error: did not find '[' after .mat command */
    err_check1(text_srch_res != GOT_WHITES_AND_DELMTR, 471, *str, DATA_ERR);

    /* now increment pointer to the next position and search for text between '[' and ']' */
    ptr ++;
    text_srch_res = gen_text_find(&ptr, ']', txt);

    /* error: find more than one text before ']'  */
    err_check1(text_srch_res == GOT_SECOND_TXT_BLK, 472, *str, DATA_ERR);
    /* error: did not fine text before ']' */
    err_check1(text_srch_res == GOT_DLMTR_AT_START ||
                   text_srch_res == GOT_EOS_AT_START ||
                   text_srch_res == GOT_WHITES_AND_EOS
                   , 473, *str, DATA_ERR);
    /* error: got text but not found ']' */
    err_check1(text_srch_res == GOT_TXT_AND_EOS, 474, *str, DATA_ERR);

    /* now we suppose that pointer point to ']' , veryfy that */
    err_check1(*ptr != ']', 475, *str, DATA_ERR);
    /* now verify that the rows text inside [] is integer number */
    rows_res = str2int(txt,&rows);
    err_check1(rows_res == NOT_NUM, 476, *str, DATA_ERR);
    /* increment pointer position and verify that '[' appears closly to ']' */
    ptr++;
    err_check1(*ptr != '[', 477, *str, DATA_ERR);
    /* increment pointer position */
    ptr++;
    /* now search for columns number before ']' */
    text_srch_res = gen_text_find(&ptr, ']', txt);

    /* error: find more than one text before ']'  */
    err_check1(text_srch_res == GOT_SECOND_TXT_BLK, 472, *str, DATA_ERR);
    /* error: did not fine text before ']' */
    err_check1(text_srch_res == GOT_DLMTR_AT_START ||
                   text_srch_res == GOT_EOS_AT_START ||
                   text_srch_res == GOT_WHITES_AND_EOS,
               478, *str, DATA_ERR);
    /* error: got text but not found ']' */
    err_check1(text_srch_res == GOT_TXT_AND_EOS, 479, *str, DATA_ERR);
    
    /* now we suppose that pointer points to second ']' , veryfy that */
    err_check1(*ptr != ']', 480, *str, DATA_ERR);

    /* now check that the columns number is valid: */
    cols_res = str2int(txt,&cols);
    err_check1(cols_res == NOT_NUM, 481, *str, DATA_ERR);

    /* calculate number of parameters : */
    data_words_num = (*d_item)->size_in_words = cols * rows;

    for (i = 1; i <= data_words_num; i++){
        /* search for i-th data in the string and check its validity */
        if (data_ended_in_str == 0)
        {
            /* increment ponter from delimiter position */
            ptr++;
            /* analyse text up the next delimiter */    
            text_srch_res = gen_text_find(&ptr, ',', txt);
            /* report err if received extra text*/
            err_check1(text_srch_res == GOT_SECOND_TXT_BLK, 482, *str, DATA_ERR);
            /* report err if received extra ',' delimiter */
            err_check1(text_srch_res == GOT_DLMTR_AT_START, 484, *str, DATA_ERR);
            /* analyse received data parameters */
            if (text_srch_res == GOT_TXT_AND_EOS || text_srch_res == GOT_TXT_AND_DLMTR)
            {
                /* convert received data to integer */
                mat_data_res = str2int(txt, &mat_data);
                /* report error in case the data is not valid */
                err_check1(mat_data_res == NOT_NUM, 483, *str, DATA_ERR);
                /* now we get valid integer number */
                /* insert it into linked list of parameters */
                add_item_to_num_lnk_lst(&((*d_item)->num_head), &((*d_item)->num_tail), mat_data);
                /* set data ended flag if it was EOS after data finished */
                if (text_srch_res == GOT_TXT_AND_EOS)
                    data_ended_in_str = 1;
                /* jump to next for iteration */
                continue;     
            }

            if (text_srch_res == GOT_WHITES_AND_EOS || text_srch_res == GOT_EOS_AT_START )
                data_ended_in_str = 1;
        }
        /* if i-th data is not found in the string complement it with zero */
        add_item_to_num_lnk_lst(&((*d_item)->num_head), &((*d_item)->num_tail), 0);

    };

    /* err : if data did not ended in string, may be it contains any extra text */
    /* analyse text up the next delimiter in this case */
    if (data_ended_in_str == 0){
        text_srch_res = gen_text_find(&ptr, EOS, txt);
        err_check1(mat_data_res == GOT_TXT_AND_EOS ||
                       mat_data_res == GOT_TXT_AND_DLMTR ||
                       mat_data_res == GOT_SECOND_TXT_BLK,
                   485, *str, DATA_ERR);
    }
    
    (*d_item)->flags.is_mat_dot_directive = 1;
    (*d_item)->flags.is_data_dot_directive = 0;
    (*d_item)->flags.is_str_dot_directive = 0;
    (*d_item)->flags.err_has_invalid_label = 0;
    (*d_item)->flags.err_has_no_operands = 0;
    (*d_item)->flags.error_in_parameters = 0;
    (*d_item)->flags.is_valid_directive = 1;
    
    return DATA_VALID;

}

/*
    function extern_directive_process:

            extern directive does not generate any data (only specifies label's type).
            It defines only one label parameter as external label.

            This label will be checked and inserted in table of labels (binary search tree).
            This function does not check if the label defined as entry also.
            This will be checked later.
        return :
        DATA_ERR   (-1) : in case of any error detection
        DATA_VALID ( 0) : in case of valid processing
*/
int extern_directive_process(char **str, int line_num){

    char *ptr;                        /* pointer to string */
    char txt[MAX_LINE_SIZE] = {'\0'}; /* temp char array */
    int text_srch_res;                /* text search result */
    is_cmd_lbl_stat label_status;     /* label status object */
    label_features label_feature;     /* label feature object */

    ptr = *str;
    /* check if there is at least one white char after the .extern command */
    err_check1(((*ptr != ' ') && (*ptr != '\t')), 501, *str, DATA_ERR);

    /* now search for label name */
    text_srch_res = gen_text_find(&ptr, EOS, txt);

    /* check if there is at least one white char after the .extern command */
    err_check1(text_srch_res == GOT_WHITES_AND_EOS || text_srch_res == GOT_WHITES_AND_DELMTR 
               || text_srch_res == GOT_EOS_AT_START , 502, *str, DATA_ERR);

    /* check if there is extra text after the .extern command */
    err_check1(text_srch_res == GOT_SECOND_TXT_BLK , 503, *str, DATA_ERR);

    /* now check the label syntax */

    /* check if the text is label */
    is_label(txt, NO_COLON_CHK, &label_status);

    /* err if label equals keyword name */
    err_check1(label_status.equals_cmd_name, 504, *str, DATA_ERR);
    /* err if label equals macro name */
    err_check1(label_status.equals_macro_name, 505, *str, DATA_ERR);
    /* err if label is too long */
    err_check1(label_status.label_is_too_long, 506, *str, DATA_ERR);
    /* err if label has non alphanum after first char*/
    err_check1(label_status.has_non_alphanum, 507, *str, DATA_ERR);
    /* err if label has non alphbet first char*/
    err_check1(label_status.non_aphabet_start, 508, *str, DATA_ERR);

    /* add label to labels table with .extern flag */
    reset_feature(&label_feature);
    label_feature.is_extrn = 1;
    add_label_feature(&global_labels_table_ptr, txt, label_feature, TO_BE_DEFINED);

    return DATA_VALID;

}

/*
    function entry_directive_process:

            entry directive does not generate any data (only specifies label's type).
            It defines only one label parameter as entry label.

            This label will be checked and inserted in table of labels (binary search tree).
            This function does not check if the label defined as external also.
            This will be checked later.     

        return :
        DATA_ERR   (-1) : in case of any error detection
        DATA_VALID ( 0) : in case of valid processing
*/
int entry_directive_process(char **str, int line_num)
{

    char *ptr;                        /* pointer to string */
    char txt[MAX_LINE_SIZE] = {'\0'}; /* temp char array */
    int text_srch_res;                /* text search result */
    is_cmd_lbl_stat label_status;     /* label status object */
    label_features label_feature;     /* label feature object */

    ptr = *str;
    /* check if there is at least one white char after the .extern command */
    err_check1(((*ptr != ' ') && (*ptr != '\t')), 511, *str, DATA_ERR);

    /* now search for label name */
    text_srch_res = gen_text_find(&ptr, EOS, txt);

    /* check if there is at least one white char after the .extern command */
    err_check1(text_srch_res == GOT_WHITES_AND_EOS || text_srch_res == GOT_WHITES_AND_DELMTR 
               || text_srch_res == GOT_EOS_AT_START, 512, *str, DATA_ERR);

    /* check if there is extra text after the .extern command */
    err_check1(text_srch_res == GOT_SECOND_TXT_BLK, 513, *str, DATA_ERR);

    /* now check the label syntax */

    /* check if the text is label */
    is_label(txt, NO_COLON_CHK, &label_status);

    /* err if label equals keyword name */
    err_check1(label_status.equals_cmd_name, 514, *str, DATA_ERR);
    /* err if label equals macro name */
    err_check1(label_status.equals_macro_name, 515, *str, DATA_ERR);
    /* err if label is too long */
    err_check1(label_status.label_is_too_long, 516, *str, DATA_ERR);
    /* err if label has non alphanum after first char*/
    err_check1(label_status.has_non_alphanum, 517, *str, DATA_ERR);
    /* err if label has non alphbet first char*/
    err_check1(label_status.non_aphabet_start, 518, *str, DATA_ERR);

    /* now we suppose that the label is valid */
    /* add label to labels table with .extern flag */
    reset_feature(&label_feature);
    label_feature.is_entry = 1;
    add_label_feature(&global_labels_table_ptr, txt, label_feature, TO_BE_DEFINED);

    return DATA_VALID;
}




/* function upd_d_items_addr :

    add same offset value to all items addresses in linked list
    (will be done at finish of assembly 1st stage )

    parameters :
        head, tail : start and finish pointers of linked list
        addr_offset : address offset number that will be added to all
                      addr parameters of all items in the list.
    return : 
        last program address
*/
int upd_d_items_addr(data_blk_item_ptr head, data_blk_item_ptr tail, int addr_offset)
{

    data_blk_item_ptr item = head;
    data_blk_item_ptr prev_item = NULL;
    int last_addr; /* total program size*/

    if (!head)
        return 0; /* no data items */
    do
    { /* add offset value to address param in all data items */
        item->start_addr += addr_offset;
        prev_item = item; /* to store last item pointer at end of loop */
        item = item->next;
        
    } while (item);

    /* calculate last program address */
    last_addr = prev_item->start_addr + prev_item->size_in_words - 1;

    return last_addr;
}


/* 
 function labels_table_data_addr_upd
    Update data addresses in labels table 

    if the labele points to .data or to .str or to .mat
    add offset to its address value. 

    The recursive update is performed "in order" (Go left, update parent, go right).

*/

void labels_table_data_addr_upd(lable_item_ptr label_table, int addr_offset)
{

    if (label_table == NULL)
    {
        ; /* do nothing */
    }
    else
    {
        /* go left*/
        labels_table_data_addr_upd(label_table->left, addr_offset);
        
        /* update parent*/
        if(label_table->features.points_to_data || 
           label_table->features.points_to_mat  ||
           label_table->features.points_to_str)
        {

            label_table->label_address += addr_offset;
        }    

        /* go right*/
        labels_table_data_addr_upd(label_table->right, addr_offset);
    }
}
