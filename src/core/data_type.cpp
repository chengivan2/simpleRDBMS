#include "data_type.h"
#include <QRegularExpression>

QString DataTypeManager::typeToString(DataType type) {
    switch (type) {
        case DataType::TINYINT:     return "TINYINT";
        case DataType::SMALLINT:    return "SMALLINT";
        case DataType::INT:         return "INT";
        case DataType::BIGINT:      return "BIGINT";
        case DataType::DECIMAL:     return "DECIMAL";
        case DataType::NUMERIC:     return "NUMERIC";
        case DataType::FLOAT:       return "FLOAT";
        case DataType::DOUBLE:      return "DOUBLE";
        case DataType::CHAR:        return "CHAR";
        case DataType::VARCHAR:     return "VARCHAR";
        case DataType::TEXT:        return "TEXT";
        case DataType::NCHAR:       return "NCHAR";
        case DataType::NVARCHAR:    return "NVARCHAR";
        case DataType::TINYTEXT:    return "TINYTEXT";
        case DataType::MEDIUMTEXT:  return "MEDIUMTEXT";
        case DataType::LONGTEXT:    return "LONGTEXT";
        case DataType::ENUM:        return "ENUM";
        case DataType::BOOL:        return "BOOL";
        case DataType::JSON:        return "JSON";
        case DataType::DATE:        return "DATE";
        case DataType::TIME:        return "TIME";
        case DataType::DATETIME:    return "DATETIME";
        case DataType::TIMESTAMP:   return "TIMESTAMP";
        default:                    return "UNKNOWN";
    }
}

DataType DataTypeManager::stringToType(const QString& str) {
    QString upper = str.toUpper();
    
    if (upper == "TINYINT") return DataType::TINYINT;
    if (upper == "SMALLINT") return DataType::SMALLINT;
    if (upper == "INT" || upper == "INTEGER") return DataType::INT;
    if (upper == "BIGINT") return DataType::BIGINT;
    if (upper == "DECIMAL") return DataType::DECIMAL;
    if (upper == "NUMERIC") return DataType::NUMERIC;
    if (upper == "FLOAT") return DataType::FLOAT;
    if (upper == "DOUBLE") return DataType::DOUBLE;
    if (upper == "CHAR") return DataType::CHAR;
    if (upper == "VARCHAR") return DataType::VARCHAR;
    if (upper == "TEXT") return DataType::TEXT;
    if (upper == "NCHAR") return DataType::NCHAR;
    if (upper == "NVARCHAR") return DataType::NVARCHAR;
    if (upper == "TINYTEXT") return DataType::TINYTEXT;
    if (upper == "MEDIUMTEXT") return DataType::MEDIUMTEXT;
    if (upper == "LONGTEXT") return DataType::LONGTEXT;
    if (upper == "ENUM") return DataType::ENUM;
    if (upper == "BOOL" || upper == "BOOLEAN") return DataType::BOOL;
    if (upper == "JSON") return DataType::JSON;
    if (upper == "DATE") return DataType::DATE;
    if (upper == "TIME") return DataType::TIME;
    if (upper == "DATETIME") return DataType::DATETIME;
    if (upper == "TIMESTAMP") return DataType::TIMESTAMP;
    
    return DataType::INT; // Default fallback
}

int DataTypeManager::getTypeSize(DataType type) {
    switch (type) {
        case DataType::TINYINT:     return 1;
        case DataType::SMALLINT:    return 2;
        case DataType::INT:         return 4;
        case DataType::BIGINT:      return 8;
        case DataType::DECIMAL:     return 16;    // Variable, estimate
        case DataType::NUMERIC:     return 16;    // Variable, estimate
        case DataType::FLOAT:       return 4;
        case DataType::DOUBLE:      return 8;
        case DataType::CHAR:        return 255;   // Variable, typical
        case DataType::VARCHAR:     return 255;   // Variable, typical
        case DataType::TEXT:        return 65536; // Variable, typical
        case DataType::NCHAR:       return 255;   // Variable, typical
        case DataType::NVARCHAR:    return 255;   // Variable, typical
        case DataType::TINYTEXT:    return 255;
        case DataType::MEDIUMTEXT:  return 1048576;
        case DataType::LONGTEXT:    return INT_MAX;  // Large size (use INT_MAX for 32-bit)
        case DataType::ENUM:        return 4;
        case DataType::BOOL:        return 1;
        case DataType::JSON:        return 65536; // Variable, typical
        case DataType::DATE:        return 3;
        case DataType::TIME:        return 3;
        case DataType::DATETIME:    return 8;
        case DataType::TIMESTAMP:   return 4;
        default:                    return 0;
    }
}

