#objdump: -dr --prefix-addresses
#name: NIOS2 gp-relative relocations

# Test the %gprel macro.

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> movui	gp,32784
0+0004 <[^>]*> ldw	at,-32756\(gp\)
0+0008 <[^>]*> ldw	r2,-32752\(gp\)
0+000c <[^>]*> ldb	r3,-32748\(gp\)
0+0010 <[^>]*> ldw	at,-32744\(gp\)
0+0014 <[^>]*> ldw	r2,-32740\(gp\)
0+0018 <[^>]*> ldb	r3,-32736\(gp\)
