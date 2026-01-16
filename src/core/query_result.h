#pragma once

#include <QString>
#include <QVector>

/**
 * @brief Represents query execution results
 */
class QueryResult {
public:
    QVector<QStringList> rows;       // Result rows
    QStringList columns;              // Column names
    int affectedRows = 0;             // Rows affected by INSERT/UPDATE/DELETE
    QString errorMessage;             // Error description
    bool success = false;             // Whether query succeeded
};
