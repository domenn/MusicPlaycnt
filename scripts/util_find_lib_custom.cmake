# Function:                   find_lib_custom
# Description:                Uses find_library twice to find debug and release libraries. Pass library name as
#                             first parameter. For example, pass gtest to search for gtst.lib and gtestd.lib
#                             (also works for .a and .so on linux).
# Return:                     Link value string (both debug and optimized in same variable) to be used by
#                             target_link_libraries. Returned via variable ${_LIB_NAME}_LINKER.
# Param _LIB_NAME  :          Input and returned List
# Optional VERBOSE :          Write more messages (STATUS)
# Optional PATHS <paths...>:  List paths where to search libraries. System directories take preference over
#                             paths listed here (pass NO_DEFAULT_PATH to skip system search paths).
# Optional NO_DEFAULT_PATH :  Pass NO_DEFAULT_PATH to find_library (only search in custom paths)
# Optional IGNORE_CACHE :     find_library does nothing if it path is already in cmake cache.
#                             Pass this to ignore cache and force another search.
# Optional HIDE_CACHE :       Do not show cache variables set by find_library in cmake GUI
FUNCTION(find_lib_custom _LIB_NAME)
  
  set(options VERBOSE IGNORE_CACHE NO_DEFAULT_PATH HIDE_CACHE REQUIRED)
  set(multiValueArgs PATHS)
  cmake_parse_arguments(FLC "${options}" "${oneValueArgs}"
      "${multiValueArgs}" ${ARGN})
  
  if (${FLC_NO_DEFAULT_PATH})
    set(find_library_EXTRA_PARAMS NO_DEFAULT_PATH)
  endif ()
  
  
  if (${FLC_VERBOSE})
    if (DEFINED CACHE{${_LIB_NAME}_LINKER_DEBUG})
      message(STATUS "${_LIB_NAME}_LINKER_DEBUG already defined: ${${_LIB_NAME}_LINKER_DEBUG}")
    else ()
      message(STATUS "${_LIB_NAME}_LINKER_DEBUG not defined yet.")
    endif ()
    if (DEFINED CACHE{${_LIB_NAME}_LINKER_RELEASE})
      message(STATUS "${_LIB_NAME}_LINKER_RELEASE already defined: ${${_LIB_NAME}_LINKER_RELEASE}")
    else ()
      message(STATUS "${_LIB_NAME}_LINKER_RELEASE not defined yet.")
    endif ()
  endif ()
  
  # If any of the current cache values are unset (or IGNORE_CACHE is passed), delete cache.
  if (("${${_LIB_NAME}_LINKER_DEBUG}" STREQUAL "${_LIB_NAME}_LINKER_DEBUG-NOTFOUND" OR "${${_LIB_NAME}_LINKER_RELEASE}" STREQUAL "${_LIB_NAME}_LINKER_RELEASE-NOTFOUND")
      OR (${FLC_IGNORE_CACHE}))
    UNSET(${_LIB_NAME}_LINKER_DEBUG CACHE)
    UNSET(${_LIB_NAME}_LINKER_RELEASE CACHE)
  endif ()
  
  find_library(${_LIB_NAME}_LINKER_DEBUG NAMES ${_LIB_NAME}d PATHS ${FLC_PATHS} ${find_library_EXTRA_PARAMS})
  find_library(${_LIB_NAME}_LINKER_RELEASE NAMES ${_LIB_NAME} PATHS ${FLC_PATHS} ${find_library_EXTRA_PARAMS})
  
  if (${_LIB_NAME}_LINKER_RELEASE MATCHES ".*[/]debug[/]lib")
    # message(STATUS "${_LIB_NAME}_LINKER_RELEASE matches")
    set(${_LIB_NAME}_LINKER_DEBUG "${${_LIB_NAME}_LINKER_RELEASE}")
    string(REPLACE /debug/lib /lib ${_LIB_NAME}_LINKER_RELEASE "${${_LIB_NAME}_LINKER_RELEASE}")
    #  else ()
    #    message(STATUS "${_LIB_NAME}_LINKER_RELEASE NOT matches")
  endif ()
  
  if ("${${_LIB_NAME}_LINKER_DEBUG}" STREQUAL "${_LIB_NAME}_LINKER_DEBUG-NOTFOUND" AND "${${_LIB_NAME}_LINKER_RELEASE}" STREQUAL "${_LIB_NAME}_LINKER_RELEASE-NOTFOUND")
    if (${FLC_REQUIRED})
      message(FATAL_ERROR "Required ${_LIB_NAME} not found!")
    else ()
      UNSET(${_LIB_NAME}_LINKER CACHE)
      UNSET(${_LIB_NAME}_LINKER)
    endif ()
  elseif ("${${_LIB_NAME}_LINKER_DEBUG}" STREQUAL "${_LIB_NAME}_LINKER_DEBUG-NOTFOUND")
    if (${FLC_VERBOSE})
      message(STATUS "Only release ${_LIB_NAME} lib exists: ${${_LIB_NAME}_LINKER_RELEASE}")
    endif ()
    set(${_LIB_NAME}_LINKER general "${${_LIB_NAME}_LINKER_RELEASE}")
  elseif ("${${_LIB_NAME}_LINKER_RELEASE}" STREQUAL "${_LIB_NAME}_LINKER_RELEASE-NOTFOUND")
    if (${FLC_VERBOSE})
      message(STATUS "Only debug ${_LIB_NAME} lib exists: ${${_LIB_NAME}_LINKER_DEBUG}")
    endif ()
    set(${_LIB_NAME}_LINKER general "${${_LIB_NAME}_LINKER_DEBUG}")
  else ()
    set(${_LIB_NAME}_LINKER debug "${${_LIB_NAME}_LINKER_DEBUG}" optimized "${${_LIB_NAME}_LINKER_RELEASE}")
    if (${FLC_VERBOSE})
      message(STATUS "${_LIB_NAME} lib found: ${${_LIB_NAME}_LINKER}")
    endif ()
  endif ()
  
  if (${FLC_HIDE_CACHE})
    if (${FLC_VERBOSE})
      message(STATUS "Hiding cache for library paths for ${_LIB_NAME}.")
    endif ()
    set(${_LIB_NAME}_LINKER_DEBUG ${${_LIB_NAME}_LINKER_DEBUG} CACHE INTERNAL "Hide found cache lib!" FORCE)
    set(${_LIB_NAME}_LINKER_RELEASE ${${_LIB_NAME}_LINKER_RELEASE} CACHE INTERNAL "Hide found cache lib!" FORCE)
  else ()
    if (${FLC_VERBOSE})
      message(STATUS "Cache variables for library ${_LIB_NAME} are displayed.")
    endif ()
  endif ()
  
  # Return linkstring to caller (set variable)
  if (${FLC_VERBOSE})
    message(STATUS "Returning: ${_LIB_NAME}_LINKER - \"${${_LIB_NAME}_LINKER}\".")
  endif ()
  set(${_LIB_NAME}_LINKER ${${_LIB_NAME}_LINKER} PARENT_SCOPE)

