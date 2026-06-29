#include "assembler.h"
#include "assembler_globals.h"

/*
     This program is assembler proj solution.
     
     The solution has several stages:

        1. Before analisys start, we perform preliminary preparations:
                1.1. Prepare keywords binary search tree (BST) data structure wich actually
                     stores project requirements. This binary tree is loaded to heap
                     memory, while the pointer to it is global pointer (the reason is
                     described later).
                     This BST contains all keywords (as keys) that are used in project.
                     Each keyword item in BST has related payload
                     (for example, command type, if it is CPU command or
                     CPU data directicve or register name).
                     For CPU comand we define how many parameters it demands,
                     what addressing modes can be used for parameters, command opcode, etc.
                     For data directive we define minimum and maximum num of operands.

                     Note : keyword names stored in binary search tree are used in following
                            stages of project to check that label name or macro name
                            is different from keyword name name.

                        Related files with functions and definitions:
                            mngr_keywords_list.c
                            mngr_keywords_list_types.h

                1.2. Linked list with most errors definitions in project
                     also loaded to heap memory. This linked list uses eror's numbers as keys and
                     each item in linked list stores error definition string as
                     structure payload.

                        Related files with functions and definitions:
                            mngr_errors.c ,
                            mngr_errors_types.h

                1.3  We define and prepare global pointers variables to all
                     dynamically allocated heap data or files.
                     This is done to allow release dynamically allocated data
                     in right order at any place of program, for example, before performing
                     exit as result of file open error or dynamic memory allocation fault.

                        Related file that contains all data release functions :
                            release_dynamic_memory_and_files.c

        1. After it pre-assembler is run. Its run is performed in 2 sub-stages:

                1.1. Pre-assembler first pass.

                        First pass scans input .as file for
                            validity of macro definitions (mcro command),
                            validity of macro name within macro definition
                            validity of macro definition finish (mcroend command).
                            validity of macro substitution command (i.e. macro name only).
                        Also checks length of every line (must be less of 80).

                        Valid macro names are added into macros binary search tree entries
                        for future use in second pre-assembler stage and first stage's macro
                        substitution commands check.

                        Before addition of macro name to binary search tree we check that it is
                        different from any keyword defined in keywords binary search tree.

                        We use macros binary search tree as macro's
                        table to check later that label names are different from macros names.

                        This tree is arranged according to macros names keys.
                                We acess every element in this tree through global pointer :
                                            global_macro_table_ptr
                                Each macro table entry can contain pointer to linked list
                                of macro lines assotiated with it:
                                            macro_table_entry.line_list_head
                                Each macro table entry can contain pointer to macro name:
                                            macro_table_entry.macro_name

                        Macro substitution commands must contain single word with macro name only.
                        In this stage we detect error if macro definition appears as first word
                        but there are additional words/non_white_chars after it.
                        In case of any error detection this tree memory will be released (freed).

                        Any error in mcro or macroend lines or in any line length will be reported.

                        In case of any error, the second pre-assembly stage will not start.
                        The function returns number of errors.

                        Note : macro names stored in binary search tree are used in following
                               stages of project to check that label name is different
                               from macro name.

                        Related files with functions and definitions:
                                pre_asm.c

                        Related files with macro's definitions binary search tree:
                                mngr_macro_tbl.c
                                mngr_macro_tbl_types.h

                1.2. Pre-assembler second pass.

                        This stage performs macros expansion and .am file preparation.
                        Based on finite state machine.

                        Macro SUBSTITUTION commands are checked in second scan stage
                        We check macro's name appearance only in first word of line
                        (again macro's names stores at first stage ) and check
                        that additional words does not appear.

                        At the second stage pre-assembler prepares an expanded .am file.

                        It passes through already validated macro definitions and macro end
                        and stores the macro substitution lines in lines linked list
                        associated with with appropriate item in binary search tree
                        (the keys of BST items are macro names).



                        Macro definition, macro body and macro end lines are not be copied into
                        .am file.

                        Each line with single word that match macro's name in macros
                        table causes to insertion of stored before macro lines
                        from linked list of macro lines into .am file.

                        Comment lines and empty lines are not be copied into .am file.
                        Note that the last line of file still is empty and contains end of file character.

                        Related files with functions and definitions:
                                pre_asm.c

        2. After it assembler stage begins. It is also divided into 2 sub-stages:
                2.1.  First assembler pass,
                            Analyses .am file line by line,
                            and prepares data structures with information that is anough
                            for second assembler pass to prepare object file and
                            .ext and .ent files.

                            Firstly checks if command line has label and if the label exists
                            stores it in labels binary search tree along with its metadata.
                            After it checks if the command is CPU command or CPU data directive or
                            .extern or .entry. According to type of command
                            analyses, checks and stores commands parameters.

                            While performing command lines analysis we prepare also labels table
                            (binary search tree structure based on label name keys),
                            with metadata stored for each lable (for example is it label of
                            CPU command, data directive, extern , entry , etc.).

                            Checks of most errors in the project is done in 1st assembler stage.
                            Errors list with almost all errors is defined in mngr_errors.c file.
                            We use errors linked list to withdraw the relevant error definition.

                            Also in project were defined several macros :

                                1. Macro that performes error check, finds error message according to
                                error ID number and prints error message.
                                (see macro err_check1 defined in asm_1st_pass.h)

                                2. Macro that performs substitution of debug instructions , as printf
                                for examnle, if debug enable define is set (see DEBUG_PRINT_EN in
                                file assembler.h )

                            At the end of first assembly pass we calculate the instruction memory size,
                            and update with it using it as offset all cpu data adresses
                            cpu data labels addresses.

                            We use the later information to update previous label data flags.
                            The special function add_label_feature in file mngr_labels_tbl.c
                            performs OR-in between the previous and new information
                            Only the flags that were not rise can be rised by new information update.
                            (see )

                            At the end if first assembly stage we check that label's metadata features
                            does not collide. For examle label can not be defined as external and
                            as internal command label, or it can not be defined both as cpu instruction label
                            and cpu data label, etc.
                            For this purpose we use binary logic (for its effectiveness)
                            (see function check_feature_validity in mngr_labels_tbl.c file)

                            If any error is detected, we store it and continue analysis until
                            the finish of first stage is acheived. If any error detected,
                            the program does not continue to second assembly stage.

                            Related files with functions and definitions for 1st
                                assembler stage:
                                    cmd_line_analyse.c
                                    cmd_line_analyse_types.h
                                    asm_1st_pass.c
                                    asm_1st_pass.h

                            Following data structures are prepared in 1-st assembler stage:
                            1.  data_blk_item : linked list to store information of data directives.
                                This linked list is arranged in oeder of data directives.
                                We access every element in this linked list through 2 global pointers :
                                            global_d_head
                                            global_d_tail
                                Every data_blk_item has associated with it linked list of data that
                                can be accessed through following pointers:
                                            data_blk_item.num_head
                                            data_blk_item.num_tail

                                Related files with functions and definitions for CPU
                                    data linked list:
                                        mngr_cpu_data_lnk_lst.c
                                        mngr_cpu_data_lnk_lst_types.h

                            2.  i_item :  linked list to store information of CPU instructions .
                                This linked list is arranged according to order of CPU commands.
                                We acceess every element in this linked list through 2 global pointers :
                                            global_i_head
                                            global_i_tail
                                    Related files with functions and definitions for instructions
                                    data linked list:
                                        mngr_cpu_instr_lnk_lst.c
                                        mngr_cpu_instr_lnk_lst_types.h

                            3.  label_item : binary search tree of labels ("labels table").
                                This tree is arranged according to labels names keys.
                                We access every element in this tree through following global pointer :
                                            global_labels_table_ptr
                                    Related files with functions and definitions for labels
                                    binary search tree :
                                        mngr_labels_tbl.c
                                        mngr_labels_tbl_types.h

                            Note : we use previous created macros binaery search tree and
                                   keywords binary search tree to check that labels names are different
                                   from macro and keywords names.

                            The above data structures are used as input to second assembly pass.


                2.2 Second assembler pass.
                       At second stage of assembly we prepare .ob , .ent and .ext files
                       based on information stored in first assembly stage.

                       Firstly we set all binary codes data information in
         
                       appropriate fields of cpu_instruction linked_list.
                       This is based on information already collected in first assembly stage.
                       (see fill_bin_data function in asm_2nd_pass.c file)
                       After it we generate output files (see object_file_gen function) using
                       translation of unsigned numbers to string with base 4 code 
                       number (see uint2base4str  function).


*/

