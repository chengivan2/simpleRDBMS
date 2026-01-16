#include <iostream>
#include <QString>
#include <QVector>
#include <memory>
#include "src/core/table_schema.h"
#include "src/core/column.h"
#include "src/core/data_type.h"
#include "src/storage/storage_engine.h"
#include "src/utils/logger.h"

int main() {
    // Initialize logger
    Logger::instance().info("=== Starting Data Persistence Test ===");
    
    // Create a temporary data directory for testing
    QString dataPath = "./test_data";
    
    // Create storage engine
    StorageEngine storage(dataPath);
    Logger::instance().info("Storage engine initialized");
    
    // Create a test schema
    auto schema = std::make_shared<TableSchema>("users");
    Column idCol("id", DataType::INT);
    idCol.setPrimaryKey(true);
    schema->addColumn(idCol);
    
    Column nameCol("name", DataType::VARCHAR);
    nameCol.setMaxLength(100);
    schema->addColumn(nameCol);
    
    Column ageCol("age", DataType::INT);
    ageCol.setNullable(true);
    schema->addColumn(ageCol);
    
    // Save the schema
    bool schemaSaved = storage.saveTableSchema(schema);
    Logger::instance().info(QString("Schema saved: %1").arg(schemaSaved ? "true" : "false"));
    
    // Create test data
    QVector<QStringList> testRows;
    testRows.append(QStringList() << "1" << "Alice" << "30");
    testRows.append(QStringList() << "2" << "Bob" << "25");
    testRows.append(QStringList() << "3" << "Charlie" << "35");
    
    // Save the data
    bool dataSaved = storage.saveTableData("users", testRows);
    Logger::instance().info(QString("Data saved: %1").arg(dataSaved ? "true" : "false"));
    
    // Now load the data back
    QVector<QStringList> loadedRows = storage.loadTableData("users");
    Logger::instance().info(QString("Data loaded: %1 rows").arg(loadedRows.size()));
    
    // Verify the data
    bool dataCorrect = true;
    if (loadedRows.size() != testRows.size()) {
        Logger::instance().error(QString("Row count mismatch: expected %1, got %2").arg(testRows.size()).arg(loadedRows.size()));
        dataCorrect = false;
    }
    
    for (int i = 0; i < loadedRows.size() && i < testRows.size(); ++i) {
        if (loadedRows[i] != testRows[i]) {
            Logger::instance().error(QString("Row %1 mismatch").arg(i));
            Logger::instance().error(QString("  Expected: %1").arg(testRows[i].join(", ")));
            Logger::instance().error(QString("  Got: %1").arg(loadedRows[i].join(", ")));
            dataCorrect = false;
        } else {
            Logger::instance().info(QString("Row %1 verified: %2").arg(i).arg(loadedRows[i].join(", ")));
        }
    }
    
    if (dataCorrect) {
        Logger::instance().info("=== All tests PASSED ===");
        return 0;
    } else {
        Logger::instance().error("=== Some tests FAILED ===");
        return 1;
    }
}
