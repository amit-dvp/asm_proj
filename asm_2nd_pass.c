/*  
 functions in this file perform second stage of assembly.
 */

#include "assembler.h"


/*
    function asm_2nd_pass 

        second stage(pass) of assembly
*/

void asm_2nd_pass(char * obj_fname, char* ext_fname, char * ent_fname, 
                   int * cpu_instr_mem_size, int * cpu_data_mem_size)
{



    printf("\n---------------------------------------\n");
    printf(" Assembler:\n\t2-nd stage is running.\n");
    printf("\tPrepare <%s> file.\n", obj_fname);
    printf("---------------------------------------\n");





    fill_bin_data(ent_fname, ext_fname,  global_i_head, global_i_tail, global_keyword_table_ptr);

    object_file_gen(obj_fname,ext_fname, ent_fname, global_i_head, global_i_tail, global_d_head,
                    global_d_tail, cpu_instr_mem_size, cpu_data_mem_size); 

    printf("\n----------------------------------------------------------\n");
    printf(" 2-nd assembler stage run is finished.\n");
    printf(" Files  %s , %s , %s were prepared.\n", global_full_fname_ext, 
             global_full_fname_ent, global_full_fname_ob);
    printf("----------------------------------------------------------\n");
}

/*
   function fill_bin_data: 
        fills binary data in linked list of CPU commands items.
*/
void fill_bin_data(char * ent_fname, char * ext_fname, i_item_ptr i_head, 
                    i_item_ptr i_tail, keywords_item_ptr cmd_reqs)
{
    i_item_ptr item = i_head;
    uint opcode = 0 ;
    uint src_addrsng = 0;
    uint dest_addrsng = 0;

    debug(printf("fill_bin_data is running\n"));

    /* do nothing if the commands list is empty */
    if (!i_head)
        return;

    do
    {   
        
               
        /*********  generate first code word (always exists) *******/
        
        opcode = item->status.opcode;
        /* shift opcode 6 bits left */
        opcode <<= 6;
        
        src_addrsng = item->status.src_addrsng;
        /* shift src_addrsng 4 bits left */
        src_addrsng <<=4;

        dest_addrsng = item->status.dest_addrsng;
        /* shift dest 2 bits left */
        dest_addrsng <<= 2;

        item->code.i_wrd1.codeval = opcode + src_addrsng + dest_addrsng;
        item->code.i_wrd1.E_R_A = 0;
        item->code.i_wrd1.exist = 1;
        item->code.i_wrd1.ic_addr = item->start_addr;


        /*********  generate first parameter code words (if exists) *******/
      
        /* if 1-st parameter exists */
        if (item->param1.param_type.no_param == 0)
            set_1st_parameter_code_words(ent_fname, ext_fname, &item);
        /* if 2-nd parameter exists */
        if (item->param2.param_type.no_param == 0)
            set_2nd_parameter_code_words(ext_fname, &item);

        item = item->next;

        if(!(item->next))   /* if item->next == NULL */
            return;

    } while (1);


}



