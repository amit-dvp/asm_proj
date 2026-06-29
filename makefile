# Target to build the executable
assembler: mngr_macro_tbl.o mngr_keywords_list.o pre_asm.o\
           assembler.o cmd_line_analyse.o mngr_errors.o mngr_cpu_data_lnk_lst.o\
		   mngr_cpu_instr_lnk_lst.o mngr_labels_tbl.o\
		   asm_1st_pass.o release_dynamic_memory_and_files.o asm_2nd_pass.o
	gcc -g -Wall -pedantic -ansi mngr_macro_tbl.o mngr_keywords_list.o cmd_line_analyse.o\
	                             mngr_errors.o pre_asm.o assembler.o\
								 mngr_cpu_instr_lnk_lst.o mngr_cpu_data_lnk_lst.o\
								 mngr_labels_tbl.o asm_1st_pass.o \
								 release_dynamic_memory_and_files.o\
								 asm_2nd_pass.o\
								-o assembler -lm

# Individual targets for each object file

assembler.o : assembler.c assembler.h mngr_macro_tbl_types.h mngr_errors_globals.h\
              mngr_keywords_list_types.h cmd_line_analyse_types.h consts.h mngr_errors_types.h\
			  assembler_globals.h
	gcc -c -g -Wall -pedantic -ansi assembler.c


mngr_macro_tbl.o: mngr_macro_tbl.c assembler.h mngr_macro_tbl_types.h \
                  mngr_keywords_list_types.h cmd_line_analyse_types.h consts.h
	gcc -c -g -Wall -pedantic -ansi mngr_macro_tbl.c 

mngr_keywords_list.o : mngr_keywords_list.c assembler.h mngr_macro_tbl_types.h mngr_keywords_list_types.h \
             cmd_line_analyse_types.h consts.h
	gcc -c -g -Wall -pedantic -ansi mngr_keywords_list.c

cmd_line_analyse.o : cmd_line_analyse.c assembler.h mngr_macro_tbl_types.h \
                     mngr_keywords_list_types.h cmd_line_analyse_types.h consts.h
	gcc -c -g -Wall -pedantic -ansi cmd_line_analyse.c

mngr_errors.o : mngr_errors.c mngr_errors_types.h assembler.h mngr_macro_tbl_types.h \
                     mngr_keywords_list_types.h cmd_line_analyse_types.h consts.h mngr_errors_globals.h
	gcc -c -g -Wall -pedantic -ansi mngr_errors.c


pre_asm.o : pre_asm.c mngr_errors_types.h assembler.h mngr_macro_tbl_types.h \
                     mngr_keywords_list_types.h cmd_line_analyse_types.h consts.h mngr_errors_globals.h	
	gcc -c -g -Wall -pedantic -ansi pre_asm.c


mngr_cpu_data_lnk_lst.o : mngr_cpu_data_lnk_lst.c mngr_cpu_data_lnk_lst_types.h assembler.h  
	gcc -c -g -Wall -pedantic -ansi mngr_cpu_data_lnk_lst.c


mngr_cpu_instr_lnk_lst.o : mngr_cpu_instr_lnk_lst.c mngr_cpu_instr_lnk_lst_types.h assembler.h
	gcc -c -g -Wall -pedantic -ansi mngr_cpu_instr_lnk_lst.c

mngr_labels_tbl.o : mngr_labels_tbl.c mngr_labels_tbl_types.h assembler.h
	gcc -c -g -Wall -pedantic -ansi mngr_labels_tbl.c


asm_1st_pass.o : asm_1st_pass.c  asm_1st_pass.h assembler.h
	gcc -c -g -Wall -pedantic -ansi asm_1st_pass.c

release_dynamic_memory_and_files.o : release_dynamic_memory_and_files.c assembler.h
	gcc -c -g -Wall -pedantic -ansi release_dynamic_memory_and_files.c

asm_2nd_pass.o : asm_2nd_pass.c asm_2nd_pass.h
	gcc -c -g -Wall -pedantic -ansi asm_2nd_pass.c

# Clean rule to remove previous files  
clean:
	rm -f assembler          \
	      assembler.o        \
		  mngr_macro_tbl.o   \
		  mngr_keywords_list.o \
		  cmd_line_analyse.o \
		  mngr_errors.o\
		  pre_asm.o\
		  mngr_cpu_data_lnk_lst.o\
		  mngr_cpu_instr_lnk_lst.o\
		  mngr_labels_tbl.o\
		  asm_1st_pass.o\
		  release_dynamic_memory_and_files.o\
		  asm_2nd_pass.o

# Add a default rule to clean before building
.PHONY: all clean
all: clean assembler

# - To clean manually, run:  
#   ```bash
#   make clean
#   ```
# - To clean and build in one step, run:  
#   ```bash
#   make all