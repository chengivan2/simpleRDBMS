#include "value.h"

// Constructors
Value::Value() : type(DataType::TEXT) {
}

Value::Value(DataType type) : type(type) {
}

Value::Value(DataType type, const QVariant& data) 
    : type(type), data(data) {
}

// Accessors
void Value::setData(const QVariant& d) {
    data = d;
}

void Value::setNull() {
    data = QVariant();
}

// String representation
QString Value::toString() const {
    if (isNull()) return "NULL";
    return data.toString();
}

QString Value::toDisplayString() const {
    if (isNull()) return "NULL";
    
    switch (type) {
        case DataType::BOOL: {
            return data.toBool() ? "TRUE" : "FALSE";
        }
        case DataType::DATE: {
            QDate date = data.toDate();
            return date.toString("yyyy-MM-dd");
        }
        case DataType::TIME: {
            QTime time = data.toTime();
            return time.toString("HH:mm:ss");
        }
        case DataType::DATETIME:
        case DataType::TIMESTAMP: {
            QDateTime datetime = data.toDateTime();
            return datetime.toString("yyyy-MM-dd HH:mm:ss");
        }
        default:
            return data.toString();
    }
}

// Type conversions
int Value::toInt(bool* ok) const {
    if (isNull()) {
        if (ok) *ok = false;
        return 0;
    }
    return data.toInt(ok);
}

long Value::toLong(bool* ok) const {
    if (isNull()) {
        if (ok) *ok = false;
        return 0;
    }
    return data.toLongLong(ok);
}

double Value::toDouble(bool* ok) const {
    if (isNull()) {
        if (ok) *ok = false;
        return 0.0;
    }
    return data.toDouble(ok);
}

bool Value::toBool() const {
    if (isNull()) return false;
    
    switch (type) {
        case DataType::BOOL:
            return data.toBool();
        case DataType::INT:
        case DataType::SMALLINT:
        case DataType::TINYINT:
        case DataType::BIGINT:
            return data.toInt() != 0;
        case DataType::FLOAT:
        case DataType::DOUBLE:
        case DataType::DECIMAL:
        case DataType::NUMERIC:
            return data.toDouble() != 0.0;
        default: {
            QString str = data.toString().toUpper();
            return str == "TRUE" || str == "1" || str == "YES";
        }
    }
}

QString Value::toQString() const {
    return data.toString();
}

QDate Value::toDate() const {
    if (isNull()) return QDate();
    return data.toDate();
}

QTime Value::toTime() const {
    if (isNull()) return QTime();
    return data.toTime();
}

QDateTime Value::toDateTime() const {
    if (isNull()) return QDateTime();
    return data.toDateTime();
}

// Conversion with type checking
Value Value::convert(const Value& value, DataType targetType) {
    if (value.getType() == targetType) {
        return value;
    }
    
    if (!DataTypeManager::canExplicitConvert(value.getType(), targetType)) {
        return Value(targetType); // Return null value of target type
    }
    
    return Value::convertFromString(targetType, value.toString());
}

Value Value::convertFromString(DataType type, const QString& str) {
    if (str.isEmpty() || str.toLower() == "null") {
        return Value(type);
    }
    
    Value result(type);
    
    switch (type) {
        case DataType::TINYINT:
        case DataType::SMALLINT:
        case DataType::INT:
        case DataType::BIGINT: {
            bool ok;
            long val = str.toLongLong(&ok);
            if (ok) result.setData(QVariant(static_cast<int>(val)));
            break;
        }
        case DataType::FLOAT:
        case DataType::DOUBLE:
        case DataType::DECIMAL:
        case DataType::NUMERIC: {
            bool ok;
            double val = str.toDouble(&ok);
            if (ok) result.setData(QVariant(val));
            break;
        }
        case DataType::BOOL: {
            QString upper = str.toUpper();
            bool val = (upper == "TRUE" || upper == "1" || upper == "YES");
            result.setData(QVariant(val));
            break;
        }
        case DataType::DATE: {
            QDate date = QDate::fromString(str, "yyyy-MM-dd");
            if (date.isValid()) result.setData(QVariant(date));
            break;
        }
        case DataType::TIME: {
            QTime time = QTime::fromString(str, "HH:mm:ss");
            if (time.isValid()) result.setData(QVariant(time));
            break;
        }
        case DataType::DATETIME:
        case DataType::TIMESTAMP: {
            QDateTime datetime = QDateTime::fromString(str, "yyyy-MM-dd HH:mm:ss");
            if (datetime.isValid()) result.setData(QVariant(datetime));
            break;
        }
        default:
            // String types - remove quotes if present
            if ((str.startsWith("'") && str.endsWith("'")) ||
                (str.startsWith("\"") && str.endsWith("\""))) {
                result.setData(QVariant(str.mid(1, str.length() - 2)));
            } else {
                result.setData(QVariant(str));
            }
    }
    
    return result;
}

// Comparison operators
bool Value::operator==(const Value& other) const {
    if (isNull() && other.isNull()) return true;
    if (isNull() || other.isNull()) return false;
    
    if (DataTypeManager::isNumericType(type) && DataTypeManager::isNumericType(other.getType())) {
        return qAbs(toDouble() - other.toDouble()) < 1e-9;
    }
    
    return data == other.data;
}

bool Value::operator!=(const Value& other) const {
    return !(*this == other);
}

bool Value::operator<(const Value& other) const {
    return compareValues(other) < 0;
}

