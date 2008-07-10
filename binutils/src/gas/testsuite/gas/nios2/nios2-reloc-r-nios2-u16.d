#objdump: -s
#name: NIOS2 R_NIOS2_U16

# Test the unsigned 16-bit relocations.
.*: +file format elf32-littlenios2

Contents of section .text:
 0000 0c004408 0c004008 ccff7f08 4c004808  ..D...@.....L.H.
 0010 4c004008                             L.@.            
