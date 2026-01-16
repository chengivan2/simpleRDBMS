#include "table_manager.h"
#include "../storage/storage_engine.h"
#include "../utils/logger.h"

TableManager::TableManager(const QString& dataPath) 
    : storageEngine(std::make_shared<StorageEngine>(dataPath)) {
    loadAllTables();
}

void TableManager::loadAllTables() {
    if (!storageEngine) return;
    
    QVector<QString> tableNames = storageEngine->listAllTables();
    for (const QString& tableName : tableNames) {
        auto schema = storageEngine->loadTableSchema(tableName);
        if (schema) {
            addTable(schema);
            
            // Load table data
            QVector<QStringList> rows = storageEngine->loadTableData(tableName);
            QVector<QVector<QString>> tableRows;
            for (const QStringList& row : rows) {
                tableRows.append(row.toVector());
            }
            tableData[tableName.toLower()] = tableRows;
            Logger::instance().info(QString("Loaded table: %1 with %2 rows").arg(tableName).arg(rows.size()));
        }
    }
}

void TableManager::saveAllTables() {
    if (!storageEngine) return;
    
    for (auto it = tables.begin(); it != tables.end(); ++it) {
        const QString& tableName = it.key();
        
        // Save schema
        storageEngine->saveTableSchema(it.value());
        
        // Save data
        QVector<QStringList> rows;
        if (tableData.contains(tableName)) {
            for (const QVector<QString>& row : tableData[tableName]) {
                rows.append(row.toList());
            }
        }
        storageEngine->saveTableData(tableName, rows);
    }
}


void TableManager::addTable(const std::shared_ptr<TableSchema>& schema) {
    tables[schema->getTableName().toLower()] = schema;
    // Initialize empty row vector for this table
    tableData[schema->getTableName().toLower()] = QVector<QVector<QString>>();
    
    // Save schema to disk immediately
    if (storageEngine) {
        storageEngine->saveTableSchema(schema);
    }
}

std::shared_ptr<TableSchema> TableManager::getTable(const QString& tableName) const {
    auto it = tables.find(tableName.toLower());
    if (it != tables.end()) {
        return it.value();
    }
    return nullptr;
}

bool TableManager::tableExists(const QString& tableName) const {
    return tables.contains(tableName.toLower());
}

void TableManager::removeTable(const QString& tableName) {
    tables.remove(tableName.toLower());
    tableData.remove(tableName.toLower());
}

// Helper: Map column values from QVector to QMap using schema column order
QMap<QString, QString> TableManager::mapColumnsToValues(
    const QString& tableName, 
    const QVector<QString>& values) const {
    
    auto schema = getTable(tableName);
    if (!schema) {
        return QMap<QString, QString>();
    }
    
    QMap<QString, QString> columnValues;
    const auto& columns = schema->getColumns();
    
    if (values.size() != columns.size()) {
        return QMap<QString, QString>(); // Mismatch
    }
    
    for (int i = 0; i < columns.size(); ++i) {
        columnValues[columns[i].getName()] = values[i];
    }
    
    return columnValues;
}

// Helper: Validate UNIQUE constraints (excluding specified row ID for updates)
bool TableManager::validateUniqueConstraints(
    const QString& tableName,
    const QMap<QString, QString>& columnValues,
    QString& errorMessage,
    int excludeRowId) const {
    
    auto schema = getTable(tableName);
    if (!schema) {
        errorMessage = "Table not found";
        return false;
    }
    
    const auto& uniqueConstraints = schema->getUniqueConstraints();
    const auto& tableRows = tableData.value(tableName.toLower());
    const auto& columns = schema->getColumns();
    
    // For each UNIQUE constraint, check if values already exist
    for (auto it = uniqueConstraints.constBegin(); it != uniqueConstraints.constEnd(); ++it) {
        const QStringList& constraintCols = it.value();
        
        // Build values for this constraint from current row
        QVector<QString> currentValues;
        for (const auto& colName : constraintCols) {
            currentValues.append(columnValues.value(colName, ""));
        }
        
        // Check against existing rows
        for (int rowIdx = 0; rowIdx < tableRows.size(); ++rowIdx) {
            // Skip the row being updated
            if (rowIdx == excludeRowId) continue;
            
            const auto& row = tableRows[rowIdx];
            
            // Build values for this constraint from existing row
            QVector<QString> existingValues;
            for (const auto& colName : constraintCols) {
                // Find column index by name
                int colIdx = schema->getColumnIndex(colName);
                if (colIdx >= 0 && colIdx < row.size()) {
                    existingValues.append(row[colIdx]);
                }
            }
            
            // Compare values
            if (currentValues == existingValues) {
                errorMessage = QString("UNIQUE constraint violation on column(s): %1")
                    .arg(constraintCols.join(", "));
                return false;
            }
        }
    }
    
    return true;
}