/* function object_file_gen

    generates an object file and may be part of .ent file

*/
void object_file_gen(char* obj_fname, char* ext_fname, char * ent_fname,  
                     i_item_ptr i_head, i_item_ptr i_tail,
                     data_blk_item_ptr d_head, data_blk_item_ptr d_tail,
                     int *cpu_instr_mem_size, int *cpu_data_mem_size)
{
    i_item_ptr item = i_head;  /* pointer to cpu instruction linked list item */
    data_blk_item_ptr ditem = d_head;  /* pointer to cpu data linked list item */
    char *str_code; /* temp pointer to code string */
    char *str_addr; /* temp pointer to code string */
    num_lnk_lst_ptr num_item ; /* pointer to item of numbers 
                                  linked list associated with data item */
    int data_cnt = 0; /* data counter */
    char * cpu_instr_size;   /* pointer to cpu_instruction memory size string */
    char * cpu_data_size; /* pointer to cpu data memory size string */
    lable_item_ptr lable_item; /* pointer to item of labels binary search tree*/
    FILE * fout_obj;    /* pointer to object file descriptor */
    int fres ;          /* result of file close */

    debug(printf("object_file_gen started\n"));
    /* return if CPU instruction linked list is empty */
    if (!i_head) 
        return;
    /* open .ob file for write only*/
    fout_obj = fopen(obj_fname, "w");
    if(!fout_obj){
        printf("Error: can not open %s file in read mode.\n", obj_fname);
        printf("Exitting...\n\n");
        release_all_dynamic_memory_and_files();
        exit(1);
    }
    

    /* convert cpu instruction memory size to string in base 4 */
    cpu_instr_size = uint2base4str(*cpu_instr_mem_size, 6);
    /* send the instr. mem. size to obj file */
    fprintf(fout_obj, "\t%s  ", cpu_instr_size);
    /* convert cpu data memory size to string in base 4 */
    cpu_data_size = uint2base4str(*cpu_data_mem_size, 4);
    /* send the instr. mem. size to obj file */
    fprintf(fout_obj, "%s\n", cpu_data_size);

    /* print to file *.ob data from cpu instruction linked list */
    do
    {
        
        /* continue to prepare .ent file */
        /* fined entry in labels table that includes command item's cpu command label */
        if (item->label_name)
            lable_item = label_entry_search(global_labels_table_ptr, item->label_name);
        /* check if this label defined with .entry , if yes , add it to .ent file */
        if (item->label_name && lable_item && lable_item->features.is_entry)
            append_to_ent_file(ent_fname, item->label_name, item->start_addr);


        /* print 1-st word in following cases */
        if (item->size_in_wrds == 1 ||
            item->size_in_wrds == 2 ||
            item->size_in_wrds == 3 ||
            item->size_in_wrds == 4 ||
            item->size_in_wrds == 5)
        {
            /* convert addr to string */
            str_addr = uint2base4str(item->start_addr, 8);
            /* send the address to obj file */
            fprintf(fout_obj, "%s\t", str_addr);
            /* convert 1st word data to string in base 4*/
            str_code = uint2base4str(item->code.i_wrd1.codeval, 10);
            /* send the code to obj file */
            fprintf(fout_obj, "%s\n", str_code);
        }

        /* print 2-nd word in following cases */
        if (item->size_in_wrds == 2 ||
            item->size_in_wrds == 3 ||
            item->size_in_wrds == 4 ||
            item->size_in_wrds == 5)
        {
            /* convert addr to string */
            str_addr = uint2base4str(item->start_addr + 1, 8);
            /* send the address to obj file */
            fprintf(fout_obj, "%s\t", str_addr);
            /* convert 2nd word data to string in base 4*/
            str_code = uint2base4str(item->code.i_wrd2.codeval, 10);
            /* send the code to obj file */
            fprintf(fout_obj, "%s\n", str_code);
        }

        /* print 3-rd word in following cases */
        if (item->size_in_wrds == 3 ||
            item->size_in_wrds == 4 ||
            item->size_in_wrds == 5)
        {
            /* convert addr to string */
            str_addr = uint2base4str(item->start_addr + 2, 8);
            /* send the address to obj file */
            fprintf(fout_obj, "%s\t", str_addr);
            /* convert 3rd word data to string in base 4 */
            str_code = uint2base4str(item->code.i_wrd3.codeval, 10);
            /* send the code to obj file */
            fprintf(fout_obj, "%s\n", str_code);
        }

        /* print 4-th word in following cases */
        if (item->size_in_wrds == 4 ||
            item->size_in_wrds == 5)
        {
            /* convert addr to string */
            str_addr = uint2base4str(item->start_addr + 3, 8);
            /* send the address to obj file */
            fprintf(fout_obj, "%s\t", str_addr);
            /* convert 4-th word data to string in base 4 */
            str_code = uint2base4str(item->code.i_wrd4.codeval, 10);
            /* send the code to obj file */
            fprintf(fout_obj, "%s\n", str_code);
        }

        /* print 5-th word in following case */
        if (item->size_in_wrds == 5)
        {

            /* convert addr to string */
            str_addr = uint2base4str(item->start_addr + 4, 8);
            /* send the address to obj file */
            fprintf(fout_obj, "%s\t", str_addr);
            /* convert 4-th word data to string in base 4 */
            str_code = uint2base4str(item->code.i_wrd5.codeval, 10);
            /* send the code to obj file */
            fprintf(fout_obj, "%s\n", str_code);
        }

        item = item->next;

    } while (item);

    /* print to file code data from cpu data linked list 
       for all ditem, need to print all numbers in linked list 
       assotiated with it.    
    */


    /* return if no data items */
    if (!ditem){
        
        fres = fclose(fout_obj);
        if(fres)
            printf("Error : %s file did not closed successfully ", obj_fname);
        else    
            fout_obj = NULL;

        return;
    }


    /* print to file code data from cpu data linked list */
    do 
    {

        /* continue to prepare .ent file */
        /* fined entry in labels table that includes command item's cpu command label */
        if (ditem->label_name)
            lable_item = label_entry_search(global_labels_table_ptr, ditem->label_name);
        /* check if this label defined with .entry , if yes , add it to .ent file */
        if (ditem->label_name && lable_item && lable_item->features.is_entry)
            append_to_ent_file(ent_fname, ditem->label_name, ditem->start_addr);

        num_item = ditem->num_head;
        data_cnt = ditem->start_addr;
        if (!num_item)
            continue;
         else
        {
            do
            {

                /* convert addr to string in base 4 */
                str_addr = uint2base4str(data_cnt, 8);
                /* send the address to obj file */
                fprintf(fout_obj, "%s\t", str_addr);
                /* convert num linked list item to string in base 4 */
                str_code = uint2base4str(num_item->num, 10);
                /* send the code to obj file */
                fprintf(fout_obj, "%s\n", str_code);
                
                num_item = num_item->next;
                data_cnt++;

            } while (num_item);
        }      

        ditem = ditem->next;

    } while (ditem);

        fres = fclose(fout_obj);
        if(fres)
            printf("Error : %s file did not closed successfully ", obj_fname);
        else    
            fout_obj = NULL;
        return;


}




