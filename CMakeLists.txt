cmake_minimum_required(VERSION 3.18)
project(figcone VERSION 2.2.0)
include(GNUInstallDirs)
include(external/seal_lake)

SealLake_FindOrDownload(
        sfun 2.4.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/sfun.git
        GIT_TAG        cmake_maintenance
)
SealLake_FindOrDownload(
        GSL 4.0.0
        GIT_REPOSITORY https://github.com/microsoft/GSL
        GIT_TAG        v4.0.0
)

set(INSTALL_FIGCONE_TREE ON)
SealLake_FindOrDownload(
        figcone_tree 0.11.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_tree.git
        GIT_TAG      cmake_maintenance
)

option(FIGCONE_USE_NAMEOF "Enable automatic registration of struct field names using the nameof library" OFF)
if (FIGCONE_USE_NAMEOF)
    SealLake_FindOrDownload(
            nameof 0.10.1
            GIT_REPOSITORY https://github.com/Neargye/nameof.git
            GIT_TAG        v0.10.1
    )
endif()

option(FIGCONE_USE_ALL  "Enable all supported config formats" ON)
option(FIGCONE_USE_JSON "Enable JSON config format" OFF)
option(FIGCONE_USE_YAML "Enable YAML config format" OFF)
option(FIGCONE_USE_TOML "Enable TOML config format" OFF)
option(FIGCONE_USE_INI  "Enable INI config format" OFF)
option(FIGCONE_USE_XML  "Enable XML config format" OFF)
option(FIGCONE_USE_SHOAL "Enable shoal config format" OFF)

if (FIGCONE_USE_JSON OR FIGCONE_USE_YAML OR FIGCONE_USE_TOML OR FIGCONE_USE_INI OR FIGCONE_USE_XML OR FIGCONE_USE_SHOAL)
    set(FIGCONE_USE_ALL OFF)
endif()

if(FIGCONE_USE_ALL OR FIGCONE_USE_JSON)
    set(INSTALL_FIGCONE_JSON ON)
    SealLake_FindOrDownload(
            figcone_json 0.11.0
            GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_json.git
            GIT_TAG        cmake_maintenance
    )
endif()

if(FIGCONE_USE_ALL OR FIGCONE_USE_YAML)
    set(INSTALL_FIGCONE_YAML ON)
    SealLake_FindOrDownload(
            figcone_yaml 0.11.0
            GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_yaml.git
            GIT_TAG        cmake_maintenance
    )
endif()

if(FIGCONE_USE_ALL OR FIGCONE_USE_TOML)
    set(INSTALL_FIGCONE_TOML ON)
    SealLake_FindOrDownload(
            figcone_toml 0.11.0
            GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_toml.git
            GIT_TAG        cmake_maintenance
    )
endif()

if(FIGCONE_USE_ALL OR FIGCONE_USE_INI)
    set(INSTALL_FIGCONE_INI ON)
    SealLake_FindOrDownload(
            figcone_ini 0.11.0
            GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_ini.git
            GIT_TAG        cmake_maintenance
    )
endif()

if(FIGCONE_USE_ALL OR FIGCONE_USE_XML)
    set(INSTALL_FIGCONE_XML ON)
    SealLake_FindOrDownload(
            figcone_xml 0.11.0
            GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_xml.git
            GIT_TAG        cmake_maintenance
    )
endif()

if(FIGCONE_USE_ALL OR FIGCONE_USE_SHOAL)
    set(INSTALL_FIGCONE_SHOAL ON)
    SealLake_FindOrDownload(
            figcone_shoal 0.2.0
            GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_shoal.git
            GIT_TAG        cmake_maintenance
    )
endif()

SealLake_HeaderOnlyLibrary(
        COMPILE_FEATURES cxx_std_17
        INCLUDES figcone/detail/external
        BUILD_STAGE_INCLUDES
            ${sfun_SOURCE_DIR}/include/
            ${gsl_SOURCE_DIR}/include/
        LIBRARIES
            figcone::figcone_tree
)

if (FIGCONE_USE_NAMEOF)
    SealLake_BuildStageIncludes(${nameof_SOURCE_DIR}/include/)
endif()

if (FIGCONE_USE_ALL OR FIGCONE_USE_JSON)
    SealLake_Libraries(figcone::figcone_json)
endif()
if (FIGCONE_USE_ALL OR FIGCONE_USE_YAML)
    SealLake_Libraries(figcone::figcone_yaml)
endif()
if (FIGCONE_USE_ALL OR FIGCONE_USE_TOML)
    SealLake_Libraries(figcone::figcone_toml)
endif()
if (FIGCONE_USE_ALL OR FIGCONE_USE_INI)
    SealLake_Libraries(figcone::figcone_ini)
endif()
if (FIGCONE_USE_ALL OR FIGCONE_USE_XML)
    SealLake_Libraries(figcone::figcone_xml)
endif()
if (FIGCONE_USE_ALL OR FIGCONE_USE_SHOAL)
    SealLake_Libraries(figcone::figcone_shoal)
endif()

SealLake_OptionalBuildSteps(tests examples)

SealLake_Install(
        PATH figcone/detail/external
        DIRECTORIES
            ${sfun_SOURCE_DIR}/include/sfun
            ${gsl_SOURCE_DIR}/include/gsl
        FILES
            ${nameof_SOURCE_DIR}/include/nameof.hpp
)