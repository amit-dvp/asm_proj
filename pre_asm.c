

/*
  Pre-assembler will be done in two stages.

  The first stage is check only phase.
  At first stage pre-assembler will scan input file
  for validity of macro definition (mcro command), validity of macro name
  in macro definition and validity of macro definition finish (mcroend command).
  Also length of every line will be checked.
  Any error in mcro or macroend lines or in any line length will be reported.
  In case of any error, the second stage will not start.

  The second stage is prepare of .am file.
  At the second stage pre-assembler will prepare expanded .am file.
  It will scan again through already validated .am file, search for  
  macro definitions and insert macro definitions into macros binary search tree. 
  
  After it it will search for macro lines binded with given macro name and insert  
  the macro substitution lines into appropriate linked list inside binary search tree 
  item that match the macro name.

  Macro definition, macro body and macro end lines will not be copied into
  .am file.

  Each line with single word that match macro name in macros table will cause to insertion
  of apropriate lines from binary search tree item's linked list into .am file.

*/

/*
  Function pre_asm_first_stage:
  performs first stage of pre-assembler task (check only).
  It gets asm file name (from command line) without extention .as
  Scans input file for
    validity of macro definitions (mcro command),
    validity of macro name within macro definition
    validity of macro definition finish (mcroend command).
    validity of macro substitution command (i.e. macro name only).
  Also checks length of every line (must be less of 80).

  Valid macro names will be added into macros binary search tree entries
  for future use in second pre-assembler stage and first stage's macro 
  substitution commands check. Macro substitution commands must 
  contain single word with macro name only. 
  In this stage we detect error if macro definition appears as first word
  but there are additional words/non_white_chars after it.
  In case of any error detection this tree memory will be released (freed).
  Any error in mcro or macroend lines or in any line length will be reported.

  In case of any error, the second pre-assembly stage will not start.
  The function returns number of errors.
*/


#include "assembler.h"