/* function set_1st_parameter_code_words

        set code words for 1-st parameter in CPU instruction.
        signs code words with .exist feature, to allow 2-nd parameter function
        to fined what word is the last.

        In case when 1-st and 2-nd parameter are registers (that is special case)
        the function does not set set 1-st parameter at all, special function will be
        prepared for this purpose.

*/
void set_1st_parameter_code_words(char * ent_fname, char * ext_fname, i_item_ptr *item )
{

    lable_item_ptr lable_item; /* pointer to item of labels binary search tree*/
    
    debug(printf("set_1st_parameter_code_words started\n"));
    
    /* return in case of NULL item */
    if (!(*item))
    {
        reset_i_item (item);
        return;
    }


    if ((*item)->status.param1_found)
    {
        /*--------------------------------------------------------*/
        /*         when 1st and 2nd parameter are registers       */
        /*------------------------------------------------------- */

        /* return immediately if both parameters are registers 
        special function will treat this case.
        */
        if ((*item)->param1.param_type.param_is_reg_name &&
            (*item)->param2.param_type.param_is_reg_name)
            return;

        /*--------------------------------------------*/
        /*         when 1st parameter is matrix       */
        /*------------------------------------------- */

        if ((*item)->param1.param_type.param_is_matrx)
        {
            /* in this case word 2 is address of matrix label 
            stored in table of labels */
            if ((*item)->param1.params_union.mat.mat_label)
                lable_item = label_entry_search(global_labels_table_ptr,
                                                (*item)->param1.params_union.mat.mat_label);

            /* if mat label is internal A,R,E = (relocatable) */
            if (lable_item && lable_item->features.is_extrn == 0)
            { /* reserve two first bit for A.R.E. */
                (*item)->code.i_wrd2.codeval = 0; /* set all bits to 0 */
                (*item)->code.i_wrd2.codeval += lable_item->label_address << 2;
                (*item)->code.i_wrd2.E_R_A = RELOCATABLE_ARE;
                /* insert A.R.E value */
                (*item)->code.i_wrd2.codeval += (*item)->code.i_wrd2.E_R_A;
                (*item)->code.i_wrd2.ic_addr = (*item)->start_addr + 1;
                (*item)->code.i_wrd2.exist = 1;

                if (lable_item->features.is_entry)
                    append_to_ent_file(ent_fname, (*item)->param1.params_union.lbl.label_name,
                                       (*item)->code.i_wrd2.ic_addr);
            }
            /* if mat label is external it gets addr = 0 and bits A.R.E. = (external) */
            if (lable_item && lable_item->features.is_extrn == 1)
            {
                (*item)->code.i_wrd2.codeval = 0; /* set all bits to 0 */
                (*item)->code.i_wrd2.E_R_A = EXTERNAL_ARE;
                (*item)->code.i_wrd2.codeval += (*item)->code.i_wrd2.E_R_A;
                (*item)->code.i_wrd2.exist = 1;
                (*item)->code.i_wrd2.ic_addr = (*item)->start_addr + 1;

                append_to_ext_file(ext_fname,(*item)->param1.params_union.lbl.label_name,
                                   (*item)->code.i_wrd2.ic_addr);
            }

            /* word 3 is combined : bits 6..9 : reg1 , bits 2..5 : reg2 , bits A.R.E = (abslute)  */
            (*item)->code.i_wrd3.codeval = 0; /* set all bits to 0 */
            (*item)->code.i_wrd3.codeval += (*item)->param1.params_union.mat.reg1_num << 6;
            (*item)->code.i_wrd3.codeval += (*item)->param1.params_union.mat.reg2_num << 2;
            (*item)->code.i_wrd3.E_R_A = ABSOLUTE_ARE;
            (*item)->code.i_wrd3.exist = 1;
            (*item)->code.i_wrd3.ic_addr = (*item)->start_addr + 2;
        }

        /*--------------------------------------------*/
        /*         when 1st parameter is label        */
        /*------------------------------------------- */

        if ((*item)->param1.param_type.param_is_label)
        {
            
            
            /* in this case word 2 is address of label
            stored in table of labels */
            if ((*item)->param1.params_union.lbl.label_name)
                lable_item = label_entry_search(global_labels_table_ptr,
                                                (*item)->param1.params_union.lbl.label_name);

            /* bits 2...9 will contain address , bits 0..1 will be A.R.E
            if label is internal, ARE will be (relocatable) */
            if (lable_item && lable_item->features.is_extrn == 0)
            {
                (*item)->code.i_wrd2.codeval = 0; /* set all bits to 0 */
                (*item)->code.i_wrd2.codeval += lable_item->label_address << 2;
                (*item)->code.i_wrd2.E_R_A = RELOCATABLE_ARE;
                (*item)->code.i_wrd2.codeval += (*item)->code.i_wrd2.E_R_A;
                (*item)->code.i_wrd2.exist = 1;
                (*item)->code.i_wrd2.ic_addr = (*item)->start_addr + 1;

                if (lable_item->features.is_entry)
                    append_to_ent_file(ent_fname, (*item)->param1.params_union.lbl.label_name,
                                       (*item)->code.i_wrd2.ic_addr);
            }

            /* if label is external , its code val is 0 , A.R,E is external */
            if (lable_item->features.is_extrn == 1)
            {
                (*item)->code.i_wrd2.codeval = 0; /* set all bits to 0 */
                (*item)->code.i_wrd2.E_R_A = RELOCATABLE_ARE;
                (*item)->code.i_wrd2.codeval += (*item)->code.i_wrd2.E_R_A;
                (*item)->code.i_wrd2.exist = 1;
                (*item)->code.i_wrd2.ic_addr = (*item)->start_addr + 1;

                append_to_ext_file(ext_fname, (*item)->param1.params_union.lbl.label_name,
                                   (*item)->code.i_wrd2.ic_addr);
            }
        }

        /*--------------------------------------------*/
        /*         when 1st parameter is number       */
        /*------------------------------------------- */

        if ((*item)->param1.param_type.param_is_number)
        {
            /* bits 2..9 will get number value , bits 0..1 will get absolute A.R.E */
            (*item)->code.i_wrd2.codeval = 0; /* reset word */
            (*item)->code.i_wrd2.codeval = (*item)->param1.params_union.num.num << 2;
            (*item)->code.i_wrd2.E_R_A = ABSOLUTE_ARE;
            (*item)->code.i_wrd2.codeval += (*item)->code.i_wrd2.E_R_A;
            (*item)->code.i_wrd2.exist = 1;
            (*item)->code.i_wrd2.ic_addr = (*item)->start_addr + 1;
        }

        /*----------------------------------------------*/
        /*         when 1st parameter is register       */
        /*----------------------------------------------*/

        if ((*item)->param1.param_type.param_is_reg_name)
        {
            /* bits 6..9 cotain sourse register value , bits 0..1  absolute A.R.E.*/
            (*item)->code.i_wrd2.codeval = 0; /* reset word */
            (*item)->code.i_wrd2.codeval = (*item)->param1.params_union.reg.reg_num << 6;
            (*item)->code.i_wrd2.E_R_A = ABSOLUTE_ARE;
            (*item)->code.i_wrd2.codeval += (*item)->code.i_wrd2.E_R_A;
            (*item)->code.i_wrd2.exist = 1;
            (*item)->code.i_wrd2.ic_addr = (*item)->start_addr + 1;
        }
    }


}






