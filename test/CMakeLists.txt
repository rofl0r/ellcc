# Test the ELLCC compiler collection.
add_custom_target(check ${CMAKE_TEST_COMMAND})
# Execute tests for all the targets.
foreach(ELLCC_TARGET ${ELLCC_TARGETS})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${ELLCC_TARGET})
    execute_process(COMMAND cmake -DELLCC_SOURCE_DIR=${ELLCC_SOURCE_DIR}
                                  -DELLCC_TARGET=${ELLCC_TARGET}
                                  ${CMAKE_CURRENT_SOURCE_DIR}/source
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${ELLCC_TARGET})
    add_custom_target(${ELLCC_TARGET}_test make
                      COMMENT "Executing tests for ${ELLCC_TARGET}"
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${ELLCC_TARGET})
endforeach(ELLCC_TARGET)
