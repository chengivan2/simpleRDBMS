#pragma once

#include <QString>
#include <QVector>
#include <memory>

/**
 * @brief Represents a SQL token
 */
struct Token {
    enum Type {
        // Keywords
        SELECT, FROM, WHERE, INSERT, INTO, VALUES, UPDATE, SET, DELETE,
        CREATE, TABLE, ALTER, ADD, MODIFY, DROP, TRUNCATE,
        JOIN, INNER, LEFT, RIGHT, FULL, OUTER, CROSS, ON,
        BEGIN, COMMIT, ROLLBACK,
        INDEX, CREATE_INDEX,
        CONSTRAINT, PRIMARY_KEY, UNIQUE, NOT_NULL, FOREIGN_KEY, CHECK, DEFAULT,
        ORDER, BY, ASC, DESC, LIMIT, OFFSET,
        AND, OR, NOT,
        INT, INTEGER, SMALLINT, BIGINT, DECIMAL, NUMERIC, FLOAT,
        CHAR, VARCHAR, TEXT, NCHAR, NVARCHAR, TINYTEXT, MEDIUMTEXT, LONGTEXT,
        ENUM, BOOL, JSON, DATE, TIME, DATETIME, TIMESTAMP,
        
        // Operators
        EQUALS, NOT_EQUALS, LESS, LESS_EQUAL, GREATER, GREATER_EQUAL,
        PLUS, MINUS, MULTIPLY, DIVIDE, MODULO,
        LPAREN, RPAREN, COMMA, SEMICOLON, DOT,
        ASTERISK, PERCENT,
        
        // Literals
        IDENTIFIER, NUMBER, STRING, TRUE_KW, FALSE_KW, NULL_KW,
        
        // Special
        EOL, END_OF_FILE, UNKNOWN
    };
    
    Type type;
    QString value;
    int line;
    int column;
};

