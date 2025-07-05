project(figcone_formats VERSION 1.3.0)

option(FIGCONE_USE_ALL "Enable all supported config formats" ON)
option(FIGCONE_USE_JSON "Enable JSON config format" OFF)
option(FIGCONE_USE_YAML "Enable YAML config format" OFF)
option(FIGCONE_USE_TOML "Enable TOML config format" OFF)
option(FIGCONE_USE_INI "Enable INI config format" OFF)
option(FIGCONE_USE_XML "Enable XML config format" OFF)
option(FIGCONE_USE_SHOAL "Enable shoal config format" OFF)
if (FIGCONE_USE_JSON OR FIGCONE_USE_YAML OR FIGCONE_USE_TOML OR FIGCONE_USE_INI OR FIGCONE_USE_XML OR FIGCONE_USE_SHOAL)
    set(FIGCONE_USE_ALL OFF)
endif ()

if (FIGCONE_USE_ALL OR FIGCONE_USE_JSON)
    add_subdirectory(deps/figcone_json)
endif ()

if (FIGCONE_USE_ALL OR FIGCONE_USE_YAML)
    add_subdirectory(deps/figcone_yaml)
endif ()

if (FIGCONE_USE_ALL OR FIGCONE_USE_TOML)
    add_subdirectory(deps/figcone_toml)
endif ()

if (FIGCONE_USE_ALL OR FIGCONE_USE_INI)
    add_subdirectory(deps/figcone_ini)
endif ()

if (FIGCONE_USE_ALL OR FIGCONE_USE_XML)
    add_subdirectory(deps/figcone_xml)
endif ()

if (FIGCONE_USE_ALL OR FIGCONE_USE_SHOAL)
    add_subdirectory(deps/figcone_shoal)
endif ()

if (FIGCONE_USE_ALL OR FIGCONE_USE_JSON OR FIGCONE_USE_YAML OR FIGCONE_USE_TOML OR FIGCONE_USE_INI OR FIGCONE_USE_XML OR FIGCONE_USE_SHOAL)
    SealLake_v040_StaticLibrary(
            NAMESPACE figcone
            COMPILE_FEATURES
            cxx_std_17
            INTERFACE_LIBRARIES
            figcone::figcone_tree
            DEPENDENCIES
            figcone_tree 2.2.0
    )
endif ()

if (FIGCONE_USE_ALL OR FIGCONE_USE_JSON)
    SealLake_v040_AddLibraries(figcone::figcone_json)
endif ()
if (FIGCONE_USE_ALL OR FIGCONE_USE_INI)
    SealLake_v040_AddLibraries(figcone::figcone_ini)
endif ()
if (FIGCONE_USE_ALL OR FIGCONE_USE_SHOAL)
    SealLake_v040_AddLibraries(figcone::figcone_shoal)
endif ()
if (FIGCONE_USE_ALL OR FIGCONE_USE_TOML)
    SealLake_v040_AddLibraries(figcone::figcone_toml)
endif ()
if (FIGCONE_USE_ALL OR FIGCONE_USE_XML)
    SealLake_v040_AddLibraries(figcone::figcone_xml)
endif ()
if (FIGCONE_USE_ALL OR FIGCONE_USE_YAML)
    SealLake_v040_AddLibraries(figcone::figcone_yaml)
endif ()