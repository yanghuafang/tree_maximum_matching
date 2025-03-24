cmake_minimum_required(VERSION 3.31)

project(TreeMatching)

set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "C Comipler: ${CMAKE_C_COMPILER}")
message(STATUS "CXX Comipler: ${CMAKE_CXX_COMPILER}")

# Find Python3 components needed for matplotlib-cpp.
find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)

# Add the external matplotlib-cpp header directory.
# (If you simply downloaded matplotlibcpp.h into external/matplotlib-cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/matplotlib-cpp)

add_library(TreeMatchingLib
    src/TreeMatching.cpp
    src/TreePreservingEmbedding.cpp
    src/HungarianAlgorithm.cpp
)

add_library(TreeMatchingVisualizerLib
    src/TreePreservingEmbeddingVisualizer.cpp
    src/TreeMatchingVisualizer.cpp
)

# Specify the public include directories for the library.
target_include_directories(TreeMatchingLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(TreeMatchingVisualizerLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Python3_INCLUDE_DIRS}  # Required for matplotlib-cpp and any Python integration.
    ${Python3_NumPy_INCLUDE_DIRS}  # for NumPy, if find_package found it.
    ${CMAKE_CURRENT_SOURCE_DIR}/external/matplotlib-cpp
)

add_executable(TreeMatchingTest
    tests/TestTreeMatching.cpp
    tests/TreeMatchingTestHelper.cpp
)

add_executable(TreePreservingEmbeddingTest
    tests/TestTreePreservingEmbedding.cpp
    tests/TreeMatchingTestHelper.cpp
)

add_executable(HungarianAlgorithmTest
    tests/TestHungarianAlgorithm.cpp
)

# Add include directories for the executable.
target_include_directories(TreeMatchingTest PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

target_include_directories(TreePreservingEmbeddingTest PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

target_include_directories(HungarianAlgorithmTest PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

# Link the library (and, if needed, Python3 libraries) to the test executable.
target_link_libraries(TreeMatchingTest PRIVATE TreeMatchingLib TreeMatchingVisualizerLib ${Python3_LIBRARIES})

target_link_libraries(TreePreservingEmbeddingTest PRIVATE TreeMatchingLib TreeMatchingVisualizerLib 
                      ${Python3_LIBRARIES})

target_link_libraries(HungarianAlgorithmTest PRIVATE TreeMatchingLib TreeMatchingVisualizerLib ${Python3_LIBRARIES})
