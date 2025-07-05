cmake_minimum_required(VERSION 3.18)
project(figcone VERSION 3.3.0)
include(external/seal_lake)

SealLake_v040_IsInstallEnabled(INSTALL_FIGCONE_TREE)
SealLake_v040_Import(
        figcone_tree 2.2.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_tree.git
        GIT_TAG dev
)

option(FIGCONE_USE_NAMEOF "Enable automatic registration of struct field names using the nameof library" ON)
if (FIGCONE_USE_NAMEOF)
    SealLake_v040_IsInstallEnabled(NAMEOF_OPT_INSTALL)
    SealLake_v040_Bundle(
            NAME figcone_nameof
            SKIP_LOAD
            GIT_REPOSITORY https://github.com/Neargye/nameof.git
            GIT_TAG v0.10.4
            DESTINATION include/figcone/detail/external
            FILES include/nameof.hpp
            TEXT_REPLACEMENTS
            "namespace nameof" "namespace figcone::nameof"
            "NAMEOF_" "FIGCONE_NAMEOF_"
    )
endif ()

SealLake_v040_Bundle(
        NAME figcone_pfr
        SKIP_LOAD
        GIT_REPOSITORY https://github.com/apolukhin/pfr_non_boost.git
        GIT_TAG 2.2.0
        DESTINATION include/figcone/detail/external
        DIRECTORIES include/pfr
        TEXT_REPLACEMENTS
        "namespace pfr" "namespace figcone::pfr"
        "::pfr::" "figcone::pfr::"
        "PFR_" "FIGCONE_PFR_"
        "<pfr/" "\""
        ".hpp>" ".hpp\""
)
SealLake_v040_Bundle(
        NAME figcone_prf_detail
        SKIP_LOAD
        GIT_REPOSITORY https://github.com/apolukhin/pfr_non_boost.git
        GIT_TAG 2.2.0
        DESTINATION include/figcone/detail/external/pfr/detail
        WILDCARDS include/pfr/detail/*
        TEXT_REPLACEMENTS
        "namespace pfr" "namespace figcone::pfr"
        "::pfr::" "figcone::pfr::"
        "PFR_" "FIGCONE_PFR_"
        "<pfr/detail/" "\""
        "<pfr/" "\"../"
        ".hpp>" ".hpp\""
)
SealLake_v040_Bundle(
        NAME figcone_prf_h
        SKIP_LOAD
        GIT_REPOSITORY https://github.com/apolukhin/pfr_non_boost.git
        GIT_TAG 2.2.0
        DESTINATION include/figcone/detail/external/
        FILES include/pfr.hpp
        TEXT_REPLACEMENTS
        "namespace pfr" "namespace figcone::pfr"
        "PFR_" "FIGCONE_PFR_"
        "<pfr/" "\"pfr/"
        ".hpp>" ".hpp\""
)

SealLake_v040_Bundle(
        NAME figcone_eel
        GIT_REPOSITORY https://github.com/kamchatka-volcano/eel.git
        GIT_TAG v0.3.0
        DESTINATION include/figcone/detail/external
        DIRECTORIES include/eel
        TEXT_REPLACEMENTS
        "namespace eel" "namespace figcone::eel"
        "EEL_" "FIGCONE_EEL_"
)

SealLake_v040_IsInstallEnabled(INSTALL_FIGCONE_FORMATS)
add_subdirectory(formats)

if (FIGCONE_USE_NAMEOF AND CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CPP_STANDARD_FEATURE cxx_std_20)
else()
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

SealLake_v040_OptionalSubProjects(
        tests
        tests_cpp20
        tests_static_refl
        examples
        examples_static_refl
)