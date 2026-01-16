#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <memory>

class SQLEditorPage;
class TableManagerPage;
class LogsPage;
class TableManager;
class UserManager;

/**
 * @brief Main application window
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    
    /**
     * @brief Set the user manager (called after login)
     */
    void setUserManager(std::shared_ptr<UserManager> manager);
    
private:
    void setupUI();
    void createMenu();
    
    QTabWidget* tabWidget;
    std::shared_ptr<TableManager> tableManager;
    std::shared_ptr<UserManager> userManager;
    std::unique_ptr<SQLEditorPage> sqlEditorPage;
    std::unique_ptr<TableManagerPage> tableManagerPage;
    std::unique_ptr<LogsPage> logsPage;
};
