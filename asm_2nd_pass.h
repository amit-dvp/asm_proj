#if !defined(asm_2nd_pass_types_h)

#define asm_2nd_pass_types_h

/*
   this file defines types for second assembler pass

*/

/*
    function asm_2nd_pass

        second stage(pass) of assembly
*/

void asm_2nd_pass(char * obj_fname, char* ext_fname, char * ent_fname, 
                   int * cpu_instr_mem_size, int * cpu_data_mem_size);
/*

       function fill_bin_data:
            fills binary data in linked list of CPU commands items.
*/
void fill_bin_data(char * ent_fname, char * ext_fname, i_item_ptr i_head, 
                i_item_ptr i_tail, keywords_item_ptr cmd_reqs);



/* function object_file_gen

    generates an object file and may be part of .ent file

*/
void object_file_gen(char* obj_fname, char* ext_fname, char * ent_fname,  
                     i_item_ptr i_head, i_item_ptr i_tail,
                     data_blk_item_ptr d_head, data_blk_item_ptr d_tail,
                     int *cpu_instr_mem_size, int *cpu_data_mem_size);

/* function set_1st_parameter_code_words

   set code words for 1-st parameter in CPU instruction.
   signs code words with .exist feature, to allow 2-nd parameter function
   to fined what word is the last.

   In case when 1-st and 2-nd parameter are registers (that is special case)
   the function does not set set 1-st parameter at all, special function will be
   prepared for this purpose.

*/
void set_1st_parameter_code_words(char * ent_fname, char * ext_fname, i_item_ptr *item );

/*
 function append_to_ext_file

    appends label to .ext file
    for every separate label opens, inserts and closes the .ext file.
*/
void append_to_ext_file(char * ext_fname, char *label_name, uint addr);

/*
 function append_to_ent_file

    appends label to .ext file
    for every separate label opens, inserts and closes the .ext file.
*/
void append_to_ent_file(char * ent_fname, char *label_name, uint addr);

/* function set_2nd_parameter_code_words

        set code words for 2-nf parameter in CPU instruction.
        signs code words with .exist feature, to allow 2-nd parameter function
        to fined what word is the last.

        In case when 1-st and 2-nd parameter are registers (that is special case)
        the function does not set set 1-st parameter at all, special function will be
        prepared for this purpose.

*/
void set_2nd_parameter_code_words(char * ext_fname, i_item_ptr *item);

    /*
        function uint2base4str
        translates unsigned int number to base4 string
        suppose that number has 10 significant bits.
        returns value of global general string pointer global_gen_str_pointer.
        Note that previous value of global_gen_str_pointer will be released, since it is
        temporary pointer.


        parameters:
            bits_number : length of binary number , can be 10 or 8 bits

    */
    char *uint2base4str(uint number, int bits_number);

    /* function  param2_start_word_num

            calculates start word number for 2nd parameter for given instruction command
            Note that command itsel takes 1 word

            return :
                command size in words in CPU memory.

    */
    int param2_start_word_num(i_item_ptr i_item);


    

#endif
