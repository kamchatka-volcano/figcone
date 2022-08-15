cmake_minimum_required(VERSION 3.18)
project(figcone VERSION 2.0.0)
include(GNUInstallDirs)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(FIGCONE_SUBPROJECT OFF)
else()
    set(FIGCONE_SUBPROJECT ON)
endif()

include(external/sfun)
include(external/gsl)
find_package(figcone_tree 0.10.0 QUIET)
if (NOT figcone_tree_FOUND)
    message("Configuration info: figcone_tree library wasn't found on your system, downloading it from the GitHub repository")
    include (external/figcone_tree)
endif()

option(FIGCONE_USE_NAMEOF "Enable automatic registration of struct field names using the nameof library" OFF)
if (FIGCONE_USE_NAMEOF)
    include(external/nameof)
endif()

option(FIGCONE_USE_ALL  "Enable all supported config formats" ON)
option(FIGCONE_USE_JSON "Enable JSON config format" OFF)
option(FIGCONE_USE_YAML "Enable YAML config format" OFF)
option(FIGCONE_USE_TOML "Enable TOML config format" OFF)
option(FIGCONE_USE_INI  "Enable INI config format" OFF)
option(FIGCONE_USE_XML  "Enable XML config format" OFF)

if (FIGCONE_USE_JSON OR FIGCONE_USE_YAML OR FIGCONE_USE_TOML OR FIGCONE_USE_INI OR FIGCONE_USE_XML)
    set(FIGCONE_USE_ALL OFF)
endif()

if(FIGCONE_USE_ALL OR FIGCONE_USE_JSON)
    find_package(figcone_json 0.10.0 QUIET)
    if (NOT figcone_json_FOUND)
        message("Configuration info: figcone_json library wasn't found on your system, downloading it from the GitHub repository")
        include (external/figcone_json)
    endif()
endif()

if(FIGCONE_USE_ALL OR FIGCONE_USE_YAML)
    find_package(figcone_yaml 0.10.0 QUIET)
    if (NOT figcone_yaml_FOUND)
        message("Configuration info: figcone_yaml library wasn't found on your system, downloading it from the GitHub repository")
        include (external/figcone_yaml)
    endif()
endif()

if(FIGCONE_USE_ALL OR FIGCONE_USE_TOML)
    find_package(figcone_toml 0.10.0 QUIET)
    if (NOT figcone_toml_FOUND)
        message("Configuration info: figcone_toml library wasn't found on your system, downloading it from the GitHub repository")
        include (external/figcone_toml)
    endif()
endif()

if(FIGCONE_USE_ALL OR FIGCONE_USE_INI)
    find_package(figcone_ini 0.10.0 QUIET)
    if (NOT figcone_ini_FOUND)
        message("Configuration info: figcone_ini library wasn't found on your system, downloading it from the GitHub repository")
        include (external/figcone_ini)
    endif()
endif()

if(FIGCONE_USE_ALL OR FIGCONE_USE_XML)
    find_package(figcone_xml 0.10.0 QUIET)
    if (NOT figcone_xml_FOUND)
        message("Configuration info: figcone_xml library wasn't found on your system, downloading it from the GitHub repository")
        include (external/figcone_xml)
    endif()
endif()

add_library(figcone INTERFACE)
add_library(figcone::figcone ALIAS figcone)
target_compile_features(figcone INTERFACE cxx_std_17)

target_include_directories(
    figcone
    INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_include_directories(
    figcone
    INTERFACE
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/figcone/detail/external>
)
if (FIGCONE_USE_NAMEOF)
    target_include_directories(
        figcone
        INTERFACE
        $<BUILD_INTERFACE:${nameof_SOURCE_DIR}/include/>
    )
endif()
target_include_directories(
    figcone
    INTERFACE
    $<BUILD_INTERFACE:${sfun_SOURCE_DIR}/include/>
)
target_include_directories(
    figcone
    INTERFACE
    $<BUILD_INTERFACE:${gsl_SOURCE_DIR}/include/>
)

target_link_libraries(figcone INTERFACE figcone::figcone_tree)
if (FIGCONE_USE_ALL OR FIGCONE_USE_JSON)
    target_link_libraries(figcone INTERFACE figcone::figcone_json)
endif()
if (FIGCONE_USE_ALL OR FIGCONE_USE_YAML)
    target_link_libraries(figcone INTERFACE figcone::figcone_yaml)
endif()
if (FIGCONE_USE_ALL OR FIGCONE_USE_TOML)
    target_link_libraries(figcone INTERFACE figcone::figcone_toml)
endif()
if (FIGCONE_USE_ALL OR FIGCONE_USE_INI)
    target_link_libraries(figcone INTERFACE figcone::figcone_ini)
endif()
if (FIGCONE_USE_ALL OR FIGCONE_USE_XML)
    target_link_libraries(figcone INTERFACE figcone::figcone_xml)
endif()


option(ENABLE_TESTS "Enable tests" OFF)
if (ENABLE_TESTS AND NOT FIGCONE_SUBPROJECT)
    enable_testing()
    add_subdirectory(tests)
endif()

option(ENABLE_EXAMPLES "Enable examples" OFF)
if (ENABLE_EXAMPLES AND NOT FIGCONE_SUBPROJECT)
    add_subdirectory(examples)
endif()

option(INSTALL_FIGCONE "Install figcone library unconditionally" OFF)
if (NOT FIGCONE_SUBPROJECT OR INSTALL_FIGCONE)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/figcone DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    install(DIRECTORY ${sfun_SOURCE_DIR}/include/sfun       DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/figcone/detail/external)
    install(DIRECTORY ${gsl_SOURCE_DIR}/include/gsl         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/figcone/detail/external)
    install(FILES ${nameof_SOURCE_DIR}/include/nameof.hpp   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/figcone/detail/external)

    set(FIGCONE_PACK_PATH ${CMAKE_INSTALL_LIBDIR}/cmake/figcone)

    install(TARGETS figcone
            EXPORT figcone-targets
            )
    install(EXPORT figcone-targets
            FILE figconeTargets.cmake
            NAMESPACE figcone::
            DESTINATION ${FIGCONE_PACK_PATH}
            )
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
            ${CMAKE_CURRENT_BINARY_DIR}/figconeConfigVersion.cmake
            COMPATIBILITY SameMajorVersion
            ARCH_INDEPENDENT
    )
    configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/figconeConfig.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/figconeConfig.cmake
            INSTALL_DESTINATION ${FIGCONE_PACK_PATH}
            )
    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/figconeConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/figconeConfigVersion.cmake
            DESTINATION ${FIGCONE_PACK_PATH}
            )
endif()