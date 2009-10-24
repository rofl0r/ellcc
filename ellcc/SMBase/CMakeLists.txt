include(${ELLCC_SOURCE_DIR}/cmake/ellcc_components.cmake)

add_library(smbase ${ELLCC_LIBRARY_TYPE}
    autofile.cc bflatten.cc bit2d.cc bitarray.cc boxprint.cc breaker.cpp
    crc.cpp cycles.c d2vector.c datablok.cpp exc.cpp flatten.cc gprintf.c
    growbuf.cc hashline.cc hashtbl.cc malloc.c malloc_stub.c missing.cpp mypopen.c
    mysig.cc nonport.cpp ofstreamts.cc point.cc pprint.cc smregexp.cc str.cpp
    strdict.cc strhash.cc stringset.cc strtable.cc strtokp.cpp strutil.cc
    svdict.cc syserr.cpp taillist_test.cc tarrayqueue.cc testarray.cc testcout.cc
    testmalloc.cc tmalloc.c tobjlist.cc tobjpool.cc trace.cc trdelete.cc
    tsobjlist.cc unixutil.c vdtllist.cc voidlist.cc vptrmap.cc warn.cpp)

install_targets(/lib smbase)
