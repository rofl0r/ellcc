#objdump: -dr --prefix-addresses
#name: NIOS2 R_NIOS2_HI16,LO16,HIADJ16

# Test the %hi, %lo and %hiadi relocations
.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> addi	at,at,-8531
0+0004 <[^>]*> addi	at,at,-16657
0+0008 <[^>]*> addi	at,at,-8530