int pre_asm_first_stage (char * as_file_name){
    int err_cnt = 0 ;        /* if this flag is 1 , at least one error is found */
    int readline_res = 0;           /* resullt of read line */
    int text_find_res = 0;          /* result of text find */ 
    int line_num=0;                 /* counter of lines numbers */
    int name_status;                /* status of name check */
    int fres;                 /* result of fclose  */
    FILE * fd_in = NULL;
    char str[MAX_LINE_SIZE+5] = {'\0'};      /* char array to store string */
    char *str_p = str;                       /* pointer to first array's element */

    char txt[MAX_LINE_SIZE] = {'\0'};         /* array for text find */

    debug(printf("The input filename is %s\n", as_file_name));

    printf("\n---------------------------------------\n");
    printf("Pre-assembler:\n\t1-st scan is running.\n");
    printf("\tScan for errors in <%s> file.\n", as_file_name);

    /* open .as file for read */
    fd_in = fopen(as_file_name, "r"); 
    if (!fd_in)
    {
        printf("Error: can not open %s file in read mode.\n", as_file_name);
        printf("Exitting...\n\n");
        release_all_dynamic_memory_and_files();
        exit(1);
    }

    do {
              /* read line from .as file */
              readline_res = readline(fd_in, str, MAX_LINE_SIZE);
              str_p = str; /* set pointer to new line */ 
              /* increment lines counter */
              line_num++;
              debug(printf("line_num = %d\n", line_num));

              /*--------------------------------------------*/
              /* check if the line contains too many chars  */
              /*--------------------------------------------*/
              if (readline_res == TOO_MANY_CHARS_AND_ENTR || readline_res == TOO_MANY_CHARS_AND_EOF)
              {
                    err_cnt++;
                    /* TBD delete fout*/
                    debug(printf("Line with too many characters in input file %s \n", as_file_name));
                    print_err(global_err_list_head, 1, line_num, str,"");
              }

              /*------------------------------------------*/ 
              /* check if the line is valid macro command */
              /*------------------------------------------*/
              /* find for text from begin of line and up to ' ' delimiter */
              text_find_res = gen_text_find(&str_p, ' ', txt);
              
              /* if there is some text in command with one space char after it */
              if ((text_find_res == GOT_TXT_AND_DLMTR) && (!strncmp(txt, MCRO_CMD,MAX_MACRO_AND_LABEL_LENGTH)))
              {
                      debug(printf("Valid mcro command name found. Now check for macro name\n"));
                      
                      /* now check command name from updated fragment position and up to EOS*/
                      text_find_res = gen_text_find(&str_p, '\0', txt);

                      /* if it is single word */ 
                      if (text_find_res == GOT_TXT_AND_EOS)
                      {
                            /* check if mcro command name length is less than 30 chars */
                            if (strlen(txt) > MAX_MACRO_AND_LABEL_LENGTH)
                            {
                                print_err(global_err_list_head, 8, line_num, str, "");
                                err_cnt++;
                            }
                            /* check if macro's name appears in keyword binary search tree */
                            if (keywords_table_search(global_keyword_table_ptr, txt))
                            {
                                print_err(global_err_list_head, 9, line_num, str, txt);
                                err_cnt++;
                            }
                            
                            /* check if macro name has illegal characters */
                            name_status = check_name(txt);
                            if (name_status == NOT_ALPHABET)
                            {
                                print_err(global_err_list_head, 7, line_num, str,txt);
                                err_cnt++;
                            }  
                            if (name_status == NOT_ALPHANUM)
                            {
                                print_err(global_err_list_head, 4, line_num, str,"");
                                err_cnt++;
                            }

                            /* store valid macro name within macros table Binary Search Tree*/  
                            if (err_cnt == 0)
                            {
                              /* enter macro's name into binary search tree.
                              Note that global_macro_table_ptr is global pointer */
                                global_macro_table_ptr = add_macro_tree(global_macro_table_ptr, txt);
                            }  

                      } else  /*if (text_find_res != GOT_TXT_AND_EOS) */
                      { /* command name does not exis or there is additional text after it */
                        /* there are too many parameters in macro line : */
                            if (text_find_res == GOT_SECOND_TXT_BLK)
                            {
                                print_err(global_err_list_head, 2, line_num, str,"");
                                err_cnt++;
                            }

                            /* ...may be macro name does not exis at all ? */
                            if ((text_find_res == GOT_WHITES_AND_EOS) ||
                                (text_find_res == GOT_EOS_AT_START) ||
                                (text_find_res == GOT_DLMTR_AT_START))
                            {
                                  print_err(global_err_list_head, 3, line_num, str,"");
                                  err_cnt++;
                            }
                      } /* if (text_find_res == GOT_TXT_AND_EOS) */

              }       /* may be mcro command alone without parameter is found */
              else 
              {     if ((text_find_res == GOT_TXT_AND_EOS)&& (!strncmp(txt, MCRO_CMD,MAX_MACRO_AND_LABEL_LENGTH)))
                    {
                      print_err(global_err_list_head, 3, line_num, str,"");
                      err_cnt++;
                    }
              }
              /*--------------------------------------------*/
              /* check if the line is valid mcroend command */
              /*--------------------------------------------*/

              /* return str_p and txt to initial values: */
              str_p = str; /* set pointer again to line start */

              /* search up to line end */
              text_find_res = gen_text_find(&str_p, '\0', txt);

              /* find microend and after it ' ' , need to check if no words are after it */
              if (text_find_res == GOT_SECOND_TXT_BLK && (!strncmp(txt, MCROEND_CMD,MAX_MACRO_AND_LABEL_LENGTH)))
              {
                  print_err(global_err_list_head, 5, line_num, str, "");
                  err_cnt++;
              }

              /*-----------------------------------------------------*/
              /* check if the line is valid macro substitution label */
              /* i.e. macro name only as alone word                  */
              /* here we check only cases when macro command         */ 
              /* is the first word                                   */
              /*-----------------------------------------------------*/

              /* return str_p and txt to initial values: */
              str_p = str; /* set pointer again to line start */
              
              /* search up to line end */
              text_find_res = gen_text_find(&str_p, '\0', txt);

              /* check if first word is really macro name 
                i.e. appears in macros table binary search tree: */
              if (macro_entry_search(global_macro_table_ptr, txt))
              {
                  /* report error is exist additional chars after it */
                  if (text_find_res == GOT_SECOND_TXT_BLK)
                  {
                      print_err(global_err_list_head, 10, line_num, str, "");
                      err_cnt++;
                  }
              }    

    } while ((readline_res != TOO_MANY_CHARS_AND_EOF) && (readline_res != NORM_LENGTH_AND_EOF));
    
    printf("Pre-assembler:\n\t1-st scan is finished.\n");
    if (err_cnt)
    {
        printf("\t%d errors found,\n\tFiles generation for file <%s> is stopped. \n", err_cnt, global_full_fname_as);
        printf("\tPlease correct the file <%s>.\n", as_file_name);
        printf("---------------------------------------\n");
        /* free macro table Binary Search tree in case if
          any error is detected in 1-st stage scan */
    } else 
    {
        printf("\tno errors found in <%s> file.\n", as_file_name);
        printf("---------------------------------------\n");
    }

    
    
    fres = fclose(fd_in);
    if(fres)
        printf("Error : %s file did not closed successfully ", as_file_name);
    else    
        fd_in = NULL;
    

    return err_cnt;

}/* end function pre_asm_first_stage */



