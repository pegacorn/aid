cmake_minimum_required (VERSION 3.1)
project(cpp-aid CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

set(cpp-aid_sources
  ${PROJECT_SOURCE_DIR}/src/Endian.cpp
  ${PROJECT_SOURCE_DIR}/src/DynamicEndianConverter.cpp
  )
add_library(c++-aid			SHARED ${cpp-aid_sources})
add_library(c++-aid-static	STATIC ${cpp-aid_sources})

include_directories("${PROJECT_SOURCE_DIR}/include")

include(TestBigEndian)
test_big_endian(is_big_endian)
if(${is_big_endian})
  set(AID_ENDIAN "AID_ENDIAN_BIG")
else()
  set(AID_ENDIAN "AID_ENDIAN_LITTLE")
endif()

configure_file(
  "${PROJECT_SOURCE_DIR}/include/aid/Config.hpp.in"
  "${PROJECT_SOURCE_DIR}/include/aid/Config.hpp"
  @ONLY)

target_compile_definitions(c++-aid
  PUBLIC	"AID_DLL"
  )
set_target_properties(c++-aid
  PROPERTIES 
  DEFINE_SYMBOL	"AID_EXPORTS"
  )

# http://www.cmake.org/Wiki/CMake_FAQ#How_do_I_make_my_shared_and_static_libraries_have_the_same_root_name.2C_but_different_suffixes.3F
set(static_library_prefix "${CMAKE_STATIC_LIBRARY_PREFIX}")
message("CMAKE_STATIC_LIBRARY_PREFIX = ${static_library_prefix}")
if("${static_library_prefix}" STREQUAL "")
  if(MSVC)
	set(static_library_prefix "lib")
  endif()
endif()
message("static_library_prefix = ${static_library_prefix}")
set_target_properties(c++-aid-static
  PROPERTIES
  OUTPUT_NAME	"c++-aid"
  PREFIX		"${static_library_prefix}"
  )

# TODO: プラットフォーム依存したくないので、CMAKE_CXX_FLAGSはCMakeで何とかできないか？
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
elseif(MSVC)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /I<clang-c-include-dir> /L<clang-c-lib-dir>")
endif()
