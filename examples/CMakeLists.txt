cmake_minimum_required(VERSION 3.18)
project(figcone_examples)

file(GLOB SRC_FILES "*.cpp")
foreach(SRC_FILE ${SRC_FILES})
    SealLake_v040_StringAfterLast(${SRC_FILE} "/" TEST_NAME)
    SealLake_v040_StringBeforeLast(${TEST_NAME} "." TEST_NAME)

    SealLake_v040_Executable(
            NAME ${TEST_NAME}
            SOURCES ${SRC_FILE}
            COMPILE_FEATURES cxx_std_17
            PROPERTIES
                CXX_EXTENSIONS OFF
            LIBRARIES
                figcone::figcone
    )
endforeach()