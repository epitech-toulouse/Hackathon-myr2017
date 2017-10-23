#
# Build configuration
#

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(EpitechClient C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# Options
option(CXXLIB_CLANG "Link against libc++" OFF)
option(CXXLIB_GCC "Link against stdlibc++" OFF)
option(BUILD_SHARED_LIBS "Set to OFF to build static libraries" ON)

# Rules
include(helpers)
include(compiler)
include(targets)

# Naio's ApiCodec headers and library
include(LibApiCodec)

# Build shared libraries
if(BUILD_SHARED_LIBS)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".so")
else()
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
endif()

# Project's sources and target binary
file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/EpitechClient/*.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_executable(EpitechClient ${SOURCE_FILES})

# Linking
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${LIBAPICODEC_HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/EpitechClient)
target_link_libraries(${PROJECT_NAME} libapicodec -lpthread -lz -lsfml-system -lsfml-window -lsfml-graphics)

# Re
message(STATUS " Build:")
message_colored(STATUS "   Type: ${CMAKE_BUILD_TYPE}" "37;2")
message_colored(STATUS "   CC: ${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_UPPER}}" "37;2")
message_colored(STATUS "   CXX: ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE_UPPER}}" "37;2")
message_colored(STATUS "   LD: ${CMAKE_LINKER} ${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_${CMAKE_BUILD_TYPE_UPPER}}" "37;2")
