#pragma once

#include "token.h"
#include "ast_nodes.h"
#include <QString>
#include <QVector>
#include <memory>

/**
 * @brief SQL Parser - Parses tokenized SQL into AST
 */
class Parser {
public:
    explicit Parser(const QVector<Token>& tokens);
    
    std::unique_ptr<ASTNode> parse();
    
private:
    QVector<Token> tokens;
    int position;
    
    // Token navigation
    Token current() const;
    Token peek() const;
    void advance();
    bool match(Token::Type type);
    Token expect(Token::Type type);
    std::string tokenTypeToString(Token::Type type) const;
    
    // Statement parsing
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<SelectStatement> parseSelectStatement();
    std::unique_ptr<InsertStatement> parseInsertStatement();
    std::unique_ptr<UpdateStatement> parseUpdateStatement();
    std::unique_ptr<DeleteStatement> parseDeleteStatement();
    std::unique_ptr<CreateTableStatement> parseCreateTableStatement();
    std::unique_ptr<AlterTableStatement> parseAlterTableStatement();
    std::unique_ptr<DropTableStatement> parseDropTableStatement();
    std::unique_ptr<CreateIndexStatement> parseCreateIndexStatement();
    std::unique_ptr<BeginStatement> parseBeginStatement();
    std::unique_ptr<CommitStatement> parseCommitStatement();
    std::unique_ptr<RollbackStatement> parseRollbackStatement();
    
    // Helper parsing methods
    QString parseIdentifier();
    QString parseExpression();
    QStringList parseColumnList();
    QString parseWhereClause();
    QString parseOrderByClause();
    int parseLimit();
    
    // Error handling
    void error(const QString& message);
    void skipUntil(Token::Type type);
};
