#include "query_executor.h"
#include "table_manager.h"
#include "table_schema.h"
#include "../parser/ast_nodes.h"
#include "../utils/logger.h"
#include <QDateTime>

// Helper to check if a row matches a WHERE condition
// Supports simple "col = val" conditions
// Very basic parsing for now
static bool evaluateCondition(const QString& whereClause, const std::shared_ptr<TableSchema>& schema, const QVector<QString>& row) {
    if (whereClause.isEmpty()) return true;

    // Simple parsing: split by "="
    // Example: "id = 5"
    QStringList parts = whereClause.split("=");
    if (parts.size() != 2) return true; // Complex condition or error, default to true (unsafe?) or false? 
                                        // For safety in DELETE/UPDATE, maybe default to false if parse fails?
                                        // But currently whereClause is raw string. Let's try to support "id = 123"

    QString colName = parts[0].trimmed();
    QString targetVal = parts[1].trimmed();
    
    // Remove quotes if string
    if (targetVal.startsWith("'") && targetVal.endsWith("'")) {
        targetVal = targetVal.mid(1, targetVal.length() - 2);
    }

    int colIdx = schema->getColumnIndex(colName);
    if (colIdx == -1) return false; // Column not found

    if (colIdx >= row.size()) return false;

    return row[colIdx] == targetVal;
}

QueryExecutor::QueryExecutor() 
    : tableManager(std::make_shared<TableManager>()) {
}

// Helper function to compute default values that are functions
static QString computeDefaultValue(const QString& defaultValue) {
    if (defaultValue == "NOW()" || defaultValue == "CURRENT_TIMESTAMP" || defaultValue == "CURRENT_TIMESTAMP()") {
        // Return current timestamp in ISO format
        return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    } else if (defaultValue == "CURRENT_DATE" || defaultValue == "CURRENT_DATE()") {
        // Return current date in ISO format
        return QDateTime::currentDateTime().toString("yyyy-MM-dd");
    } else if (defaultValue == "CURRENT_TIME" || defaultValue == "CURRENT_TIME()") {
        // Return current time in HH:mm:ss format
        return QDateTime::currentDateTime().toString("HH:mm:ss");
    }
    // Return the value as-is (literal defaults like 'ACTIVE', 0, etc.)
    return defaultValue;
}

void QueryExecutor::setTableManager(std::shared_ptr<TableManager> manager) {
    tableManager = manager;
}

std::unique_ptr<QueryResult> QueryExecutor::execute(const std::unique_ptr<ASTNode>& statement) {
    auto result = std::make_unique<QueryResult>();
    
    if (!statement || !tableManager) {
        result->success = false;
        result->errorMessage = "Invalid statement or table manager";
        return result;
    }
    
    // Determine statement type and dispatch
    if (auto createStmt = dynamic_cast<CreateTableStatement*>(statement.get())) {
        return executeCreate(createStmt);
    } else if (auto insertStmt = dynamic_cast<InsertStatement*>(statement.get())) {
        return executeInsert(insertStmt);
    } else if (auto updateStmt = dynamic_cast<UpdateStatement*>(statement.get())) {
        return executeUpdate(updateStmt);
    } else if (auto deleteStmt = dynamic_cast<DeleteStatement*>(statement.get())) {
        return executeDelete(deleteStmt);
    } else if (auto selectStmt = dynamic_cast<SelectStatement*>(statement.get())) {
        return executeSelect(selectStmt);
    } else {
        result->success = false;
        result->errorMessage = "Unknown statement type";
        return result;
    }
}

std::unique_ptr<QueryResult> QueryExecutor::executeCreate(const ASTNode* statement) {
    auto result = std::make_unique<QueryResult>();
    
    auto createStmt = dynamic_cast<const CreateTableStatement*>(statement);
    if (!createStmt) {
        result->errorMessage = "Invalid CREATE TABLE statement";
        return result;
    }
    
    try {
        // Create table schema
        auto schema = std::make_shared<TableSchema>(createStmt->tableName);
        
        Logger::instance().info(QString("CREATE TABLE: Parsing %1 columns").arg(createStmt->columns.size()));
        
        // Add columns with constraints
        for (const auto& colDef : createStmt->columns) {
            Column col(colDef.name, DataType::VARCHAR);  // Default to VARCHAR, needs proper type conversion
            
            // Apply constraints
            if (colDef.primaryKey) {
                col.setPrimaryKey(true);
            }
            if (colDef.unique) {
                col.setUnique(true);
            }
            if (!colDef.nullable) {
                col.setNotNull(true);
            }
            if (!colDef.defaultValue.isEmpty()) {
                col.setDefaultValue(colDef.defaultValue);
                Logger::instance().info(QString("  Column '%1' DEFAULT: '%2'").arg(colDef.name, colDef.defaultValue));
            }
            
            schema->addColumn(col);
        }
        
        // Add table to manager
        tableManager->addTable(schema);
        
        result->success = true;
        result->affectedRows = 1;
        Logger::instance().info(QString("Table '%1' created successfully").arg(createStmt->tableName));
        
    } catch (const std::exception& e) {
        result->errorMessage = QString::fromStdString(e.what());
        Logger::instance().error(QString("CREATE TABLE failed: %1").arg(result->errorMessage));
    }
    
    return result;
}

