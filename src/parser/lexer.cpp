#include "lexer.h"
#include <QDebug>

Lexer::Lexer(const QString& input) 
    : input(input), position(0), line(1), column(1) {
}

QVector<Token> Lexer::tokenize() {
    QVector<Token> tokens;
    while (position < input.length()) {
        skipWhitespace();
        if (position >= input.length()) break;
        
        // Handle comments
        if (current() == '-' && peek() == '-') {
            skipComment();
            continue;
        }
        
        tokens.push_back(nextToken());
    }
    
    tokens.push_back(makeToken(Token::END_OF_FILE, ""));
    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    if (position >= input.length()) {
        return makeToken(Token::END_OF_FILE, "");
    }
    
    char ch = current();
    
    // Identifiers and keywords
    if (isIdentifierStart(ch)) {
        return readIdentifierOrKeyword();
    }
    
    // Numbers
    if (isDigit(ch)) {
        return readNumber();
    }
    
    // Strings
    if (ch == '\'' || ch == '"') {
        return readString(ch);
    }
    
    // Operators and punctuation
    advance();
    switch (ch) {
        case '(': return makeToken(Token::LPAREN, "(");
        case ')': return makeToken(Token::RPAREN, ")");
        case ',': return makeToken(Token::COMMA, ",");
        case ';': return makeToken(Token::SEMICOLON, ";");
        case '.': return makeToken(Token::DOT, ".");
        case '*': return makeToken(Token::ASTERISK, "*");
        case '%': return makeToken(Token::PERCENT, "%");
        case '+': return makeToken(Token::PLUS, "+");
        case '-': return makeToken(Token::MINUS, "-");
        case '/': return makeToken(Token::DIVIDE, "/");
        case '=': return makeToken(Token::EQUALS, "=");
        case '<': {
            if (current() == '=') {
                advance();
                return makeToken(Token::LESS_EQUAL, "<=");
            } else if (current() == '>') {
                advance();
                return makeToken(Token::NOT_EQUALS, "<>");
            }
            return makeToken(Token::LESS, "<");
        }
        case '>': {
            if (current() == '=') {
                advance();
                return makeToken(Token::GREATER_EQUAL, ">=");
            }
            return makeToken(Token::GREATER, ">");
        }
        case '!': {
            if (current() == '=') {
                advance();
                return makeToken(Token::NOT_EQUALS, "!=");
            }
            return makeToken(Token::UNKNOWN, "!");
        }
        default:
            return makeToken(Token::UNKNOWN, QString(ch));
    }
}

char Lexer::current() const {
    if (position >= input.length()) return '\0';
    return input[position].toLatin1();
}

char Lexer::peek() const {
    if (position + 1 >= input.length()) return '\0';
    return input[position + 1].toLatin1();
}