/* function set_2nd_parameter_code_words

        set code words for 2-nd parameter in CPU instruction.
        signs code words with .exist feature, to allow 2-nd parameter function
        to fined what word is the last.

        In case when 1-st and 2-nd parameter are registers (that is special case)
        the function does not set 1-st parameter at all, special function will be
        prepared for this purpose.

*/
void set_2nd_parameter_code_words(char * ext_fname, i_item_ptr *item)
{

    lable_item_ptr lable_item;
    int start_word_num; /* parameter to store start word number of 2nd parameter */

    debug(printf("run set_2nd_parameter_code_words\n"));

    /* reset parameters and return in case of emply item  */    
    if (!(*item))
    {
        reset_i_item (item);
        return;
    }
   
    if ((*item)->status.param2_found )
    {

        
        /*--------------------------------------------------------*/
        /*         when 1st and 2nd parameter are registers       */
        /*------------------------------------------------------- */

        /* return immediately if both parameters are registers
        special function will treat this case.
        */
        if ((*item)->param1.param_type.param_is_reg_name &&
            (*item)->param2.param_type.param_is_reg_name)
            return;

        /* start word number for 2ns parameter can be:
             2 , if 1st parameter takes 0 words 
             3 , if 1st parameter takes 1 word
             4 , if 1st parameter takes 2 words
        */
        start_word_num = param2_start_word_num(*item);

        /*--------------------------------------------*/
        /*         when 2nd parameter is matrix       */
        /*------------------------------------------- */

        if ((*item)->param2.param_type.param_is_matrx)
        {

            /* check for matrix label address */
            if ((*item)->param2.params_union.mat.mat_label)
                lable_item = label_entry_search(global_labels_table_ptr,
                                                (*item)->param2.params_union.mat.mat_label);

            switch(start_word_num)
            {

                case 2 :
                        /* if mat label is internal A,R,E = (relocatable) */
                        if (lable_item->features.is_extrn == 0)
                        {                                     /* reserve two first bit for A.R.E. */
                            (*item)->code.i_wrd2.codeval = 0; /* set all bits to 0 */
                            (*item)->code.i_wrd2.codeval += lable_item->label_address << 2;
                            (*item)->code.i_wrd2.E_R_A = RELOCATABLE_ARE;
                            /* insert A.R.E value */
                            (*item)->code.i_wrd2.codeval += (*item)->code.i_wrd2.E_R_A;
                            (*item)->code.i_wrd2.ic_addr = (*item)->start_addr + 1;
                            (*item)->code.i_wrd2.exist = 1;

                        }
                        /* if mat label is external it gets addr = 0 and bits A.R.E. = (external) */
                        if (lable_item->features.is_extrn == 1)
                        {
                            (*item)->code.i_wrd2.codeval = 0; /* set all bits to 0 */
                            (*item)->code.i_wrd2.E_R_A = EXTERNAL_ARE;
                            (*item)->code.i_wrd2.codeval += (*item)->code.i_wrd2.E_R_A;
                            (*item)->code.i_wrd2.exist = 1;
                            (*item)->code.i_wrd2.ic_addr = (*item)->start_addr + 1;

                            append_to_ext_file(ext_fname, (*item)->param2.params_union.lbl.label_name,
                                            (*item)->code.i_wrd2.ic_addr);
                        }

                        /* word 3 is combined : bits 6..9 : reg1 , bits 2..5 : reg2 , bits A.R.E = (abslute)  */
                        (*item)->code.i_wrd3.codeval = 0; /* set all bits to 0 */
                        (*item)->code.i_wrd3.codeval += (*item)->param2.params_union.mat.reg1_num << 6;
                        (*item)->code.i_wrd3.codeval += (*item)->param2.params_union.mat.reg2_num << 2;
                        (*item)->code.i_wrd3.E_R_A = ABSOLUTE_ARE;
                        (*item)->code.i_wrd3.exist = 1;
                        (*item)->code.i_wrd3.ic_addr = (*item)->start_addr + 2;

                        break;
                case 3 :
                    /* if mat label is internal A,R,E = (relocatable) */
                    if (lable_item->features.is_extrn == 0)
                    {                                     /* reserve two first bit for A.R.E. */
                        (*item)->code.i_wrd3.codeval = 0; /* set all bits to 0 */
                        (*item)->code.i_wrd3.codeval += lable_item->label_address << 2;
                        (*item)->code.i_wrd3.E_R_A = RELOCATABLE_ARE;
                        /* insert A.R.E value */
                        (*item)->code.i_wrd3.codeval += (*item)->code.i_wrd3.E_R_A;
                        (*item)->code.i_wrd3.ic_addr = (*item)->start_addr + 2;
                        (*item)->code.i_wrd3.exist = 1;

                    }
                    /* if mat label is external it gets addr = 0 and bits A.R.E. = (external) */
                    if (lable_item->features.is_extrn == 1)
                    {
                        (*item)->code.i_wrd3.codeval = 0; /* set all bits to 0 */
                        (*item)->code.i_wrd3.E_R_A = EXTERNAL_ARE;
                        (*item)->code.i_wrd3.codeval += (*item)->code.i_wrd3.E_R_A;
                        (*item)->code.i_wrd3.exist = 1;
                        (*item)->code.i_wrd3.ic_addr = (*item)->start_addr + 2;

                        append_to_ext_file(ext_fname, (*item)->param2.params_union.lbl.label_name,
                                           (*item)->code.i_wrd3.ic_addr);
                    }

                    /* word 4 is combined : bits 6..9 : reg1 , bits 2..5 : reg2 , bits A.R.E = (abslute)  */
                    (*item)->code.i_wrd4.codeval = 0; /* set all bits to 0 */
                    (*item)->code.i_wrd4.codeval += (*item)->param2.params_union.mat.reg1_num << 6;
                    (*item)->code.i_wrd4.codeval += (*item)->param2.params_union.mat.reg2_num << 2;
                    (*item)->code.i_wrd4.E_R_A = ABSOLUTE_ARE;
                    (*item)->code.i_wrd4.exist = 1;
                    (*item)->code.i_wrd4.ic_addr = (*item)->start_addr + 3;

                    break;
                case 4 :

                    /* if mat label is internal A,R,E = (relocatable) */
                    if (lable_item->features.is_extrn == 0)
                    {                                     /* reserve two first bit for A.R.E. */
                        (*item)->code.i_wrd4.codeval = 0; /* set all bits to 0 */
                        (*item)->code.i_wrd4.codeval += lable_item->label_address << 2;
                        (*item)->code.i_wrd4.E_R_A = RELOCATABLE_ARE;
                        /* insert A.R.E value */
                        (*item)->code.i_wrd4.codeval += (*item)->code.i_wrd4.E_R_A;
                        (*item)->code.i_wrd4.ic_addr = (*item)->start_addr + 3;
                        (*item)->code.i_wrd4.exist = 1;

                    }
                    /* if mat label is external it gets addr = 0 and bits A.R.E. = (external) */
                    if (lable_item->features.is_extrn == 1)
                    {
                        (*item)->code.i_wrd4.codeval = 0; /* set all bits to 0 */
                        (*item)->code.i_wrd4.E_R_A = EXTERNAL_ARE;
                        (*item)->code.i_wrd4.codeval += (*item)->code.i_wrd4.E_R_A;
                        (*item)->code.i_wrd4.exist = 1;
                        (*item)->code.i_wrd4.ic_addr = (*item)->start_addr + 3;

                        append_to_ext_file(ext_fname, (*item)->param2.params_union.lbl.label_name,
                                           (*item)->code.i_wrd4.ic_addr);
                    }

                    /* word 5 is combined : bits 6..9 : reg1 , bits 2..5 : reg2 , bits A.R.E = (abslute)  */
                    (*item)->code.i_wrd5.codeval = 0; /* set all bits to 0 */
                    (*item)->code.i_wrd5.codeval += (*item)->param2.params_union.mat.reg1_num << 6;
                    (*item)->code.i_wrd5.codeval += (*item)->param2.params_union.mat.reg2_num << 2;
                    (*item)->code.i_wrd5.E_R_A = ABSOLUTE_ARE;
                    (*item)->code.i_wrd5.exist = 1;
                    (*item)->code.i_wrd5.ic_addr = (*item)->start_addr + 4;

                    break;

            } /* switch(start_word_num)*/
        }

        /*--------------------------------------------*/
        /*         when 2nd parameter is label        */
        /*------------------------------------------- */


        
        if ((*item) && (*item)->param2.param_type.param_is_label)
        {
                /* in this case word 2 is address of label
                stored in table of labels */
                if ((*item)->param2.params_union.lbl.label_name)
                    lable_item = label_entry_search(global_labels_table_ptr,
                                                    (*item)->param2.params_union.lbl.label_name);

                switch (start_word_num)
                {

                    case 2:

                            /* bits 2...9 will contain address , bits 0..1 will be A.R.E
                            if label is internal, ARE will be (relocatable) */
                            if (lable_item->features.is_extrn == 0)
                            {
                                (*item)->code.i_wrd2.codeval = 0; /* set all bits to 0 */
                                (*item)->code.i_wrd2.codeval += lable_item->label_address << 2;
                                (*item)->code.i_wrd2.E_R_A = RELOCATABLE_ARE;
                                (*item)->code.i_wrd2.codeval += (*item)->code.i_wrd2.E_R_A;
                                (*item)->code.i_wrd2.exist = 1;
                                (*item)->code.i_wrd2.ic_addr = (*item)->start_addr + 1;


                            }

                            /* if label is external , its code val is 0 , A.R,E is external */
                            if (lable_item->features.is_extrn == 1)
                            {
                                (*item)->code.i_wrd2.codeval = 0; /* set all bits to 0 */
                                (*item)->code.i_wrd2.E_R_A = RELOCATABLE_ARE;
                                (*item)->code.i_wrd2.codeval += (*item)->code.i_wrd2.E_R_A;
                                (*item)->code.i_wrd2.exist = 1;
                                (*item)->code.i_wrd2.ic_addr = (*item)->start_addr + 1;

                                append_to_ext_file(ext_fname,(*item)->param2.params_union.lbl.label_name,
                                                        (*item)->code.i_wrd2.ic_addr);
                            }
                            
                        break; 

                    case 3:
    
                            /* bits 2...9 will contain address , bits 0..1 will be A.R.E
                            if label is internal, ARE will be (relocatable) */
                            if (lable_item->features.is_extrn == 0)
                            {
                                (*item)->code.i_wrd3.codeval = 0; /* set all bits to 0 */
                                (*item)->code.i_wrd3.codeval += lable_item->label_address << 2;
                                (*item)->code.i_wrd3.E_R_A = RELOCATABLE_ARE;
                                (*item)->code.i_wrd3.codeval += (*item)->code.i_wrd3.E_R_A;
                                (*item)->code.i_wrd3.exist = 1;
                                (*item)->code.i_wrd3.ic_addr = (*item)->start_addr + 2;

                            }

                            /* if label is external , its code val is 0 , A.R,E is external */
                            if (lable_item->features.is_extrn == 1)
                            {
                                (*item)->code.i_wrd3.codeval = 0; /* set all bits to 0 */
                                (*item)->code.i_wrd3.E_R_A = RELOCATABLE_ARE;
                                (*item)->code.i_wrd3.codeval += (*item)->code.i_wrd2.E_R_A;
                                (*item)->code.i_wrd3.exist = 1;
                                (*item)->code.i_wrd3.ic_addr = (*item)->start_addr + 2;

                                append_to_ext_file(ext_fname,(*item)->param2.params_union.lbl.label_name,(*item)->code.i_wrd3.ic_addr);
                            }
                            
                        break;

                    case 4:

                            /* bits 2...9 will contain address , bits 0..1 will be A.R.E
                            if label is internal, ARE will be (relocatable) */
                            if (lable_item->features.is_extrn == 0)
                            {
                                (*item)->code.i_wrd4.codeval = 0; /* set all bits to 0 */
                                (*item)->code.i_wrd4.codeval += lable_item->label_address << 2;
                                (*item)->code.i_wrd4.E_R_A = RELOCATABLE_ARE;
                                (*item)->code.i_wrd4.codeval += (*item)->code.i_wrd4.E_R_A;
                                (*item)->code.i_wrd4.exist = 1;
                                (*item)->code.i_wrd4.ic_addr = (*item)->start_addr + 3;


                            }

                            /* if label is external , its code val is 0 , A.R,E is external */
                            if (lable_item->features.is_extrn == 1)
                            {
                                (*item)->code.i_wrd4.codeval = 0; /* set all bits to 0 */
                                (*item)->code.i_wrd4.E_R_A = RELOCATABLE_ARE;
                                (*item)->code.i_wrd4.codeval += (*item)->code.i_wrd4.E_R_A;
                                (*item)->code.i_wrd4.exist = 1;
                                (*item)->code.i_wrd4.ic_addr = (*item)->start_addr + 3;

                                append_to_ext_file(ext_fname, (*item)->param2.params_union.lbl.label_name,
                                                (*item)->code.i_wrd4.ic_addr);
                            }

                        break;

                } /* switch (start_word_num)*/
            } /* if ((*item)->param2.param_type.param_is_label)*/

        /*--------------------------------------------*/
        /*         when 2nd parameter is number       */
        /*------------------------------------------- */
        if ((*item)->param2.param_type.param_is_number)
        {
            switch (start_word_num)
            {
                case 2:


                        /* bits 2..9 will get number value , bits 0..1 will get absolute A.R.E */
                        (*item)->code.i_wrd2.codeval = 0; /* reset word */
                        (*item)->code.i_wrd2.codeval = (*item)->param2.params_union.num.num << 2;
                        (*item)->code.i_wrd2.E_R_A = ABSOLUTE_ARE;
                        (*item)->code.i_wrd2.codeval += (*item)->code.i_wrd2.E_R_A;
                        (*item)->code.i_wrd2.exist = 1;
                        (*item)->code.i_wrd2.ic_addr = (*item)->start_addr + 1;
                        
                        break;

                case 3:


                        /* bits 2..9 will get number value , bits 0..1 will get absolute A.R.E */
                        (*item)->code.i_wrd3.codeval = 0; /* reset word */
                        (*item)->code.i_wrd3.codeval = (*item)->param2.params_union.num.num << 2;
                        (*item)->code.i_wrd3.E_R_A = ABSOLUTE_ARE;
                        (*item)->code.i_wrd3.codeval += (*item)->code.i_wrd3.E_R_A;
                        (*item)->code.i_wrd3.exist = 1;
                        (*item)->code.i_wrd3.ic_addr = (*item)->start_addr + 2;

                        break;
                        
                case 4:

                        /* bits 2..9 will get number value , bits 0..1 will get absolute A.R.E */
                        (*item)->code.i_wrd4.codeval = 0; /* reset word */
                        (*item)->code.i_wrd4.codeval = (*item)->param2.params_union.num.num << 2;
                        (*item)->code.i_wrd4.E_R_A = ABSOLUTE_ARE;
                        (*item)->code.i_wrd4.codeval += (*item)->code.i_wrd4.E_R_A;
                        (*item)->code.i_wrd4.exist = 1;
                        (*item)->code.i_wrd4.ic_addr = (*item)->start_addr + 3;

                        break;
            } /* switch (start_word_num)*/
        } /* if ((*item)->param2.param_type.param_is_number) */

        /*----------------------------------------------*/
        /*         when 2nd parameter is register       */
        /*----------------------------------------------*/
        if ((*item)->param2.param_type.param_is_reg_name)
        {
            switch (start_word_num)
            {
                case 2:
                    /* bits 6..9 cotain sourse register value , bits 0..1  absolute A.R.E.*/
                    (*item)->code.i_wrd2.codeval = 0; /* reset word */
                    (*item)->code.i_wrd2.codeval = (*item)->param2.params_union.reg.reg_num << 6;
                    (*item)->code.i_wrd2.E_R_A = ABSOLUTE_ARE;
                    (*item)->code.i_wrd2.codeval += (*item)->code.i_wrd2.E_R_A;
                    (*item)->code.i_wrd2.exist = 1;
                    (*item)->code.i_wrd2.ic_addr = (*item)->start_addr + 1;

                    break;
                case 3:

                    /* bits 6..9 cotain sourse register value , bits 0..1  absolute A.R.E.*/
                    (*item)->code.i_wrd3.codeval = 0; /* reset word */
                    (*item)->code.i_wrd3.codeval = (*item)->param2.params_union.reg.reg_num << 6;
                    (*item)->code.i_wrd3.E_R_A = ABSOLUTE_ARE;
                    (*item)->code.i_wrd3.codeval += (*item)->code.i_wrd3.E_R_A;
                    (*item)->code.i_wrd3.exist = 1;
                    (*item)->code.i_wrd3.ic_addr = (*item)->start_addr + 2;
                    
                    break;

                case 4:

                    /* bits 6..9 cotain sourse register value , bits 0..1  absolute A.R.E.*/
                    (*item)->code.i_wrd4.codeval = 0; /* reset word */
                    (*item)->code.i_wrd4.codeval = (*item)->param2.params_union.reg.reg_num << 6;
                    (*item)->code.i_wrd4.E_R_A = ABSOLUTE_ARE;
                    (*item)->code.i_wrd4.codeval += (*item)->code.i_wrd4.E_R_A;
                    (*item)->code.i_wrd4.exist = 1;
                    (*item)->code.i_wrd4.ic_addr = (*item)->start_addr + 3;

                    break;
            }


        } /*  if ((*item)->param2.param_type.param_is_reg_name) */
    } /* if ((*item)->status.param2_found)*/
} /* set_2nd_parameter_code_words*/


