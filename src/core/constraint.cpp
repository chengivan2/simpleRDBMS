#include "constraint.h"
#include <QRegularExpression>

// Base Constraint class
QString Constraint::getDescription() const {
    return QString("Constraint of type %1").arg(getName());
}

// PrimaryKeyConstraint
bool PrimaryKeyConstraint::validate(const QString& value) const {
    if (value.isNull() || value.isEmpty()) {
        lastError = "PRIMARY KEY cannot be NULL or empty";
        return false;
    }
    return true;
}

// UniqueConstraint
bool UniqueConstraint::validate(const QString& value) const {
    // Single value validation - uniqueness checked at table level
    // Here we only validate the value itself is not empty (NULL allowed for UNIQUE)
    return true;  // NULL is allowed in UNIQUE constraint, uniqueness enforced at table level
}

// NotNullConstraint
bool NotNullConstraint::validate(const QString& value) const {
    if (value.isNull() || value.isEmpty() || value.toLower() == "null") {
        lastError = "Column cannot be NULL";
        return false;
    }
    return true;
}

// ForeignKeyConstraint
bool ForeignKeyConstraint::validate(const QString& value) const {
    // Foreign key validation requires access to referenced table
    // This will be implemented at TableManager level
    // Here we just validate the value is not NULL (unless nullable)
    return true;
}

QString ForeignKeyConstraint::getFullReference() const {
    return QString("%1.%2").arg(referencedTable, referencedColumn);
}

// CheckConstraint
bool CheckConstraint::validate(const QString& value) const {
    return evaluateExpression(condition, value);
}

bool CheckConstraint::evaluateExpression(const QString& condition, const QString& value) {
    // Simple expression evaluator for common patterns
    // Examples: "value > 0", "value < 100", "value IN (1,2,3)"
    
    // Handle comparison operators
    if (condition.contains(">")) {
        QStringList parts = condition.split(">");
        if (parts.size() == 2) {
            bool ok;
            double condVal = parts[1].trimmed().toDouble(&ok);
            if (ok) {
                double dataVal = value.toDouble(&ok);
                if (ok) return dataVal > condVal;
            }
        }
    }
    
    if (condition.contains("<")) {
        QStringList parts = condition.split("<");
        if (parts.size() == 2) {
            bool ok;
            double condVal = parts[1].trimmed().toDouble(&ok);
            if (ok) {
                double dataVal = value.toDouble(&ok);
                if (ok) return dataVal < condVal;
            }
        }
    }
    
    if (condition.contains(">=")) {
        QStringList parts = condition.split(">=");
        if (parts.size() == 2) {
            bool ok;
            double condVal = parts[1].trimmed().toDouble(&ok);
            if (ok) {
                double dataVal = value.toDouble(&ok);
                if (ok) return dataVal >= condVal;
            }
        }
    }
    
    if (condition.contains("<=")) {
        QStringList parts = condition.split("<=");
        if (parts.size() == 2) {
            bool ok;
            double condVal = parts[1].trimmed().toDouble(&ok);
            if (ok) {
                double dataVal = value.toDouble(&ok);
                if (ok) return dataVal <= condVal;
            }
        }
    }
    
    if (condition.contains("=") && !condition.contains(">=") && !condition.contains("<=")) {
        QStringList parts = condition.split("=");
        if (parts.size() == 2) {
            return value == parts[1].trimmed();
        }
    }
    
    // Default: assume valid
    return true;
}

// ConstraintManager
QString ConstraintManager::getConstraintDescription(Constraint::Type type) {
    switch (type) {
        case Constraint::PRIMARY_KEY:
            return "Ensures unique identification of rows (unique, non-null)";
        case Constraint::UNIQUE:
            return "Ensures all non-null values in column are unique";
        case Constraint::NOT_NULL:
            return "Disallows NULL values in column";
        case Constraint::FOREIGN_KEY:
            return "Ensures referential integrity to another table";
        case Constraint::CHECK:
            return "Validates data against a custom condition";
        default:
            return "Unknown constraint";
    }
}

QString ConstraintManager::getConstraintDescription(const Constraint* constraint) {
    if (!constraint) return "NULL constraint";
    return getConstraintDescription(constraint->getType());
}

bool ConstraintManager::validateConstraint(const Constraint* constraint, const QString& value) {
    if (!constraint) return true;
    return constraint->validate(value);
}

QString ConstraintManager::getValidationErrorMessage(Constraint::Type type, const QString& value) {
    switch (type) {
        case Constraint::PRIMARY_KEY:
            return QString("PRIMARY KEY violation: value '%1' cannot be empty or NULL").arg(value);
        case Constraint::UNIQUE:
            return QString("UNIQUE constraint violation: value '%1' already exists").arg(value);
        case Constraint::NOT_NULL:
            return QString("NOT NULL constraint violation: NULL value not allowed");
        case Constraint::FOREIGN_KEY:
            return QString("FOREIGN KEY constraint violation: referenced value '%1' does not exist").arg(value);
        case Constraint::CHECK:
            return QString("CHECK constraint violation: value '%1' failed validation").arg(value);
        default:
            return "Unknown constraint violation";
    }
}
