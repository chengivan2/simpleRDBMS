#pragma once

#include <QString>
#include <QDateTime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

/**
 * @brief Centralized logging system for SimpleRDBMS
 * 
 * Supports logging to file and console with different severity levels.
 * Thread-safe logging with timestamps.
 * Supports UI callbacks for displaying logs in the application.
 */
class Logger {
public:
    static Logger& instance();
    
    void setLogLevel(LogLevel level);
    void setLogFile(const QString& filePath);
    
    // UI callback for receiving log messages
    using LogCallback = std::function<void(const QString&)>;
    void setLogCallback(LogCallback callback);
    
    void debug(const QString& message);
    void info(const QString& message);
    void warning(const QString& message);
    void error(const QString& message);
    void critical(const QString& message);
    
    QString getLogFilePath() const;
    
private:
    Logger();
    ~Logger();
    
    void log(LogLevel level, const QString& message);
    QString levelToString(LogLevel level) const;
    
    LogLevel currentLevel;
    QString logFilePath;
    std::ofstream logStream;
    LogCallback uiCallback;
};

#define LOG_DEBUG(msg) Logger::instance().debug(msg)

#define LOG_INFO(msg) Logger::instance().info(msg)
#define LOG_WARNING(msg) Logger::instance().warning(msg)
#define LOG_ERROR(msg) Logger::instance().error(msg)
#define LOG_CRITICAL(msg) Logger::instance().critical(msg)
