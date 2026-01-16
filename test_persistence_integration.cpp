#include <iostream>
#include <QString>
#include <QVector>
#include <memory>
#include "src/core/table_manager.h"
#include "src/core/table_schema.h"
#include "src/core/column.h"
#include "src/core/data_type.h"
#include "src/utils/logger.h"

int main() {
    Logger::instance().info("=== Data Persistence Integration Test ===");
    
    // Session 1: Create table and insert data
    {
        Logger::instance().info("\n--- Session 1: Create and Insert ---");
        TableManager manager("./persistence_test_data");
        
        // Create a new table
        auto schema = std::make_shared<TableSchema>("products");
        Column idCol("id", DataType::INT);
        schema->addColumn(idCol);
        
        Column nameCol("product_name", DataType::VARCHAR);
        nameCol.setMaxLength(100);
        schema->addColumn(nameCol);
        
        Column priceCol("price", DataType::DECIMAL);
        schema->addColumn(priceCol);
        
        // Add primary key constraint
        schema->addPrimaryKey(QStringList() << "id");
        
        manager.addTable(schema);
        Logger::instance().info("Table 'products' created");
        
        // Insert data
        QVector<QString> row1;
        row1 << "1" << "Laptop" << "999.99";
        manager.insertRow("products", row1);
        
        QVector<QString> row2;
        row2 << "2" << "Mouse" << "29.99";
        manager.insertRow("products", row2);
        
        QVector<QString> row3;
        row3 << "3" << "Keyboard" << "79.99";
        manager.insertRow("products", row3);
        
        Logger::instance().info("3 rows inserted into 'products'");
        
        // Save all tables and their data
        manager.saveAllTables();
        Logger::instance().info("All tables saved");
    }
    
    // Session 2: Load table and verify data persistence
    {
        Logger::instance().info("\n--- Session 2: Verify Persistence ---");
        TableManager manager("./persistence_test_data");
        
        // Load existing tables
        manager.loadAllTables();
        Logger::instance().info("Existing tables loaded");
        
        // Get the schema
        auto schema = manager.getTable("products");
        if (!schema) {
            Logger::instance().error("Failed to load schema for 'products'");
            return 1;
        }
        Logger::instance().info(QString("Schema loaded: table=%1, columns=%2")
            .arg(schema->getTableName())
            .arg(schema->getColumnCount()));
        
        // Verify columns
        bool schemaCorrect = true;
        if (schema->getColumnCount() != 3) {
            Logger::instance().error(QString("Expected 3 columns, got %1").arg(schema->getColumnCount()));
            schemaCorrect = false;
        } else {
            Logger::instance().info("Column count verified: 3");
            const Column* idCol = schema->getColumn(0);
            const Column* nameCol = schema->getColumn(1);
            const Column* priceCol = schema->getColumn(2);
            
            if (idCol && idCol->getName() == "id" && idCol->isPrimaryKey()) {
                Logger::instance().info("Column 0 (id) verified");
            } else {
                Logger::instance().error("Column 0 verification failed");
                schemaCorrect = false;
            }
            
            if (nameCol && nameCol->getName() == "product_name") {
                Logger::instance().info("Column 1 (product_name) verified");
            } else {
                Logger::instance().error("Column 1 verification failed");
                schemaCorrect = false;
            }
            
            if (priceCol && priceCol->getName() == "price") {
                Logger::instance().info("Column 2 (price) verified");
            } else {
                Logger::instance().error("Column 2 verification failed");
                schemaCorrect = false;
            }
        }
        
        if (!schemaCorrect) {
            return 1;
        }
        
        // Query all rows
        auto result = manager.selectAll("products");
        if (result.size() != 3) {
            Logger::instance().error(QString("Expected 3 rows, got %1").arg(result.size()));
            return 1;
        }
        Logger::instance().info(QString("Row count verified: %1").arg(result.size()));
        
        // Verify row contents
        QVector<QVector<QString>> expectedRows;
        expectedRows.append(QVector<QString>() << "1" << "Laptop" << "999.99");
        expectedRows.append(QVector<QString>() << "2" << "Mouse" << "29.99");
        expectedRows.append(QVector<QString>() << "3" << "Keyboard" << "79.99");
        
        bool dataCorrect = true;
        for (int i = 0; i < result.size(); ++i) {
            if (result[i] == expectedRows[i]) {
                QString rowStr;
                for (int j = 0; j < result[i].size(); ++j) {
                    if (j > 0) rowStr += ", ";
                    rowStr += result[i][j];
                }
                Logger::instance().info(QString("Row %1 verified: %2").arg(i).arg(rowStr));
            } else {
                Logger::instance().error(QString("Row %1 mismatch").arg(i));
                QString expectedStr, gotStr;
                for (int j = 0; j < expectedRows[i].size(); ++j) {
                    if (j > 0) expectedStr += ", ";
                    expectedStr += expectedRows[i][j];
                }
                for (int j = 0; j < result[i].size(); ++j) {
                    if (j > 0) gotStr += ", ";
                    gotStr += result[i][j];
                }
                Logger::instance().error(QString("  Expected: %1").arg(expectedStr));
                Logger::instance().error(QString("  Got: %1").arg(gotStr));
                dataCorrect = false;
            }
        }
        
        if (!dataCorrect) {
            return 1;
        }
    }
    
    Logger::instance().info("\n=== All Integration Tests PASSED ===");
    return 0;
}

