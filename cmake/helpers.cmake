function(message_colored message_level text color)
  string(ASCII 27 esc)
  message(${message_level} "${esc}[${color}m${text}${esc}[0m")
endfunction()

function(cxx_compiler_minimum_required compiler_id version_minimum level text)
  if(${CMAKE_CXX_COMPILER_ID} STREQUAL ${compiler_id})
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${version_minimum})
      message_colored(${level} ${text} 31)
    else()
      message_colored(STATUS "Using supported compiler ${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}" 32)
      set(CMAKE_CXX_COMPILER_IS_SUPPORTED ON PARENT_SCOPE)
    endif()
  endif()
endfunction()

function(cmake_default_build_type build_type)
  if(NOT CMAKE_BUILD_TYPE)
          message_colored(STATUS "No build type specified; using ${build_type}" 33)
          set(CMAKE_BUILD_TYPE "${build_type}")
  endif()
  string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UPPER PARENT_SCOPE)
endfunction()
