# Make libraries for all the targets.
foreach(ELLCC_TARGET ${ELLCC_TARGETS})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${ELLCC_TARGET})
    list(FIND LINUX_TARGETS ${ELLCC_TARGET} index)
    execute_process(COMMAND cmake -DELLCC_SOURCE_DIR=${ELLCC_SOURCE_DIR}
                                  -DELLCC_TARGET=${ELLCC_TARGET}
                                  -DLINUX_TARGET=${index}
                                  ${CMAKE_CURRENT_SOURCE_DIR}/source
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${ELLCC_TARGET})
    add_custom_target(${ELLCC_TARGET} ALL make
                      COMMENT "Building libraries for ${ELLCC_TARGET}"
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${ELLCC_TARGET})
endforeach(ELLCC_TARGET)

#add_subdirectory(source)
