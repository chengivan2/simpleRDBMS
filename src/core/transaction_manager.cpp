#include "transaction_manager.h"

TransactionManager::TransactionManager() : inTransaction(false) {
}

void TransactionManager::beginTransaction() {
    inTransaction = true;
    rollbackLog.clear();
}

void TransactionManager::commit() {
    inTransaction = false;
    rollbackLog.clear();
}

void TransactionManager::rollback() {
    // To be implemented in Phase 8
    inTransaction = false;
    rollbackLog.clear();
}