// Helper: Validate FOREIGN KEY constraints
bool TableManager::validateForeignKeyConstraints(
    const QString& tableName,
    const QMap<QString, QString>& columnValues,
    QString& errorMessage) const {
    
    auto schema = getTable(tableName);
    if (!schema) {
        errorMessage = "Table not found";
        return false;
    }
    
    const auto& foreignKeyConstraints = schema->getForeignKeyConstraints();
    
    for (auto it = foreignKeyConstraints.constBegin(); it != foreignKeyConstraints.constEnd(); ++it) {
        ForeignKeyConstraint* constraint = it.value();
        if (!constraint) continue;
        
        const QString& refTable = constraint->getReferencedTable();
        const QString& refCol = constraint->getReferencedColumn();
        
        // Find source columns for this FK (constraint name may help identify them)
        // For now, we assume FK columns are named similarly or we need to track them separately
        // This is a limitation of the current design - we'll need to enhance it
        
        auto refSchema = getTable(refTable);
        if (!refSchema) {
            errorMessage = QString("Referenced table '%1' does not exist").arg(refTable);
            return false;
        }
    }
    
    return true;
}

// Insert a row using vector of values (columns in order)
OperationResult TableManager::insertRow(
    const QString& tableName,
    const QVector<QString>& values) {
    
    auto schema = getTable(tableName);
    if (!schema) {
        return OperationResult{false, "Table not found", 0, -1};
    }
    
    // Check column count
    if (values.size() != schema->getColumns().size()) {
        return OperationResult{false, 
            QString("Column count mismatch: expected %1, got %2")
            .arg(schema->getColumns().size()).arg(values.size()), 
            0, -1};
    }
    
    // Validate row at schema level (all constraints)
    if (!schema->validateRow(values)) {
        return OperationResult{false, schema->getValidationError(), 0, -1};
    }
    
    // Map values to column names for additional validation
    QMap<QString, QString> columnValues = mapColumnsToValues(tableName, values);
    
    // Validate UNIQUE constraints
    QString uniqueError;
    if (!validateUniqueConstraints(tableName, columnValues, uniqueError)) {
        return OperationResult{false, uniqueError, 0, -1};
    }
    
    // Validate FOREIGN KEY constraints
    QString fkError;
    if (!validateForeignKeyConstraints(tableName, columnValues, fkError)) {
        return OperationResult{false, fkError, 0, -1};
    }
    
    // All validations passed - insert the row
    auto& tableRows = tableData[tableName.toLower()];
    int newRowId = tableRows.size(); // Row ID is simply the index
    tableRows.append(values);
    
    // Save to disk
    if (storageEngine) {
        QVector<QStringList> rows;
        for (const QVector<QString>& row : tableRows) {
            rows.append(row.toList());
        }
        storageEngine->saveTableData(tableName, rows);
    }
    
    return OperationResult{true, "", 1, newRowId};
}

// Insert a row using map of column names to values
OperationResult TableManager::insertRow(
    const QString& tableName,
    const QMap<QString, QString>& columnValues) {
    
    auto schema = getTable(tableName);
    if (!schema) {
        return OperationResult{false, "Table not found", 0, -1};
    }
    
    // Convert map to vector in column order
    QVector<QString> values;
    const auto& columns = schema->getColumns();
    
    for (const auto& column : columns) {
        QString value = columnValues.value(column.getName(), "");
        values.append(value);
    }
    
    // Use the vector-based insertRow
    return insertRow(tableName, values);
}

// Update a row by row ID
OperationResult TableManager::updateRow(
    const QString& tableName,
    int rowId,
    const QVector<QString>& values) {
    
    auto schema = getTable(tableName);
    if (!schema) {
        return OperationResult{false, "Table not found", 0, -1};
    }
    
    auto& tableRows = tableData[tableName.toLower()];
    
    if (rowId < 0 || rowId >= tableRows.size()) {
        return OperationResult{false, "Row ID out of bounds", 0, -1};
    }
    
    // Check column count
    if (values.size() != schema->getColumns().size()) {
        return OperationResult{false, 
            QString("Column count mismatch: expected %1, got %2")
            .arg(schema->getColumns().size()).arg(values.size()), 
            0, -1};
    }
    
    // Validate row at schema level
    if (!schema->validateRow(values)) {
        return OperationResult{false, schema->getValidationError(), 0, -1};
    }
    
    // Map values to column names for additional validation
    QMap<QString, QString> columnValues = mapColumnsToValues(tableName, values);
    
    // Validate UNIQUE constraints (excluding this row being updated)
    QString uniqueError;
    if (!validateUniqueConstraints(tableName, columnValues, uniqueError, rowId)) {
        return OperationResult{false, uniqueError, 0, -1};
    }
    
    // Validate FOREIGN KEY constraints
    QString fkError;
    if (!validateForeignKeyConstraints(tableName, columnValues, fkError)) {
        return OperationResult{false, fkError, 0, -1};
    }
    
    // All validations passed - update the row
    tableRows[rowId] = values;
    
    // Save to disk
    if (storageEngine) {
        QVector<QStringList> rows;
        for (const QVector<QString>& row : tableRows) {
            rows.append(row.toList());
        }
        storageEngine->saveTableData(tableName, rows);
    }
    
    return OperationResult{true, "", 1, rowId};
}

