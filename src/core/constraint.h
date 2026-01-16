#pragma once

#include <QString>

/**
 * @brief Base class for all constraints
 */
class Constraint {
public:
    enum Type {
        PRIMARY_KEY,    // Unique identifier for row
        UNIQUE,         // Column values must be unique
        NOT_NULL,       // Column cannot be NULL
        FOREIGN_KEY,    // References another table
        CHECK          // Custom condition validation
    };
    
    virtual ~Constraint() = default;
    
    virtual Type getType() const = 0;
    virtual QString getName() const = 0;
    virtual QString getDescription() const;
    
    // Validation
    virtual bool validate(const QString& value) const = 0;
    virtual QString getValidationError() const { return lastError; }
    
protected:
    mutable QString lastError;
};

/**
 * @brief PRIMARY KEY constraint - ensures unique non-NULL values
 */
class PrimaryKeyConstraint : public Constraint {
public:
    Type getType() const override { return PRIMARY_KEY; }
    QString getName() const override { return "PRIMARY KEY"; }
    bool validate(const QString& value) const override;
};

/**
 * @brief UNIQUE constraint - ensures all non-NULL values are unique
 */
class UniqueConstraint : public Constraint {
public:
    Type getType() const override { return UNIQUE; }
    QString getName() const override { return "UNIQUE"; }
    bool validate(const QString& value) const override;
};

/**
 * @brief NOT NULL constraint - disallows NULL values
 */
class NotNullConstraint : public Constraint {
public:
    Type getType() const override { return NOT_NULL; }
    QString getName() const override { return "NOT NULL"; }
    bool validate(const QString& value) const override;
};

/**
 * @brief FOREIGN KEY constraint - references another table's column
 */
class ForeignKeyConstraint : public Constraint {
public:
    ForeignKeyConstraint(const QString& refTable, const QString& refCol)
        : referencedTable(refTable), referencedColumn(refCol) {}
    
    Type getType() const override { return FOREIGN_KEY; }
    QString getName() const override { return "FOREIGN KEY"; }
    bool validate(const QString& value) const override;
    
    // Reference information
    QString getReferencedTable() const { return referencedTable; }
    QString getReferencedColumn() const { return referencedColumn; }
    QString getFullReference() const;
    
    // CASCADE options
    enum CascadeAction { RESTRICT, SET_NULL, SET_DEFAULT, CASCADE };
    CascadeAction getOnDelete() const { return onDelete; }
    CascadeAction getOnUpdate() const { return onUpdate; }
    void setOnDelete(CascadeAction action) { onDelete = action; }
    void setOnUpdate(CascadeAction action) { onUpdate = action; }
    
private:
    QString referencedTable;
    QString referencedColumn;
    CascadeAction onDelete = RESTRICT;
    CascadeAction onUpdate = RESTRICT;
};

/**
 * @brief CHECK constraint - validates value against condition
 */
class CheckConstraint : public Constraint {
public:
    explicit CheckConstraint(const QString& condition)
        : condition(condition), constraintName("CHECK") {}
    
    Type getType() const override { return CHECK; }
    QString getName() const override { return constraintName; }
    bool validate(const QString& value) const override;
    
    // Condition details
    QString getCondition() const { return condition; }
    void setName(const QString& name) { constraintName = name; }
    
    // Expression evaluation helper
    static bool evaluateExpression(const QString& condition, const QString& value);
    
private:
    QString condition;
    QString constraintName;
};

/**
 * @brief Constraint manager utility
 */
class ConstraintManager {
public:
    static QString getConstraintDescription(Constraint::Type type);
    static QString getConstraintDescription(const Constraint* constraint);
    static bool validateConstraint(const Constraint* constraint, const QString& value);
    static QString getValidationErrorMessage(Constraint::Type type, const QString& value);
};
