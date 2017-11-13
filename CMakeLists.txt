cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

# project name, version & languages
project(EpitechClient)
set(TARGET ${PROJECT_NAME})
set(${PROJECT_NAME}_VERSION 0.0.1)
enable_language(C)  # for pthread
enable_language(CXX)

# prevents builds in root tree
if(NOT ${PROJECT_SOURCE_DIR}/build STREQUAL ${PROJECT_BINARY_DIR})
	message(FATAL_ERROR "You must compile in the build subdirectory at project root! (${PROJECT_SOURCE_DIR}/build)")
endif()

# build settings
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# modules & build path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# set default build type as Debug
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

# enabled warnings goes here
add_compile_options(
	-Wall -Wextra -Winline -Wc++11-compat -Wconversion -Wconversion-null
	-Wsign-conversion -Wctor-dtor-privacy -Wdelete-non-virtual-dtor
	-Wnarrowing -Wnon-virtual-dtor -Wreorder -Wshadow -Woverloaded-virtual
	-Wsign-promo -Wcast-align -Wmissing-declarations -Woverlength-strings
	-Wunreachable-code
)

# disabled warnings goes here
add_compile_options(
	-Wno-unknown-pragmas
)

# other flags goes here
add_compile_options(
	-fconstexpr-depth=10 -fno-elide-constructors -ffor-scope
	-ftemplate-backtrace-limit=10 -fuse-cxa-atexit -fexceptions
	-fsigned-char -fdiagnostics-color
)

# test versions, set compiler-specific flags if necessary
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.1.0")
		message(FATAL_ERROR "GCC 5.1+ is required to build this project!")
	endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "3.4.0")
		message(FATAL_ERROR "Clang 3.4+ is required to build this project!")
	endif()
endif()

# target-specific flags
set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math")

# sanitizers flags
if(WITH_SANITIZERS)
	add_compile_options(
		-fno-optimize-sibling-calls -fno-omit-frame-pointer
		-fsanitize=leak -fsanitize=address -fsanitize=undefined
		-fsanitize=thread)
endif(WITH_SANITIZERS)

# find project source files
file(GLOB_RECURSE PROJECT_SOURCE_FILES ${PROJECT_SOURCE_DIR}/EpitechClient/*.cpp)

# requires dependencies
find_package(Threads REQUIRED)
find_package(SFML 2.3 REQUIRED graphics window system)
find_package(ZLIB 1.2 REQUIRED)
find_package(ApiCodec 0.0.1 REQUIRED)

# directories with headers files for `#include` directives
include_directories(${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/${TARGET})
include_directories(${SFML_INCLUDE_DIR})
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${ApiCodec_INCLUDE_DIRS})

# linking
add_executable(${TARGET} ${PROJECT_SOURCE_FILES})
add_dependencies(${TARGET} ${ApiCodec_DEPENDENCIES})
target_link_libraries(${TARGET} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
target_link_libraries(${TARGET} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${TARGET} ${ZLIB_LIBRARIES})
target_link_libraries(${TARGET} ${ApiCodec_LIBRARIES})