/*
 function append_to_ext_file
    appends label to .ext file
    for every separate label opens, inserts and closes the .ext file.
*/
void append_to_ext_file(char * ext_fname, char *label_name, uint addr)
{
    char *addr_base_4 = NULL;
    FILE *fd_ext; /* file descriptor */
    int fres ; /* rersult of file open  */

    /* convert addr to base4 string */
    addr_base_4 = uint2base4str(addr,8);
    fd_ext = fopen(ext_fname, "a");
    if(!fd_ext){
        printf("Error: can not open %s file in read mode.\n", ext_fname);
        printf("Exitting...\n\n");
        release_all_dynamic_memory_and_files();
        exit(1);
    }
    
    fprintf(fd_ext, "%s\t%s\n", label_name, addr_base_4);

    fres = fclose(fd_ext);
    if(fres)
        printf("Error : %s file did not closed successfully ", ext_fname);
    else    
        fd_ext = NULL;
    
    return;



}
/*
 function append_to_ent_file
    appends label to .ext file
    for every separate label opens, inserts and closes the .ext file.
*/
void append_to_ent_file(char * ent_fname, char *label_name, uint addr)
{
    char *addr_base_4 = NULL;
    FILE *fd_ent; 
    int fres ;  /* result of file close  */
    /* convert addr to base4 string */
    addr_base_4 = uint2base4str(addr,8);
    fd_ent = fopen(ent_fname, "a");
    if(!fd_ent){
        printf("Error: can not open %s file in read mode.\n", ent_fname);
        printf("Exitting...\n\n");
        release_all_dynamic_memory_and_files();
        exit(1);
    }
    
    fprintf(fd_ent, "%s\t%s\n", label_name, addr_base_4);


    fres = fclose(fd_ent);
    if(fres)
        printf("Error : %s file did not closed successfully ", ent_fname);
    else    
        fd_ent = NULL;
    
    return;
}