QString DataTypeManager::getTypeDescription(DataType type) {
    switch (type) {
        case DataType::TINYINT:     return "Tiny integer (-128 to 127)";
        case DataType::SMALLINT:    return "Small integer (-32768 to 32767)";
        case DataType::INT:         return "Integer (-2147483648 to 2147483647)";
        case DataType::BIGINT:      return "Big integer (64-bit)";
        case DataType::DECIMAL:     return "Decimal number with precision";
        case DataType::NUMERIC:     return "Numeric type with precision";
        case DataType::FLOAT:       return "Floating point (32-bit)";
        case DataType::DOUBLE:      return "Double precision floating point (64-bit)";
        case DataType::CHAR:        return "Fixed-length ASCII string";
        case DataType::VARCHAR:     return "Variable-length ASCII string";
        case DataType::TEXT:        return "Large variable-length text";
        case DataType::NCHAR:       return "Fixed-length Unicode string";
        case DataType::NVARCHAR:    return "Variable-length Unicode string";
        case DataType::TINYTEXT:    return "Tiny text (up to 255 bytes)";
        case DataType::MEDIUMTEXT:  return "Medium text (up to 16 MB)";
        case DataType::LONGTEXT:    return "Long text (up to 4 GB)";
        case DataType::ENUM:        return "Enumeration type";
        case DataType::BOOL:        return "Boolean (TRUE/FALSE)";
        case DataType::JSON:        return "JSON document";
        case DataType::DATE:        return "Date (YYYY-MM-DD)";
        case DataType::TIME:        return "Time (HH:MM:SS)";
        case DataType::DATETIME:    return "Date and time";
        case DataType::TIMESTAMP:   return "Unix timestamp";
        default:                    return "Unknown type";
    }
}

bool DataTypeManager::isNumericType(DataType type) {
    return isIntegerType(type) || isFloatingPointType(type) ||
           type == DataType::DECIMAL || type == DataType::NUMERIC;
}

bool DataTypeManager::isIntegerType(DataType type) {
    return type == DataType::TINYINT || type == DataType::SMALLINT ||
           type == DataType::INT || type == DataType::BIGINT;
}

bool DataTypeManager::isFloatingPointType(DataType type) {
    return type == DataType::FLOAT || type == DataType::DOUBLE;
}

bool DataTypeManager::isStringType(DataType type) {
    return type == DataType::CHAR || type == DataType::VARCHAR ||
           type == DataType::TEXT || type == DataType::NCHAR ||
           type == DataType::NVARCHAR || type == DataType::TINYTEXT ||
           type == DataType::MEDIUMTEXT || type == DataType::LONGTEXT ||
           type == DataType::JSON;
}

bool DataTypeManager::isTemporalType(DataType type) {
    return type == DataType::DATE || type == DataType::TIME ||
           type == DataType::DATETIME || type == DataType::TIMESTAMP;
}

bool DataTypeManager::isBlobType(DataType type) {
    return type == DataType::MEDIUMTEXT || type == DataType::LONGTEXT ||
           type == DataType::JSON;
}

bool DataTypeManager::canImplicitConvert(DataType from, DataType to) {
    // Same type always convertible
    if (from == to) return true;
    
    // Numeric type conversions
    if (isIntegerType(from) && isIntegerType(to)) return true;
    if (isIntegerType(from) && isFloatingPointType(to)) return true;
    if (isIntegerType(from) && (to == DataType::DECIMAL || to == DataType::NUMERIC)) return true;
    if (isFloatingPointType(from) && isFloatingPointType(to)) return true;
    if (isFloatingPointType(from) && (to == DataType::DECIMAL || to == DataType::NUMERIC)) return true;
    
    // To string types
    if (isNumericType(from) && isStringType(to)) return true;
    if (isTemporalType(from) && isStringType(to)) return true;
    if (from == DataType::BOOL && isStringType(to)) return true;
    
    // From string types to numeric
    if (isStringType(from) && isNumericType(to)) return true;
    if (isStringType(from) && isTemporalType(to)) return true;
    
    return false;
}