/* Function check_name: 
   checks that the name has illegal characters
   returns
    0 (VALID_NAME)   - the name is valid
    1 (NOT_ALPHABET) - the first char is not aphabetic (small of uppercase letter)
    2 (NOT_ALPHANUM) - the remain chars are not alphanumeric
*/
int check_name (char * name){

  /* the first letter must be alphabetic */
  if (!isalpha(name[0]))
    return NOT_ALPHABET;
  while(*name){
    /* the remainder letters must be alphanumeric or '_' */
    if (!(isalnum(*name)||(*name=='_')))
      return NOT_ALPHANUM;
    name++;
  }

  return VALID_NAME;
}

/*
  Function pre_asm_second_stage :

    Performs second stage of pre-assembler second task, i.e.
    macros expansion and .am file preparation.
    Based on finite state machine.

    Macro SUBSTITUTION commands are checked in second scan stage
    We check macro's name appearance only in first word of line
    (again macro's names stores at first stage ) and check
    that additional words does not appear.

    At the second stage pre-assembler prepares an expanded .am file.
    It passes through already validated macro definitions and macro end and stores
    the macro names and the macro substitution lines in linked list inside binary search
    tree binded with appropriate macro names.

    Macro definition, macro body and macro end lines will not be copied into
    .am file.

    Each line with single word that match macro's name in macros table will cause to insertion
    of apropriate binded macro lines into .am file.

    Comment lines and empty lines will not be copied into .am file.

 */
