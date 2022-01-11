################################################################################
# CMake minimum version required
cmake_minimum_required(VERSION 3.0)

################################################################################
# RapidJSON source dir
set( RapidJSON_SOURCE_DIR "D:/Projects/rapidjson-master/rapidjson-master")

################################################################################
# RapidJSON build dir
set( RapidJSON_DIR "C:/Users/hancom/CMakeBuilds/221053aa-d171-d83c-8879-cebcbbf46cb1/build/x64-Debug(기본값)")

################################################################################
# Compute paths
get_filename_component(RapidJSON_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

set( RapidJSON_INCLUDE_DIR  "${RapidJSON_SOURCE_DIR}/include" )
set( RapidJSON_INCLUDE_DIRS  "${RapidJSON_SOURCE_DIR}/include" )
message(STATUS "RapidJSON found. Headers: ${RapidJSON_INCLUDE_DIRS}")

if(NOT TARGET rapidjson)
  add_library(rapidjson INTERFACE IMPORTED)
  set_property(TARGET rapidjson PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES ${RapidJSON_INCLUDE_DIRS})
endif()
