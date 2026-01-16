#include "main_window.h"
#include "sql_editor_page.h"
#include "table_manager_page.h"
#include "logs_page.h"
#include "../core/table_manager.h"
#include "../core/user_manager.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), tableManager(std::make_shared<TableManager>()) {
    setupUI();
    createMenu();
    
    setWindowTitle("SimpleRDBMS");
    setGeometry(100, 100, 1000, 700);
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    // Create tab widget
    tabWidget = new QTabWidget(this);
    
    // Create pages with shared table manager
    sqlEditorPage = std::make_unique<SQLEditorPage>(tableManager);
    tableManagerPage = std::make_unique<TableManagerPage>(tableManager);
    logsPage = std::make_unique<LogsPage>();
    
    // Connect signal from SQL editor to refresh table manager
    connect(sqlEditorPage.get(), &SQLEditorPage::tableCreated, 
            tableManagerPage.get(), &TableManagerPage::refreshTableList);
    
    // Add tabs in order: SQL Manager, Table Manager, Logs
    tabWidget->addTab(sqlEditorPage.get(), "SQL Manager");
    tabWidget->addTab(tableManagerPage.get(), "Table Manager");
    tabWidget->addTab(logsPage.get(), "Logs");
    
    setCentralWidget(tabWidget);
}

void MainWindow::setUserManager(std::shared_ptr<UserManager> manager) {
    userManager = manager;
    // Update window title with logged-in user
    setWindowTitle(QString("SimpleRDBMS - Logged in as: %1").arg(userManager->getCurrentUser()));
}

void MainWindow::createMenu() {
    // To be implemented in Phase 11
}
