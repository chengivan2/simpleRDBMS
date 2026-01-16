#include "create_table_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QHeaderView>
#include <QCheckBox>

CreateTableDialog::CreateTableDialog(QWidget* parent) : QDialog(parent) {
    setupUI();
    setWindowTitle("Create New Table");
    resize(600, 400);
}

void CreateTableDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Table Name Input
    QHBoxLayout* nameLayout = new QHBoxLayout();
    nameLayout->addWidget(new QLabel("Table Name:"));
    tableNameInput = new QLineEdit();
    nameLayout->addWidget(tableNameInput);
    mainLayout->addLayout(nameLayout);

    // Columns Section
    mainLayout->addWidget(new QLabel("Columns:"));
    columnsTable = new QTableWidget(0, 5); // 5 columns: Name, Type, PK, NotNull, Unique
    columnsTable->setHorizontalHeaderLabels({"Name", "Type", "Primary Key", "Not Null", "Unique"});
    columnsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    columnsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    mainLayout->addWidget(columnsTable);

    // Buttons for Columns
    QHBoxLayout* btnLayout = new QHBoxLayout();
    addColumnBtn = new QPushButton("Add Column");
    removeColumnBtn = new QPushButton("Remove Column");
    btnLayout->addWidget(addColumnBtn);
    btnLayout->addWidget(removeColumnBtn);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    // Dialog Buttons
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    // Connect signals
    connect(addColumnBtn, &QPushButton::clicked, this, &CreateTableDialog::addColumn);
    connect(removeColumnBtn, &QPushButton::clicked, this, &CreateTableDialog::removeColumn);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &CreateTableDialog::validateInput);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
    // Add one empty column by default
    addColumn();
}

void CreateTableDialog::addColumn() {
    int row = columnsTable->rowCount();
    columnsTable->insertRow(row);

    // Column Name
    columnsTable->setItem(row, 0, new QTableWidgetItem("new_column"));

    // Column Type
    columnsTable->setCellWidget(row, 1, createTypeComboBox());

    // Constraints (Checkboxes)
    // Primary Key
    QCheckBox* pkCheck = new QCheckBox();
    QWidget* pkWidget = new QWidget();
    QHBoxLayout* pkLayout = new QHBoxLayout(pkWidget);
    pkLayout->addWidget(pkCheck);
    pkLayout->setAlignment(Qt::AlignCenter);
    pkLayout->setContentsMargins(0,0,0,0);
    columnsTable->setCellWidget(row, 2, pkWidget);
    
    // Not Null
    QCheckBox* nnCheck = new QCheckBox();
    QWidget* nnWidget = new QWidget();
    QHBoxLayout* nnLayout = new QHBoxLayout(nnWidget);
    nnLayout->addWidget(nnCheck);
    nnLayout->setAlignment(Qt::AlignCenter);
    nnLayout->setContentsMargins(0,0,0,0);
    columnsTable->setCellWidget(row, 3, nnWidget);
    
    // Unique
    QCheckBox* uCheck = new QCheckBox();
    QWidget* uWidget = new QWidget();
    QHBoxLayout* uLayout = new QHBoxLayout(uWidget);
    uLayout->addWidget(uCheck);
    uLayout->setAlignment(Qt::AlignCenter);
    uLayout->setContentsMargins(0,0,0,0);
    columnsTable->setCellWidget(row, 4, uWidget);
}

void CreateTableDialog::removeColumn() {
    int currentRow = columnsTable->currentRow();
    if (currentRow >= 0) {
        columnsTable->removeRow(currentRow);
    } else if (columnsTable->rowCount() > 0) {
        columnsTable->removeRow(columnsTable->rowCount() - 1);
    }
}

QComboBox* CreateTableDialog::createTypeComboBox() const {
    QComboBox* combo = new QComboBox();
    // Add common SQL types
    combo->addItems({
        "INT", "INTEGER", "SMALLINT", "BIGINT",
        "VARCHAR", "TEXT", "CHAR",
        "FLOAT", "DOUBLE", "DECIMAL",
        "BOOLEAN", "DATE", "DATETIME", "TIMESTAMP"
    });
    return combo;
}

