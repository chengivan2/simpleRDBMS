#pragma once

#include <QString>
#include <QMap>
#include <QVector>

/**
 * @brief Index structure for query optimization
 */
class Index {
public:
    Index(const QString& indexName, const QString& tableName, const QStringList& columns);
    
    QString getIndexName() const { return indexName; }
    QString getTableName() const { return tableName; }
    QStringList getColumns() const { return columns; }
    
    // To be implemented in Phase 10
    void insert(const QString& key, int rowId);
    bool search(const QString& key) const;
    void remove(const QString& key);
    
private:
    QString indexName;
    QString tableName;
    QStringList columns;
    QMap<QString, QVector<int>> indexMap;  // key -> row IDs
};
