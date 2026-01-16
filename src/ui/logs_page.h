#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>

/**
 * @brief Logs Page - Display execution logs and debug information
 */
class LogsPage : public QWidget {
    Q_OBJECT
    
public:
    explicit LogsPage(QWidget* parent = nullptr);
    
    /**
     * @brief Get the static instance for logging
     */
    static LogsPage* instance();
    
    /**
     * @brief Add a log message to display
     */
    void addLog(const QString& message);
    
private slots:
    void clearLogs();
    
private:
    void setupUI();
    
    QTextEdit* logDisplay;
    QPushButton* clearButton;
    
    static LogsPage* staticInstance;
};
