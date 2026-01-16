#pragma once

#include <QWidget>
#include <QListWidget>
#include <QTableWidget>
#include <memory>

class TableManager;

/**
 * @brief Table Manager Page - Browse and manage tables
 */
class TableManagerPage : public QWidget {
    Q_OBJECT
    
public:
    explicit TableManagerPage(std::shared_ptr<TableManager> tableManager, QWidget* parent = nullptr);
    void refreshTableList();
    
private slots:
    void onTableSelected();
    void createNewTable();
    void deleteTable();
    
private:
    void setupUI();
    
    std::shared_ptr<TableManager> tableManager;
    QListWidget* tableList;
    QTableWidget* tableDataDisplay;
};
