# Hunspell provides no upstream CMake package, so expose it as an imported target.

if(TARGET Hunspell::Hunspell)
    set(Hunspell_FOUND TRUE)
    return()
endif()

set(_Hunspell_LIBRARY_NAMES hunspell hunspell-1.7)

# Keep vcpkg's release and debug library roots from crossing configurations.
if(DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_TARGET_TRIPLET)
    set(_Hunspell_VCPKG_ROOT "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
    find_path(Hunspell_INCLUDE_DIR
        NAMES hunspell/hunspell.hxx
        PATHS "${_Hunspell_VCPKG_ROOT}/include"
        NO_DEFAULT_PATH
    )
    find_library(Hunspell_LIBRARY_RELEASE
        NAMES ${_Hunspell_LIBRARY_NAMES}
        PATHS "${_Hunspell_VCPKG_ROOT}/lib"
        NO_DEFAULT_PATH
    )
    find_library(Hunspell_LIBRARY_DEBUG
        NAMES ${_Hunspell_LIBRARY_NAMES}
        PATHS "${_Hunspell_VCPKG_ROOT}/debug/lib"
        NO_DEFAULT_PATH
    )
else()
    find_path(Hunspell_INCLUDE_DIR
        NAMES hunspell/hunspell.hxx
    )
    find_library(Hunspell_LIBRARY_RELEASE
        NAMES ${_Hunspell_LIBRARY_NAMES}
        PATH_SUFFIXES lib
    )
    find_library(Hunspell_LIBRARY_DEBUG
        NAMES ${_Hunspell_LIBRARY_NAMES}
        PATH_SUFFIXES debug/lib
    )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Hunspell
    REQUIRED_VARS Hunspell_INCLUDE_DIR Hunspell_LIBRARY_RELEASE
)

if(Hunspell_FOUND)
    add_library(Hunspell::Hunspell UNKNOWN IMPORTED)
    set_target_properties(Hunspell::Hunspell PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${Hunspell_INCLUDE_DIR}"
        IMPORTED_CONFIGURATIONS RELEASE
        IMPORTED_LOCATION "${Hunspell_LIBRARY_RELEASE}"
        IMPORTED_LOCATION_RELEASE "${Hunspell_LIBRARY_RELEASE}"
        MAP_IMPORTED_CONFIG_MINSIZEREL RELEASE
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE
    )
    if(Hunspell_LIBRARY_DEBUG)
        set_property(TARGET Hunspell::Hunspell APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
        set_property(TARGET Hunspell::Hunspell PROPERTY IMPORTED_LOCATION_DEBUG "${Hunspell_LIBRARY_DEBUG}")
    else()
        set_property(TARGET Hunspell::Hunspell PROPERTY MAP_IMPORTED_CONFIG_DEBUG RELEASE)
    endif()
endif()

mark_as_advanced(
    Hunspell_INCLUDE_DIR
    Hunspell_LIBRARY_RELEASE
    Hunspell_LIBRARY_DEBUG
)

unset(_Hunspell_LIBRARY_NAMES)
unset(_Hunspell_VCPKG_ROOT)
