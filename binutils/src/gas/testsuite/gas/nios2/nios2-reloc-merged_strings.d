#objdump: -dr --prefix-addresses 
#name: NIOS2 reloc against merged strings

# Test the merged duplicate strings

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> addi	r2,r2,32
0+0004 <[^>]*> addi	r3,r3,63
0+0008 <[^>]*> addi	r4,r4,70
0+000c <[^>]*> call	00000000 <func1>
0+0010 <[^>]*> addi	r2,r2,101
0+0014 <[^>]*> addi	r3,r3,63
0+0018 <[^>]*> addi	r4,r4,132
0+001c <[^>]*> call	00000000 <func1>

