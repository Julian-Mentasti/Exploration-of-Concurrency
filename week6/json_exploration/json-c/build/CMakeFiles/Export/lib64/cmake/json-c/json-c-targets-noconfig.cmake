#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "json-c::json-c" for configuration ""
set_property(TARGET json-c::json-c APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(json-c::json-c PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib64/libjson-c.so"
  IMPORTED_SONAME_NOCONFIG "libjson-c.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS json-c::json-c )
list(APPEND _IMPORT_CHECK_FILES_FOR_json-c::json-c "${_IMPORT_PREFIX}/lib64/libjson-c.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