/*
    function uint2base4str
        converts unsigned int number to base4 string
        suppose that number has 10 significant bits.
        returns value of global general string pointer global_gen_str_pointer.
        Note that previous value of global_gen_str_pointer will be released, since it is
        temporary pointer.

    parameters: 
        number : number to be converted
        bits_number : length of binary number, 
                      we use 10bits , 8bits, 6 bits or 4 bits 

*/
char *uint2base4str(uint number, int bits_number)
{
    int i;    /* indexes */
    int mask;    /* two last bits */
    int res = 0; /* intermediate result */
    char *str;   /* string pointer */
    /* set bits 8..9 to 1 in mask */
    mask = 0x3;
    /* if pointer is already allocated , free it */
    if (global_gen_str_pointer)
    {
        free(global_gen_str_pointer);
        global_gen_str_pointer = NULL;
    }
    /* allocate heap memory for 5 chars (for 10 bit number, one char per 2 bits )*/
    global_gen_str_pointer = (char *)malloc(8 * sizeof(char));
    for (i = bits_number - 2, str = global_gen_str_pointer; i >= 0; i -= 2, str++)
    {
        res = (number >> i) & mask;
        switch (res)
        {
        case 0:
            *str = 'a';
            break;
        case 1:
            *str = 'b';
            break;
        case 2:
            *str = 'c';
            break;
        case 3:
            *str = 'd';
            break;
        default:
            *str = '*';
        }
    }
    *(str)=EOS; /* add end of string */

    return global_gen_str_pointer;
}




