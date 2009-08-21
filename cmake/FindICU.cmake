FIND_PROGRAM (ICU_CONFIG_EXECUTABLE icu-config)

IF (NOT ICU_CONFIG_EXECUTABLE)
  SET (ICU_FOUND FALSE)
ELSE (NOT ICU_CONFIG_EXECUTABLE)

EXEC_PROGRAM ("${ICU_CONFIG_EXECUTABLE} --ldflags-libsonly"
  OUTPUT_VARIABLE ICU_LIBRARY
  RETURN_VALUE ERROR_CODE
)
SEPARATE_ARGUMENTS(ICU_LIBRARY)

EXEC_PROGRAM ("${ICU_CONFIG_EXECUTABLE} --cppflags-searchpath|sed s/^-I//"
  OUTPUT_VARIABLE ICU_INCLUDE
  RETURN_VALUE ERROR_CODE
)
SEPARATE_ARGUMENTS(ICU_INCLUDE)

IF (ICU_LIBRARY)
  SET (ICU_FOUND TRUE)
ENDIF (ICU_LIBRARY)

ENDIF (NOT ICU_CONFIG_EXECUTABLE)
