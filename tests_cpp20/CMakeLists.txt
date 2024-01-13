project(test_figcone_cpp20)

set(SRC
        ../tests/test_param.cpp
        ../tests/test_node.cpp
        ../tests/test_nameformat.cpp
        ../tests/test_nodelist.cpp
        ../tests/test_copynodelist.cpp
        ../tests/test_paramlist.cpp
        ../tests/test_dict.cpp
        ../tests/test_postprocessor.cpp
        ../tests/test_unregisteredfieldhandler.cpp
        ../tests/test_defaultunregisteredfieldhandler.cpp)

SealLake_GoogleTest(
        SOURCES ${SRC}
        COMPILE_FEATURES cxx_std_20
        PROPERTIES
        CXX_EXTENSIONS OFF
        LIBRARIES
        figcone::figcone
)
