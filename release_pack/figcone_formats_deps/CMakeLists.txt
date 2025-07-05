cmake_minimum_required(VERSION 3.18)

project(figcone_formats_deps)
include(seal_lake.cmake)

SealLake_v040_Bundle(
        NAME figcone_json
        URL https://github.com/kamchatka-volcano/figcone_json/releases/download/v1.2.0/figcone_json-v1.2.0.zip
        SKIP_LOAD
        DESTINATION deps/figcone_json
)
SealLake_v040_Bundle(
        NAME figcone_yaml
        URL https://github.com/kamchatka-volcano/figcone_yaml/releases/download/v1.2.0/figcone_yaml-v1.2.0.zip
        SKIP_LOAD
        DESTINATION deps/figcone_yaml
)
SealLake_v040_Bundle(
        NAME figcone_toml
        URL https://github.com/kamchatka-volcano/figcone_toml/releases/download/v1.2.0/figcone_toml-v1.2.0.zip
        SKIP_LOAD
        DESTINATION deps/figcone_toml
)
SealLake_v040_Bundle(
        NAME figcone_ini
        URL https://github.com/kamchatka-volcano/figcone_ini/releases/download/v1.2.0/figcone_ini-v1.2.0.zip
        SKIP_LOAD
        DESTINATION deps/figcone_ini
)
SealLake_v040_Bundle(
        NAME figcone_xml
        URL https://github.com/kamchatka-volcano/figcone_xml/releases/download/v1.3.0/figcone_xml-v1.3.0.zip
        SKIP_LOAD
        DESTINATION deps/figcone_xml
)
SealLake_v040_Bundle(
        NAME figcone_shoal
        URL https://github.com/kamchatka-volcano/figcone_shoal/releases/download/v1.2.0/figcone_shoal-v1.2.0.zip
        SKIP_LOAD
        DESTINATION deps/figcone_shoal
)