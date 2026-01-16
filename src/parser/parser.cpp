#include "parser.h"
#include "../utils/logger.h"
#include <QDebug>

Parser::Parser(const QVector<Token>& tokens) 
    : tokens(tokens), position(0) {
}

std::unique_ptr<ASTNode> Parser::parse() {
    return parseStatement();
}

Token Parser::current() const {
    if (position >= tokens.size()) {
        return Token{Token::END_OF_FILE, "", 0, 0};
    }
    return tokens[position];
}

Token Parser::peek() const {
    if (position + 1 >= tokens.size()) {
        return Token{Token::END_OF_FILE, "", 0, 0};
    }
    return tokens[position + 1];
}

void Parser::advance() {
    if (position < tokens.size()) {
        position++;
    }
}

bool Parser::match(Token::Type type) {
    if (current().type == type) {
        advance();
        return true;
    }
    return false;
}

Token Parser::expect(Token::Type type) {
    Token token = current();
    if (token.type != type) {
        std::string msg = "Expected " + tokenTypeToString(type) + 
                         " but got " + tokenTypeToString(token.type) + 
                         " (\"" + token.value.toStdString() + "\") at line " + 
                         std::to_string(token.line) + " col " + std::to_string(token.column);
        throw std::runtime_error(msg);
    }
    advance();
    return token;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    Token token = current();
    
    switch (token.type) {
        case Token::SELECT:
            return parseSelectStatement();
        case Token::INSERT:
            return parseInsertStatement();
        case Token::UPDATE:
            return parseUpdateStatement();
        case Token::DELETE:
            return parseDeleteStatement();
        case Token::CREATE:
            advance();
            if (current().type == Token::TABLE) {
                return parseCreateTableStatement();
            } else if (current().type == Token::INDEX) {
                return parseCreateIndexStatement();
            }
            error("Expected TABLE or INDEX after CREATE");
            break;
        case Token::ALTER:
            return parseAlterTableStatement();
        case Token::DROP:
            return parseDropTableStatement();
        case Token::BEGIN:
            return parseBeginStatement();
        case Token::COMMIT:
            return parseCommitStatement();
        case Token::ROLLBACK:
            return parseRollbackStatement();
        default:
            error("Unknown statement");
    }
    return std::make_unique<ASTNode>();
}

std::unique_ptr<SelectStatement> Parser::parseSelectStatement() {
    auto stmt = std::make_unique<SelectStatement>();
    
    expect(Token::SELECT);
    
    // Parse column list
    if (current().type == Token::ASTERISK) {
        stmt->columns.append("*");
        advance();
    } else {
        stmt->columns = parseColumnList();
    }
    
    // Parse FROM clause
    if (match(Token::FROM)) {
        stmt->fromTable = parseIdentifier();
    }
    
    // Parse WHERE clause
    if (current().type == Token::WHERE) {
        stmt->whereClause = parseWhereClause();
    }
    
    // Parse ORDER BY clause
    if (current().type == Token::ORDER) {
        stmt->orderBy = parseOrderByClause();
    }
    
    // Parse LIMIT clause
    if (match(Token::LIMIT)) {
        stmt->limit = parseLimit();
    }
    
    return stmt;
}

std::unique_ptr<InsertStatement> Parser::parseInsertStatement() {
    auto stmt = std::make_unique<InsertStatement>();
    
    expect(Token::INSERT);
    expect(Token::INTO);
    
    stmt->tableName = parseIdentifier();
    
    // Parse column list (optional)
    if (current().type == Token::LPAREN) {
        advance();
        stmt->columns = parseColumnList();
        expect(Token::RPAREN);
    }
    
    // Parse VALUES keyword
    expect(Token::VALUES);
    
    // Parse value tuples
    while (current().type == Token::LPAREN) {
        advance();
        QStringList values;
        values.append(parseExpression());
        
        while (match(Token::COMMA)) {
            values.append(parseExpression());
        }
        
        expect(Token::RPAREN);
        stmt->values.append(values);
        
        if (!match(Token::COMMA)) {
            break;
        }
    }
    
    return stmt;
}

std::unique_ptr<UpdateStatement> Parser::parseUpdateStatement() {
    auto stmt = std::make_unique<UpdateStatement>();
    
    expect(Token::UPDATE);
    
    stmt->tableName = parseIdentifier();
    
    expect(Token::SET);
    
    // Parse column = value pairs
    stmt->columns.append(parseIdentifier());
    expect(Token::EQUALS);
    stmt->values.append(parseExpression());
    
    while (match(Token::COMMA)) {
        stmt->columns.append(parseIdentifier());
        expect(Token::EQUALS);
        stmt->values.append(parseExpression());
    }
    
    // Parse WHERE clause (optional)
    if (current().type == Token::WHERE) {
        stmt->whereClause = parseWhereClause();
    }
    
    return stmt;
}

