project(LIBS)
SET(LIBECC_DIR ${PROJECT_SOURCE_DIR})
cmake_minimum_required(VERSION 2.6)
include(${ELLCC_SOURCE_DIR}/cmake/target.cmake)
add_subdirectory(common)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/compiler-rt)
execute_process(COMMAND ${CMAKE_COMMAND}
                    -DELLCC_SOURCE_DIR=${ELLCC_SOURCE_DIR}
                    -DELLCC_TARGET=${ELLCC_TARGET}
                    -DCMAKE_TOOLCHAIN_FILE=${ELLCC_SOURCE_DIR}/cmake/target.cmake
                    ${COMPILER_RT_DIR}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/compiler-rt)

add_custom_target(compiler-rt ALL make
                  COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    lib/libCompilerRT-Common.a
                    ${LIBECC_DIR}/../lib/${ELLCC_TARGET}/libcompiler-rt.a
                  COMMENT "Building compiler-rt for ${ELLCC_TARGET}"
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/compiler-rt)
if(LINUX_TARGET GREATER -1)
    add_subdirectory(elf)
endif(LINUX_TARGET GREATER -1)
