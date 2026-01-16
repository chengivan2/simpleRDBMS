#pragma once

#include <QString>
#include <QDate>
#include <QTime>
#include <QDateTime>

/**
 * @brief Represents SQL data types
 */
enum class DataType {
    // Numeric types (exact)
    TINYINT,    // 1 byte, -128 to 127
    SMALLINT,   // 2 bytes, -32768 to 32767
    INT,        // 4 bytes, -2147483648 to 2147483647
    BIGINT,     // 8 bytes
    
    // Numeric types (approximate)
    DECIMAL,    // Exact decimal with precision
    NUMERIC,    // Alias for DECIMAL
    FLOAT,      // 4 bytes, single precision
    DOUBLE,     // 8 bytes, double precision
    
    // String types
    CHAR,       // Fixed-length ASCII
    VARCHAR,    // Variable-length ASCII
    TEXT,       // Large variable-length ASCII
    NCHAR,      // Fixed-length Unicode
    NVARCHAR,   // Variable-length Unicode
    TINYTEXT,   // 255 bytes max
    MEDIUMTEXT, // 16MB max
    LONGTEXT,   // 4GB max
    
    // Other types
    ENUM,       // Enumeration type
    BOOL,       // Boolean (TRUE/FALSE)
    JSON,       // JSON document
    DATE,       // YYYY-MM-DD format
    TIME,       // HH:MM:SS format
    DATETIME,   // YYYY-MM-DD HH:MM:SS format
    TIMESTAMP   // Unix timestamp
};

/**
 * @brief Manages data type operations and conversions
 */
class DataTypeManager {
public:
    // String conversion
    static QString typeToString(DataType type);
    static DataType stringToType(const QString& str);
    
    // Type information
    static int getTypeSize(DataType type);
    static QString getTypeDescription(DataType type);
    
    // Type classification
    static bool isNumericType(DataType type);
    static bool isIntegerType(DataType type);
    static bool isFloatingPointType(DataType type);
    static bool isStringType(DataType type);
    static bool isTemporalType(DataType type);
    static bool isBlobType(DataType type);
    
    // Type compatibility
    static bool canImplicitConvert(DataType from, DataType to);
    static bool canExplicitConvert(DataType from, DataType to);
    static DataType getCommonType(DataType type1, DataType type2);
    
    // Validation
    static bool isValidValue(DataType type, const QString& value);
    static QString validateAndSanitize(DataType type, const QString& value);
};
