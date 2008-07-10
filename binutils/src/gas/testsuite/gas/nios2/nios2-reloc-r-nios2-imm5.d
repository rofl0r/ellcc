#objdump: -dr --prefix-addresses
#name: NIOS2 R_NIOS2_IMM5

# Test the branch instructions.
.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> roli	at,at,31
0+0004 <.[^>]*> Address 0x4 is out of bounds.

