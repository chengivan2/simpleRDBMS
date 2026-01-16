#pragma once

#include <QString>
#include <QVector>

/**
 * @brief Manages database transactions
 */
class TransactionManager {
public:
    TransactionManager();
    
    bool isInTransaction() const { return inTransaction; }
    
    // To be implemented in Phase 8
    void beginTransaction();
    void commit();
    void rollback();
    
private:
    bool inTransaction;
    QVector<QString> rollbackLog;
};
