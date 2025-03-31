cmake_minimum_required(VERSION 3.15)

project(TreeMatching)

set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "C Comipler: ${CMAKE_C_COMPILER}")
message(STATUS "CXX Comipler: ${CMAKE_CXX_COMPILER}")

# Find Python3 components needed for matplotlib-cpp.
find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)

# If you installed nlohmann/json via Homebrew or Conda, this should find the package.
find_package(nlohmann_json REQUIRED)

get_target_property(NLOHMANN_INCLUDE_DIR nlohmann_json::nlohmann_json INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "nlohmann_json include directory: ${NLOHMANN_INCLUDE_DIR}")

find_package(argparse REQUIRED)

add_library(TreeMatchingLib
    src/TreeMatching.cpp
    src/TreePreservingEmbedding.cpp
    src/HungarianAlgorithm.cpp
)

add_library(UtilityLib
    src/TreePreservingEmbeddingVisualizer.cpp
    src/TreeMatchingVisualizer.cpp
    src/TreeLoader.cpp
)

# Specify the public include directories for the library.
target_include_directories(TreeMatchingLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(UtilityLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Python3_INCLUDE_DIRS}  # Required for matplotlib-cpp and any Python integration.
    ${Python3_NumPy_INCLUDE_DIRS}  # for NumPy, if find_package found it.
    ${CMAKE_CURRENT_SOURCE_DIR}/external/matplotlib-cpp
    ${NLOHMANN_INCLUDE_DIR}
)

add_executable(TreeMatchingTest
    tests/TestTreeMatching.cpp
    tests/TreeMatchingTestHelper.cpp
)

add_executable(TreeMatchingTimeTest
    tests/TestTreeMatchingTime.cpp
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
target_link_libraries(TreeMatchingTest PRIVATE TreeMatchingLib UtilityLib ${Python3_LIBRARIES} 
                      nlohmann_json::nlohmann_json argparse::argparse)

target_link_libraries(TreeMatchingTimeTest PRIVATE TreeMatchingLib UtilityLib ${Python3_LIBRARIES} 
                      nlohmann_json::nlohmann_json argparse::argparse)

target_link_libraries(TreePreservingEmbeddingTest PRIVATE TreeMatchingLib UtilityLib 
                      ${Python3_LIBRARIES} nlohmann_json::nlohmann_json argparse::argparse)

target_link_libraries(HungarianAlgorithmTest PRIVATE TreeMatchingLib UtilityLib ${Python3_LIBRARIES})
