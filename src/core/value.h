#pragma once

#include "data_type.h"
#include <QString>
#include <QVariant>
#include <QDate>
#include <QTime>
#include <QDateTime>

/**
 * @brief Represents a value with its type information
 */
class Value {
public:
    Value();
    explicit Value(DataType type);
    Value(DataType type, const QVariant& data);
    
    // Accessors
    DataType getType() const { return type; }
    QVariant getData() const { return data; }
    void setData(const QVariant& d);
    
    // Null handling
    bool isNull() const { return data.isNull(); }
    void setNull();
    
    // String representation
    QString toString() const;
    QString toDisplayString() const;
    
    // Type conversions
    int toInt(bool* ok = nullptr) const;
    long toLong(bool* ok = nullptr) const;
    double toDouble(bool* ok = nullptr) const;
    bool toBool() const;
    QString toQString() const;
    QDate toDate() const;
    QTime toTime() const;
    QDateTime toDateTime() const;
    
    // Conversion with type checking
    static Value convert(const Value& value, DataType targetType);
    static Value convertFromString(DataType type, const QString& str);
    
    // Comparison operators
    bool operator==(const Value& other) const;
    bool operator!=(const Value& other) const;
    bool operator<(const Value& other) const;
    bool operator<=(const Value& other) const;
    bool operator>(const Value& other) const;
    bool operator>=(const Value& other) const;
    
    // Arithmetic operators (for numeric types)
    Value operator+(const Value& other) const;
    Value operator-(const Value& other) const;
    Value operator*(const Value& other) const;
    Value operator/(const Value& other) const;
    Value operator%(const Value& other) const;
    
    // String operations
    Value concat(const Value& other) const;  // String concatenation
    Value substring(int start, int length) const;
    int length() const;
    Value upperCase() const;
    Value lowerCase() const;
    
    // Validation
    bool isValidValue() const;
    QString getValidationError() const;
    
    // Serialization
    QString serialize() const;
    static Value deserialize(DataType type, const QString& serialized);
    
private:
    DataType type;
    QVariant data;
    
    // Helper methods
    int compareValues(const Value& other) const;
    bool isCompatible(DataType otherType) const;
    QString getTypeError() const;
};
