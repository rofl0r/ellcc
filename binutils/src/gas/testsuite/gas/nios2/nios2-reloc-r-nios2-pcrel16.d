#objdump: -s
#name: NIOS2 R_NIOS2_PCREL16

# Test the relative branch relocations.
.*: +file format elf32-littlenios2

Disassembly of section .text:

0+0000 <[^>]*> br	00000008 <ext_label>
0+0004 <[^>]*> br	00000018 <__bss_start\+0x8>
0+0008 <[^>]*> nop
0+000c <[^>]*> nop
