#include "logs_page.h"
#include "../utils/logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QTextCursor>

LogsPage* LogsPage::staticInstance = nullptr;

LogsPage::LogsPage(QWidget* parent)
    : QWidget(parent) {
    setupUI();
    staticInstance = this;
    
    // Register this page with the logger so it receives all log messages
    Logger::instance().setLogCallback([this](const QString& message) {
        this->addLog(message);
    });
}

void LogsPage::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Logs display area
    logDisplay = new QTextEdit(this);
    logDisplay->setReadOnly(true);
    logDisplay->setFont(QFont("Courier", 9));
    mainLayout->addWidget(logDisplay);
    
    // Button layout
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    clearButton = new QPushButton("Clear Logs", this);
    connect(clearButton, &QPushButton::clicked, this, &LogsPage::clearLogs);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(clearButton);
    
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
}

LogsPage* LogsPage::instance() {
    return staticInstance;
}

void LogsPage::clearLogs() {
    logDisplay->clear();
}

void LogsPage::addLog(const QString& message) {
    logDisplay->append(message);
    // Auto-scroll to bottom
    QTextCursor cursor = logDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);
    logDisplay->setTextCursor(cursor);
}
