cmake_minimum_required(VERSION 3.8)

project(common_test VERSION 1.0.0 LANGUAGES C CXX)

option(DEBUG "DEBUG BUILD" ON)
if (DEBUG)
	set(CMAKE_BUILD_TYPE Debug)
	add_definitions(-D_DEBUG=1)
elseif (NOT DEBUG)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -g ")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -DNDEBUG -Wall -Wno-unused-value")

MESSAGE(STATUS "---> environment : ${CMAKE_SYSTEM_NAME} ${CMAKE_BUILD_TYPE} ${CMAKE_CXX_COMPILER_ID} c++${CMAKE_CXX_STANDARD}")

if(CMAKE_HOST_WIN32)
	add_definitions(-D_WIN32=1)
	set(LIB_TYPE "_mt_32")
	if (CMAKE_BUILD_TYPE MATCHES Debug)
		set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} /DEBUG /Od /MTd /Zi /EHsc")
	elseif (CMAKE_BUILD_TYPE MATCHES Release)
		set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} /MT /Zi")
		set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "$ENV{CXXFLAGS} /OPT:REF /OPT:ICF")
	endif()
elseif(CMAKE_HOST_APPLE)
	add_definitions(-D_MAC=1)
elseif(CMAKE_HOST_UNIX)
	add_definitions(-D_LINUX=1)
endif()

set(ProjectOutDir ${PROJECT_SOURCE_DIR}/../output)
###
# 可执行文件
SET(EXECUTABLE_OUTPUT_PATH ${ProjectOutDir})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ProjectOutDir}) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ProjectOutDir})

#############################################
#
#############################################

MESSAGE(STATUS "---> building : ${PROJECT_NAME} ${PROJECT_SOURCE_DIR}")

# common
add_subdirectory(${PROJECT_SOURCE_DIR}/../common ${ProjectOutDir})
# gtest
set(GTEST_DIR ${PROJECT_SOURCE_DIR}/../common/3rds/gtest_1.11.0/${CMAKE_SYSTEM_NAME})
# zlib
set(ZLIB_DIR ${PROJECT_SOURCE_DIR}/../common/3rds/zlib_1.2.11/${CMAKE_SYSTEM_NAME})
# include && link
include_directories(. ${GTEST_DIR}/include ${ZLIB_DIR}/include ${PROJECT_SOURCE_DIR}/..)
link_directories(${GTEST_DIR}/lib_${CMAKE_BUILD_TYPE} ${ZLIB_DIR}/lib${LIB_TYPE}_${CMAKE_BUILD_TYPE} ${ProjectOutDir})

set(SRCS main.cpp t_ole.cpp t_rle.cpp t_xls.cpp t_doc.cpp t_ppt.cpp t_mht.cpp t_ecma.cpp t_pcode.cpp t_scanner.cpp t_encryption.cpp)
set(Target ${CMAKE_PROJECT_NAME})
add_executable(${Target} ${SRCS})
add_dependencies(${Target} common)
if (CMAKE_BUILD_TYPE MATCHES Debug)
	target_link_libraries(${Target} PRIVATE common_debug gtestd -lpthread)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
	target_link_libraries(${Target} PRIVATE common gtest -lpthread)
endif()

#############################################
#
# cmake -B ./build [-G "NMake Makefiles"] -DCMAKE_INSTALL_PREFIX=. -DCMAKE_BUILD_TYPE=Debug
# make && make install
#
#############################################
