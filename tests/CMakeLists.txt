cmake_minimum_required(VERSION 3.20)

# Test executable
enable_testing()

set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/tests/test_constraints.cpp
)

# Core library sources needed for tests
set(CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/column.cpp
    ${CMAKE_SOURCE_DIR}/src/core/table_schema.cpp
    ${CMAKE_SOURCE_DIR}/src/core/constraint.cpp
    ${CMAKE_SOURCE_DIR}/src/core/data_type.cpp
    ${CMAKE_SOURCE_DIR}/src/core/value.cpp
    ${CMAKE_SOURCE_DIR}/src/parser/lexer.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/logger.cpp
)

# Create test executable
add_executable(test_constraints ${TEST_SOURCES} ${CORE_SOURCES})

# Link Qt libraries
target_link_libraries(test_constraints PRIVATE
    Qt6::Core
    Qt6::Sql
)

# Include directories
target_include_directories(test_constraints PRIVATE
    ${CMAKE_SOURCE_DIR}/src/utils
    ${CMAKE_SOURCE_DIR}/src/parser
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/storage
)

# Set output directory
set_target_properties(test_constraints PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Register test
add_test(NAME ConstraintTests COMMAND test_constraints)
