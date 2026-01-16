#pragma once

#include <QWidget>
#include <QLabel>
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QTextEdit>
#include <QPushButton>
#include <memory>

class QueryExecutor;
class TableManager;

/**
 * @brief SQL Editor Page - Write and execute SQL queries with integrated logging
 */
class SQLEditorPage : public QWidget {
    Q_OBJECT
    
public:
    explicit SQLEditorPage(std::shared_ptr<TableManager> tableManager, QWidget* parent = nullptr);
    
signals:
    void tableCreated();
    
private slots:
    void executeQuery();
    void clearResults();
    
private:
    void setupUI();
    void displayResults(const QVector<QStringList>& rows, const QStringList& columns);
    
    QPlainTextEdit* sqlEditor;
    QTableWidget* resultsTable;
    QPushButton* executeButton;
    QPushButton* clearButton;
    QLabel* statusLabel;
    
    QueryExecutor* queryExecutor;
    std::shared_ptr<TableManager> tableManager;
};
