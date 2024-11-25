# C Timing Analysis Neon VS Standard

The standard C code is in main.c, while the NEON C code is in neon.c.
To run the corresponding code, you need to rename the appropriate Makefile:

To run main.c, rename Makefile_main to Makefile.
To run neon.c, rename Makefile_neon to Makefile.

It is recommended to use two terminals to run the code.
Additionally, using PuTTY is recommended to view the output.



One terminal is used to execute the make command:
$ make



The other terminal is used to execute commands after executing xsct:
$ xsct

connect -host 127.0.0.1 -port 3121
targets
targets 2

source ps7_init.zyboz7 (modify the suffix according to your board)
ps7_init

dow main.elf
con
stop