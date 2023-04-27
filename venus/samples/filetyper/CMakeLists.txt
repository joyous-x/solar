cmake_minimum_required (VERSION 3.8)

set(BUILD_TARGET filetyper)
project(${BUILD_TARGET} VERSION 0.1 LANGUAGES C CXX)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")


set(BUILD_TYPE_DEBUG debug)
set(BUILD_TYPE_RELEASE release)
set(CMAKE_BUILD_TYPE ${BUILD_TYPE_DEBUG} CACHE STRING "set build type to debug")
string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)


# add definitions
add_definitions(-D_N_OFFCRYPTO)
if(CMAKE_HOST_WIN32)
    if (CMAKE_CL_64)
        set(OS_ARCH 64)
    else()
        set(OS_ARCH 32)
        add_definitions(-D_WIN32=1)
    endif()

    if (OS_ARCH MATCHES 32)
        set(LIB_TYPE "_mt_32")
    else()
        set(LIB_TYPE "_mt_64")
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

    set(LIB_TYPE "")
    if (OS_ARCH MATCHES arm64)
        set(LIB_TYPE "_arm64")
    endif()
elseif(CMAKE_HOST_UNIX)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8 )
        set(OS_ARCH 64)
    else()
        set(OS_ARCH 32)
    endif()

    set(LIB_TYPE "")
    add_definitions(-D_LINUX=1)
endif()

MESSAGE(STATUS "---> CMAKE_BUILD_TYPE buil_type: ${CMAKE_BUILD_TYPE}")
if (CMAKE_BUILD_TYPE MATCHES ${BUILD_TYPE_DEBUG})
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
    if(WIN32)
        set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
    endif()
elseif (NOT CMAKE_BUILD_TYPE MATCHES ${BUILD_TYPE_DEBUG})
    set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
    if(WIN32)
        set(CMAKE_CXX_FLAGS_RELEASE "/MT")
        set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} /Zi")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "$ENV{CXXFLAGS} /DEBUG /OPT:REF /OPT:ICF")
    endif()
endif()

# 设置语言标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Meta information about the project
set(META_PROJECT_NAME        ${BUILD_TARGET})
set(META_PROJECT_DESCRIPTION "CMake for ${BUILD_TARGET}")
set(META_VERSION_MAJOR       "1")
set(META_VERSION_MINOR       "0")
set(META_VERSION_PATCH       "0")
set(META_VERSION             "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")
set(META_NAME_VERSION        "${META_PROJECT_NAME} v${META_VERSION}")

# Create version file
file(WRITE "${PROJECT_BINARY_DIR}/VERSION" "${META_NAME_VERSION}")

# 分别设置Debug和Release输出目录
MESSAGE(STATUS "---> CMAKE_BINARY_DIR dir: ${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/output)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/output)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/library)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(COMPILE_PDB_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
string(TOLOWER ${CMAKE_SYSTEM_NAME} OS_NAME_LOWER)

message(STATUS "Start building: " ${BUILD_TARGET})
message(STATUS "\tCMAKE_SYSTEM_NAME: " ${OS_NAME_LOWER})
message(STATUS "\tOS_ARCH : " ${OS_ARCH})
message(STATUS "\tCMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
message(STATUS "\tLIB_TYPE: "  ${LIB_TYPE})

# 设置包含路径
# 设置依赖动态库、静态库路径
set(GTEST_DIR ${PROJECT_SOURCE_DIR}/../common/3rds/gtest_1.11.0/${OS_NAME_LOWER})
set(GFLAGS_DIR ${PROJECT_SOURCE_DIR}/../common/3rds/gflags_2.2.2/${OS_NAME_LOWER})
include_directories(. .. ${GTEST_DIR}/include ${GFLAGS_DIR}/include)
set(GTEST_LIB_DIR ${GTEST_DIR}/lib${LIB_TYPE}_${CMAKE_BUILD_TYPE})
set(GFLAGS_LIB_DIR ${GFLAGS_DIR}/lib${LIB_TYPE}_${CMAKE_BUILD_TYPE})

# 设置依赖动态库、静态库路径

# 添加当前文件夹下所有文件为源文件
aux_source_directory(${PROJECT_SOURCE_DIR}/../common/3rds THIRD_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/../common/3rds/zip ZIP_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/../common/file FILE_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/../common/office OFFICE_SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/../common/utils UTILS_SRCS)
aux_source_directory(. LOCAL_SRCS)
set(SRCS ${THIRD_SRCS} ${ZIP_SRCS} ${FILE_SRCS} ${OFFICE_SRCS} ${UTILS_SRCS} ${LOCAL_SRCS})
# 生成动态库
#add_library(${Target} SHARED ${SRCS})

# 生成可执行文件
add_executable(${BUILD_TARGET} ${SRCS})

if (CMAKE_BUILD_TYPE MATCHES ${BUILD_TYPE_DEBUG})
    find_library(GTEST_LIB_PATH gtestd.a ${GTEST_LIB_DIR})
    find_library(GFLAGS_LIB_PATH gflags_static_debug.a ${GFLAGS_LIB_DIR})
    target_link_libraries(${BUILD_TARGET} PRIVATE ${GTEST_LIB_PATH} ${GFLAGS_LIB_PATH} -lpthread)
elseif (CMAKE_BUILD_TYPE MATCHES ${BUILD_TYPE_RELEASE})
    find_library(GTEST_LIB_PATH gtest.a ${GTEST_LIB_DIR})
    find_library(GFLAGS_LIB_PATH gflags_static.a ${GFLAGS_LIB_DIR})
    target_link_libraries(${BUILD_TARGET} PRIVATE ${GTEST_LIB_PATH} ${GFLAGS_LIB_PATH} -lpthread)
endif()

message(STATUS "Stop building: " ${BUILD_TARGET} "\n")

#####
# cmake -S . -B ./build
#
#