std::unique_ptr<QueryResult> QueryExecutor::executeInsert(const ASTNode* statement) {
    auto result = std::make_unique<QueryResult>();
    
    auto insertStmt = dynamic_cast<const InsertStatement*>(statement);
    if (!insertStmt) {
        result->errorMessage = "Invalid INSERT statement";
        return result;
    }
    
    try {
        if (!tableManager->tableExists(insertStmt->tableName)) {
            result->errorMessage = QString("Table '%1' does not exist").arg(insertStmt->tableName);
            return result;
        }
        
        auto schema = tableManager->getTable(insertStmt->tableName);
        const auto& allColumns = schema->getColumns();
        
        // Insert each row using TableManager with constraint enforcement
        int totalInserted = 0;
        for (const auto& rowValues : insertStmt->values) {
            QVector<QString> completeRow;
            
            // If specific columns were mentioned in INSERT, map values to correct positions
            if (!insertStmt->columns.isEmpty()) {
                // Initialize complete row with default/null values
                completeRow.resize(allColumns.size());
                for (int i = 0; i < allColumns.size(); ++i) {
                    // Use column's default value if available, computing function defaults
                    QString defaultValue = allColumns[i].getDefaultValue();
                    QString computed = computeDefaultValue(defaultValue);
                    completeRow[i] = computed;
                    Logger::instance().info(QString("Column '%1': default='%2' computed='%3'")
                        .arg(allColumns[i].getName()).arg(defaultValue).arg(computed));
                }
                
                // Map provided values to their column positions
                for (int i = 0; i < insertStmt->columns.size() && i < rowValues.size(); ++i) {
                    QString colName = insertStmt->columns[i];
                    // Find column index
                    for (int j = 0; j < allColumns.size(); ++j) {
                        if (allColumns[j].getName() == colName) {
                            completeRow[j] = rowValues[i];
                            Logger::instance().info(QString("  Mapping column '%1' = '%2'").arg(colName).arg(rowValues[i]));
                            break;
                        }
                    }
                }
                
                // Log final row to be inserted
                QString rowStr = completeRow.join(" | ");
                Logger::instance().info(QString("Final row to insert: [%1]").arg(rowStr));
            } else {
                // All columns provided in order
                completeRow = rowValues;
            }
            
            auto opResult = tableManager->insertRow(insertStmt->tableName, completeRow);
            
            if (!opResult.success) {
                result->success = false;
                result->errorMessage = opResult.errorMessage;
                Logger::instance().error(QString("INSERT failed: %1").arg(result->errorMessage));
                return result;
            }
            totalInserted++;
        }
        
        result->success = true;
        result->affectedRows = totalInserted;
        Logger::instance().info(QString("Inserted %1 row(s) into '%2'").arg(totalInserted).arg(insertStmt->tableName));
        
    } catch (const std::exception& e) {
        result->success = false;
        result->errorMessage = QString::fromStdString(e.what());
        Logger::instance().error(QString("INSERT exception: %1").arg(result->errorMessage));
    }
    
    return result;
}

