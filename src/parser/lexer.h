#pragma once

#include "token.h"
#include <QString>
#include <QVector>

/**
 * @brief SQL Lexer - Tokenizes SQL input
 */
class Lexer {
public:
    explicit Lexer(const QString& input);
    
    QVector<Token> tokenize();
    Token nextToken();
    
private:
    QString input;
    int position;
    int line;
    int column;
    
    char current() const;
    char peek() const;
    void advance();
    void skipWhitespace();
    void skipComment();
    
    Token readIdentifierOrKeyword();
    Token readNumber();
    Token readString(char quote);
    Token makeToken(Token::Type type, const QString& value);
    
    Token::Type keywordToTokenType(const QString& keyword) const;
    bool isIdentifierStart(char c) const;
    bool isIdentifierPart(char c) const;
    bool isDigit(char c) const;
};
