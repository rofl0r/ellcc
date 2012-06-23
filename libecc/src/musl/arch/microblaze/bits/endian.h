#ifdef __ELLCC__
  #ifdef __BIG_ENDIAN__
    #if __BIG_ENDIAN__
      #define __BYTE_ORDER __BIG_ENDIAN
    #endif
  #endif /* __BIG_ENDIAN__ */

  #ifdef __LITTLE_ENDIAN__
    #if __LITTLE_ENDIAN__
      #define __BYTE_ORDER __LITTLE_ENDIAN
    #endif
  #endif /* __LITTLE_ENDIAN__ */
#else
  #define __BYTE_ORDER __BIG_ENDIAN
#endif
