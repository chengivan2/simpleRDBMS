#include <QApplication>
#include "main_window.h"
#include "ui/login_dialog.h"
#include "utils/logger.h"
#include "core/user_manager.h"
#include "core/table_manager.h"
#include "server/db_server.h"
#include <memory>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Modern Dark Theme
    app.setStyleSheet(
        "QMainWindow { background-color: #1e1e1e; }"
        "QWidget { font-family: 'Segoe UI', Arial; font-size: 10pt; color: #e0e0e0; background-color: #1e1e1e; }"
        "QTabWidget::pane { border: 1px solid #3c3c3c; background: #2d2d2d; }"
        "QTabBar::tab { background: #2d2d2d; color: #aaaaaa; padding: 8px 12px; border-top-left-radius: 4px; border-top-right-radius: 4px; margin-right: 2px; }"
        "QTabBar::tab:selected { background: #3d3d3d; color: #ffffff; border-bottom: 2px solid #3a86ff; }"
        "QTableWidget { background-color: #252526; alternate-background-color: #2d2d2d; gridline-color: #3c3c3c; border: 1px solid #3c3c3c; }"
        "QHeaderView::section { background-color: #333333; padding: 4px; border: 1px solid #3c3c3c; color: #e0e0e0; }"
        "QLineEdit, QComboBox, QTextEdit { background-color: #2d2d2d; border: 1px solid #3c3c3c; padding: 4px; border-radius: 3px; }"
        "QPushButton { background-color: #0e639c; color: white; border: none; padding: 6px 14px; border-radius: 4px; }"
        "QPushButton:hover { background-color: #1177bb; }"
        "QPushButton:pressed { background-color: #0d5889; }"
        "QListWidget { background-color: #252526; border: 1px solid #3c3c3c; }"
        "QListWidget::item:selected { background-color: #37373d; }"
        "QScrollBar:vertical { border: none; background: #1e1e1e; width: 10px; margin: 0px; }"
        "QScrollBar::handle:vertical { background: #424242; min-height: 20px; border-radius: 5px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
    );

    // Initialize logger
    Logger::instance().setLogLevel(LogLevel::INFO);
    Logger::instance().setLogFile("./simplerdbms.log");
    
    LOG_INFO("SimpleRDBMS starting up...");
    
    // Create user manager
    auto userManager = std::make_shared<UserManager>();
    userManager->initializeSystemDatabase();
    userManager->loadUsers();

    // DEBUG: Print all users
    // (This is a temporary debug step to help the user diagnose login issues)
    /*
    const auto& users = userManager->getAllUsers(); // Assuming getter exists or we just rely on logs
    */
    if (userManager->hasUsers()) {
        LOG_INFO("Users loaded successfully.");
    } else {
        LOG_INFO("No users found in database.");
    }

    // Ensure Admin Exists (Run this ALWAYS, not just in server mode)
    if (!userManager->authenticate("admin", "admin")) {
        LOG_INFO("Admin user validation failed (missing or wrong password). Re-creating 'admin'...");
        // internal check might be needed if user exists but wrong pass, but createUser checks existence.
        // For simplicity, if we can't auth admin/admin, we try to create it.
        // If it exists with different pass, createUser will fail (log error), which is fine.
        userManager->createUser("admin", "admin", true);
    }

    // Check for server mode
    if (app.arguments().contains("--server")) {
        LOG_INFO("Starting in SERVER MODE...");
        
        // Ensure table manager is initialized
        auto tableManager = std::make_shared<TableManager>();
        
        // Start Server
        DatabaseServer server(tableManager);
        if (server.start(8081)) {
            LOG_INFO("Database Server is running. Press Ctrl+C to stop.");
            return app.exec();
        } else {
            return 1;
        }
    }

    // Show login dialog
    LoginDialog loginDialog(userManager);
    if (loginDialog.exec() != QDialog::Accepted) {
        LOG_INFO("User cancelled login");
        return 0;
    }
    
    LOG_INFO(QString("User '%1' logged in").arg(loginDialog.getAuthenticatedUser()));
    
    // Create and show main window
    MainWindow window;
    window.setUserManager(userManager);
    window.show();
    
    LOG_INFO("SimpleRDBMS UI initialized");
    
    return app.exec();
}
