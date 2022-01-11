#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "zstd::libzstd_shared" for configuration "Debug"
set_property(TARGET zstd::libzstd_shared APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(zstd::libzstd_shared PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/zstdd.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/zstdd.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS zstd::libzstd_shared )
list(APPEND _IMPORT_CHECK_FILES_FOR_zstd::libzstd_shared "${_IMPORT_PREFIX}/debug/lib/zstdd.lib" "${_IMPORT_PREFIX}/debug/bin/zstdd.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
