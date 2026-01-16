#include "table_manager_page.h"
#include "../core/table_manager.h"
#include "../core/column.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include "create_table_dialog.h"

TableManagerPage::TableManagerPage(std::shared_ptr<TableManager> tableManager, QWidget* parent)
    : QWidget(parent), tableManager(tableManager) {
    setupUI();
    refreshTableList();
}

void TableManagerPage::setupUI() {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    
    // Left side - Table list
    QVBoxLayout* leftLayout = new QVBoxLayout();
    tableList = new QListWidget();
    leftLayout->addWidget(tableList, 1);
    
    QPushButton* createButton = new QPushButton("Create Table");
    QPushButton* deleteButton = new QPushButton("Delete Table");
    leftLayout->addWidget(createButton);
    leftLayout->addWidget(deleteButton);
    
    // Right side - Table data display
    tableDataDisplay = new QTableWidget();
    
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addWidget(tableDataDisplay, 2);
    
    // Connect signals
    connect(tableList, &QListWidget::itemClicked, this, &TableManagerPage::onTableSelected);
    connect(createButton, &QPushButton::clicked, this, &TableManagerPage::createNewTable);
    connect(deleteButton, &QPushButton::clicked, this, &TableManagerPage::deleteTable);
}

void TableManagerPage::onTableSelected() {
    if (!tableManager) return;
    
    QListWidgetItem* currentItem = tableList->currentItem();
    if (!currentItem) return;
    
    QString tableName = currentItem->text();
    
    // Get the table schema
    auto schema = tableManager->getTable(tableName);
    if (!schema) return;
    
    // Get the table data
    auto rows = tableManager->selectAll(tableName);
    
    // Clear the table display
    tableDataDisplay->clear();
    tableDataDisplay->setColumnCount(0);
    tableDataDisplay->setRowCount(0);
    
    // Set up columns from schema
    int columnCount = schema->getColumnCount();
    tableDataDisplay->setColumnCount(columnCount);
    
    QStringList columnHeaders;
    for (int i = 0; i < columnCount; ++i) {
        const Column* col = schema->getColumn(i);
        if (col) {
            columnHeaders << col->getName();
        }
    }
    tableDataDisplay->setHorizontalHeaderLabels(columnHeaders);
    
    // Populate rows
    tableDataDisplay->setRowCount(rows.size());
    for (int rowIdx = 0; rowIdx < rows.size(); ++rowIdx) {
        const auto& row = rows[rowIdx];
        for (int colIdx = 0; colIdx < row.size() && colIdx < columnCount; ++colIdx) {
            QTableWidgetItem* item = new QTableWidgetItem(row[colIdx]);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);  // Make read-only
            tableDataDisplay->setItem(rowIdx, colIdx, item);
        }
    }
    
    // Resize columns to fit content
    tableDataDisplay->resizeColumnsToContents();
}

void TableManagerPage::createNewTable() {
    CreateTableDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        auto schema = dialog.getTableSchema();
        if (schema) {
            tableManager->addTable(schema);
            refreshTableList();
            QMessageBox::information(this, "Success", QString("Table '%1' created successfully.").arg(schema->getTableName()));
        } else {
             QMessageBox::critical(this, "Error", "Failed to create table schema.");
        }
    }
}

void TableManagerPage::deleteTable() {
    QListWidgetItem* currentItem = tableList->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "Selection Error", "Please select a table to delete.");
        return;
    }

    QString tableName = currentItem->text();
    auto reply = QMessageBox::question(this, "Confirm Deletion", 
                                       QString("Are you sure you want to delete table '%1'?\nThis action cannot be undone.").arg(tableName),
                                       QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        tableManager->removeTable(tableName);
        refreshTableList();
        
        // Clear display if the deleted table was showing
        tableDataDisplay->clear();
        tableDataDisplay->setColumnCount(0);
        tableDataDisplay->setRowCount(0);
        
        QMessageBox::information(this, "Success", QString("Table '%1' deleted.").arg(tableName));
    }
}

void TableManagerPage::refreshTableList() {
    if (!tableManager) return;
    
    tableList->clear();
    
    // Get all tables from table manager
    const auto& tables = tableManager->getTables();
    
    for (auto it = tables.begin(); it != tables.end(); ++it) {
        tableList->addItem(it.key());
    }
}
