set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set default build type if not specified
cmake_default_build_type(Debug)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsigned-char -fdiagnostics-color")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-Wall -Wextra -Winline -Wc++11-compat -Wconversion \
-Wconversion-null -Wsign-conversion -Wctor-dtor-privacy -Wdelete-non-virtual-dtor -Wnarrowing \
-Wnon-virtual-dtor -Wreorder -Wshadow -Woverloaded-virtual -Wsign-promo \
-Wcast-align -Wmissing-declarations -Woverlength-strings -Wunreachable-code \
-fconstexpr-depth=10 -fno-elide-constructors -ffor-scope \
-ftemplate-backtrace-limit=10 -fuse-cxa-atexit -fexceptions -fsigned-char -fdiagnostics-color"
)

# Check compiler
cxx_compiler_minimum_required(Clang "3.4.0" FATAL_ERROR "Compiler not supported (Requires clang-3.4+ or gcc-5.1+)")
cxx_compiler_minimum_required(GNU "5.1.0" FATAL_ERROR "Compiler not supported (Requires clang-3.4+ or gcc-5.1+)")
if(NOT CMAKE_CXX_COMPILER_IS_SUPPORTED)
  message_colored(WARNING "Using unsupported compiler ${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION} !" 31)
endif()

# Set compiler and linker flags for preferred C++ library
if(CXXLIB_CLANG)
  message_colored(STATUS "Linking against libc++" 32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++ -lc++abi")
elseif(CXXLIB_GCC)
  message_colored(STATUS "Linking against libstdc++" 32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++")
endif()
