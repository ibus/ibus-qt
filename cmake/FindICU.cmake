find_program(ICU_CONFIG_EXECUTABLE NAMES icu-config DOC "icu-config executable")
mark_as_advanced(ICU_CONFIG_EXECUTABLE)

if(ICU_CONFIG_EXECUTABLE)
  set(ICU_FOUND "YES")
else(ICU_CONFIG_EXECUTABLE)
  set(ICU_FOUND "NO")
  # if(ICU_FIND_REQUIRED)
  #   message(FATAL_ERROR "Could not find icu")
  # endif(ICU_FIND_REQUIRED)
endif(ICU_CONFIG_EXECUTABLE)

if(ICU_FOUND)
  # get include dirs
  execute_process(
    COMMAND ${ICU_CONFIG_EXECUTABLE} --cppflags-searchpath
    COMMAND sed s/^-I//
    OUTPUT_VARIABLE ICU_INCLUDE_DIRS
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
  )
  separate_arguments(ICU_INCLUDE_DIRS)

  # get libraries
  execute_process(
    COMMAND ${ICU_CONFIG_EXECUTABLE} --ldflags-searchpath
    OUTPUT_VARIABLE ICU_LIBRARY_DIRS
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
  )
  separate_arguments(ICU_LIBRARY_DIRS)

  execute_process(
    COMMAND ${ICU_CONFIG_EXECUTABLE} --ldflags-libsonly
    OUTPUT_VARIABLE ICU_LIBRARIES
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
  )
  separate_arguments(ICU_LIBRARIES)

endif(ICU_FOUND)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ICU DEFAULT_MSG
  ICU_LIBRARIES ICU_LIBRARY_DIRS ICU_INCLUDE_DIRS)
