#objdump: -dr --prefix-addresses
#name: NIOS2 R_NIOS2_CACHE_OPX

# Test the cache opx relocation
.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> cache	0x1f,0x1000\(\$1\)
0+0004 <[^>]*> Address 0x4 is out of bounds.

