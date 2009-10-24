INCLUDE(CMakeForceCompiler)

# the name of the target operating system
set(CMAKE_SYSTEM_NAME Generic)

# Define the archiving command
set(TARGET_AR llvm-ar rf)

#add_definitions(-native)
#set(ELLCC_TARGET_SUBDIR /native/)

if(ELLCC_TARGET)
    set(sep "-")
endif(ELLCC_TARGET)
# which compilers to use for C and C++
CMAKE_FORCE_C_COMPILER(${ELLCC_SOURCE_DIR}/bin/${ELLCC_TARGET}${sep}ecc GNU)
CMAKE_FORCE_CXX_COMPILER(${ELLCC_SOURCE_DIR}/bin/${ELLCC_TARGET}${sep}e++ GNU)

# here is the target environment is located
SET(CMAKE_FIND_ROOT_PATH  ${PROJECT_SOURCE_DIR})

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

