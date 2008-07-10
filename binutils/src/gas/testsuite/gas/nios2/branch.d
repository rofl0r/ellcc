#objdump: -dr --prefix-addresses
#name: NIOS2 branch

# Test the branch instructions.
dump.o:     file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> beq	r4,r5,00000000 <text_label>
0+0004 <[^>]*> bge	r4,r5,00000000 <text_label>
0+0008 <[^>]*> bgeu	r4,r5,00000000 <text_label>
0+000c <[^>]*> blt	r4,r5,00000000 <text_label>
0+0010 <[^>]*> bltu	r4,r5,00000000 <text_label>
0+0014 <[^>]*> bne	r4,r5,00000000 <text_label>
0+0018 <[^>]*> br	0000001c <text_label\+0x1c>
[	]*18: R_NIOS2_PCREL16	external_label

