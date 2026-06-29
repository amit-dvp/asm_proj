

/* This file contains some project requirements from project file and forum */

/*
    The project can be delivered up to 19.08 without degradation of grades.

    1. If the input file contains errors, output files will not be prepared. (p.46)
    2. Continue to find errors also after the first error.(p. 46)
    3. Maximum line length in input file is 80 characters (not including '\n') (p.25)
    4. Name of matrix created from label in .mat instruction. (forum)

    6. Pre-assembler stage can delete empty lines (forum)
    7. If assembly command in command line has no arguments, we can report warning,
       it is no error. (forum)
    8. In case of program memory overload, the program can be stopped after release the memory
       that was allocated... (forum)
    9. We dont need to check labels between several files... (forum)
         - the label in same file can not be external and internal simultaneously
         - if the label is internal, it must be defined in the same file
         - if the label is external, it must be not defined in the same file.

    11. Comment line must start with ";"


    13. in command .data 1, -999 we must check the limits of number to be
        represented in 10bit number. The number is represented with 8 bits
        so it can be -128...127

    14. line numbers can appear in source file or after pre-assembly (forum).

    15. If only part of values defined in matrix, other values will be 0. (forum).
    16. Empty line that end with EOF and not '\n' - is not error.
    17. Comments start from begin of line only.
    18. Strig can contain ASCII code only, from 32 to 126. (forum)
    19. IC and DC are relevant for title of output file and check that there is no
        overflow in memory.
    20. name of main program and main folder and run file will be "assembler"(forum)
    21. am file can be without empty lines and comment lines.
    22. First memory location for loading is 100(forum)


Matrix:
    1. .mat can not be defined with non constant values in size of matrix:
        ERROR:
          XYZ: .mat[r2][r1]
    2. matrix definition with too many values is error :
        ERROR:
        .mat [2][2] 1, 2, 3, 4, 5, 6, 7
    3. if CPU command that use matrix, name of matrix must be closed to "[" ,
       "]" must be closed to "[". (forum)
    4. White characters allowed inside [] in matrix. (forum)

    5. Partial definition of matrix is valid, for example (forum)
        .mat [2][2] 1, 2



Program length, arrays usage:
    1. Assume that program length is unknown beforehand,
       so also length of translated program. (p. 46)
    2. ONLY machine code can be stored in array.(p. 46)
    3. Other data structures (as table of labels and table of macros)
       must be implemented in more efficient way
       (for example dynamic linked list) (p. 46)


White characters and ",":
    1. Allow white characters in input file:
        1.1. In CPU command with 2 operands separated by "," allowed any
             numbers of spaces and tabs before and after ",". (p. 46, p.28, p.26)
    2. Spaces are not alowed between label name and ":" after it (p. 29)
    3. One or more spaces will separate between name of CPU command and first
       operand. (p.28)
    4. "," is forbidden at the end of instruction or CPU command
    5. More than one "," to separate two operands is forbidden.
    6. Empty instruction (white characters only) is allowed.
    7. Every instruction will appear in new line.
    8. Before first operand "," is forbidden. (mazeget, p.60)
    9. There must be space between name of directive (.ddd) and first operand of it.
       (forum)
    10.White chars can appear at the begin of line.(forum)
    11. There must be at least one space between : of label and next word(forum)
    12. For check of space use the isspace function. (forum)




Macros:
    1. [don't check] Suppose no nested macro definitions. (p.25)
    2. [don't check] Suppose every macro line in code closed with mcroend (p.25)
    3. [don't check] Macro definition appears always before macro call (p.25)
    4. [don't check] Suppose that for every macro definition in code
                     exists macro close with "mcroend" (p.25)
    5. We must check that in line of macro definition and macro closure
       there in no additional characters. (p.25)
    4. We can suppose that macro name is limited to 30 characters as
       name of label (forum).
    5. [don't check] We can suppose that macro name does not appear more than one time.
    6. We must check in pre-assembly stage that there is no additional
       chars in the line of macro definition command and macro def. close command
       (forum)
       mcro amacro
       ...
       mcroend
       amacro extra text
    7. The assembly must check that there are no additional chars after macroend (forum).
    8. Name of macro has the same rules as name of label. Character "_" is allowed.
    9. If the program find error in macro substitution stage, it must stop
       the execution, report errors and pass to next input file processing
       if it exists (p. 25).



Labels:

    1.1. Label definition always ends with ":" without space
         between label word and ":" ;
    1.2. There must be space between label definition and keyword after it.
         (answer in forum)
    1.2  Label always starts with alphabetic symbol (uppercase or lowercase).
    1.3  Label length can be up to 30 symbols (not including : symbol).
    1.4  Label must be defined only once.
    1.5  Label must differ from any keyword (command names and names of registers).
    1.6  Label must differ from any macro name.
    1.7  Label can be defined in 4 ways (page 21):
            1.7.1. At the begin of CPU command (LABEL: OPCODE ...):
                    HELLO: add r7, B
            1.7.2. At the begin of instruction and before .data keyword:
                    XYZ: .data 7, -57, +17, 9
            1.7.3. At the begin of instruction and before .string keyword:
                    STR: .string "abcdef"
            1.7.4. At the begin of instruction and before .mat keyword.
                    In this case label is name of matrix:
                    MAT8: .mat [2][3] 4, -5, 7, 9

    1.8  Labels without meaning (will cause warning message):
            1.8.1 Label defined BEFORE .entry instruction:
                NOMEANING: .entry HELLO (page 27)
            1.8.2 Label defined BEFORE .extern instruction:
                NOMEANING: .entry HELLO (page 28)

    1.9  Note: non-meaning labels MUST be still different from macros names and
         keywords (because they are still labels).
    1.10 There must be at least one space between : of label and next word(forum)

    1.8  Label that is exported from other files is defined as:
            .extern HELLO
    1.9  Label that can be exported to others files from the current file:
            .entry HELLO
    2.0  The same label can not be defined in the same file
         with .extern and also .entry instructions.
    2.1. Can be defined label with name r20, if it is not defined
         assembly program will generate error that there
         is no such register. (forum)
    2.2. We dont need to check labels between several files... (forum)
         - the label in same file can not be external and internal simultaneously
         - if the label is internal, it must be defined in the same file
         - if the label is external, it must be not defined in the same file.
    2.3 To run program from address 100, there will be need to
        add 100 to labels table (forum)


 Types of assembly lines:

    1. comment      : ';'[any chars]'\n'
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