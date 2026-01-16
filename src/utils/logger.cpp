#include "logger.h"

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

Logger::Logger() : currentLevel(LogLevel::INFO) {
    // Default: don't open file, only console logging
}

Logger::~Logger() {
    if (logStream.is_open()) {
        logStream.close();
    }
}

void Logger::setLogLevel(LogLevel level) {
    currentLevel = level;
}

void Logger::setLogFile(const QString& filePath) {
    if (logStream.is_open()) {
        logStream.close();
    }
    
    logFilePath = filePath;
    logStream.open(filePath.toStdString(), std::ios::app);
    
    if (!logStream.is_open()) {
        std::cerr << "Failed to open log file: " << filePath.toStdString() << std::endl;
    }
}

void Logger::setLogCallback(LogCallback callback) {
    uiCallback = callback;
}

QString Logger::getLogFilePath() const {
    return logFilePath;
}

void Logger::log(LogLevel level, const QString& message) {
    if (level < currentLevel) {
        return;  // Don't log messages below current level
    }
    
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString levelStr = levelToString(level);
    QString formattedMsg = QString("[%1] %2: %3").arg(timestamp, levelStr, message);
    
    // Console output
    std::cout << formattedMsg.toStdString() << std::endl;
    
    // File output
    if (logStream.is_open()) {
        logStream << formattedMsg.toStdString() << std::endl;
        logStream.flush();
    }
    
    // UI callback
    if (uiCallback) {
        uiCallback(formattedMsg);
    }
}

QString Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO";
        case LogLevel::WARNING:  return "WARN";
        case LogLevel::ERROR:    return "ERROR";
        case LogLevel::CRITICAL: return "CRIT";
        default:                 return "UNKNOWN";
    }
}

void Logger::debug(const QString& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const QString& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const QString& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const QString& message) {
    log(LogLevel::ERROR, message);
}

void Logger::critical(const QString& message) {
    log(LogLevel::CRITICAL, message);
}