int main(int argc, char *argv [] ) 
{
    int pre_asm_res = 0 ;            /* error counter result of pre assembly */
    int asm_1st_pass_res = 0 ;       /* assembly first pass result */
    int cpu_instr_mem_size = 0;      /* cpu instructions memory size */
    int cpu_data_mem_size = 0 ;      /* cpu data memory size         */
    int i ;                          /* index */
    FILE * fd = NULL;                /* file descriptor  */    

        /* if no params given */
        if (argc == NO_ARGS)
    {
        printf("Error : at least one parameter must be defined. \n");
        printf("Exitting...\n\n");
        exit(1);
    }



    /* create linked list of errors definitions */
    errors_list_create(&global_err_list_head);

    /* create linked list of keywords */
    global_keyword_table_ptr = keywords_bst_create();
    debug(keywords_table_print(global_keyword_table_ptr));

    /* perform files generation */
    for (i=1;i<argc;i++)
    {

        /* generate full file names in heap with assignment to global pointers */
        global_full_file_names_gen(argv[i]);
       
        printf("---------------------------------------------------\n");
        printf(" Processing %s file (parameter number: %d)\n", global_full_fname_as, i);    
        printf("---------------------------------------------------\n");

        /* perform first stage of pre-assembler */
        pre_asm_res = pre_asm_first_stage(global_full_fname_as);

        if (pre_asm_res)
        {           
                    /* free the dynamic memory before the next run */
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

           
            continue;
        }    

        /* perform second stage of pre-assembler */
        pre_asm_second_stage(global_full_fname_as, global_full_fname_am);


        /* perform 1-st stage of assembler */
        asm_1st_pass_res = asm_1st_pass(global_full_fname_am, &cpu_instr_mem_size, &cpu_data_mem_size);

        if (asm_1st_pass_res)
        {
             /* free the dynamic memory before the next run */        
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

            
            continue;
        }

        asm_2nd_pass(global_full_fname_ob, global_full_fname_ext, global_full_fname_ent,
                    &cpu_instr_mem_size, &cpu_data_mem_size);

        /*----------------------------------------------------------*/    
        /*  prepare log file with all data analysis results         */
        /*----------------------------------------------------------*/
 
        print_log_file(fd,global_full_fname_log, global_i_head,global_i_tail,global_d_head,
                    global_d_tail, global_labels_table_ptr); 


        /* free memory for next run */
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




    }

    /* free the reminder dynamic memory before finish the program */
    remainder_dynamic_memory_free();



    return 0;
}

    

