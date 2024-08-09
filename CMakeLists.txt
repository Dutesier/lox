#######################################
#######################################
#############  DUTESIER  ##############
#######################################
#######################################

cmake_minimum_required(VERSION 3.14)

# Define the project name and the programming language
project(Lox LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set compiler flags
add_compile_options(-Wall -Werror -Wextra)

# Define source files
set(SRC_FILES 
    src/lox.cpp
    src/interpreter.cpp
    src/logger.cpp
    src/lexer.cpp
    src/parser.cpp
    src/token.cpp
    src/BaseExpression.cpp
    )

# Add the library target
add_library(Lox ${SRC_FILES})
target_include_directories(Lox PUBLIC include)

# Add target for manually testing
add_executable(LoxMainTest ${SRC_FILES} main.cpp)
target_link_libraries(LoxMainTest PRIVATE Lox)

# Add Google Test
find_package(GTest)

# Enable testing
enable_testing()

# Add the test executable
add_executable(LoxTest 
    tests/test_AstPrinter.cpp
    tests/test_lox.cpp
    tests/test_interpreter.cpp
    tests/test_lexer.cpp
    tests/test_parser.cpp
    )


# Link the test executable with the library and Google Test
target_link_libraries(LoxTest PRIVATE Lox GTest::gtest_main)

# Add tests
include(GoogleTest)
gtest_discover_tests(LoxTest)