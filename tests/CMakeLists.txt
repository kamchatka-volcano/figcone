project(test_figcone)

set(SRC
        test_param.cpp
        test_node.cpp
        test_nameformat.cpp
        test_nodelist.cpp
        test_copynodelist.cpp
        test_paramlist.cpp
        test_dict.cpp
        test_postprocessor.cpp)

SealLake_GoogleTest(
        SOURCES ${SRC}
        COMPILE_FEATURES cxx_std_17
        PROPERTIES
            CXX_EXTENSIONS OFF
        LIBRARIES
            figcone::figcone
)
