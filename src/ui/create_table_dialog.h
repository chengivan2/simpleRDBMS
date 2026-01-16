#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QTableWidget>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QPushButton>
#include "../core/table_schema.h"

class CreateTableDialog : public QDialog {
    Q_OBJECT

public:
    explicit CreateTableDialog(QWidget* parent = nullptr);
    
    // Returns the created schema if accepted, otherwise nullptr
    std::shared_ptr<TableSchema> getTableSchema() const;

private slots:
    void addColumn();
    void removeColumn();
    void validateInput();

private:
    void setupUI();
    QComboBox* createTypeComboBox() const;

    QLineEdit* tableNameInput;
    QTableWidget* columnsTable;
    QPushButton* addColumnBtn;
    QPushButton* removeColumnBtn;
    QDialogButtonBox* buttonBox;
};
