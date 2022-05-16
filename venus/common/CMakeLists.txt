cmake_minimum_required(VERSION 3.8)

project(common VERSION 1.0.0 LANGUAGES C CXX)

if(CMAKE_HOST_WIN32)
    if (CMAKE_CL_64) 
        set(OS_ARCH 64)
    else()
        set(OS_ARCH 32)
        add_definitions(-D_WIN32=1)
    endif()
elseif(CMAKE_HOST_APPLE)
    add_definitions(-D_MAC=1)
    if (CMAKE_OSX_ARCHITECTURES MATCHES i386) 
        set(OS_ARCH 32)
    elseif (CMAKE_OSX_ARCHITECTURES MATCHES x86_64) 
        set(OS_ARCH 64)
	elseif (CMAKE_OSX_ARCHITECTURES MATCHES arm64) 
        set(OS_ARCH arm64)
    endif()
elseif(CMAKE_HOST_UNIX)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8 )
        set(OS_ARCH 64)
    else()
        set(OS_ARCH 32)
    endif()
    add_definitions(-D_LINUX=1)
endif()

if (CMAKE_BUILD_TYPE MATCHES Debug) 
    option(DEBUG "DEBUG BUILD" ON)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    option(DEBUG "DEBUG BUILD" OFF)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g -ggdb -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -DNDEBUG")

# also, we can use: if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -Wno-missing-braces")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	if (CMAKE_BUILD_TYPE MATCHES Debug)
        set(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} /DEBUG /Od /MTd /Zi /EHsc")
        set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} /DEBUG /Od /MTd /Zi /EHsc")
    elseif (CMAKE_BUILD_TYPE MATCHES Release)
        set(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} /MT /Zi")
        set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} /MT /Zi")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "$ENV{CXXFLAGS} /OPT:REF /OPT:ICF")
    endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
endif()


MESSAGE(STATUS "---> environment : ${CMAKE_SYSTEM_NAME}${OS_ARCH} ${CMAKE_BUILD_TYPE} ${CMAKE_CXX_COMPILER_ID} c++${CMAKE_CXX_STANDARD}")


set(ProjectOutDir ${PROJECT_SOURCE_DIR}/../output)
### output
SET(LIBRARY_OUTPUT_PATH ${ProjectOutDir})
SET(EXECUTABLE_OUTPUT_PATH ${ProjectOutDir})
### runtime output
# exe
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ProjectOutDir}) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ProjectOutDir})
# lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${ProjectOutDir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${ProjectOutDir})
### postfix && prefix
# lib
set(CMAKE_DEBUG_POSTFIX "_debug")
# exe
set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_PREFIX "debug_")


###
# 安装
INSTALL(TARGETS ${TARGET_NAME} LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
# 要求明确每一个文件
INSTALL(FILES file/ifile.h file/filetype_define.h file/struct_ole.h DESTINATION ${ProjectOutDir}/include/file)
INSTALL(FILES office/cfb_base.h office/ole.h office/ms_doc.h office/ms_ppt.h 
              office/ms_xls.h office/offcrypto_defines.h office/errors.h 
        DESTINATION ${ProjectOutDir}/include/office)
# 可以通过模式安装多个文件
INSTALL(DIRECTORY scanner DESTINATION ${ProjectOutDir}/include FILES_MATCHING PATTERN "*scan*.h")


#############################################
#
# Reference: https://stackoverflow.com/questions/6902149/listing-include-directories-in-cmake
#
# get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#     message(STATUS "dir='${dir}'")
# endforeach()
#
#############################################
#
# @travel current directory and sub-directories
# a. aux_source_directory(path variable) 不遍历子文件
# b. file(GLOB_RECURSE variable [RELATIVE path] 遍历子文件夹(但对于符号链接需要使用 FOLLOW_SYMLINKS 指定)
#
#############################################
include_directories(. utils file office 3rds matcher scanner)
file(GLOB_RECURSE HXX_LIST "${CMAKE_CURRENT_LIST_DIR}/*.hpp")
file(GLOB_RECURSE CXX_LIST "${CMAKE_CURRENT_LIST_DIR}/*.c" "${CMAKE_CURRENT_LIST_DIR}/*.c??")
set(SRCS ${HXX_LIST} ${CXX_LIST})


if (CMAKE_HOST_WIN32)
    if (OS_ARCH MATCHES 32)
        set(LIB_TYPE "_mt_32")
    else()
        set(LIB_TYPE "_mt_64")
    endif()

    if (CMAKE_BUILD_TYPE MATCHES Debug)
        set(ZLIB_NAME "zlibstaticd")
    elseif (CMAKE_BUILD_TYPE MATCHES Release)
        set(ZLIB_NAME "zlibstatic")
    endif()
else()
    set(ZLIB_NAME "z")
endif()


# zlib
set(ZLIB_DIR ${PROJECT_SOURCE_DIR}/../common/3rds/zlib_1.2.11/${CMAKE_SYSTEM_NAME})
# include && link
include_directories(${ZLIB_DIR}/include)
link_directories(${ZLIB_DIR}/lib${LIB_TYPE}_${CMAKE_BUILD_TYPE})


MESSAGE(STATUS "---> building : ${PROJECT_NAME} ${PROJECT_SOURCE_DIR}")
add_library(${PROJECT_NAME} STATIC ${SRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${ZLIB_NAME})

#############################################
#
# cmake -B ./build [-G "NMake Makefiles"] -DCMAKE_INSTALL_PREFIX=.
# make && make install
#
#############################################
