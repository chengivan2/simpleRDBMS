#pragma once

#include <QString>
#include <QVector>
#include <memory>

/**
 * @brief AST Node definitions for all SQL statement types
 */

class ASTNode {
public:
    virtual ~ASTNode() = default;
};

// Column Definition for CREATE TABLE
class ColumnDefinition {
public:
    QString name;
    QString dataType;
    bool nullable = true;
    bool primaryKey = false;
    bool unique = false;
    QString defaultValue;
    
    ColumnDefinition(const QString& n = "", const QString& t = "")
        : name(n), dataType(t) {}
};

// SELECT Statement
class SelectStatement : public ASTNode {
public:
    QStringList columns;              // Column names or "*"
    QString fromTable;                // Table name
    QString whereClause;              // WHERE condition
    QString orderBy;                  // ORDER BY clause
    int limit = -1;                   // LIMIT value (-1 = no limit)
    QString joinClause;               // JOIN clause (future)
};

// INSERT Statement
class InsertStatement : public ASTNode {
public:
    QString tableName;                // Target table
    QStringList columns;              // Column names (optional)
    QVector<QStringList> values;      // Multiple rows of values
};

// UPDATE Statement
class UpdateStatement : public ASTNode {
public:
    QString tableName;                // Target table
    QStringList columns;              // Column names being updated
    QStringList values;               // New values for columns
    QString whereClause;              // WHERE condition
};

// DELETE Statement
class DeleteStatement : public ASTNode {
public:
    QString tableName;                // Target table
    QString whereClause;              // WHERE condition (recommended)
};

// CREATE TABLE Statement
class CreateTableStatement : public ASTNode {
public:
    QString tableName;                // New table name
    QVector<ColumnDefinition> columns; // Column definitions
    QStringList primaryKeys;          // Primary key columns
    QStringList uniqueColumns;        // Unique columns
    QVector<QPair<QString, QString>> foreignKeys; // (column, refTable.refColumn)
};

// ALTER TABLE Statement
class AlterTableStatement : public ASTNode {
public:
    QString tableName;                // Target table
    QString alterAction;              // ADD, DROP, MODIFY
    QString columnName;               // Column to add/drop/modify
    QString columnDefinition;         // New definition (for ADD/MODIFY)
};

// DROP TABLE Statement
class DropTableStatement : public ASTNode {
public:
    QString tableName;                // Table to drop
    bool ifExists = false;            // IF EXISTS flag
};

// CREATE INDEX Statement
class CreateIndexStatement : public ASTNode {
public:
    QString indexName;                // Index name
    QString tableName;                // Target table
    QStringList columns;              // Indexed columns
    bool unique = false;              // UNIQUE index
};

// Transaction Statements
class BeginStatement : public ASTNode {};
class CommitStatement : public ASTNode {};
class RollbackStatement : public ASTNode {};
