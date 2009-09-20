add_library(softfp
    adddf3.c  extenddftf2.c  fixunsdfdi.c  floatsidf.c    gedf2.c   negdf2.c   truncdfsf2.c
    addsf3.c  extendsfdf2.c  fixunsdfsi.c  floatsisf.c    gesf2.c   negsf2.c   trunctfdf2.c
    addtf3.c  extendsftf2.c  fixunssfdi.c  floatsitf.c    getf2.c   negtf2.c   trunctfsf2.c
    divdf3.c  fixdfdi.c      fixunssfsi.c  floatundidf.c  ledf2.c   sqrtdf2.c  unorddf2.c
    divsf3.c  fixdfsi.c      fixunstfdi.c  floatundisf.c  lesf2.c   sqrtsf2.c  unordsf2.c
    divtf3.c  fixsfdi.c      fixunstfsi.c  floatunditf.c  letf2.c   sqrttf2.c  unordtf2.c
    eqdf2.c   fixsfsi.c      floatdidf.c   floatunsidf.c  muldf3.c  subdf3.c
    eqsf2.c   fixtfdi.c      floatdisf.c   floatunsisf.c  mulsf3.c  subsf3.c
    eqtf2.c   fixtfsi.c      floatditf.c   floatunsitf.c  multf3.c  subtf3.c)

add_custom_command(TARGET softfp POST_BUILD
    COMMAND mkdir -p ${LIBECC_DIR}/../lib/${ELLCC_TARGET}
    COMMAND ${TARGET_AR} ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/libecc.a $(softfp_OBJECTS))
