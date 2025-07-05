cmake_minimum_required(VERSION 3.18)

project(figcone_formats_deps)
include(seal_lake.cmake)

SealLake_v040_Bundle(
        NAME figcone_tree
        URL https://github.com/kamchatka-volcano/figcone_tree/releases/download/v2.2.0/figcone_tree-v2.2.0.zip
        SKIP_LOAD
        DESTINATION deps/figcone_tree
)
