cmake_minimum_required(VERSION 3.18)
project(figcone VERSION 2.4.4)
include(GNUInstallDirs)
include(external/seal_lake)

SealLake_IsInstalled(INSTALL_FIGCONE_TREE)
SealLake_Import(
        figcone_tree   1.0.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_tree.git
        GIT_TAG        v1.0.0
)

option(FIGCONE_USE_NAMEOF "Enable automatic registration of struct field names using the nameof library" OFF)
if (FIGCONE_USE_NAMEOF)
    SealLake_IsInstalled(NAMEOF_OPT_INSTALL)
    SealLake_Bundle(
            NAME nameof
            GIT_REPOSITORY https://github.com/Neargye/nameof.git
            GIT_TAG        84ce6d8
    )
endif()

SealLake_Bundle(
        NAME figcone_sfun
        GIT_REPOSITORY https://github.com/kamchatka-volcano/sfun.git
        GIT_TAG        v3.1.1
        DESTINATION include/figcone/detail/external
        DIRECTORIES include/sfun
        TEXT_REPLACEMENTS
            "namespace sfun" "namespace figcone::sfun"
            "SFUN_" "FIGCONE_SFUN_"
)

SealLake_IsInstalled(INSTALL_FIGCONE_FORMATS)
add_subdirectory(formats)

if (FIGCONE_USE_NAMEOF AND CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CPP_STANDARD_FEATURE cxx_std_20)
else()
    set(CPP_STANDARD_FEATURE cxx_std_17)
endif()

SealLake_HeaderOnlyLibrary(
        COMPILE_FEATURES  ${CPP_STANDARD_FEATURE}
        LIBRARIES
            figcone::figcone_tree
        DEPENDENCIES
            figcone_tree 1.0.0
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
            figcone_formats 0.12.3
    )
endif()

SealLake_OptionalBuildSteps(tests examples)