cmake_minimum_required(VERSION 3.18)
project(figcone_examples_static_refl)

file(GLOB SRC_FILES "../examples/demo_*.cpp")
foreach(SRC_FILE ${SRC_FILES})
    SealLake_StringAfterLast(${SRC_FILE} "/" TEST_NAME)
    SealLake_StringBeforeLast(${TEST_NAME} "." TEST_NAME)

    SealLake_Executable(
            NAME ${TEST_NAME}_static_refl
            SOURCES ${SRC_FILE}
            COMPILE_FEATURES cxx_std_20
            PROPERTIES
                CXX_EXTENSIONS OFF
            LIBRARIES
                figcone::figcone
    )
    target_compile_definitions(${TEST_NAME}_static_refl PUBLIC "FIGCONE_EXAMPLE_STATIC_REFLECTION")
endforeach()

set(EXAMPLE_SRC
        ex01_static_refl.cpp
        ex02_static_refl.cpp
        ex03_static_refl.cpp
        ex04_static_refl.cpp
        ex05_static_refl.cpp
)
foreach(SRC_FILE ${EXAMPLE_SRC})
    SealLake_StringAfterLast(${SRC_FILE} "/" TEST_NAME)
    SealLake_StringBeforeLast(${TEST_NAME} "." TEST_NAME)

    SealLake_Executable(
            NAME ${TEST_NAME}
            SOURCES ${SRC_FILE}
            COMPILE_FEATURES cxx_std_20
            PROPERTIES
            CXX_EXTENSIONS OFF
            LIBRARIES
            figcone::figcone
    )
endforeach()