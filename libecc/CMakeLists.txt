# Make libraries for all the targets.
foreach(dir ${ELLCC_TARGETS})
    file(MAKE_DIRECTORY ${dir})
endforeach(dir)

add_subdirectory(source)
