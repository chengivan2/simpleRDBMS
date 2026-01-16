#include "sql_editor_page.h"
#include "../core/query_executor.h"
#include "../core/table_manager.h"
#include "../core/data_type.h"
#include "../parser/parser.h"
#include "../parser/lexer.h"
#include "../utils/logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QHeaderView>
#include <QShortcut>
#include <QKeySequence>

SQLEditorPage::SQLEditorPage(std::shared_ptr<TableManager> tableManager, QWidget* parent)
    : QWidget(parent), 
      queryExecutor(new QueryExecutor()),
      tableManager(tableManager) {
    setupUI();
    
    // Set table manager for query executor
    if (queryExecutor && tableManager) {
        queryExecutor->setTableManager(tableManager);
    }
    
    Logger::instance().info("SQL Editor initialized");
}

void SQLEditorPage::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Title
    QLabel* editorLabel = new QLabel("SQL Query Editor");
    editorLabel->setStyleSheet("font-weight: bold; font-size: 12px;");
    mainLayout->addWidget(editorLabel);
    
    // SQL Editor
    sqlEditor = new QPlainTextEdit();
    sqlEditor->setPlaceholderText("Write your SQL query here...\n\nExample:\nCREATE TABLE users (id INT PRIMARY KEY, name VARCHAR(100));\nINSERT INTO users VALUES (1, 'John');\nSELECT * FROM users;");
    sqlEditor->setMaximumHeight(150);
    mainLayout->addWidget(sqlEditor);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    executeButton = new QPushButton("Execute (Ctrl+Enter)");
    clearButton = new QPushButton("Clear Results");
    buttonLayout->addWidget(executeButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    // Splitter for results only (logs are now in separate tab)
    QSplitter* splitter = new QSplitter(Qt::Vertical);
    
    // Results section
    QWidget* resultsWidget = new QWidget();
    QVBoxLayout* resultsLayout = new QVBoxLayout(resultsWidget);
    QLabel* resultsLabel = new QLabel("Query Results");
    resultsLabel->setStyleSheet("font-weight: bold; font-size: 11px;");
    resultsLayout->addWidget(resultsLabel);
    resultsLabel->setStyleSheet("font-weight: bold; font-size: 11px;");
    resultsLayout->addWidget(resultsLabel);

    // Status Label
    statusLabel = new QLabel("");
    statusLabel->setStyleSheet("color: #e0e0e0; font-size: 10pt; margin-bottom: 5px;");
    statusLabel->setWordWrap(true);
    resultsLayout->addWidget(statusLabel);

    resultsTable = new QTableWidget();
    resultsLayout->addWidget(resultsTable);
    resultsWidget->setLayout(resultsLayout);
    splitter->addWidget(resultsWidget);
    
    splitter->setCollapsible(0, false);
    mainLayout->addWidget(splitter, 1);
    
    // Connect signals
    connect(executeButton, &QPushButton::clicked, this, &SQLEditorPage::executeQuery);
    connect(clearButton, &QPushButton::clicked, this, &SQLEditorPage::clearResults);
    
    // Allow Ctrl+Enter to execute
    // Shortcuts: Ctrl+Enter and Ctrl+Return
    QShortcut* shortcutEnter = new QShortcut(QKeySequence("Ctrl+Enter"), this);
    connect(shortcutEnter, &QShortcut::activated, this, &SQLEditorPage::executeQuery);

    QShortcut* shortcutReturn = new QShortcut(QKeySequence("Ctrl+Return"), this);
    connect(shortcutReturn, &QShortcut::activated, this, &SQLEditorPage::executeQuery);
}

void SQLEditorPage::executeQuery() {
    QString queryText = sqlEditor->toPlainText().trimmed();
    
    if (queryText.isEmpty()) {
        Logger::instance().warning("No query to execute");
        return;
    }
    
    Logger::instance().info(QString("Executing query: %1").arg(queryText));
    resultsTable->clear();
    resultsTable->setColumnCount(0);
    resultsTable->setRowCount(0);
    
    try {
        // Tokenize
        Lexer lexer(queryText);
        QVector<Token> tokens = lexer.tokenize();
        
        if (tokens.isEmpty()) {
            Logger::instance().error("Failed to tokenize query");
            return;
        }
        
        // Log first 10 tokens for debugging
        Logger::instance().info(QString("Tokenized %1 tokens").arg(tokens.size()));
        for (int i = 0; i < qMin(10, tokens.size()); i++) {
            Logger::instance().debug(QString("Token %1: %2 = '%3'").arg(i).arg(QString::number(tokens[i].type)).arg(tokens[i].value));
        }
        
        // Parse
        Parser parser(tokens);
        auto statement = parser.parse();
        
        if (!statement) {
            Logger::instance().error("Failed to parse query");
            statusLabel->setText("❌ Error: Failed to parse query. Check syntax.");
            statusLabel->setStyleSheet("color: #ff5555; font-size: 10pt; font-weight: bold;");
            return;
        }
        
        // Execute
        auto result = queryExecutor->execute(statement);
        
        if (result->success) {
            Logger::instance().info(QString("Query successful - %1 row(s) affected").arg(result->affectedRows));
            displayResults(result->rows, result->columns);
            
            // Check if it was a CREATE TABLE statement
            if (queryText.toUpper().startsWith("CREATE TABLE")) {
                emit tableCreated();
            }

            // Success Message
            QString msg = QString("✅ Success: %1 row(s) affected").arg(result->affectedRows);
            if (result->rows.size() > 0) {
                msg += QString(", %1 row(s) returned").arg(result->rows.size());
            }
            statusLabel->setText(msg);
            statusLabel->setStyleSheet("color: #55ff55; font-size: 10pt; font-weight: bold;");

        } else {
            Logger::instance().error(QString("Query failed: %1").arg(result->errorMessage));
            statusLabel->setText(QString("❌ Error: %1").arg(result->errorMessage));
            statusLabel->setStyleSheet("color: #ff5555; font-size: 10pt; font-weight: bold;");
        }
    } catch (const std::exception& e) {
        Logger::instance().error(QString("Exception: %1").arg(QString::fromStdString(e.what())));
        statusLabel->setText(QString("❌ Exception: %1").arg(QString::fromStdString(e.what())));
        statusLabel->setStyleSheet("color: #ff5555; font-size: 10pt; font-weight: bold;");
    }
}

void SQLEditorPage::clearResults() {
    resultsTable->clear();
    resultsTable->setColumnCount(0);
    resultsTable->setRowCount(0);
    statusLabel->setText("");
    Logger::instance().info("Results cleared");
}

void SQLEditorPage::displayResults(const QVector<QStringList>& rows, const QStringList& columns) {
    resultsTable->setColumnCount(columns.size());
    resultsTable->setHorizontalHeaderLabels(columns);
    resultsTable->setRowCount(rows.size());
    
    for (int row = 0; row < rows.size(); ++row) {
        for (int col = 0; col < columns.size() && col < rows[row].size(); ++col) {
            auto item = new QTableWidgetItem(rows[row][col]);
            resultsTable->setItem(row, col, item);
        }
    }
    
    resultsTable->resizeColumnsToContents();
}
