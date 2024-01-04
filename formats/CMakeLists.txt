project(figcone_formats VERSION 1.0.0)

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

file(REMOVE_RECURSE ${figcone_SOURCE_DIR}/include/figcone/format)

if(FIGCONE_USE_ALL OR FIGCONE_USE_JSON)
    SealLake_Import(
            figcone_json 1.0.0
            GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_json.git
            GIT_TAG v1.0.0
    )
    SealLake_Copy(
            WILDCARDS ${figcone_json_SOURCE_DIR}/include/figcone_json/*
            DESTINATION ${figcone_SOURCE_DIR}/include/figcone/format/json
    )
endif()

if(FIGCONE_USE_ALL OR FIGCONE_USE_YAML)
    SealLake_Import(
            figcone_yaml 1.0.0
            GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_yaml.git
            GIT_TAG v1.0.0
    )
    SealLake_Copy(
            WILDCARDS ${figcone_yaml_SOURCE_DIR}/include/figcone_yaml/*
            DESTINATION ${figcone_SOURCE_DIR}/include/figcone/format/yaml
    )
endif()

if(FIGCONE_USE_ALL OR FIGCONE_USE_TOML)
    SealLake_Import(
            figcone_toml 1.0.0
            GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_toml.git
            GIT_TAG v1.0.0
    )
    SealLake_Copy(
            WILDCARDS ${figcone_toml_SOURCE_DIR}/include/figcone_toml/*
            DESTINATION ${figcone_SOURCE_DIR}/include/figcone/format/toml
    )
endif()

if(FIGCONE_USE_ALL OR FIGCONE_USE_INI)
    SealLake_Import(
            figcone_ini 1.0.0
            GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_ini.git
            GIT_TAG v1.0.0
    )
    SealLake_Copy(
            WILDCARDS ${figcone_ini_SOURCE_DIR}/include/figcone_ini/*
            DESTINATION ${figcone_SOURCE_DIR}/include/figcone/format/ini
    )
endif()


if(FIGCONE_USE_ALL OR FIGCONE_USE_XML)
    SealLake_Import(
            figcone_xml 1.0.0
            GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_xml.git
            GIT_TAG v1.0.0
    )
    SealLake_Copy(
            WILDCARDS ${figcone_xml_SOURCE_DIR}/include/figcone_xml/*
            DESTINATION ${figcone_SOURCE_DIR}/include/figcone/format/xml
    )
endif()

if(FIGCONE_USE_ALL OR FIGCONE_USE_SHOAL)
    SealLake_Import(
            figcone_shoal 1.0.0
            GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_shoal.git
            GIT_TAG v1.0.0
    )
    SealLake_Copy(
            WILDCARDS ${figcone_shoal_SOURCE_DIR}/include/figcone_shoal/*
            DESTINATION ${figcone_SOURCE_DIR}/include/figcone/format/shoal
    )
endif()

if (FIGCONE_USE_ALL OR FIGCONE_USE_JSON OR FIGCONE_USE_YAML OR FIGCONE_USE_TOML OR FIGCONE_USE_INI OR FIGCONE_USE_XML OR FIGCONE_USE_SHOAL)
SealLake_StaticLibrary(
        NAMESPACE figcone
        COMPILE_FEATURES
        cxx_std_17
        INTERFACE_LIBRARIES
        figcone::figcone_tree
        DEPENDENCIES
        figcone_tree 2.0.0
)
endif()

if (FIGCONE_USE_ALL OR FIGCONE_USE_JSON)
    SealLake_Libraries(figcone::figcone_json)
endif()
if (FIGCONE_USE_ALL OR FIGCONE_USE_INI)
    SealLake_Libraries(figcone::figcone_ini)
endif()
if (FIGCONE_USE_ALL OR FIGCONE_USE_SHOAL)
    SealLake_Libraries(figcone::figcone_shoal)
endif()
if (FIGCONE_USE_ALL OR FIGCONE_USE_TOML)
    SealLake_Libraries(figcone::figcone_toml)
endif()
if (FIGCONE_USE_ALL OR FIGCONE_USE_XML)
    SealLake_Libraries(figcone::figcone_xml)
endif()
if (FIGCONE_USE_ALL OR FIGCONE_USE_YAML)
    SealLake_Libraries(figcone::figcone_yaml)
endif()