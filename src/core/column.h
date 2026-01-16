#pragma once

#include "data_type.h"
#include <QString>
#include <QVector>
#include <memory>

// Forward declaration
class Constraint;

/**
 * @brief Represents a column in a table
 */
class Column {
public:
    Column(const QString& name, DataType type);
    
    // Basic information
    QString getName() const { return name; }
    DataType getType() const { return type; }
    
    // Constraint status
    bool isNullable() const { return nullable; }
    bool isPrimaryKey() const { return primaryKey; }
    bool isUnique() const { return unique; }
    bool hasNotNull() const { return notNull; }
    bool hasDefault() const { return !defaultValue.isEmpty(); }
    
    // Constraint details
    QString getDefaultValue() const { return defaultValue; }
    QString getForeignKeyTable() const { return foreignKeyTable; }
    QString getForeignKeyColumn() const { return foreignKeyColumn; }
    QString getCheckCondition() const { return checkCondition; }
    
    // Constraint setters
    void setNullable(bool n) { nullable = n; }
    void setPrimaryKey(bool pk) { primaryKey = pk; if (pk) nullable = false; }
    void setUnique(bool u) { unique = u; }
    void setNotNull(bool nn) { notNull = nn; if (nn) nullable = false; }
    void setDefaultValue(const QString& def) { defaultValue = def; }
    void setForeignKey(const QString& table, const QString& column);
    void setCheckCondition(const QString& condition) { checkCondition = condition; }
    
    // Column properties
    int getMaxLength() const { return maxLength; }
    void setMaxLength(int len) { maxLength = len; }
    
    int getPrecision() const { return precision; }
    int getScale() const { return scale; }
    void setDecimalPrecision(int p, int s) { precision = p; scale = s; }
    
    bool isAutoIncrement() const { return autoIncrement; }
    void setAutoIncrement(bool ai) { autoIncrement = ai; }
    
    // Constraint validation
    bool validateValue(const QString& value) const;
    QString getConstraintError() const;
    
    // Metadata
    QString getDescription() const { return description; }
    void setDescription(const QString& desc) { description = desc; }
    
private:
    QString name;
    DataType type;
    
    // Null constraints
    bool nullable = true;
    bool notNull = false;
    
    // Key constraints
    bool primaryKey = false;
    bool unique = false;
    bool autoIncrement = false;
    
    // Default
    QString defaultValue;
    
    // Foreign key reference
    QString foreignKeyTable;
    QString foreignKeyColumn;
    
    // Check constraint
    QString checkCondition;
    
    // Type-specific properties
    int maxLength = -1;           // For CHAR/VARCHAR
    int precision = -1;           // For DECIMAL/NUMERIC
    int scale = -1;               // For DECIMAL/NUMERIC
    
    // Metadata
    QString description;
    mutable QString constraintError;
    
    // Helper methods
    bool validateAgainstType(const QString& value) const;
    bool validateCheckConstraint(const QString& value) const;
};