bool DataTypeManager::canExplicitConvert(DataType from, DataType to) {
    return canImplicitConvert(from, to) ||
           isNumericType(from) && isTemporalType(to) ||
           isTemporalType(from) && isNumericType(to);
}

DataType DataTypeManager::getCommonType(DataType type1, DataType type2) {
    if (type1 == type2) return type1;
    
    // If one is string, result is string
    if (isStringType(type1)) return type1;
    if (isStringType(type2)) return type2;
    
    // Numeric type promotion
    if (isNumericType(type1) && isNumericType(type2)) {
        // Promote to larger type
        if (type1 == DataType::DOUBLE || type2 == DataType::DOUBLE) return DataType::DOUBLE;
        if (type1 == DataType::FLOAT || type2 == DataType::FLOAT) return DataType::FLOAT;
        if (type1 == DataType::BIGINT || type2 == DataType::BIGINT) return DataType::BIGINT;
        return DataType::INT;
    }
    
    return DataType::VARCHAR; // Default to string
}

bool DataTypeManager::isValidValue(DataType type, const QString& value) {
    if (value.isEmpty() || value.toLower() == "null") return true;
    
    switch (type) {
        case DataType::TINYINT:
        case DataType::SMALLINT:
        case DataType::INT:
        case DataType::BIGINT: {
            bool ok;
            value.toLongLong(&ok);
            return ok;
        }
        case DataType::FLOAT:
        case DataType::DOUBLE:
        case DataType::DECIMAL:
        case DataType::NUMERIC: {
            bool ok;
            value.toDouble(&ok);
            return ok;
        }
        case DataType::BOOL: {
            QString upper = value.toUpper();
            return upper == "TRUE" || upper == "FALSE" || upper == "1" || upper == "0";
        }
        case DataType::DATE: {
            QRegularExpression dateRegex("^\\d{4}-\\d{2}-\\d{2}$");
            return dateRegex.match(value).hasMatch();
        }
        case DataType::TIME: {
            QRegularExpression timeRegex("^\\d{2}:\\d{2}:\\d{2}$");
            return timeRegex.match(value).hasMatch();
        }
        case DataType::DATETIME:
        case DataType::TIMESTAMP: {
            QRegularExpression datetimeRegex("^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}$");
            return datetimeRegex.match(value).hasMatch();
        }
        case DataType::JSON: {
            // Basic JSON validation - starts with { or [
            QString trimmed = value.trimmed();
            return (trimmed.startsWith("{") && trimmed.endsWith("}")) ||
                   (trimmed.startsWith("[") && trimmed.endsWith("]"));
        }
        default:
            return true; // String types accept any value
    }
}

QString DataTypeManager::validateAndSanitize(DataType type, const QString& value) {
    if (value.isEmpty() || value.toLower() == "null") return "NULL";
    
    // Trim whitespace
    QString sanitized = value.trimmed();
    
    switch (type) {
        case DataType::BOOL:
            if (sanitized.toUpper() == "TRUE" || sanitized == "1") return "TRUE";
            if (sanitized.toUpper() == "FALSE" || sanitized == "0") return "FALSE";
            return "FALSE";
            
        case DataType::VARCHAR:
        case DataType::CHAR:
        case DataType::NVARCHAR:
        case DataType::NCHAR:
        case DataType::TEXT:
        case DataType::TINYTEXT:
        case DataType::MEDIUMTEXT:
        case DataType::LONGTEXT:
            // Remove quotes if present
            if ((sanitized.startsWith("'") && sanitized.endsWith("'")) ||
                (sanitized.startsWith("\"") && sanitized.endsWith("\""))) {
                return sanitized.mid(1, sanitized.length() - 2);
            }
            return sanitized;
            
        default:
            return sanitized;
    }
}
