

# A fixed version for qt5_wrap_cpp()
function(QTHELPER_WRAP_CPP outfiles)
    # qt5_wrap_cpp() generated a wrong "moc_.cpp" that blocked compilation. 
    # This function removed "moc_.cpp" of the outfiles, then pass to 
    # qt5_wrap_cpp()
    # 
    # This function use as a replacement of qt5_wrap_cpp().
    #
    # Issued when CMake with Qt 5.9.5, other versions not tested.

    # get include dirs
    qt5_get_moc_flags(moc_flags)

    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs OPTIONS DEPENDS)

    cmake_parse_arguments(_WRAP_CPP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(moc_files ${_WRAP_CPP_UNPARSED_ARGUMENTS})
    set(moc_options ${_WRAP_CPP_OPTIONS})
    set(moc_target ${_WRAP_CPP_TARGET})
    set(moc_depends ${_WRAP_CPP_DEPENDS})

    set(qt5_wrap_cpp_args)

    if(moc_files)
        set(qt5_wrap_cpp_args ${qt5_wrap_cpp_args} ${moc_files})
    endif()

    if(moc_target)
        set(qt5_wrap_cpp_args ${qt5_wrap_cpp_args} TARGET ${moc_target})
    endif()

    if(moc_depends)
        set(qt5_wrap_cpp_args ${qt5_wrap_cpp_args} DEPENDS ${moc_depends})
    endif()

    if(moc_options)
        set(qt5_wrap_cpp_args ${qt5_wrap_cpp_args} OPTIONS ${moc_options})
    endif()

    qt5_wrap_cpp(${outfiles} ${qt5_wrap_cpp_args})
    # Removed wrong generated "moc_.cpp"
    list(FILTER ${outfiles} EXCLUDE REGEX "[^a-zA-Z0-9_\\.]*moc_\\.cpp")

    set(${outfiles} ${${outfiles}} PARENT_SCOPE)
endfunction()