bool Value::operator<=(const Value& other) const {
    return compareValues(other) <= 0;
}

bool Value::operator>(const Value& other) const {
    return compareValues(other) > 0;
}

bool Value::operator>=(const Value& other) const {
    return compareValues(other) >= 0;
}

// Arithmetic operators
Value Value::operator+(const Value& other) const {
    if (!DataTypeManager::isNumericType(type) || !DataTypeManager::isNumericType(other.getType())) {
        return Value(); // Return null for non-numeric types
    }
    
    DataType resultType = DataTypeManager::getCommonType(type, other.getType());
    double result = toDouble() + other.toDouble();
    return Value(resultType, QVariant(result));
}

Value Value::operator-(const Value& other) const {
    if (!DataTypeManager::isNumericType(type) || !DataTypeManager::isNumericType(other.getType())) {
        return Value();
    }
    
    DataType resultType = DataTypeManager::getCommonType(type, other.getType());
    double result = toDouble() - other.toDouble();
    return Value(resultType, QVariant(result));
}

Value Value::operator*(const Value& other) const {
    if (!DataTypeManager::isNumericType(type) || !DataTypeManager::isNumericType(other.getType())) {
        return Value();
    }
    
    DataType resultType = DataTypeManager::getCommonType(type, other.getType());
    double result = toDouble() * other.toDouble();
    return Value(resultType, QVariant(result));
}

Value Value::operator/(const Value& other) const {
    if (!DataTypeManager::isNumericType(type) || !DataTypeManager::isNumericType(other.getType())) {
        return Value();
    }
    
    if (qAbs(other.toDouble()) < 1e-9) {
        return Value(); // Division by zero
    }
    
    DataType resultType = DataTypeManager::getCommonType(type, other.getType());
    double result = toDouble() / other.toDouble();
    return Value(resultType, QVariant(result));
}

Value Value::operator%(const Value& other) const {
    if (!DataTypeManager::isIntegerType(type) || !DataTypeManager::isIntegerType(other.getType())) {
        return Value();
    }
    
    if (other.toInt() == 0) {
        return Value(); // Division by zero
    }
    
    int result = toInt() % other.toInt();
    return Value(type, QVariant(result));
}

// String operations
Value Value::concat(const Value& other) const {
    QString result = toString() + other.toString();
    return Value(DataType::VARCHAR, QVariant(result));
}

Value Value::substring(int start, int length) const {
    QString str = toString();
    if (start < 0 || start >= str.length()) {
        return Value(DataType::VARCHAR, QVariant(""));
    }
    return Value(DataType::VARCHAR, QVariant(str.mid(start, length)));
}

int Value::length() const {
    return toString().length();
}

Value Value::upperCase() const {
    return Value(type, QVariant(toString().toUpper()));
}

Value Value::lowerCase() const {
    return Value(type, QVariant(toString().toLower()));
}

// Validation
bool Value::isValidValue() const {
    if (isNull()) return true;
    return DataTypeManager::isValidValue(type, toString());
}

QString Value::getValidationError() const {
    if (!isValidValue()) {
        return QString("Invalid value '%1' for type %2")
            .arg(toString())
            .arg(DataTypeManager::typeToString(type));
    }
    return "";
}

// Serialization
QString Value::serialize() const {
    if (isNull()) return "NULL";
    
    switch (type) {
        case DataType::BOOL:
            return toBool() ? "1" : "0";
        case DataType::DATE:
            return toDate().toString("yyyy-MM-dd");
        case DataType::TIME:
            return toTime().toString("HH:mm:ss");
        case DataType::DATETIME:
        case DataType::TIMESTAMP:
            return toDateTime().toString("yyyy-MM-dd HH:mm:ss");
        default:
            return toString();
    }
}

Value Value::deserialize(DataType type, const QString& serialized) {
    return convertFromString(type, serialized);
}

// Helper methods
int Value::compareValues(const Value& other) const {
    if (isNull() && other.isNull()) return 0;
    if (isNull()) return -1;
    if (other.isNull()) return 1;
    
    // Numeric comparison
    if (DataTypeManager::isNumericType(type) && DataTypeManager::isNumericType(other.getType())) {
        double d1 = toDouble();
        double d2 = other.toDouble();
        if (d1 < d2) return -1;
        if (d1 > d2) return 1;
        return 0;
    }
    
    // Temporal comparison
    if (DataTypeManager::isTemporalType(type) && DataTypeManager::isTemporalType(other.getType())) {
        if (type == DataType::DATE && other.getType() == DataType::DATE) {
            QDate d1 = toDate();
            QDate d2 = other.toDate();
            if (d1 < d2) return -1;
            if (d1 > d2) return 1;
            return 0;
        }
        if (type == DataType::TIME && other.getType() == DataType::TIME) {
            QTime t1 = toTime();
            QTime t2 = other.toTime();
            if (t1 < t2) return -1;
            if (t1 > t2) return 1;
            return 0;
        }
    }
    
    // String comparison
    QString s1 = toString();
    QString s2 = other.toString();
    if (s1 < s2) return -1;
    if (s1 > s2) return 1;
    return 0;
}

bool Value::isCompatible(DataType otherType) const {
    return DataTypeManager::canImplicitConvert(type, otherType);
}

QString Value::getTypeError() const {
    return QString("Type mismatch: %1").arg(DataTypeManager::typeToString(type));
}
