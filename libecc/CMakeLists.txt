# Make libraries for all the targets.
foreach(ELLCC_TARGET ${ELLCC_TARGETS})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${ELLCC_TARGET})
    #execute_process(COMMAND cmake ${CMAKE_CURRENT_SOURCE_DIR}/source
    #                              -DELLCC_SOURCE_DIR=${ELLCC_SOURCE_DIR}
    #                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${ELLCC_TARGET})
endforeach(ELLCC_TARGET)

#add_subdirectory(source)
