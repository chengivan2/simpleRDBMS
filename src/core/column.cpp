#include "column.h"
#include "constraint.h"

Column::Column(const QString& name, DataType type)
    : name(name), type(type), nullable(true), notNull(false),
      primaryKey(false), unique(false), autoIncrement(false) {
}

void Column::setForeignKey(const QString& table, const QString& column) {
    foreignKeyTable = table;
    foreignKeyColumn = column;
}

bool Column::validateValue(const QString& value) const {
    // Check for NULL
    if (value.isNull() || value.isEmpty() || value.toLower() == "null") {
        if (!nullable || notNull) {
            constraintError = "Column does not accept NULL values";
            return false;
        }
        return true;  // NULL is acceptable
    }
    
    // Validate against type
    if (!validateAgainstType(value)) {
        return false;
    }
    
    // Validate PRIMARY KEY (no NULL, no empty)
    if (primaryKey && value.isEmpty()) {
        constraintError = "PRIMARY KEY cannot be empty";
        return false;
    }
    
    // Validate CHECK constraint
    if (!checkCondition.isEmpty() && !validateCheckConstraint(value)) {
        return false;
    }
    
    return true;
}

QString Column::getConstraintError() const {
    return constraintError;
}

bool Column::validateAgainstType(const QString& value) const {
    if (!DataTypeManager::isValidValue(type, value)) {
        constraintError = QString("Invalid value '%1' for type %2")
            .arg(value)
            .arg(DataTypeManager::typeToString(type));
        return false;
    }
    
    // Check length constraints
    if (maxLength > 0) {
        if (DataTypeManager::isStringType(type) && value.length() > maxLength) {
            constraintError = QString("Value exceeds maximum length of %1").arg(maxLength);
            return false;
        }
    }
    
    // Check precision/scale for DECIMAL types
    if ((type == DataType::DECIMAL || type == DataType::NUMERIC) && precision > 0) {
        // Parse decimal value and check precision
        QStringList parts = value.split(".");
        if (parts.size() > 2) {
            constraintError = "Invalid decimal format";
            return false;
        }
        
        int integerPart = parts[0].length();
        int fractionalPart = (parts.size() == 2) ? parts[1].length() : 0;
        
        if (integerPart + fractionalPart > precision) {
            constraintError = QString("Decimal precision exceeded: %1 digits (max %2)")
                .arg(integerPart + fractionalPart)
                .arg(precision);
            return false;
        }
        
        if (scale > 0 && fractionalPart > scale) {
            constraintError = QString("Decimal scale exceeded: %1 digits (max %2)")
                .arg(fractionalPart)
                .arg(scale);
            return false;
        }
    }
    
    return true;
}

bool Column::validateCheckConstraint(const QString& value) const {
    if (!CheckConstraint::evaluateExpression(checkCondition, value)) {
        constraintError = QString("CHECK constraint failed: %1").arg(checkCondition);
        return false;
    }
    return true;
}
