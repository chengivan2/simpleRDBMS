#include "table_schema.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

TableSchema::TableSchema(const QString& tableName) 
    : tableName(tableName) {
}

void TableSchema::addColumn(const Column& column) {
    columns.append(column);
}

const Column* TableSchema::getColumn(const QString& columnName) const {
    for (const auto& col : columns) {
        if (col.getName().toLower() == columnName.toLower()) {
            return &col;
        }
    }
    return nullptr;
}

const Column* TableSchema::getColumn(int index) const {
    if (index >= 0 && index < columns.size()) {
        return &columns[index];
    }
    return nullptr;
}

int TableSchema::getColumnIndex(const QString& columnName) const {
    for (int i = 0; i < columns.size(); ++i) {
        if (columns[i].getName().toLower() == columnName.toLower()) {
            return i;
        }
    }
    return -1;
}

void TableSchema::addPrimaryKey(const QStringList& columnNames) {
    primaryKeyColumns = columnNames;
    
    // Mark columns as primary key
    for (const auto& colName : columnNames) {
        int idx = getColumnIndex(colName);
        if (idx >= 0) {
            columns[idx].setPrimaryKey(true);
            columns[idx].setNullable(false);
        }
    }
}

void TableSchema::addUnique(const QString& constraintName, const QStringList& columnNames) {
    uniqueConstraints[constraintName] = columnNames;
    
    // Mark columns as unique
    for (const auto& colName : columnNames) {
        int idx = getColumnIndex(colName);
        if (idx >= 0) {
            columns[idx].setUnique(true);
        }
    }
}

void TableSchema::addForeignKey(const QString& constraintName, const QStringList& columnNames,
                                const QString& refTable, const QStringList& refColumns) {
    if (columnNames.size() != refColumns.size()) {
        lastValidationError = "Foreign key column count mismatch";
        return;
    }
    
    // Create foreign key constraint
    auto* fk = new ForeignKeyConstraint(refTable, refColumns.first());
    foreignKeys[constraintName] = fk;
    
    // Set foreign key info on columns
    for (int i = 0; i < columnNames.size(); ++i) {
        int idx = getColumnIndex(columnNames[i]);
        if (idx >= 0) {
            columns[idx].setForeignKey(refTable, refColumns[i]);
        }
    }
}

void TableSchema::addCheck(const QString& constraintName, const QString& condition) {
    auto* check = new CheckConstraint(condition);
    check->setName(constraintName);
    checkConstraints[constraintName] = check;
}

bool TableSchema::validateRow(const QVector<QString>& values) const {
    // Check column count
    if (values.size() != columns.size()) {
        lastValidationError = QString("Column count mismatch: expected %1, got %2")
            .arg(columns.size())
            .arg(values.size());
        return false;
    }
    
    // Validate each column
    for (int i = 0; i < columns.size(); ++i) {
        if (!columns[i].validateValue(values[i])) {
            lastValidationError = columns[i].getConstraintError();
            return false;
        }
    }
    
    // Validate table-level constraints
    if (!validatePrimaryKey(values)) return false;
    if (!validateUnique(values)) return false;
    if (!validateForeignKeys(values)) return false;
    if (!validateChecks(values)) return false;
    
    return true;
}

bool TableSchema::validateColumn(const QString& columnName, const QString& value) const {
    const Column* col = getColumn(columnName);
    if (!col) {
        lastValidationError = QString("Column '%1' not found").arg(columnName);
        return false;
    }
    
    if (!col->validateValue(value)) {
        lastValidationError = col->getConstraintError();
        return false;
    }
    
    return true;
}

bool TableSchema::validatePrimaryKey(const QVector<QString>& values) const {
    for (const auto& pkCol : primaryKeyColumns) {
        int idx = getColumnIndex(pkCol);
        if (idx >= 0 && (values[idx].isNull() || values[idx].isEmpty())) {
            lastValidationError = QString("PRIMARY KEY column '%1' cannot be NULL").arg(pkCol);
            return false;
        }
    }
    return true;
}

bool TableSchema::validateUnique(const QVector<QString>& values) const {
    // Unique validation is typically done at table manager level (across all rows)
    // Here we just check if value is not empty (NULL allowed)
    return true;
}

bool TableSchema::validateForeignKeys(const QVector<QString>& values) const {
    // Foreign key validation requires access to referenced tables
    // This will be implemented at TableManager level
    return true;
}