std::unique_ptr<DeleteStatement> Parser::parseDeleteStatement() {
    auto stmt = std::make_unique<DeleteStatement>();
    
    expect(Token::DELETE);
    expect(Token::FROM);
    
    stmt->tableName = parseIdentifier();
    
    // Parse WHERE clause (optional but recommended)
    if (current().type == Token::WHERE) {
        stmt->whereClause = parseWhereClause();
    }
    
    return stmt;
}

std::unique_ptr<CreateTableStatement> Parser::parseCreateTableStatement() {
    auto stmt = std::make_unique<CreateTableStatement>();
    
    // Note: CREATE token was already consumed by parseStatement()
    expect(Token::TABLE);
    
    stmt->tableName = parseIdentifier();
    
    Logger::instance().debug(QString("Parsed table name: %1, next token type: %2").arg(stmt->tableName).arg(current().type));
    
    expect(Token::LPAREN);
    
    Logger::instance().debug(QString("After LPAREN, next token: %1 (%2)").arg(current().value).arg(current().type));
    
    // Parse column definitions
    while (current().type != Token::RPAREN && current().type != Token::END_OF_FILE) {
        // Column name
        QString colName = parseIdentifier();
        
        // Data type - can be a keyword (INT, VARCHAR, etc.) or identifier
        QString dataType;
        Token typeToken = current();
        if (typeToken.type == Token::IDENTIFIER || 
            (typeToken.type >= Token::INT && typeToken.type <= Token::TIMESTAMP)) {
            // Accept both identifier and data type keywords
            dataType = typeToken.value;
            advance();
            
            // Handle data type parameters like VARCHAR(255), DECIMAL(10, 2), etc.
            if (current().type == Token::LPAREN) {
                advance();  // consume LPAREN
                // Skip tokens until we find the closing RPAREN for the data type
                int parenDepth = 1;
                while (parenDepth > 0 && current().type != Token::END_OF_FILE) {
                    if (current().type == Token::LPAREN) {
                        parenDepth++;
                    } else if (current().type == Token::RPAREN) {
                        parenDepth--;
                    }
                    advance();
                }
            }
        } else {
            throw std::runtime_error(
                "Expected data type but got " + tokenTypeToString(typeToken.type) + 
                " (\"" + typeToken.value.toStdString() + "\") at line " + 
                std::to_string(typeToken.line) + " col " + std::to_string(typeToken.column)
            );
        }
        
        ColumnDefinition colDef(colName, dataType);
        
        // Parse constraints
        while (current().type != Token::COMMA && current().type != Token::RPAREN && current().type != Token::END_OF_FILE) {
            if (match(Token::PRIMARY_KEY)) {
                colDef.primaryKey = true;
                stmt->primaryKeys.append(colName);
            } else if (match(Token::UNIQUE)) {
                colDef.unique = true;
                stmt->uniqueColumns.append(colName);
            } else if (match(Token::NOT)) {
                // Match NOT NULL
                if (match(Token::NULL_KW)) {
                    colDef.nullable = false;
                } else {
                    // Unexpected token after NOT
                    throw std::runtime_error("Expected NULL after NOT");
                }
            } else if (match(Token::CHECK)) {
                // Parse CHECK constraint: CHECK (expression)
                expect(Token::LPAREN);
                int parenDepth = 1;
                while (parenDepth > 0 && current().type != Token::END_OF_FILE) {
                    if (current().type == Token::LPAREN) {
                        parenDepth++;
                        advance();
                    } else if (current().type == Token::RPAREN) {
                        parenDepth--;
                        if (parenDepth > 0) {
                            advance();
                        }
                    } else {
                        advance();
                    }
                }
                expect(Token::RPAREN);
            } else if (current().type == Token::DEFAULT) {
                advance();
                // Handle DEFAULT values: DEFAULT <value> or DEFAULT <function()>
                // Support DEFAULT NOW(), DEFAULT CURRENT_DATE, literals, etc.
                if (current().type == Token::IDENTIFIER) {
                    // Could be NOW(), CURRENT_DATE, etc.
                    QString defaultFunc = current().value;
                    advance();
                    // Check for function call
                    if (current().type == Token::LPAREN) {
                        defaultFunc += "(";
                        advance();  // Skip LPAREN
                        int parenDepth = 1;
                        while (parenDepth > 0 && current().type != Token::END_OF_FILE) {
                            if (current().type == Token::LPAREN) {
                                parenDepth++;
                                defaultFunc += "(";
                            } else if (current().type == Token::RPAREN) {
                                parenDepth--;
                                if (parenDepth > 0) {
                                    defaultFunc += ")";
                                }
                            } else {
                                // For simplicity, just add the token value
                                if (!defaultFunc.isEmpty() && !defaultFunc.endsWith("(")) {
                                    defaultFunc += " ";
                                }
                                defaultFunc += current().value;
                            }
                            advance();
                        }
                        defaultFunc += ")";
                    }
                    colDef.defaultValue = defaultFunc;
                } else if (current().type == Token::STRING) {
                    // String literal default
                    colDef.defaultValue = current().value;
                    advance();
                } else if (current().type == Token::NUMBER) {
                    // Numeric literal default
                    colDef.defaultValue = current().value;
                    advance();
                } else {
                    // Other default literal value
                    while (current().type != Token::COMMA && 
                           current().type != Token::RPAREN && 
                           current().type != Token::END_OF_FILE) {
                        colDef.defaultValue += current().value + " ";
                        advance();
                    }
                    colDef.defaultValue = colDef.defaultValue.trimmed();
                }
            } else if (current().type == Token::FOREIGN_KEY || current().type == Token::CONSTRAINT) {
                // Skip FOREIGN KEY or CONSTRAINT for now
                while (current().type != Token::COMMA && 
                       current().type != Token::RPAREN && 
                       current().type != Token::END_OF_FILE) {
                    advance();
                }
            } else {
                // Check if we've hit a comma or rparen, break the constraint loop
                if (current().type == Token::COMMA || current().type == Token::RPAREN) {
                    break;
                }
                // Unknown token in constraint, skip it
                advance();
            }
        }
        
        stmt->columns.append(colDef);
        
        if (!match(Token::COMMA)) {
            break;
        }
    }
    
    expect(Token::RPAREN);
    
    Logger::instance().info(QString("Parsed %1 columns for table").arg(stmt->columns.size()));
    return stmt;
}

