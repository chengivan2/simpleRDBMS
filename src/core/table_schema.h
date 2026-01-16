#pragma once

#include "column.h"
#include "constraint.h"
#include <QString>
#include <QVector>
#include <QMap>
#include <QDateTime>

/**
 * @brief Metadata container for table
 */
struct TableMetadata {
    QString description;
    QDateTime createdAt;
    QDateTime lastModifiedAt;
    QString createdBy;
    int rowCount = 0;
    bool isTemp = false;
    
    TableMetadata() : createdAt(QDateTime::currentDateTime()), 
                     lastModifiedAt(QDateTime::currentDateTime()) {}
};

/**
 * @brief Represents the schema of a table
 */
class TableSchema {
public:
    explicit TableSchema(const QString& tableName);
    
    // Column management
    void addColumn(const Column& column);
    const Column* getColumn(const QString& columnName) const;
    const Column* getColumn(int index) const;
    int getColumnIndex(const QString& columnName) const;
    int getColumnCount() const { return columns.size(); }
    const QVector<Column>& getColumns() const { return columns; }
    
    // Table identification
    QString getTableName() const { return tableName; }
    
    // Constraint management
    void addPrimaryKey(const QStringList& columnNames);
    void addUnique(const QString& constraintName, const QStringList& columnNames);
    void addForeignKey(const QString& constraintName, const QStringList& columnNames,
                       const QString& refTable, const QStringList& refColumns);
    void addCheck(const QString& constraintName, const QString& condition);
    
    // Constraint retrieval
    bool hasPrimaryKey() const { return !primaryKeyColumns.isEmpty(); }
    QStringList getPrimaryKeyColumns() const { return primaryKeyColumns; }
    QMap<QString, QStringList> getUniqueConstraints() const { return uniqueConstraints; }
    QMap<QString, ForeignKeyConstraint*> getForeignKeyConstraints() const { return foreignKeys; }
    QMap<QString, CheckConstraint*> getCheckConstraints() const { return checkConstraints; }
    
    // Constraint validation
    bool validateRow(const QVector<QString>& values) const;
    bool validateColumn(const QString& columnName, const QString& value) const;
    QString getValidationError() const { return lastValidationError; }
    
    // Metadata
    void setMetadata(const TableMetadata& meta) { metadata = meta; }
    TableMetadata getMetadata() const { return metadata; }
    void setDescription(const QString& desc) { metadata.description = desc; }
    QString getDescription() const { return metadata.description; }
    
    // Row count tracking
    void setRowCount(int count) { metadata.rowCount = count; }
    int getRowCount() const { return metadata.rowCount; }
    
    // Serialization
    QString toJson() const;
    static TableSchema* fromJson(const QString& json);
    
private:
    QString tableName;
    QVector<Column> columns;
    TableMetadata metadata;
    
    // Constraints
    QStringList primaryKeyColumns;
    QMap<QString, QStringList> uniqueConstraints;  // constraintName -> columnNames
    QMap<QString, ForeignKeyConstraint*> foreignKeys;  // constraintName -> constraint
    QMap<QString, CheckConstraint*> checkConstraints;  // constraintName -> constraint
    
    mutable QString lastValidationError;
    
    // Helper methods
    bool validatePrimaryKey(const QVector<QString>& values) const;
    bool validateUnique(const QVector<QString>& values) const;
    bool validateForeignKeys(const QVector<QString>& values) const;
    bool validateChecks(const QVector<QString>& values) const;
};