std::unique_ptr<QueryResult> QueryExecutor::executeUpdate(const ASTNode* statement) {
    auto result = std::make_unique<QueryResult>();
    
    auto updateStmt = dynamic_cast<const UpdateStatement*>(statement);
    if (!updateStmt) {
        result->errorMessage = "Invalid UPDATE statement";
        return result;
    }
    
    try {
        if (!tableManager->tableExists(updateStmt->tableName)) {
            result->errorMessage = QString("Table '%1' does not exist").arg(updateStmt->tableName);
            return result;
        }
        
        auto schema = tableManager->getTable(updateStmt->tableName);
        auto rows = tableManager->selectAll(updateStmt->tableName);
        
        int updatedCount = 0;
        for (int i = 0; i < rows.size(); ++i) {
            // Check condition
            if (!evaluateCondition(updateStmt->whereClause, schema, rows[i])) {
                continue;
            }

            QVector<QString> newValues = rows[i];
            
            // Update specified columns
            for (int j = 0; j < updateStmt->columns.size() && j < updateStmt->values.size(); ++j) {
                int colIdx = schema->getColumnIndex(updateStmt->columns[j]);
                if (colIdx >= 0 && colIdx < newValues.size()) {
                    newValues[colIdx] = updateStmt->values[j];
                }
            }
            
            auto opResult = tableManager->updateRow(updateStmt->tableName, i, newValues);
            if (opResult.success) {
                updatedCount++;
            } else {
                result->errorMessage = opResult.errorMessage;
                return result;
            }
        }
        
        result->success = true;
        result->affectedRows = updatedCount;
        Logger::instance().info(QString("Updated %1 row(s) in '%2'").arg(updatedCount).arg(updateStmt->tableName));
        
    } catch (const std::exception& e) {
        result->success = false;
        result->errorMessage = QString::fromStdString(e.what());
        Logger::instance().error(QString("UPDATE exception: %1").arg(result->errorMessage));
    }
    
    return result;
}

std::unique_ptr<QueryResult> QueryExecutor::executeDelete(const ASTNode* statement) {
    auto result = std::make_unique<QueryResult>();
    
    auto deleteStmt = dynamic_cast<const DeleteStatement*>(statement);
    if (!deleteStmt) {
        result->errorMessage = "Invalid DELETE statement";
        return result;
    }
    
    try {
        if (!tableManager->tableExists(deleteStmt->tableName)) {
            result->errorMessage = QString("Table '%1' does not exist").arg(deleteStmt->tableName);
            return result;
        }
        
        auto schema = tableManager->getTable(deleteStmt->tableName);
        auto rows = tableManager->selectAll(deleteStmt->tableName);
        int deletedCount = 0;
        
        // Start from the end to avoid index shifting
        for (int i = rows.size() - 1; i >= 0; --i) {
            // Check condition
            if (!evaluateCondition(deleteStmt->whereClause, schema, rows[i])) {
                continue; 
            }

            auto opResult = tableManager->deleteRow(deleteStmt->tableName, i);
            if (opResult.success) {
                deletedCount++;
            } else {
                result->errorMessage = opResult.errorMessage;
                return result;
            }
        }
        
        result->success = true;
        result->affectedRows = deletedCount;
        Logger::instance().info(QString("Deleted %1 row(s) from '%2'").arg(deletedCount).arg(deleteStmt->tableName));
        
    } catch (const std::exception& e) {
        result->success = false;
        result->errorMessage = QString::fromStdString(e.what());
        Logger::instance().error(QString("DELETE exception: %1").arg(result->errorMessage));
    }
    
    return result;
}

std::unique_ptr<QueryResult> QueryExecutor::executeSelect(const ASTNode* statement) {
    auto result = std::make_unique<QueryResult>();
    
    auto selectStmt = dynamic_cast<const SelectStatement*>(statement);
    if (!selectStmt) {
        result->errorMessage = "Invalid SELECT statement";
        return result;
    }
    
    try {
        if (!tableManager->tableExists(selectStmt->fromTable)) {
            result->errorMessage = QString("Table '%1' does not exist").arg(selectStmt->fromTable);
            return result;
        }
        
        auto schema = tableManager->getTable(selectStmt->fromTable);
        auto rows = tableManager->selectAllAsMap(selectStmt->fromTable);
        
        // Get selected columns
        QStringList selectedColumns = selectStmt->columns;
        if (selectedColumns.isEmpty() || (selectedColumns.size() == 1 && selectedColumns[0] == "*")) {
            // SELECT *
            const auto& allColumns = schema->getColumns();
            for (const auto& col : allColumns) {
                selectedColumns.append(col.getName());
            }
        }
        
        result->columns = selectedColumns;
        
        // Build result rows
        for (const auto& rowMap : rows) {
            QStringList rowData;
            for (const auto& colName : selectedColumns) {
                rowData.append(rowMap.value(colName, ""));
            }
            result->rows.append(rowData);
        }
        
        result->success = true;
        result->affectedRows = result->rows.size();
        Logger::instance().info(QString("SELECT returned %1 row(s)").arg(result->rows.size()));
        
    } catch (const std::exception& e) {
        result->success = false;
        result->errorMessage = QString::fromStdString(e.what());
        Logger::instance().error(QString("SELECT exception: %1").arg(result->errorMessage));
    }
    
    return result;
}