bool TableSchema::validateChecks(const QVector<QString>& values) const {
    for (const auto& checkName : checkConstraints.keys()) {
        CheckConstraint* check = checkConstraints[checkName];
        
        // Extract column value for this check
        // Assuming check condition uses column names
        if (!check->validate("")) {  // Simplified - full implementation would parse condition
            lastValidationError = QString("CHECK constraint '%1' failed").arg(checkName);
            return false;
        }
    }
    return true;
}

QString TableSchema::toJson() const {
    QJsonObject tableObj;
    tableObj["name"] = tableName;
    tableObj["description"] = metadata.description;
    tableObj["rowCount"] = metadata.rowCount;
    tableObj["isTemp"] = metadata.isTemp;
    
    // Columns array
    QJsonArray columnsArray;
    for (const auto& col : columns) {
        QJsonObject colObj;
        colObj["name"] = col.getName();
        colObj["type"] = DataTypeManager::typeToString(col.getType());
        colObj["nullable"] = col.isNullable();
        colObj["primaryKey"] = col.isPrimaryKey();
        colObj["unique"] = col.isUnique();
        colObj["autoIncrement"] = col.isAutoIncrement();
        colObj["description"] = col.getDescription();
        columnsArray.append(colObj);
    }
    tableObj["columns"] = columnsArray;
    
    // Constraints
    QJsonObject constraintsObj;
    
    // Primary key
    if (!primaryKeyColumns.isEmpty()) {
        QJsonArray pkArray;
        for (const auto& pkCol : primaryKeyColumns) {
            pkArray.append(pkCol);
        }
        constraintsObj["primaryKey"] = pkArray;
    }
    
    // Unique constraints
    QJsonObject uniqueObj;
    for (const auto& name : uniqueConstraints.keys()) {
        QJsonArray cols;
        for (const auto& col : uniqueConstraints[name]) {
            cols.append(col);
        }
        uniqueObj[name] = cols;
    }
    if (!uniqueObj.isEmpty()) {
        constraintsObj["unique"] = uniqueObj;
    }
    
    // Foreign keys
    QJsonObject fkObj;
    for (const auto& name : foreignKeys.keys()) {
        QJsonObject fk;
        fk["refTable"] = foreignKeys[name]->getReferencedTable();
        fk["refColumn"] = foreignKeys[name]->getReferencedColumn();
        fkObj[name] = fk;
    }
    if (!fkObj.isEmpty()) {
        constraintsObj["foreignKeys"] = fkObj;
    }
    
    // Check constraints
    QJsonObject checkObj;
    for (const auto& name : checkConstraints.keys()) {
        checkObj[name] = checkConstraints[name]->getCondition();
    }
    if (!checkObj.isEmpty()) {
        constraintsObj["checks"] = checkObj;
    }
    
    tableObj["constraints"] = constraintsObj;
    
    QJsonDocument doc(tableObj);
    return QString::fromUtf8(doc.toJson());
}

TableSchema* TableSchema::fromJson(const QString& json) {
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    if (!doc.isObject()) {
        return nullptr;
    }
    
    QJsonObject tableObj = doc.object();
    QString tableName = tableObj["name"].toString();
    auto* schema = new TableSchema(tableName);
    
    // Load columns
    QJsonArray columnsArray = tableObj["columns"].toArray();
    for (const auto& colVal : columnsArray) {
        QJsonObject colObj = colVal.toObject();
        Column col(colObj["name"].toString(), 
                  DataTypeManager::stringToType(colObj["type"].toString()));
        col.setNullable(colObj["nullable"].toBool(true));
        col.setDescription(colObj["description"].toString());
        schema->addColumn(col);
    }
    
    // Load constraints
    QJsonObject constraintsObj = tableObj["constraints"].toObject();
    
    // Primary key
    QJsonArray pkArray = constraintsObj["primaryKey"].toArray();
    QStringList pkCols;
    for (const auto& pk : pkArray) {
        pkCols.append(pk.toString());
    }
    if (!pkCols.isEmpty()) {
        schema->addPrimaryKey(pkCols);
    }
    
    // Load metadata
    schema->setDescription(tableObj["description"].toString());
    schema->setRowCount(tableObj["rowCount"].toInt(0));
    
    return schema;
}