void Lexer::advance() {
    if (position < input.length()) {
        if (input[position] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }
}

void Lexer::skipWhitespace() {
    while (position < input.length() && input[position].isSpace()) {
        advance();
    }
}

void Lexer::skipComment() {
    // Skip '--' comment until end of line
    while (position < input.length() && input[position] != '\n') {
        advance();
    }
}

Token Lexer::readIdentifierOrKeyword() {
    QString value;
    int startPos = position;
    int startLine = line;
    int startCol = column;
    
    while (position < input.length() && isIdentifierPart(current())) {
        value += current();
        advance();
    }
    
    Token::Type type = keywordToTokenType(value);
    if (type == Token::UNKNOWN) {
        type = Token::IDENTIFIER;
    }
    
    return Token{type, value, startLine, startCol};
}

Token Lexer::readNumber() {
    QString value;
    int startPos = position;
    int startLine = line;
    int startCol = column;
    
    while (position < input.length() && (isDigit(current()) || current() == '.')) {
        value += current();
        advance();
    }
    
    return Token{Token::NUMBER, value, startLine, startCol};
}

Token Lexer::readString(char quote) {
    QString value;
    int startLine = line;
    int startCol = column;
    advance();  // Skip opening quote
    
    while (position < input.length() && current() != quote) {
        if (current() == '\\' && peek() == quote) {
            advance();  // Skip backslash
            value += current();
            advance();
        } else {
            value += current();
            advance();
        }
    }
    
    if (current() == quote) {
        advance();  // Skip closing quote
    }
    
    return Token{Token::STRING, value, startLine, startCol};
}

Token Lexer::makeToken(Token::Type type, const QString& value) {
    return Token{type, value, line, column};
}

Token::Type Lexer::keywordToTokenType(const QString& keyword) const {
    QString upper = keyword.toUpper();
    
    if (upper == "SELECT") return Token::SELECT;
    if (upper == "FROM") return Token::FROM;
    if (upper == "WHERE") return Token::WHERE;
    if (upper == "INSERT") return Token::INSERT;
    if (upper == "INTO") return Token::INTO;
    if (upper == "VALUES") return Token::VALUES;
    if (upper == "UPDATE") return Token::UPDATE;
    if (upper == "SET") return Token::SET;
    if (upper == "DELETE") return Token::DELETE;
    if (upper == "CREATE") return Token::CREATE;
    if (upper == "TABLE") return Token::TABLE;
    if (upper == "ALTER") return Token::ALTER;
    if (upper == "ADD") return Token::ADD;
    if (upper == "MODIFY") return Token::MODIFY;
    if (upper == "DROP") return Token::DROP;
    if (upper == "TRUNCATE") return Token::TRUNCATE;
    if (upper == "JOIN") return Token::JOIN;
    if (upper == "INNER") return Token::INNER;
    if (upper == "LEFT") return Token::LEFT;
    if (upper == "RIGHT") return Token::RIGHT;
    if (upper == "FULL") return Token::FULL;
    if (upper == "OUTER") return Token::OUTER;
    if (upper == "CROSS") return Token::CROSS;
    if (upper == "ON") return Token::ON;
    if (upper == "BEGIN") return Token::BEGIN;
    if (upper == "COMMIT") return Token::COMMIT;
    if (upper == "ROLLBACK") return Token::ROLLBACK;
    if (upper == "INDEX") return Token::INDEX;
    if (upper == "CONSTRAINT") return Token::CONSTRAINT;
    if (upper == "PRIMARY") return Token::PRIMARY_KEY;
    if (upper == "UNIQUE") return Token::UNIQUE;
    if (upper == "NOT") return Token::NOT;
    if (upper == "NULL") return Token::NULL_KW;
    if (upper == "FOREIGN") return Token::FOREIGN_KEY;
    if (upper == "KEY") return Token::PRIMARY_KEY;
    if (upper == "CHECK") return Token::CHECK;
    if (upper == "DEFAULT") return Token::DEFAULT;
    if (upper == "ORDER") return Token::ORDER;
    if (upper == "BY") return Token::BY;
    if (upper == "ASC") return Token::ASC;
    if (upper == "DESC") return Token::DESC;
    if (upper == "LIMIT") return Token::LIMIT;
    if (upper == "OFFSET") return Token::OFFSET;
    if (upper == "AND") return Token::AND;
    if (upper == "OR") return Token::OR;
    if (upper == "TRUE") return Token::TRUE_KW;
    if (upper == "FALSE") return Token::FALSE_KW;
    if (upper == "NOW") return Token::IDENTIFIER;  // NOW() is a function, treated as identifier
    if (upper == "CURRENT_DATE") return Token::IDENTIFIER;
    if (upper == "CURRENT_TIME") return Token::IDENTIFIER;
    if (upper == "CURRENT_TIMESTAMP") return Token::IDENTIFIER;
    
    // Data types
    if (upper == "INT") return Token::INT;
    if (upper == "INTEGER") return Token::INTEGER;
    if (upper == "SMALLINT") return Token::SMALLINT;
    if (upper == "BIGINT") return Token::BIGINT;
    if (upper == "DECIMAL") return Token::DECIMAL;
    if (upper == "NUMERIC") return Token::NUMERIC;
    if (upper == "FLOAT") return Token::FLOAT;
    if (upper == "CHAR") return Token::CHAR;
    if (upper == "VARCHAR") return Token::VARCHAR;
    if (upper == "TEXT") return Token::TEXT;
    if (upper == "NCHAR") return Token::NCHAR;
    if (upper == "NVARCHAR") return Token::NVARCHAR;
    if (upper == "TINYTEXT") return Token::TINYTEXT;
    if (upper == "MEDIUMTEXT") return Token::MEDIUMTEXT;
    if (upper == "LONGTEXT") return Token::LONGTEXT;
    if (upper == "ENUM") return Token::ENUM;
    if (upper == "BOOL") return Token::BOOL;
    if (upper == "JSON") return Token::JSON;
    if (upper == "DATE") return Token::DATE;
    if (upper == "TIME") return Token::TIME;
    if (upper == "DATETIME") return Token::DATETIME;
    if (upper == "TIMESTAMP") return Token::TIMESTAMP;
    
    return Token::UNKNOWN;
}

bool Lexer::isIdentifierStart(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isIdentifierPart(char c) const {
    return isIdentifierStart(c) || (c >= '0' && c <= '9');
}

bool Lexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}