endfunction()

# Function:                 COPY_FILES_TO_OTHER_LIST
# Description:              Copy files from one list to other and optionally remove them from source list. Does not
#                           modify input lists.
# Param _InFileList:        Input list, returned as SOURCE_FILES
# Param _excludeDirName:    File or directory name to match. For directories, add / at to not match recursively.
# Param _verbose:           Print the names of the files handled
# Param _remove:            If true, also remove matched files from source list
# Return SOURCE_FILES:      Modified input list
# Return TRANSFERED_FILES:  List of matched files
FUNCTION(COPY_FILES_TO_OTHER_LIST _InFileList _relevantFileName _verbose _remove)
  #  if ("${_verbose}")
  #    message(STATUS "tHE mE VeRBoSE ${_InFileList} ${_relevantFileName} ${_verbose} ${_remove}")
  #  else ()
  #    message(STATUS "____noVerbose  ${_InFileList} ${_relevantFileName} ${_verbose} ${_remove}")
  #  endif ("${_verbose}")
  foreach (ITR ${_InFileList})
    if ("${ITR}" MATCHES "(.*)${_relevantFileName}([A-Za-z_\\.0-9]*)")    # Check if the item matches the name received in _relevantFileName
      # Above if is also true for partial matches. So we have to make sure whole string was matched.
      # https://cmake.org/cmake/help/latest/variable/CMAKE_MATCH_n.html#variable:CMAKE_MATCH_%3Cn%3E
      if ("${ITR}" STREQUAL "${CMAKE_MATCH_0}")
        if ("${_remove}")
          if ("${_verbose}")
            message(STATUS "COPY_FILES_TO_OTHER_LIST Removing ${ITR} from _InFileList.")
          endif ("${_verbose}")
          list(REMOVE_ITEM _InFileList ${ITR})                            # Remove the item from the list
        endif ()
        if ("${_verbose}")
          message(STATUS "COPY_FILES_TO_OTHER_LIST Appending ${ITR} to _OutFileList.")
        endif ("${_verbose}")
        list(APPEND _OutFileList ${ITR})
      endif ()
    endif ()
  endforeach (ITR)
  set(SOURCE_FILES ${_InFileList} PARENT_SCOPE)                           # Return the SOURCE_FILES variable to the calling parent
  set(TRANSFERED_FILES ${_OutFileList} PARENT_SCOPE)                      # Return the TRANSFERED_FILES variable to the calling parent
ENDFUNCTION(COPY_FILES_TO_OTHER_LIST)