/* function  param2_start_word_num

        calculates start word number for 2nd parameter for given instruction command
        Note that command itsel takes 1 word

        return : 
            command size in words in CPU memory.

*/
int param2_start_word_num(i_item_ptr i_item)
{

    if (i_item->is_valid){

        if (i_item->status.params_num == 2){

            if (i_item->status.src_addrsng == 0)
            {
                if (i_item->status.dest_addrsng == 0)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + 1;

                if (i_item->status.dest_addrsng == 1)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + 1;

                if (i_item->status.dest_addrsng == 2)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + 1;

                if (i_item->status.dest_addrsng == 3)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + 1;
            }

            if (i_item->status.src_addrsng == 1)
            {
                if (i_item->status.dest_addrsng == 0)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + 1;

                if (i_item->status.dest_addrsng == 1)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + 1;

                if (i_item->status.dest_addrsng == 2)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + 1;

                if (i_item->status.dest_addrsng == 3)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + 1;
            }

            if (i_item->status.src_addrsng == 2)
            {
                if (i_item->status.dest_addrsng == 0)
                    return CMD_WORD_SIZE + TWO_WORDS_SRC + 1;

                if (i_item->status.dest_addrsng == 1)
                    return CMD_WORD_SIZE + TWO_WORDS_SRC + 1;

                if (i_item->status.dest_addrsng == 2)
                    return CMD_WORD_SIZE + TWO_WORDS_SRC + 1;

                if (i_item->status.dest_addrsng == 3)
                    return CMD_WORD_SIZE + TWO_WORDS_SRC + 1;
            }

            if (i_item->status.src_addrsng == 3)
            {
                if (i_item->status.dest_addrsng == 0)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + 1;

                if (i_item->status.dest_addrsng == 1)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + 1;

                if (i_item->status.dest_addrsng == 2)
                    return CMD_WORD_SIZE + ONE_WORD_SRC + 1;

                if (i_item->status.dest_addrsng == 3)  /* !!!! special case !!!!*/
                    return CMD_WORD_SIZE + 1;
            }
        }
        /* if number of params is 1, only dest param is used */
        if (i_item->status.params_num == 1)
        {

            if (i_item->status.dest_addrsng == 0)
                return CMD_WORD_SIZE + 1;

            if (i_item->status.dest_addrsng == 1)
                return CMD_WORD_SIZE + 1;

            if (i_item->status.dest_addrsng == 2)
                return CMD_WORD_SIZE + 1;

            if (i_item->status.dest_addrsng == 3)
                return CMD_WORD_SIZE + 1;
        }
        /* in case of no parameters */
        if (i_item->status.params_num == 0)
        {
                return CMD_WORD_SIZE + 1;
        }

    } /* if (i_item->is_valid)*/

    return 0;
} /* i_item_size_calc*/