std::unique_ptr<AlterTableStatement> Parser::parseAlterTableStatement() {
    auto stmt = std::make_unique<AlterTableStatement>();
    
    expect(Token::ALTER);
    expect(Token::TABLE);
    
    stmt->tableName = parseIdentifier();
    
    // Parse ALTER action (ADD, DROP, MODIFY, etc.)
    if (match(Token::ADD)) {
        stmt->alterAction = "ADD";
    } else if (match(Token::DROP)) {
        stmt->alterAction = "DROP";
    } else if (match(Token::MODIFY)) {
        stmt->alterAction = "MODIFY";
    } else {
        error("Expected ADD, DROP, or MODIFY in ALTER TABLE");
    }
    
    stmt->columnName = parseIdentifier();
    
    // Skip remaining tokens for action details
    while (current().type != Token::END_OF_FILE && 
           current().type != Token::SEMICOLON) {
        advance();
    }
    
    return stmt;
}

std::unique_ptr<DropTableStatement> Parser::parseDropTableStatement() {
    auto stmt = std::make_unique<DropTableStatement>();
    
    expect(Token::DROP);
    expect(Token::TABLE);
    
    stmt->tableName = parseIdentifier();
    
    return stmt;
}

std::unique_ptr<CreateIndexStatement> Parser::parseCreateIndexStatement() {
    auto stmt = std::make_unique<CreateIndexStatement>();
    
    expect(Token::CREATE);
    expect(Token::INDEX);
    
    stmt->indexName = parseIdentifier();
    
    expect(Token::ON);
    
    stmt->tableName = parseIdentifier();
    
    expect(Token::LPAREN);
    stmt->columns = parseColumnList();
    expect(Token::RPAREN);
    
    return stmt;
}

std::unique_ptr<BeginStatement> Parser::parseBeginStatement() {
    expect(Token::BEGIN);
    return std::make_unique<BeginStatement>();
}

std::unique_ptr<CommitStatement> Parser::parseCommitStatement() {
    expect(Token::COMMIT);
    return std::make_unique<CommitStatement>();
}

std::unique_ptr<RollbackStatement> Parser::parseRollbackStatement() {
    expect(Token::ROLLBACK);
    return std::make_unique<RollbackStatement>();
}

QString Parser::parseIdentifier() {
    Token token = current();
    if (token.type != Token::IDENTIFIER) {
        std::string msg = "Expected identifier but got " + tokenTypeToString(token.type) + 
                         " (\"" + token.value.toStdString() + "\") at line " + 
                         std::to_string(token.line) + " col " + std::to_string(token.column);
        error(QString::fromStdString(msg));
        return "";
    }
    advance();
    return token.value;
}

