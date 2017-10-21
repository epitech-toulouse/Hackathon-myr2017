# Debug target
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DDEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -DDEBUG")
  if(WITH_SANITIZERS)
    add_compile_options(
      -fno-optimize-sibling-calls -fno-omit-frame-pointer
      -fsanitize=leak -fsanitize=address -fsanitize=undefined
      -fsanitize=thread)
  endif(WITH_SANITIZERS)
endif()

# Release target
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ffast-math")
endif()