int pre_asm_second_stage(char * file_name_as , char * file_name_am)
{
  char macro_name[MAX_LINE_SIZE] = {'\0'}; /* temp. array for macro's name */

  int rd_line_num = 0; /* counter of lines numbers */

  char str[MAX_LINE_SIZE + 5] = {'\0'}; /* temp. array to store line */
  int readline_res = 0;  /* result of read line function */
  int detect_result = 0; /* detect result of different functions (as macro_def_detect)*/  
  int fres1,fres2; /* result of fclose */
  FILE * fd_in = NULL;
  FILE * fd_out = NULL;

  macro_table_entry_ptr macro_entry = NULL;
  macro_line_node_ptr macro_line_item = NULL;

  pre_asm_2_stage_state state = SEARCH_MACRO_CMD_OR_MACRO_SUBST;
  

  printf("Pre-assembler:\n\t2-nd scan stage is runnung.\n");
  printf("\tprepare <%s> file...\n", global_full_fname_am);

  debug(printf("The input filename is %s\n", file_name_as));
  debug(printf("The output filename is %s\n", file_name_am));

    fd_in = fopen(file_name_as, "r");
    
    if (!fd_in)
    {
        printf("Error: can not open %s file in read mode.\n", file_name_as);
        printf("Exitting...\n\n");
        release_all_dynamic_memory_and_files();
        exit(1);
    }

    fd_out = fopen(file_name_am, "w");

    if (!fd_out)
    {
        printf("Error: can not open %s file in read mode.\n", file_name_am);
        printf("Exitting...\n\n");
        release_all_dynamic_memory_and_files();
        exit(1);
    }

  do
  {
    debug(printf("read_line_num: %d\n", rd_line_num));

    /* finite state machine to perform macros expansion */
    switch (state){
    case SEARCH_MACRO_CMD_OR_MACRO_SUBST:
      readline_res = readline(fd_in, str, MAX_LINE_SIZE);
      rd_line_num++;
      debug(printf("pre assembler 2ns stage .... rd_line_num=%d",rd_line_num));

      /* if macro command found */
      if ((detect_result = macro_def_detect(str, macro_name)))
        state = STORE_LINES;
      /* if macro substitution command found */
      else if ((detect_result = macro_subst_detect(str, macro_name)))
        state = MACRO_SUBST;

      if (state == SEARCH_MACRO_CMD_OR_MACRO_SUBST) {
            /* if not macro command detected and not macro subst detected
               insert the string into output file */
            /* also check that the line is not empty line or comment line */
            if ((!empty_line_detect(str)) && (!comment_line_detect(str)))
            {
                fprintf(fd_out, "%s\n", str);
                fflush(fd_out);
            }  
      }      
      break;
    case STORE_LINES:    /* store lines in macros binary search tree */
      /* read next line */
      readline_res = readline(fd_in, str, MAX_LINE_SIZE);
      rd_line_num++;
      if ((detect_result = macro_end_def_detect(str))) /* detected end of macro */
        state = SEARCH_MACRO_CMD_OR_MACRO_SUBST;
      else { /* insert the line into macros BST*/
        add_line_to_table_entry(global_macro_table_ptr, macro_name, str);
        state = STORE_LINES; }        
      break;
    case MACRO_SUBST: /* print lines from binary search tree to output file */
      macro_entry = macro_entry_search(global_macro_table_ptr, macro_name);
      macro_line_item = macro_entry->line_list_head;
      while (macro_line_item){ /*is not NULL*/
        /* print the line into output file */
        /* check before it that the line is not empty line or comment line */
        if ((!empty_line_detect(macro_line_item->line)) && (!comment_line_detect(macro_line_item->line)))
        {
          fprintf(fd_out, "%s\n", macro_line_item->line);
        }
        macro_line_item = macro_line_item->next_line_node;
      }
      state = SEARCH_MACRO_CMD_OR_MACRO_SUBST;
      break;
    default:   
      state = SEARCH_MACRO_CMD_OR_MACRO_SUBST;
      break;
    }

  } while (readline_res != NORM_LENGTH_AND_EOF);

  printf("Pre-assembler:\n\t2-nd scan is finished.\n");
  printf("\tFile <%s> generated.\n", file_name_am);
  printf("---------------------------------------\n");
  printf("Pre-assembler:\n\ttask is finished.\n");
  printf("---------------------------------------\n");

  debug(printf("Macros table print: \n"));
  debug(macros_table_print(global_macro_table_ptr));

  


    fres1 = fclose(fd_in);
    if(fres1)
        printf("Error : %s file did not closed successfully ", file_name_as);
    else    
        fd_in = NULL;

    fres2 = fclose(fd_out);
    if(fres2)
        printf("Error : %s file did not closed successfully ", file_name_am);
    else    
        fd_out = NULL;


  
  return 0;

  }



