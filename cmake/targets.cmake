# Debug target
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DDEBUG)
  add_compile_options(
    -g -march=atom -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -ffast-math
    -ftree-loop-if-convert -funroll-loops -mfpmath=sse -DDEBUG)
  if(WITH_SANITIZERS)
    add_compile_options(
      -fno-optimize-sibling-calls -fno-omit-frame-pointer
      -fsanitize=leak -fsanitize=address -fsanitize=undefined
      -fsanitize=thread)
  endif(WITH_SANITIZERS)
endif()

# Release target
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_compile_options(-O3 -march=atom -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -ffast-math
    -ftree-loop-if-convert -funroll-loops -mfpmath=sse)
endif()
