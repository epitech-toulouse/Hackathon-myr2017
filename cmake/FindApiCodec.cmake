include(ExternalProject)

set(ApiCodec_DIR "ApiCodec")
set(ApiCodec_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/${ApiCodec_DIR}")
set(ApiCodec_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${ApiCodec_DIR}")
set(ApiCodec_BINARY_DIR "${ApiCodec_PREFIX}/build")
set(ApiCodec_FOUND FALSE)

if(EXISTS ${ApiCodec_SOURCE_DIR}/CMakeLists.txt)

	# From ApiCodec/CMakeLists.txt
	file(READ "${ApiCodec_SOURCE_DIR}/src/BuildVersion.hpp" LIB_VERSION)
	string(REGEX MATCH "define[ \t]+LIBRARY_MAJOR_VERSION[ \t]+([0-9]+)+" LIBRARY_MAJOR_VERSION_MATCH "${LIB_VERSION}")
	set(ApiCodec_MAJOR_VERSION "${CMAKE_MATCH_1}")
	string(REGEX MATCH "define[ \t]+LIBRARY_MINOR_VERSION[ \t]+([0-9]+)+" LIBRARY_MINOR_VERSION_MATCH "${LIB_VERSION}")
	set(ApiCodec_MINOR_VERSION "${CMAKE_MATCH_1}")
	string(REGEX MATCH "define[ \t]+LIBRARY_PATCH_VERSION[ \t]+([0-9]+)+" LIBRARY_PATCH_VERSION_MATCH "${LIB_VERSION}")
	set(ApiCodec_PATCH_VERSION "${CMAKE_MATCH_1}")
	set(ApiCodec_VERSION ${ApiCodec_MAJOR_VERSION}.${ApiCodec_MINOR_VERSION}.${ApiCodec_PATCH_VERSION})

	set(ApiCodec_TARGET "libapicodec.so.${ApiCodec_VERSION}")

	ExternalProject_Add(
		ApiCodec
		PREFIX ${ApiCodec_PREFIX}
		GIT_SUBMODULES ${ApiCodec_DIR}
		SOURCE_DIR "${ApiCodec_SOURCE_DIR}"
		PATCH_COMMAND git checkout .
			COMMAND patch -p1 --forward --force --input=${CMAKE_CURRENT_SOURCE_DIR}/patch/ApiCodec.patch
		BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG>
		#UPDATE_DISCONNECTED TRUE
		UPDATE_COMMAND ""
		INSTALL_COMMAND install -D <BINARY_DIR>/${ApiCodec_TARGET} <INSTALL_DIR>/lib/${ApiCodec_TARGET}
			COMMAND ln -sf ${ApiCodec_TARGET} <INSTALL_DIR>/lib/libapicodec.so
	)
	link_directories(${ApiCodec_PREFIX}/lib)
	set(ApiCodec_INCLUDE_DIRS "${ApiCodec_SOURCE_DIR}/src")
	set(ApiCodec_LIBRARIES apicodec)
	set(ApiCodec_DEPENDENCIES ApiCodec)

	if(${ApiCodec_VERSION} VERSION_EQUAL ${ApiCodec_FIND_VERSION})
		if(NOT ApiCodec_FIND_QUIETLY)
			message(STATUS "Found ApiCodec ${ApiCodec_VERSION} in ${ApiCodec_SOURCE_DIR}")
		endif()
		set(ApiCodec_FOUND TRUE)
	endif()
endif()

if(NOT ApiCodec_FOUND)
	message(FATAL_ERROR "Could NOT find ApiCodec ${ApiCodec_FIND_VERSION}")
endif()
