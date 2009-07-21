# Make libraries for all the targets.
foreach(dir ${ELLCC_TARGETS})
    add_subdirectory(${dir})
endforeach(dir)

add_subdirectory(source)