// Update a row by row ID using map
OperationResult TableManager::updateRow(
    const QString& tableName,
    int rowId,
    const QMap<QString, QString>& columnValues) {
    
    auto schema = getTable(tableName);
    if (!schema) {
        return OperationResult{false, "Table not found", 0, -1};
    }
    
    // Convert map to vector in column order
    QVector<QString> values;
    const auto& columns = schema->getColumns();
    
    for (const auto& column : columns) {
        QString value = columnValues.value(column.getName(), "");
        values.append(value);
    }
    
    // Use the vector-based updateRow
    return updateRow(tableName, rowId, values);
}

// Delete a row by row ID
OperationResult TableManager::deleteRow(
    const QString& tableName,
    int rowId) {
    
    auto schema = getTable(tableName);
    if (!schema) {
        return OperationResult{false, "Table not found", 0, -1};
    }
    
    auto& tableRows = tableData[tableName.toLower()];
    
    if (rowId < 0 || rowId >= tableRows.size()) {
        return OperationResult{false, "Row ID out of bounds", 0, -1};
    }
    
    // Check referential integrity - any foreign keys pointing to this row?
    const auto& deletingRow = tableRows[rowId];
    const auto& pkCols = schema->getPrimaryKeyColumns();
    
    if (!pkCols.isEmpty()) {
        QVector<QString> pkValues;
        
        for (const auto& pkCol : pkCols) {
            int colIdx = schema->getColumnIndex(pkCol);
            if (colIdx >= 0 && colIdx < deletingRow.size()) {
                pkValues.append(deletingRow[colIdx]);
            }
        }
        
        // Check all tables for foreign keys pointing to this table
        for (const auto& [tblName, tblSchema] : tables.asKeyValueRange()) {
            const auto& fkConstraints = tblSchema->getForeignKeyConstraints();
            for (auto it = fkConstraints.constBegin(); it != fkConstraints.constEnd(); ++it) {
                ForeignKeyConstraint* fkConstraint = it.value();
                if (!fkConstraint) continue;
                
                if (fkConstraint->getReferencedTable().toLower() == tableName.toLower()) {
                    // This table has an FK pointing to us - simplified check
                    // Full implementation would need to track which columns participate in FK
                    return OperationResult{false,
                        QString("FOREIGN KEY constraint violation: row referenced by table '%1'")
                        .arg(tblName), 0, -1};
                }
            }
        }
    }
    
    // All checks passed - delete the row
    tableRows.removeAt(rowId);
    
    // Save to disk
    if (storageEngine) {
        QVector<QStringList> rows;
        for (const QVector<QString>& row : tableRows) {
            rows.append(row.toList());
        }
        storageEngine->saveTableData(tableName, rows);
    }
    
    return OperationResult{true, "", 1, rowId};
}

// Validate a row without modifying data
bool TableManager::validateRow(
    const QString& tableName,
    const QVector<QString>& values,
    QString& errorMessage) const {
    
    auto schema = getTable(tableName);
    if (!schema) {
        errorMessage = "Table not found";
        return false;
    }
    
    if (values.size() != schema->getColumns().size()) {
        errorMessage = QString("Column count mismatch: expected %1, got %2")
            .arg(schema->getColumns().size()).arg(values.size());
        return false;
    }
    
    // Validate at schema level
    if (!schema->validateRow(values)) {
        errorMessage = schema->getValidationError();
        return false;
    }
    
    // Map values for additional validation
    QMap<QString, QString> columnValues = mapColumnsToValues(tableName, values);
    
    // Validate UNIQUE constraints
    if (!validateUniqueConstraints(tableName, columnValues, errorMessage)) {
        return false;
    }
    
    // Validate FOREIGN KEY constraints
    if (!validateForeignKeyConstraints(tableName, columnValues, errorMessage)) {
        return false;
    }
    
    return true;
}

// Select all rows from a table
QVector<QVector<QString>> TableManager::selectAll(const QString& tableName) const {
    return tableData.value(tableName.toLower());
}

// Select all rows as maps (column name -> value)
QVector<QMap<QString, QString>> TableManager::selectAllAsMap(const QString& tableName) const {
    QVector<QMap<QString, QString>> result;
    auto schema = getTable(tableName);
    
    if (!schema) {
        return result;
    }
    
    const auto& rows = tableData.value(tableName.toLower());
    const auto& columns = schema->getColumns();
    
    for (const auto& row : rows) {
        QMap<QString, QString> rowMap;
        for (int i = 0; i < columns.size() && i < row.size(); ++i) {
            rowMap[columns[i].getName()] = row[i];
        }
        result.append(rowMap);
    }
    
    return result;
}