QString Parser::parseExpression() {
    QString expr;
    
    // Handle simple expressions: literals, identifiers, function calls
    Token token = current();
    
    if (token.type == Token::STRING) {
        expr = token.value;
        advance();
    } else if (token.type == Token::NUMBER) {
        expr = token.value;
        advance();
    } else if (token.type == Token::IDENTIFIER) {
        expr = token.value;
        advance();
        
        // Handle function calls: name(args)
        if (current().type == Token::LPAREN) {
            expr += "(";
            advance();
            while (current().type != Token::RPAREN && 
                   current().type != Token::END_OF_FILE) {
                expr += current().value + " ";
                advance();
            }
            expect(Token::RPAREN);
            expr += ")";
        }
    } else if (token.type == Token::NULL_KW) {
        expr = "NULL";
        advance();
    } else if (token.type == Token::TRUE_KW) {
        expr = "TRUE";
        advance();
    } else if (token.type == Token::FALSE_KW) {
        expr = "FALSE";
        advance();
    } else {
        error("Expected expression");
    }
    
    // Handle operators and continuation
    while (current().type == Token::PLUS || current().type == Token::MINUS ||
           current().type == Token::MULTIPLY || current().type == Token::DIVIDE ||
           current().type == Token::MODULO) {
        expr += " " + current().value + " ";
        advance();
        expr += parseExpression();
        break; // Simple: parse one more expression
    }
    
    return expr;
}

QStringList Parser::parseColumnList() {
    QStringList columns;
    
    columns.append(parseIdentifier());
    
    while (match(Token::COMMA)) {
        columns.append(parseIdentifier());
    }
    
    return columns;
}

QString Parser::parseWhereClause() {
    QString clause;
    
    expect(Token::WHERE);
    
    // Collect tokens until ORDER BY, LIMIT, or statement end
    while (current().type != Token::ORDER && 
           current().type != Token::LIMIT &&
           current().type != Token::SEMICOLON &&
           current().type != Token::END_OF_FILE &&
           current().type != Token::RPAREN) {
        clause += current().value + " ";
        advance();
    }
    
    return clause.trimmed();
}

QString Parser::parseOrderByClause() {
    QString clause;
    
    expect(Token::ORDER);
    expect(Token::BY);
    
    clause = parseIdentifier();
    
    if (match(Token::ASC)) {
        clause += " ASC";
    } else if (match(Token::DESC)) {
        clause += " DESC";
    }
    
    while (match(Token::COMMA)) {
        clause += ", " + parseIdentifier();
        if (match(Token::ASC)) {
            clause += " ASC";
        } else if (match(Token::DESC)) {
            clause += " DESC";
        }
    }
    
    return clause;
}

int Parser::parseLimit() {
    Token token = current();
    if (token.type != Token::NUMBER) {
        error("Expected number after LIMIT");
        return 0;
    }
    int limit = token.value.toInt();
    advance();
    return limit;
}

void Parser::error(const QString& message) {
    Token token = current();
    QString errorMsg = QString("Parse error at line %1, column %2: %3")
        .arg(token.line)
        .arg(token.column)
        .arg(message);
    qWarning() << errorMsg;
    throw std::runtime_error(errorMsg.toStdString());
}

void Parser::skipUntil(Token::Type type) {
    while (current().type != type && current().type != Token::END_OF_FILE) {
        advance();
    }
}
std::string Parser::tokenTypeToString(Token::Type type) const {
    switch(type) {
        case Token::SELECT: return "SELECT";
        case Token::FROM: return "FROM";
        case Token::WHERE: return "WHERE";
        case Token::INSERT: return "INSERT";
        case Token::INTO: return "INTO";
        case Token::VALUES: return "VALUES";
        case Token::UPDATE: return "UPDATE";
        case Token::SET: return "SET";
        case Token::DELETE: return "DELETE";
        case Token::CREATE: return "CREATE";
        case Token::TABLE: return "TABLE";
        case Token::PRIMARY_KEY: return "PRIMARY KEY";
        case Token::UNIQUE: return "UNIQUE";
        case Token::NOT_NULL: return "NOT NULL";
        case Token::CHECK: return "CHECK";
        case Token::DEFAULT: return "DEFAULT";
        case Token::LPAREN: return "(";
        case Token::RPAREN: return ")";
        case Token::COMMA: return ",";
        case Token::SEMICOLON: return ";";
        case Token::IDENTIFIER: return "identifier";
        case Token::NUMBER: return "number";
        case Token::STRING: return "string";
        case Token::INT: return "INT";
        case Token::VARCHAR: return "VARCHAR";
        case Token::DATE: return "DATE";
        case Token::END_OF_FILE: return "end of file";
        default: return "unknown";
    }
}