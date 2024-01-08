project(test_figcone_cpp20)

set(SRC
        test_node_cpp20.cpp
        test_param_cpp20.cpp
        test_paramlist_cpp20.cpp
        test_nodelist_cpp20.cpp
        test_copynodelist_cpp20.cpp
        test_dict_cpp20.cpp
        )

SealLake_GoogleTest(
        SOURCES ${SRC}
        COMPILE_FEATURES cxx_std_20
        PROPERTIES
        CXX_EXTENSIONS OFF
        LIBRARIES
        figcone::figcone
)
