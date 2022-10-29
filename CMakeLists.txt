cmake_minimum_required(VERSION 3.18)
project(figcone VERSION 2.2.0)
include(GNUInstallDirs)
include(external/seal_lake)

set(INSTALL_FIGCONE_TREE ON)
SealLake_Import(
        figcone_tree   0.12.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_tree.git
        GIT_TAG        v0.12.0
)

option(FIGCONE_USE_NAMEOF "Enable automatic registration of struct field names using the nameof library" OFF)
if (FIGCONE_USE_NAMEOF)
    set(NAMEOF_OPT_INSTALL ON)
    SealLake_Import(
            nameof         0.10.2
            GIT_REPOSITORY https://github.com/Neargye/nameof.git
            GIT_TAG        v0.10.2
    )
endif()

SealLake_Bundle(
        NAME figcone_sfun
        GIT_REPOSITORY https://github.com/kamchatka-volcano/sfun.git
        GIT_TAG        v2.4.0
        DESTINATION include/figcone/detail/external
        DIRECTORIES include/sfun
        TEXT_REPLACEMENTS
            "namespace sfun" "namespace figcone::sfun"
            "SFUN_" "FIGCONE_SFUN_"
)

set(INSTALL_FIGCONE_FORMATS ON)
add_subdirectory(formats)

SealLake_HeaderOnlyLibrary(
        COMPILE_FEATURES
            cxx_std_17
        LIBRARIES
            figcone::figcone_tree
        DEPENDENCIES
            figcone_tree 0.12.0
)

if (FIGCONE_USE_NAMEOF)
    SealLake_Libraries(
            nameof::nameof
    )
    SealLake_Dependencies(
            nameof 0.10.2
    )
endif()

if (FIGCONE_USE_ALL OR FIGCONE_USE_JSON OR FIGCONE_USE_YAML OR FIGCONE_USE_TOML OR FIGCONE_USE_INI OR FIGCONE_USE_XML OR FIGCONE_USE_SHOAL)
    SealLake_Libraries(
            figcone::figcone_formats
    )
    SealLake_Dependencies(
            figcone_formats 0.12.0
    )
endif()

SealLake_OptionalBuildSteps(tests examples)