/* Function macro_def_detect:
      Detects if the given command line is mcro command.

      This function assumes that the command line syntax is valid,
      including macro's name (can be used in second stage of pre-assembler).

      Returns 1 (MCRO_CMD_FOUND) if it is mcro command line,
              0 (MCRO_CMD_NOT_FOUND) it is not mcro command line.

      Also returns macro_name that appears after mcro command.
*/
int macro_def_detect (char * cmd_line, char * macro_name ){
  
  char * str_p = cmd_line;
  char txt[MAX_LINE_SIZE] = {'\0'}; /* temp. array for text find */

  /* firstly search for mcro command begin */
  gen_text_find(&str_p, ' ', txt);
  
  /* if find mcro command */
  if (!strncmp(txt, MCRO_CMD,MAX_MACRO_AND_LABEL_LENGTH))
  {
    gen_text_find(&str_p, ' ', txt); /* search for macro's name*/
    /* copy string pointed by txt pointer to string pointed by macro_name pointer*/
    strncpy(macro_name,txt,MAX_MACRO_AND_LABEL_LENGTH+2);
    return MCRO_CMD_FOUND;
  }

  return MCRO_CMD_NOT_FOUND;
}

/* Function macro_end_def_detect:
      Checks if the given line has mcroend command.
      Assumes that the file is already checked in first pre-assembler scan
      stage and the line with mcroend command is valid.

      Returns 1 (MCROEND_CMD_FOUND) if it is mcro command line,
              0 (MCROEND_CMD_NOT_FOUND) it is not mcro command line.

*/
int macro_end_def_detect(char *cmd_line)
{

  char *str_p = cmd_line;
  char txt[MAX_LINE_SIZE] = {'\0'}; /* temp. array for text find */

  /* firstly search for mcro command begin */
  gen_text_find(&str_p, EOS, txt);
  /* if find mcro command */
  if (!strncmp(txt, MCROEND_CMD,MAX_MACRO_AND_LABEL_LENGTH))
  {
    return MCROEND_CMD_FOUND;
  }

  return MCROEND_CMD_NOT_FOUND;
}


/* Function macro_subst_detect:
      Checks if the given line has macro substitution command.
      We suppose that all cases where the first word is macro subst command
      are already checked in first stage.  

      Returns 1 (MCRO_SUBST_FOUND) if it is mcro substitution command line,
              0 (MCRO_SUBST_NOT_FOUND) it is not mcro substitution command line.

              Also returns macro name in case when MCRO_SUBST_FOUND.
*/
int macro_subst_detect(char *cmd_line, char * macro_name)
{

  char *str_p = cmd_line;
  char txt[MAX_LINE_SIZE] = {'\0'}; /* temp. array for text find */
  macro_table_entry_ptr macro_entry_ptr = NULL;
      /* firstly search for mcro command begin */
  gen_text_find(&str_p, EOS, txt);
  /* check if first word is really macro name 
         i.e. appears in macros table binary search tree: */
      if ((macro_entry_ptr = macro_entry_search(global_macro_table_ptr, txt)))
      {
        strncpy(macro_name, txt, MAX_LINE_SIZE - 2);
        return MCRO_SUBST_FOUND;
      }
  macro_name = NULL;
  return MCRO_SUBST_NOT_FOUND;
}

/* Function comment_line_detect:
    Checks if the given line is comment line.
    Return:
        0 (IS_NOT_COMMENT_LINE) if it is not comment line
        1 (IS_COMMENT_LINE) if it is comment line
*/
int comment_line_detect(char *cmd_line)
{
  /* check first char of the line */
  if (cmd_line[0] == ';')
  {
    return IS_COMMENT_LINE;
  }

  return IS_NOT_COMMENT_LINE;
}

/* Function empty_line_detect:
    Checks if the given line has empty line:  
        - starts with EOS
        - contains only white characters and EOS

        Note: we already ommited '\n' and EOF while read the line.

    Return:
        0 (IS_NOT_EMPTY_LINE) if it is not empty line
        1 (IS_EMPTY_LINE) if it is empty line
*/
int empty_line_detect(char *cmd_line)
{
  char *str_p = cmd_line;
  char txt[MAX_LINE_SIZE] = {'\0'}; /* temp. array for text find */
  int gen_text_find_res;
  gen_text_find_res = gen_text_find(&str_p, EOS, txt);
  if (gen_text_find_res == GOT_EOS_AT_START || gen_text_find_res == GOT_WHITES_AND_EOS)
  {
    return IS_EMPTY_LINE;
  }

  return IS_NOT_EMPTY_LINE;
}

