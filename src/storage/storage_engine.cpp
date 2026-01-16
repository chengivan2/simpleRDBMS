#include "storage_engine.h"
#include "../core/table_schema.h"
#include "../core/column.h"
#include "../utils/logger.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

StorageEngine::StorageEngine(const QString& dataPath) : dataPath(dataPath) {
    initializeDataPath();
}

bool StorageEngine::initializeDataPath() {
    QDir dir(dataPath);
    if (!dir.exists()) {
        if (!dir.mkpath(dataPath)) {
            Logger::instance().error(QString("Failed to create data directory: %1").arg(dataPath));
            return false;
        }
        Logger::instance().info(QString("Created data directory: %1").arg(dataPath));
    }
    return true;
}

QString StorageEngine::getTableDataPath(const QString& tableName) const {
    return QDir(dataPath).filePath(tableName + ".json");
}

QString StorageEngine::getTableSchemaPath(const QString& tableName) const {
    return QDir(dataPath).filePath(tableName + "_schema.json");
}

bool StorageEngine::saveTableSchema(const std::shared_ptr<TableSchema>& schema) {
    if (!schema) {
        Logger::instance().error("Cannot save null schema");
        return false;
    }
    
    QString schemaPath = getTableSchemaPath(schema->getTableName());
    
    // Use TableSchema's built-in JSON serialization
    QString jsonStr = schema->toJson();
    
    QFile file(schemaPath);
    if (!file.open(QIODevice::WriteOnly)) {
        Logger::instance().error(QString("Failed to open schema file for writing: %1").arg(schemaPath));
        return false;
    }
    
    file.write(jsonStr.toUtf8());
    file.close();
    Logger::instance().debug(QString("Saved schema for table: %1").arg(schema->getTableName()));
    return true;
}

std::shared_ptr<TableSchema> StorageEngine::loadTableSchema(const QString& tableName) {
    QString schemaPath = getTableSchemaPath(tableName);
    
    QFile file(schemaPath);
    if (!file.open(QIODevice::ReadOnly)) {
        Logger::instance().warning(QString("Schema file not found: %1").arg(schemaPath));
        return nullptr;
    }
    
    QString jsonStr = QString::fromUtf8(file.readAll());
    file.close();
    
    // Use TableSchema's built-in JSON deserialization
    TableSchema* schema = TableSchema::fromJson(jsonStr);
    if (!schema) {
        Logger::instance().error(QString("Failed to deserialize schema for table: %1").arg(tableName));
        return nullptr;
    }
    
    Logger::instance().debug(QString("Loaded schema for table: %1").arg(tableName));
    return std::shared_ptr<TableSchema>(schema);
}

bool StorageEngine::saveTableData(const QString& tableName, const QVector<QStringList>& rows) {
    QString dataPath = getTableDataPath(tableName);
    
    // Load schema for proper data formatting
    auto schema = loadTableSchema(tableName);
    
    QJsonObject dataObj;
    dataObj["tableName"] = tableName;
    dataObj["rowCount"] = static_cast<int>(rows.size());
    
    QJsonArray rowsArray;
    for (const QStringList& row : rows) {
        if (schema) {
            rowsArray.append(rowToJson(row, schema));
        } else {
            // Fallback: store as simple string array
            rowsArray.append(QJsonArray::fromStringList(row));
        }
    }
    dataObj["rows"] = rowsArray;
    
    QJsonDocument doc(dataObj);
    QFile file(dataPath);
    if (!file.open(QIODevice::WriteOnly)) {
        Logger::instance().error(QString("Failed to open data file for writing: %1").arg(dataPath));
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    Logger::instance().debug(QString("Saved data for table: %1 (%2 rows)").arg(tableName).arg(rows.size()));
    return true;
}

QVector<QStringList> StorageEngine::loadTableData(const QString& tableName) {
    QString dataPath = getTableDataPath(tableName);
    
    QFile file(dataPath);
    if (!file.open(QIODevice::ReadOnly)) {
        Logger::instance().warning(QString("Data file not found: %1").arg(dataPath));
        return QVector<QStringList>();
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (!doc.isObject()) {
        Logger::instance().error("Invalid data JSON format");
        return QVector<QStringList>();
    }
    
    QJsonObject dataObj = doc.object();
    QJsonArray rowsArray = dataObj["rows"].toArray();
    
    auto schema = loadTableSchema(tableName);
    QVector<QStringList> rows;
    
    for (const QJsonValue& rowVal : rowsArray) {
        if (schema && rowVal.isObject()) {
            rows.append(jsonToRow(rowVal.toObject(), schema));
        } else if (rowVal.isArray()) {
            // Fallback: convert from string array
            QStringList row;
            QJsonArray rowArray = rowVal.toArray();
            for (const QJsonValue& val : rowArray) {
                row.append(val.toString());
            }
            rows.append(row);
        }
    }
    
    Logger::instance().debug(QString("Loaded data for table: %1 (%2 rows)").arg(tableName).arg(rows.size()));
    return rows;
}

bool StorageEngine::tableFileExists(const QString& tableName) const {
    return QFile::exists(getTableDataPath(tableName));
}

bool StorageEngine::schemaFileExists(const QString& tableName) const {
    return QFile::exists(getTableSchemaPath(tableName));
}

bool StorageEngine::deleteTableFile(const QString& tableName) {
    QString dataPath = getTableDataPath(tableName);
    if (QFile::exists(dataPath)) {
        if (!QFile::remove(dataPath)) {
            Logger::instance().error(QString("Failed to delete data file: %1").arg(dataPath));
            return false;
        }
    }
    return deleteSchemaFile(tableName);
}

bool StorageEngine::deleteSchemaFile(const QString& tableName) {
    QString schemaPath = getTableSchemaPath(tableName);
    if (QFile::exists(schemaPath)) {
        if (!QFile::remove(schemaPath)) {
            Logger::instance().error(QString("Failed to delete schema file: %1").arg(schemaPath));
            return false;
        }
    }
    return true;
}

QVector<QString> StorageEngine::listAllTables() const {
    QDir dir(dataPath);
    QStringList filter;
    filter << "*_schema.json";
    QStringList schemaFiles = dir.entryList(filter);
    
    QVector<QString> tableNames;
    for (const QString& file : schemaFiles) {
        // Remove "_schema.json" suffix
        QString tableName = file;
        tableName.chop(12); // Remove "_schema.json"
        tableNames.append(tableName);
    }
    return tableNames;
}

QJsonObject StorageEngine::rowToJson(const QStringList& row, const std::shared_ptr<TableSchema>& schema) {
    QJsonObject obj;
    for (int i = 0; i < row.size() && i < schema->getColumnCount(); ++i) {
        const Column* col = schema->getColumn(i);
        if (col) {
            obj[col->getName()] = row[i];
        }
    }
    return obj;
}

QStringList StorageEngine::jsonToRow(const QJsonObject& obj, const std::shared_ptr<TableSchema>& schema) {
    QStringList row;
    for (int i = 0; i < schema->getColumnCount(); ++i) {
        const Column* col = schema->getColumn(i);
        if (col) {
            row.append(obj[col->getName()].toString());
        }
    }
    return row;
}
