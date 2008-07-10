#objdump: -s
#name: NIOS2 R_NIOS2_BFD_RELOC_XX

# Test the branch instructions.
.*: +file format elf32-littlenios2

Contents of section .text:
 0000 fa00cefa efbeadde facefaef beadde00  ................
