cmake_minimum_required(VERSION 3.18)
project(test_shoal)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include(CheckPIESupported)
check_pie_supported()

set(SRC
        test_param.cpp
        test_node.cpp
        test_nameformat.cpp
        test_nodelist.cpp
        test_copynodelist.cpp
        test_paramlist.cpp
        test_dict.cpp)

add_executable(test_shoal ${SRC})
target_compile_features(test_shoal PUBLIC cxx_std_17)
set_target_properties(test_shoal PROPERTIES CXX_EXTENSIONS OFF POSITION_INDEPENDENT_CODE OFF)

add_test(NAME test_shoal COMMAND test_shoal)
target_link_libraries(test_shoal PRIVATE ${GTEST_BOTH_LIBRARIES} Threads::Threads figcone::figcone)
