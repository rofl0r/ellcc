#objdump: -dr --prefix-addresses 
#name: NIOS2 relax_callr
# Test relaxation of callr

.*: +file format elf32-littlenios2

Disassembly of section text1:
00000000 <[^>]*> call	08000000 <func>
00000004 <[^>]*> nop
00000008 <[^>]*> nop
0000000c <[^>]*> call	0800001c <func1>
00000010 <[^>]*> nop
00000014 <[^>]*> nop
Disassembly of section text2:
08000000 <[^>]*> nop
08000004 <[^>]*> br	0800001c <func1>
08000008 <[^>]*> nop
0800000c <[^>]*> nop
08000010 <[^>]*> nop
08000014 <[^>]*> nop
08000018 <[^>]*> nop
0800001c <[^>]*> nop
