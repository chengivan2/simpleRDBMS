#pragma once

#include <QString>
#include <QVector>
#include <QJsonObject>
#include <memory>

class TableSchema;

/**
 * @brief Handles persistence of tables to disk
 */
class StorageEngine {
public:
    explicit StorageEngine(const QString& dataPath = "./data");
    virtual ~StorageEngine() = default;
    
    QString getDataPath() const { return dataPath; }
    
    // Initialize data directory
    bool initializeDataPath();
    
    // Schema persistence
    bool saveTableSchema(const std::shared_ptr<TableSchema>& schema);
    std::shared_ptr<TableSchema> loadTableSchema(const QString& tableName);
    
    // Data persistence
    bool saveTableData(const QString& tableName, const QVector<QStringList>& rows);
    QVector<QStringList> loadTableData(const QString& tableName);
    
    // File management
    bool tableFileExists(const QString& tableName) const;
    bool schemaFileExists(const QString& tableName) const;
    bool deleteTableFile(const QString& tableName);
    bool deleteSchemaFile(const QString& tableName);
    QVector<QString> listAllTables() const;
    
private:
    QString dataPath;
    QString getTableDataPath(const QString& tableName) const;
    QString getTableSchemaPath(const QString& tableName) const;
    
    // JSON conversion helpers
    QJsonObject rowToJson(const QStringList& row, const std::shared_ptr<TableSchema>& schema);
    QStringList jsonToRow(const QJsonObject& obj, const std::shared_ptr<TableSchema>& schema);};