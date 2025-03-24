cmake_minimum_required(VERSION 3.31)

project(TreeMatching)

set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "C Comipler: ${CMAKE_C_COMPILER}")
message(STATUS "CXX Comipler: ${CMAKE_CXX_COMPILER}")

# Find Python3 components needed for matplotlib-cpp.
find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)

# Config argparse
find_package(argparse REQUIRED)

# Add the external matplotlib-cpp header directory.
# (If you simply downloaded matplotlibcpp.h into external/matplotlib-cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/matplotlib-cpp)

add_library(TreeMatchingLib
    src/TreeMatching.cpp
    src/TreePreservingEmbedding.cpp
    src/HungarianAlgorithm.cpp
)

# Specify the public include directories for the library.
target_include_directories(TreeMatchingLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Python3_INCLUDE_DIRS}  # Required for matplotlib-cpp and any Python integration.
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src              # To find tree generation code and related modules.
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
    ${CMAKE_CURRENT_SOURCE_DIR}/external/matplotlib-cpp
    ${Python3_INCLUDE_DIRS}
)

# Link the library (and, if needed, Python3 libraries) to the test executable.
target_link_libraries(TreeMatchingTest PRIVATE TreeMatchingLib ${Python3_LIBRARIES} argparse::argparse)

target_link_libraries(TreePreservingEmbeddingTest PRIVATE TreeMatchingLib ${Python3_LIBRARIES} argparse::argparse)

target_link_libraries(HungarianAlgorithmTest PRIVATE TreeMatchingLib ${Python3_LIBRARIES} argparse::argparse)
