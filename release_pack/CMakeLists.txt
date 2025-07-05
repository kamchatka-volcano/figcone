cmake_minimum_required(VERSION 3.18)
project(figcone VERSION 3.3.0)
include(${CMAKE_CURRENT_LIST_DIR}/seal_lake.cmake)

SealLake_v040_IsInstallEnabled(INSTALL_FIGCONE_TREE)
find_package(figcone_tree 2.2.0 QUIET)
if (NOT TARGET figcone::figcone_tree)
    add_subdirectory(deps/figcone_tree)
endif ()

SealLake_v040_IsInstallEnabled(INSTALL_FIGCONE_FORMATS)
add_subdirectory(formats)

if (FIGCONE_USE_NAMEOF AND CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CPP_STANDARD_FEATURE cxx_std_20)
else ()
    set(CPP_STANDARD_FEATURE cxx_std_17)
endif ()

SealLake_v040_HeaderOnlyLibrary(
        COMPILE_FEATURES ${CPP_STANDARD_FEATURE}
        LIBRARIES
        figcone::figcone_tree
        DEPENDENCIES
        figcone_tree 2.2.0
)

if (FIGCONE_USE_ALL OR FIGCONE_USE_JSON OR FIGCONE_USE_YAML OR FIGCONE_USE_TOML OR FIGCONE_USE_INI OR FIGCONE_USE_XML OR FIGCONE_USE_SHOAL)
    SealLake_v040_AddLibraries(
            figcone::figcone_formats
    )
    SealLake_v040_AddDependencies(
            figcone_formats 1.3.0
    )
endif ()