/* 
    function print_log_file
        prints log file with following information:
            - cpu instructions data 
            - cpu data 
            - labels table
*/
void print_log_file(FILE * fd, char * log_fname, i_item_ptr i_head,i_item_ptr i_tail,
                    data_blk_item_ptr d_head, data_blk_item_ptr d_tail,
                    lable_item_ptr labels_table_ptr )
{
    
    int fres; /* result of close file  */
    
    /* open auxiliary log file to store all data structures info */
    fd = fopen(log_fname, "w");
    if (!fd)
    {
        printf("Error: can not open %s file in read mode.\n", log_fname);
        printf("Exitting...\n\n");
        release_all_dynamic_memory_and_files();
        exit(1);
    }
    fprintf(fd, "\n\n----------------------------------------------------------\n");
    fprintf(fd, "          log file with all data analysis results         \n");
    fprintf(fd, "                 of %s file processing                 \n", log_fname);
    fprintf(fd, "----------------------------------------------------------\n\n");
    fprintf(fd, "------------------ PRINT CPU INSTR. ITEMS ---------------\n");
    /* add to log file CPU instructions info */
    f_print_i_items_list(fd, global_i_head, global_i_tail);
    fprintf(fd, "\n\n------------ PRINT DATA ITEMS -----------------\n");
    /* add to log file CPU data info */
    f_print_d_items_list(fd, global_d_head, global_d_tail);
    fprintf(fd, "\n\n----------- PRINT LABELS TABLE -----------------\n");
    /* add to log file labels table info*/
    f_label_table_print(fd, global_labels_table_ptr);


    fres = fclose(fd);
    if(fres)
        printf("Error : %s file did not closed successfully ", log_fname);
    else    
        fd = NULL;
}




