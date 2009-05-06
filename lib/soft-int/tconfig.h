/*
  QI  Quarter-Integer: a single byte integer.
  HI  Half-Integer: a two-byte integer.
  SI  Single Integer: a four-byte integer.
  DI  Double Integer: an eight-byte integer.
  TI  Tetra Integer: a sixteen-byte integer.
  OI  Octa Integer: thirty-two-byte integer.
  SF  Single Floating: a four byte floating point number.
  DF  Double Floating: an eight byte floating point number.
  XF  Extended Floating: an IEEE extended floating point number. This mode has 80 meaningful bits.
  TF  Tetra Floating: sixteen byte floating point number.
  SC, DC, XC, TC Complex numbers: pairs of SF, DF, XF, and TF values respectively.
*/

#define __libgcc_cmp_return__ SI
#define __libgcc_shift_count__ SI
typedef unsigned long W_TYPE;   // The type of a word.
typedef W_TYPE size_t;
#define BITS_BIG_ENDIAN 0       // 1 if the highest bit in a byte is 0.
#define BYTES_BIG_ENDIAN 0      // 1 of the most significant byte in a word is byte 0.
#define WORDS_BIG_ENDIAN 0      // 1 if the most significant word in a multiword is word 0.
#define FLOATS_BIG_ENDIAN 0     // 1 if the sign bit of a float is at the lowest address.
#ifndef BITS_PER_UNIT
#define BITS_PER_UNIT 8         // Number of bits in an addressable storage unit (byte).
#endif
#ifndef UNITS_PER_WORD
#define UNITS_PER_WORD 4        // Number of storage units in a word.
#endif
#ifndef LIBGCC2_UNITS_PER_WORD
#define LIBGCC2_UNITS_PER_WORD UNITS_PER_WORD
#endif
#ifndef MIN_UNITS_PER_WORD
#define MIN_UNITS_PER_WORD UNITS_PER_WORD               // Mimimum units per word.
#endif
#ifndef BITS_PER_WORD
#define BITS_PER_WORD (BITS_PER_UNIT * UNITS_PER_WORD)  // Number of bits in a word.
#endif
#ifndef MAX_BITS_PER_WORD
#define MAX_BITS_PER_WORD BITS_PER_WORD                 // Maximum number of bits in a word.
#endif
#ifndef POINTER_SIZE
#define POINTER_SIZE BITS_PER_WORD                      // Number of bits in a pointer.
#endif

#define LONG_LONG_TYPE_SIZE 64
#define DOUBLE_TYPE_SIZE 64
//#define LONG_DOUBLE_TYPE_SIZE 64    // DF=64, XF=80, TF=128
#define __LDBL_MANT_DIG__ 53       // 53, 113, 106.
#define ROUND_TOWARDS_ZERO 0        // 1 if round to zero rather than nearest.
#define _FP_W_TYPE_SIZE BITS_PER_WORD
#define _FP_W_TYPE W_TYPE
#define _FP_I_TYPE int

#define L_negdi2
#define L_addvsi3
#define L_addvdi3
#define L_subvsi3
#define L_subvdi3
#define L_mulvsi3
#define L_negvsi2
#define L_negvdi2
#define L_absvsi2
#define L_absvdi2
#define L_mulvdi3
#define L_lshrdi3
#define L_ashldi3
#define L_ashrdi3
#define L_bswapsi2
#define L_bswapdi2
#define L_ffssi2
#define L_ffsdi2
#define L_muldi3
#define L_udiv_w_sdiv
#define L_clz
#define L_clzsi2
#define L_clzdi2
#define L_ctzsi2
#define L_ctzdi2
#define L_popcount_tab
#define L_popcountsi2
#define L_popcountdi2
#define L_paritysi2
#define L_paritydi2
#define L_udivmoddi4
#define L_divdi3
#define L_moddi3
#define L_umoddi3
#define L_udivdi3
#define L_cmpdi2
#define L_ucmpdi2
#define L_fixunstfdi
#define L_fixtfdi
#define L_fixunsxfdi
#define L_fixxfdi
#define L_fixunsdfdi
#define L_fixdfdi
#define L_fixunssfdi
#define L_fixsfdi
#define L_floatdixf
#define L_floatditf
#define L_floatunditf
#define L_floatdisf
#define L_floatundisf
#define L_floatundisf
#define L_floatundidf
#define L_fixunsxfsi
#define L_fixunsdfsi
#define L_fixunssfsi
#define L_powisf2
#define L_powidf2
#define L_powixf2
#define L_powitf2
#define L_mulsc3
#define L_divsc3
#define L_muldc3
#define L_divdc3
#define L_mulxc3
#define L_divxc3
#define L_multc3
#define L_divtc3
#define L__gcc_bcmp
#define L_clear_cache
#define L_enable_execute_stack
#define L_trampoline
#define L__main
#define EH_FRAME_SECTION_NAME
#define OBJECT_FORMAT_ELF
#define INVOKE__main
// #define L_ctors

