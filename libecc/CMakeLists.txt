# Make libraries for all the targets.
foreach(ELLCC_TARGET ${ELLCC_TARGETS})
    add_subdirectory(${ELLCC_TARGET})
endforeach(ELLCC_TARGET)

add_subdirectory(source)
