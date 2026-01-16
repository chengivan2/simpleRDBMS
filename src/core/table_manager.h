#pragma once

#include "table_schema.h"
#include "value.h"
#include <QString>
#include <QMap>
#include <QVector>
#include <memory>

class StorageEngine;

/**
 * @brief Result of row operation (INSERT, UPDATE, DELETE)
 */
struct OperationResult {
    bool success;
    QString errorMessage;
    int rowsAffected;
    int rowId;
    
    OperationResult(bool ok = true, const QString& msg = "", int rows = 0, int id = -1)
        : success(ok), errorMessage(msg), rowsAffected(rows), rowId(id) {}
};

/**
 * @brief Manages all tables in the database with constraint enforcement
 */
class TableManager {
public:
    TableManager(const QString& dataPath = "./data");
    virtual ~TableManager() = default;
    
    // Persistence
    void loadAllTables();
    void saveAllTables();
    
    // Table management
    void addTable(const std::shared_ptr<TableSchema>& schema);
    std::shared_ptr<TableSchema> getTable(const QString& tableName) const;
    bool tableExists(const QString& tableName) const;
    void removeTable(const QString& tableName);
    
    const QMap<QString, std::shared_ptr<TableSchema>>& getTables() const {
        return tables;
    }
    
    // Row operations with constraint enforcement
    OperationResult insertRow(const QString& tableName, const QVector<QString>& values);
    OperationResult insertRow(const QString& tableName, const QMap<QString, QString>& columnValues);
    
    OperationResult updateRow(const QString& tableName, int rowId, const QVector<QString>& values);
    OperationResult updateRow(const QString& tableName, int rowId, const QMap<QString, QString>& columnValues);
    
    OperationResult deleteRow(const QString& tableName, int rowId);
    
    // Data retrieval
    QVector<QVector<QString>> selectAll(const QString& tableName) const;
    QVector<QMap<QString, QString>> selectAllAsMap(const QString& tableName) const;
    
    // Constraint validation
    bool validateRow(const QString& tableName, const QVector<QString>& values, QString& errorMessage) const;
    QString getLastError() const { return lastError; }
    
private:
    QMap<QString, std::shared_ptr<TableSchema>> tables;
    QMap<QString, QVector<QVector<QString>>> tableData;  // table name -> rows
    std::shared_ptr<StorageEngine> storageEngine;
    mutable QString lastError;
    
    // Helper methods
    QMap<QString, QString> mapColumnsToValues(const QString& tableName, 
                                              const QVector<QString>& values) const;
    bool validateUniqueConstraints(const QString& tableName, 
                                   const QMap<QString, QString>& columnValues,
                                   QString& errorMessage,
                                   int excludeRowId = -1) const;
    bool validateForeignKeyConstraints(const QString& tableName,
                                       const QMap<QString, QString>& columnValues,
                                       QString& errorMessage) const;
};
