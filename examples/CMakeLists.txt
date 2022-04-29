cmake_minimum_required(VERSION 3.18)
project(figcone_examples)

add_executable(ex01 ex01.cpp)
target_compile_features(ex01 PUBLIC cxx_std_17)
target_link_libraries(ex01 PRIVATE figcone::figcone)

add_executable(ex02 ex02.cpp)
target_compile_features(ex02 PUBLIC cxx_std_17)
target_link_libraries(ex02 PRIVATE figcone::figcone)

add_executable(ex03 ex03.cpp yaml_impl.cpp)
target_compile_features(ex03 PUBLIC cxx_std_17)
target_link_libraries(ex03 PRIVATE figcone::figcone)

add_executable(ex04 ex04.cpp yaml_impl.cpp)
target_compile_features(ex04 PUBLIC cxx_std_17)
target_link_libraries(ex04 PRIVATE figcone::figcone)

add_executable(demo_json demo_json.cpp)
target_compile_features(demo_json PUBLIC cxx_std_17)
target_link_libraries(demo_json PRIVATE figcone::figcone)

add_executable(demo_yaml demo_yaml.cpp yaml_impl.cpp)
target_compile_features(demo_yaml PUBLIC cxx_std_17)
target_link_libraries(demo_yaml PRIVATE figcone::figcone)

add_executable(demo_toml demo_toml.cpp)
target_compile_features(demo_toml PUBLIC cxx_std_17)
target_link_libraries(demo_toml PRIVATE figcone::figcone)

add_executable(demo_xml demo_xml.cpp)
target_compile_features(demo_xml PUBLIC cxx_std_17)
target_link_libraries(demo_xml PRIVATE figcone::figcone)

add_executable(demo_ini demo_ini.cpp)
target_compile_features(demo_ini PUBLIC cxx_std_17)
target_link_libraries(demo_ini PRIVATE figcone::figcone)

add_executable(demo_parser demo_parser.cpp)
target_compile_features(demo_parser PUBLIC cxx_std_17)
target_link_libraries(demo_parser PRIVATE figcone::figcone)