void CreateTableDialog::validateInput() {
    QString tableName = tableNameInput->text().trimmed();
    if (tableName.isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Table name cannot be empty.");
        return;
    }

    if (tableName.contains(" ")) {
        QMessageBox::warning(this, "Validation Error", "Table name cannot contain spaces.");
        return;
    }

    if (columnsTable->rowCount() == 0) {
        QMessageBox::warning(this, "Validation Error", "Table must have at least one column.");
        return;
    }

    // Check for duplicate column names
    QStringList colNames;
    bool hasPrimaryKey = false;

    for (int i = 0; i < columnsTable->rowCount(); ++i) {
        QString colName = columnsTable->item(i, 0)->text().trimmed();
        if (colName.isEmpty()) {
            QMessageBox::warning(this, "Validation Error", QString("Column name at row %1 cannot be empty.").arg(i + 1));
            return;
        }
        if (colNames.contains(colName, Qt::CaseInsensitive)) {
            QMessageBox::warning(this, "Validation Error", QString("Duplicate column name: %1").arg(colName));
            return;
        }
        colNames << colName;

        // Check for primary key
        QWidget* pkWidget = columnsTable->cellWidget(i, 2);
        QCheckBox* pkCheck = pkWidget->findChild<QCheckBox*>();
        if (pkCheck && pkCheck->isChecked()) {
            hasPrimaryKey = true;
        }
    }

    if (!hasPrimaryKey) {
        QMessageBox::warning(this, "Validation Error", "Table must have at least one Primary Key column.");
        return;
    }

    accept();
}

std::shared_ptr<TableSchema> CreateTableDialog::getTableSchema() const {
    auto schema = std::make_shared<TableSchema>(tableNameInput->text().trimmed());

    for (int i = 0; i < columnsTable->rowCount(); ++i) {
        QString name = columnsTable->item(i, 0)->text().trimmed();
        
        QComboBox* typeCombo = qobject_cast<QComboBox*>(columnsTable->cellWidget(i, 1));
        QString typeStr = typeCombo->currentText();
        
        // Convert string to DataType enum (simplified)
        DataType type = DataType::VARCHAR; // Default
        if (typeStr == "INT" || typeStr == "INTEGER") type = DataType::INT;
        else if (typeStr == "SMALLINT") type = DataType::SMALLINT;
        else if (typeStr == "BIGINT") type = DataType::BIGINT;
        else if (typeStr == "TEXT") type = DataType::TEXT;
        else if (typeStr == "CHAR") type = DataType::CHAR;
        else if (typeStr == "FLOAT") type = DataType::FLOAT;
        else if (typeStr == "DOUBLE") type = DataType::DOUBLE;
        else if (typeStr == "DECIMAL") type = DataType::DECIMAL;
        else if (typeStr == "BOOLEAN") type = DataType::BOOL;
        else if (typeStr == "DATE") type = DataType::DATE;
        else if (typeStr == "DATETIME") type = DataType::DATETIME;
        else if (typeStr == "TIMESTAMP") type = DataType::TIMESTAMP;

        Column col(name, type);

        // Get constraints
        QWidget* pkWidget = columnsTable->cellWidget(i, 2);
        QCheckBox* pkCheck = pkWidget->findChild<QCheckBox*>();
        if (pkCheck && pkCheck->isChecked()) col.setPrimaryKey(true);

        QWidget* nnWidget = columnsTable->cellWidget(i, 3);
        QCheckBox* nnCheck = nnWidget->findChild<QCheckBox*>();
        if (nnCheck && nnCheck->isChecked()) col.setNotNull(true);
        
        QWidget* uWidget = columnsTable->cellWidget(i, 4);
        QCheckBox* uCheck = uWidget->findChild<QCheckBox*>();
        if (uCheck && uCheck->isChecked()) col.setUnique(true);

        schema->addColumn(col);
    }

    return schema;
}
