#include "index.h"

Index::Index(const QString& indexName, const QString& tableName, const QStringList& columns)
    : indexName(indexName), tableName(tableName), columns(columns) {
}

void Index::insert(const QString& key, int rowId) {
    if (!indexMap.contains(key)) {
        indexMap[key] = QVector<int>();
    }
    indexMap[key].append(rowId);
}

bool Index::search(const QString& key) const {
    return indexMap.contains(key);
}

void Index::remove(const QString& key) {
    indexMap.remove(key);
}
