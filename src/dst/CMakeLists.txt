list(APPEND DSTLIB_SOURCE_FILES
	Evidence.cpp
	Universe.cpp
)

if (DSTLIB_BUILD_SHARED)
	add_library(dst SHARED ${DSTLIB_SOURCE_FILES})
else()
	add_library(dst STATIC ${DSTLIB_SOURCE_FILES})
endif()

add_library(dstlib::dst ALIAS dst)

target_include_directories(dst PUBLIC ${PROJECT_SOURCE_DIR}/include/)

if (DSTLIB_INSTALL_TARGETS)
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${INSTALL_DIR_INCLUDE})
	install(TARGETS dst DESTINATION ${LIB_INSTALL_DIR})
endif()
