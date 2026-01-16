cmake_minimum_required(VERSION 3.20)
project(SimpleRDBMS VERSION 0.1.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt Configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set Qt6 installation path for CMake
set(CMAKE_PREFIX_PATH "C:/Qt/6.10.1/mingw_64/lib/cmake")

find_package(Qt6 COMPONENTS 
    Core 
    Gui 
    Widgets 
    Sql
    Network
    REQUIRED
)

# Project directories
set(SOURCES_DIR ${CMAKE_SOURCE_DIR}/src)
set(UTILS_DIR ${SOURCES_DIR}/utils)
set(PARSER_DIR ${SOURCES_DIR}/parser)
set(CORE_DIR ${SOURCES_DIR}/core)
set(STORAGE_DIR ${SOURCES_DIR}/storage)
set(UI_DIR ${SOURCES_DIR}/ui)
set(SERVER_DIR ${SOURCES_DIR}/server)

# Executable sources
set(EXECUTABLE_SOURCES
    ${SOURCES_DIR}/main.cpp
    
    # Utils
    ${UTILS_DIR}/logger.cpp
    ${UTILS_DIR}/logger.h
    
    # Parser
    ${PARSER_DIR}/token.h
    ${PARSER_DIR}/lexer.h
    ${PARSER_DIR}/lexer.cpp
    ${PARSER_DIR}/parser.h
    ${PARSER_DIR}/parser.cpp
    ${PARSER_DIR}/ast_nodes.h
    
    # Core
    ${CORE_DIR}/data_type.h
    ${CORE_DIR}/data_type.cpp
    ${CORE_DIR}/value.h
    ${CORE_DIR}/value.cpp
    ${CORE_DIR}/column.h
    ${CORE_DIR}/column.cpp
    ${CORE_DIR}/table_schema.h
    ${CORE_DIR}/table_schema.cpp
    ${CORE_DIR}/constraint.h
    ${CORE_DIR}/constraint.cpp
    ${CORE_DIR}/query_executor.h
    ${CORE_DIR}/query_executor.cpp
    ${CORE_DIR}/table_manager.h
    ${CORE_DIR}/table_manager.cpp
    ${CORE_DIR}/index.h
    ${CORE_DIR}/index.cpp
    ${CORE_DIR}/transaction_manager.h
    ${CORE_DIR}/transaction_manager.cpp
    ${CORE_DIR}/user_manager.h
    ${CORE_DIR}/user_manager.cpp
    
    # Storage
    ${STORAGE_DIR}/storage_engine.h
    ${STORAGE_DIR}/storage_engine.cpp
    
    # UI
    ${UI_DIR}/main_window.h
    ${UI_DIR}/main_window.cpp
    ${UI_DIR}/sql_editor_page.h
    ${UI_DIR}/sql_editor_page.cpp
    ${UI_DIR}/table_manager_page.h
    ${UI_DIR}/table_manager_page.cpp
    ${UI_DIR}/login_dialog.h
    ${UI_DIR}/login_dialog.cpp
    ${UI_DIR}/create_table_dialog.h
    ${UI_DIR}/create_table_dialog.cpp
    ${UI_DIR}/logs_page.h
    ${UI_DIR}/logs_page.cpp

    # Server
    ${SERVER_DIR}/db_server.h
    ${SERVER_DIR}/db_server.cpp
)

# Create executable
add_executable(SimpleRDBMS_new ${EXECUTABLE_SOURCES})

# Link Qt libraries
target_link_libraries(SimpleRDBMS_new PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::Network
)

# Include directories
target_include_directories(SimpleRDBMS_new PRIVATE
    ${UTILS_DIR}
    ${PARSER_DIR}
    ${CORE_DIR}
    ${STORAGE_DIR}
    ${UI_DIR}
    ${SERVER_DIR}
)

# Output directory
set_target_properties(SimpleRDBMS_new PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Storage persistence test executable
add_executable(test_storage test_storage.cpp
    # Utils
    ${UTILS_DIR}/logger.cpp
    ${UTILS_DIR}/logger.h
    
    # Core
    ${CORE_DIR}/data_type.h
    ${CORE_DIR}/data_type.cpp
    ${CORE_DIR}/value.h
    ${CORE_DIR}/value.cpp
    ${CORE_DIR}/column.h
    ${CORE_DIR}/column.cpp
    ${CORE_DIR}/table_schema.h
    ${CORE_DIR}/table_schema.cpp
    ${CORE_DIR}/constraint.h
    ${CORE_DIR}/constraint.cpp
    
    # Storage
    ${STORAGE_DIR}/storage_engine.h
    ${STORAGE_DIR}/storage_engine.cpp
)

target_link_libraries(test_storage PRIVATE
    Qt6::Core
)

target_include_directories(test_storage PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${UTILS_DIR}
    ${CORE_DIR}
    ${STORAGE_DIR}
)

set_target_properties(test_storage PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Persistence integration test executable
add_executable(test_persistence_integration test_persistence_integration.cpp
    # Utils
    ${UTILS_DIR}/logger.cpp
    ${UTILS_DIR}/logger.h
    
    # Parser
    ${PARSER_DIR}/token.h
    ${PARSER_DIR}/lexer.h
    ${PARSER_DIR}/lexer.cpp
    ${PARSER_DIR}/parser.h
    ${PARSER_DIR}/parser.cpp
    ${PARSER_DIR}/ast_nodes.h
    
    # Core
    ${CORE_DIR}/data_type.h
    ${CORE_DIR}/data_type.cpp
    ${CORE_DIR}/value.h
    ${CORE_DIR}/value.cpp
    ${CORE_DIR}/column.h
    ${CORE_DIR}/column.cpp
    ${CORE_DIR}/table_schema.h
    ${CORE_DIR}/table_schema.cpp
    ${CORE_DIR}/constraint.h
    ${CORE_DIR}/constraint.cpp
    ${CORE_DIR}/query_executor.h
    ${CORE_DIR}/query_executor.cpp
    ${CORE_DIR}/table_manager.h
    ${CORE_DIR}/table_manager.cpp
    ${CORE_DIR}/index.h
    ${CORE_DIR}/index.cpp
    ${CORE_DIR}/transaction_manager.h
    ${CORE_DIR}/transaction_manager.cpp
    
    # Storage
    ${STORAGE_DIR}/storage_engine.h
    ${STORAGE_DIR}/storage_engine.cpp
)

target_link_libraries(test_persistence_integration PRIVATE
    Qt6::Core
)

target_include_directories(test_persistence_integration PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${UTILS_DIR}
    ${PARSER_DIR}
    ${CORE_DIR}
    ${STORAGE_DIR}
)

set_target_properties(test_persistence_integration PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Enable testing
enable_testing()
add_subdirectory(tests)


