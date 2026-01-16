#pragma once

#include <QString>
#include <QVector>
#include <memory>
#include "query_result.h"

class ASTNode;
class TableManager;

/**
 * @brief Executes SQL queries against the database
 * 
 * Handles INSERT, UPDATE, DELETE, SELECT, and CREATE TABLE statements
 * by delegating to TableManager for data operations.
 */
class QueryExecutor {
public:
    QueryExecutor();
    virtual ~QueryExecutor() = default;
    
    std::unique_ptr<QueryResult> execute(const std::unique_ptr<ASTNode>& statement);
    
    void setTableManager(std::shared_ptr<TableManager> manager);
    
private:
    std::shared_ptr<TableManager> tableManager;
    
    // Statement execution methods
    std::unique_ptr<QueryResult> executeCreate(const ASTNode* statement);
    std::unique_ptr<QueryResult> executeInsert(const ASTNode* statement);
    std::unique_ptr<QueryResult> executeUpdate(const ASTNode* statement);
    std::unique_ptr<QueryResult> executeDelete(const ASTNode* statement);
    std::unique_ptr<QueryResult> executeSelect(const ASTNode* statement);
};
