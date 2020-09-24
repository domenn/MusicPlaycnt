
macro(write_single_item m_var)
  message(STATUS ${m_var}: ${${m_var}})
endmacro()


function(write_all_stuff)
  get_cmake_property(_variableNames VARIABLES)
  list (SORT _variableNames)
  foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
  endforeach()
endfunction()