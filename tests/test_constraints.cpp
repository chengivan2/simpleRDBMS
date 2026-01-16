#include <cassert>
#include <iostream>
#include "../src/core/column.h"
#include "../src/core/table_schema.h"
#include "../src/core/constraint.h"
#include "../src/core/data_type.h"

using namespace std;

// Test counter
int testsRun = 0;
int testsPassed = 0;
int testsFailed = 0;

void assert_test(bool condition, const QString& testName) {
    testsRun++;
    if (condition) {
        testsPassed++;
        cout << "✓ " << testName.toStdString() << endl;
    } else {
        testsFailed++;
        cout << "✗ " << testName.toStdString() << endl;
    }
}

void print_separator(const QString& section) {
    cout << "\n" << string(60, '=') << endl;
    cout << section.toStdString() << endl;
    cout << string(60, '=') << endl;
}

// Test Suite 1: Column NOT NULL Validation
void test_column_not_null() {
    print_separator("TEST SUITE 1: Column NOT NULL Validation");
    
    Column col("age", DataType::INT);
    col.setNotNull(true);
    
    // Test 1.1: NULL value should fail
    bool result1 = col.validateValue("");
    assert_test(!result1, "NOT NULL rejects empty value");
    assert_test(col.getConstraintError().contains("NULL"), "NOT NULL error message contains 'NULL'");
    
    // Test 1.2: Valid value should pass
    bool result2 = col.validateValue("25");
    assert_test(result2, "NOT NULL accepts valid integer");
    
    // Test 1.3: String NULL should fail
    bool result3 = col.validateValue("null");
    assert_test(!result3, "NOT NULL rejects 'null' string");
}

// Test Suite 2: Column PRIMARY KEY Validation
void test_column_primary_key() {
    print_separator("TEST SUITE 2: Column PRIMARY KEY Validation");
    
    Column col("id", DataType::INT);
    col.setPrimaryKey(true);
    
    // Test 2.1: PRIMARY KEY sets nullable to false
    assert_test(!col.isNullable(), "PRIMARY KEY sets nullable to false");
    
    // Test 2.2: Empty value should fail
    bool result1 = col.validateValue("");
    assert_test(!result1, "PRIMARY KEY rejects empty value");
    assert_test(col.getConstraintError().contains("PRIMARY KEY"), "Error mentions PRIMARY KEY");
    
    // Test 2.3: Valid value should pass
    bool result2 = col.validateValue("123");
    assert_test(result2, "PRIMARY KEY accepts valid value");
}

// Test Suite 3: Column Type Validation
void test_column_type_validation() {
    print_separator("TEST SUITE 3: Column Type Validation");
    
    // Test 3.1: INTEGER type validation
    Column intCol("count", DataType::INT);
    bool result1 = intCol.validateValue("42");
    assert_test(result1, "INTEGER accepts valid number");
    
    bool result2 = intCol.validateValue("abc");
    assert_test(!result2, "INTEGER rejects non-numeric string");
    assert_test(intCol.getConstraintError().contains("Invalid"), "Type error message");
    
    // Test 3.2: VARCHAR type validation
    Column varCol("name", DataType::VARCHAR);
    bool result3 = varCol.validateValue("John Doe");
    assert_test(result3, "VARCHAR accepts string");
    
    // Test 3.3: BOOLEAN type validation
    Column boolCol("active", DataType::BOOL);
    bool result4 = boolCol.validateValue("true");
    assert_test(result4, "BOOLEAN accepts 'true'");
    
    bool result5 = boolCol.validateValue("false");
    assert_test(result5, "BOOLEAN accepts 'false'");
}

// Test Suite 4: Column Length Constraints
void test_column_length_constraints() {
    print_separator("TEST SUITE 4: Column Length Constraints");
    
    Column col("email", DataType::VARCHAR);
    col.setMaxLength(50);
    
    // Test 4.1: Value within length
    bool result1 = col.validateValue("test@example.com");
    assert_test(result1, "Length constraint allows valid string");
    
    // Test 4.2: Value exceeds length
    QString longEmail(60, 'a');
    bool result2 = col.validateValue(longEmail);
    assert_test(!result2, "Length constraint rejects long string");
    assert_test(col.getConstraintError().contains("length"), "Length error message");
}

// Test Suite 5: DECIMAL Precision Validation
void test_column_decimal_precision() {
    print_separator("TEST SUITE 5: DECIMAL Precision Validation");
    
    Column col("price", DataType::DECIMAL);
    col.setDecimalPrecision(10, 2);  // 10 total digits, 2 after decimal
    
    // Test 5.1: Valid decimal within precision
    bool result1 = col.validateValue("1234.56");
    assert_test(result1, "Decimal within precision is valid");
    
    // Test 5.2: Too many total digits
    bool result2 = col.validateValue("12345678901.56");
    assert_test(!result2, "Decimal exceeding precision is rejected");
    assert_test(col.getConstraintError().contains("precision"), "Precision error message");
    
    // Test 5.3: Too many decimal places
    bool result3 = col.validateValue("123.456");
    assert_test(!result3, "Decimal with too many places is rejected");
    assert_test(col.getConstraintError().contains("scale"), "Scale error message");
}

// Test Suite 6: Column CHECK Constraint
void test_column_check_constraint() {
    print_separator("TEST SUITE 6: Column CHECK Constraint");
    
    Column col("age", DataType::INT);
    col.setCheckCondition("age >= 0");
    
    // Test 6.1: Value passes CHECK
    bool result1 = col.validateValue("25");
    assert_test(result1, "CHECK constraint passes for valid value");
    
    // Test 6.2: Value fails CHECK
    bool result2 = col.validateValue("-5");
    assert_test(!result2, "CHECK constraint fails for invalid value");
    assert_test(col.getConstraintError().contains("CHECK"), "CHECK error message");
}

// Test Suite 7: TableSchema PRIMARY KEY Management
void test_table_primary_key() {
    print_separator("TEST SUITE 7: TableSchema PRIMARY KEY Management");
    
    TableSchema schema("users");
    Column idCol("id", DataType::INT);
    schema.addColumn(idCol);
    
    // Test 7.1: Add PRIMARY KEY
    schema.addPrimaryKey(QStringList() << "id");
    assert_test(schema.hasPrimaryKey(), "TableSchema has PRIMARY KEY");
    assert_test(schema.getPrimaryKeyColumns().size() == 1, "PRIMARY KEY has 1 column");
    assert_test(schema.getPrimaryKeyColumns()[0] == "id", "PRIMARY KEY column is 'id'");
    
    // Test 7.2: Column marked as PRIMARY KEY
    const Column* col = schema.getColumn("id");
    assert_test(col != nullptr, "Column retrieved from schema");
    assert_test(col->isPrimaryKey(), "Column marked as PRIMARY KEY");
    assert_test(!col->isNullable(), "PRIMARY KEY column is not nullable");
}

// Test Suite 8: TableSchema UNIQUE Constraint
void test_table_unique_constraint() {
    print_separator("TEST SUITE 8: TableSchema UNIQUE Constraint");
    
    TableSchema schema("users");
    Column emailCol("email", DataType::VARCHAR);
    schema.addColumn(emailCol);
    
    // Test 8.1: Add UNIQUE constraint
    schema.addUnique("email_unique", QStringList() << "email");
    QMap<QString, QStringList> uniques = schema.getUniqueConstraints();
    assert_test(!uniques.isEmpty(), "UNIQUE constraint added");
    assert_test(uniques.contains("email_unique"), "UNIQUE constraint has correct name");
    
    // Test 8.2: Column marked as UNIQUE
    const Column* col = schema.getColumn("email");
    assert_test(col->isUnique(), "Column marked as UNIQUE");
}

// Test Suite 9: TableSchema FOREIGN KEY Constraint
void test_table_foreign_key() {
    print_separator("TEST SUITE 9: TableSchema FOREIGN KEY Constraint");
    
    TableSchema schema("orders");
    Column userIdCol("user_id", DataType::INT);
    schema.addColumn(userIdCol);
    
    // Test 9.1: Add FOREIGN KEY
    schema.addForeignKey("user_fk", 
                        QStringList() << "user_id",
                        "users",
                        QStringList() << "id");
    
    QMap<QString, ForeignKeyConstraint*> fks = schema.getForeignKeyConstraints();
    assert_test(!fks.isEmpty(), "FOREIGN KEY constraint added");
    assert_test(fks.contains("user_fk"), "FOREIGN KEY has correct name");
    
    // Test 9.2: Check reference info
    ForeignKeyConstraint* fk = fks["user_fk"];
    assert_test(fk->getReferencedTable() == "users", "Referenced table is 'users'");
    assert_test(fk->getReferencedColumn() == "id", "Referenced column is 'id'");
}

// Test Suite 10: TableSchema CHECK Constraint
void test_table_check_constraint() {
    print_separator("TEST SUITE 10: TableSchema CHECK Constraint");
    
    TableSchema schema("employees");
    
    // Test 10.1: Add CHECK constraint
    schema.addCheck("salary_check", "salary > 0");
    QMap<QString, CheckConstraint*> checks = schema.getCheckConstraints();
    assert_test(!checks.isEmpty(), "CHECK constraint added");
    assert_test(checks.contains("salary_check"), "CHECK constraint has correct name");
    
    // Test 10.2: Check constraint details
    CheckConstraint* check = checks["salary_check"];
    assert_test(check->getCondition() == "salary > 0", "CHECK condition stored correctly");
}

// Test Suite 11: Row Validation
void test_row_validation() {
    print_separator("TEST SUITE 11: Row Validation");
    
    TableSchema schema("users");
    
    // Setup columns
    Column idCol("id", DataType::INT);
    idCol.setPrimaryKey(true);
    schema.addColumn(idCol);
    
    Column nameCol("name", DataType::VARCHAR);
    nameCol.setNotNull(true);
    schema.addColumn(nameCol);
    
    Column ageCol("age", DataType::INT);
    ageCol.setCheckCondition("age >= 0");
    schema.addColumn(ageCol);
    
    // Test 11.1: Valid row
    QVector<QString> validRow;
    validRow << "1" << "John" << "30";
    bool result1 = schema.validateRow(validRow);
    assert_test(result1, "Valid row passes validation");
    
    // Test 11.2: Row with NULL in NOT NULL column
    QVector<QString> invalidRow1;
    invalidRow1 << "2" << "" << "25";
    bool result2 = schema.validateRow(invalidRow1);
    assert_test(!result2, "Row with NULL in NOT NULL column fails");
    
    // Test 11.3: Row with wrong column count
    QVector<QString> wrongCountRow;
    wrongCountRow << "3" << "Jane";
    bool result3 = schema.validateRow(wrongCountRow);
    assert_test(!result3, "Row with wrong column count fails");
}

// Test Suite 12: Column Validation
void test_column_in_schema_validation() {
    print_separator("TEST SUITE 12: Column Validation in Schema");
    
    TableSchema schema("users");
    
    Column ageCol("age", DataType::INT);
    ageCol.setCheckCondition("age > 0");
    schema.addColumn(ageCol);
    
    // Test 12.1: Validate existing column
    bool result1 = schema.validateColumn("age", "25");
    assert_test(result1, "Valid value passes column validation");
    
    // Test 12.2: Invalid value
    bool result2 = schema.validateColumn("age", "-5");
    assert_test(!result2, "Invalid value fails column validation");
    
    // Test 12.3: Non-existent column
    bool result3 = schema.validateColumn("salary", "5000");
    assert_test(!result3, "Non-existent column fails validation");
    assert_test(schema.getValidationError().contains("not found"), "Error indicates column not found");
}

// Test Suite 13: JSON Serialization
void test_json_serialization() {
    print_separator("TEST SUITE 13: JSON Serialization");
    
    TableSchema schema("users");
    schema.setDescription("User management table");
    schema.setRowCount(42);
    
    Column idCol("id", DataType::INT);
    idCol.setPrimaryKey(true);
    schema.addColumn(idCol);
    
    Column nameCol("name", DataType::VARCHAR);
    nameCol.setNotNull(true);
    nameCol.setDescription("User full name");
    schema.addColumn(nameCol);
    
    schema.addPrimaryKey(QStringList() << "id");
    
    // Test 13.1: Serialize to JSON
    QString json = schema.toJson();
    assert_test(!json.isEmpty(), "Schema serializes to non-empty JSON");
    assert_test(json.contains("users"), "JSON contains table name");
    assert_test(json.contains("id"), "JSON contains column 'id'");
    assert_test(json.contains("primaryKey"), "JSON contains PRIMARY KEY");
}

// Test Suite 14: JSON Deserialization
void test_json_deserialization() {
    print_separator("TEST SUITE 14: JSON Deserialization");
    
    // Create schema
    TableSchema schema("users");
    Column idCol("id", DataType::INT);
    idCol.setPrimaryKey(true);
    schema.addColumn(idCol);
    
    Column nameCol("name", DataType::VARCHAR);
    schema.addColumn(nameCol);
    
    schema.addPrimaryKey(QStringList() << "id");
    schema.setDescription("Test users table");
    
    // Serialize
    QString json = schema.toJson();
    
    // Test 14.1: Deserialize
    TableSchema* restored = TableSchema::fromJson(json);
    assert_test(restored != nullptr, "Schema deserialized successfully");
    
    if (restored) {
        // Test 14.2: Check table name
        assert_test(restored->getTableName() == "users", "Deserialized table name correct");
        
        // Test 14.3: Check columns
        assert_test(restored->getColumnCount() == 2, "Deserialized column count correct");
        
        // Test 14.4: Check PRIMARY KEY
        assert_test(restored->hasPrimaryKey(), "Deserialized PRIMARY KEY exists");
        assert_test(restored->getPrimaryKeyColumns().contains("id"), "Deserialized PRIMARY KEY correct");
        
        // Test 14.5: Check description
        assert_test(restored->getDescription() == "Test users table", "Deserialized description correct");
        
        delete restored;
    }
}

// Test Suite 15: Error Reporting
void test_error_reporting() {
    print_separator("TEST SUITE 15: Error Reporting");
    
    Column col("salary", DataType::DECIMAL);
    col.setDecimalPrecision(8, 2);
    
    // Test 15.1: Precision error
    col.validateValue("123456789.99");
    QString error1 = col.getConstraintError();
    assert_test(!error1.isEmpty(), "Error message is not empty");
    assert_test(error1.contains("precision"), "Error mentions precision");
    
    // Test 15.2: Type error
    Column intCol("age", DataType::INT);
    intCol.validateValue("not_a_number");
    QString error2 = intCol.getConstraintError();
    assert_test(error2.contains("Invalid"), "Type error is specific");
    
    // Test 15.3: NOT NULL error
    Column notNullCol("email", DataType::VARCHAR);
    notNullCol.setNotNull(true);
    notNullCol.validateValue("");
    QString error3 = notNullCol.getConstraintError();
    assert_test(error3.contains("NULL"), "NOT NULL error is specific");
}

// Main test runner
int main() {
    cout << "\n" << string(60, '#') << endl;
    cout << "# SimpleRDBMS Phase 4 - Constraint System Test Suite #" << endl;
    cout << string(60, '#') << endl;
    
    // Run all test suites
    test_column_not_null();
    test_column_primary_key();
    test_column_type_validation();
    test_column_length_constraints();
    test_column_decimal_precision();
    test_column_check_constraint();
    test_table_primary_key();
    test_table_unique_constraint();
    test_table_foreign_key();
    test_table_check_constraint();
    test_row_validation();
    test_column_in_schema_validation();
    test_json_serialization();
    test_json_deserialization();
    test_error_reporting();
    
    // Print summary
    print_separator("TEST SUMMARY");
    cout << "Tests Run:    " << testsRun << endl;
    cout << "Tests Passed: " << testsPassed << endl;
    cout << "Tests Failed: " << testsFailed << endl;
    
    if (testsFailed == 0) {
        cout << "\n✓ ALL TESTS PASSED!" << endl;
    } else {
        cout << "\n✗ " << testsFailed << " test(s) failed" << endl;
    }
    
    cout << string(60, '#') << endl << endl;
    
    return testsFailed == 0 ? 0 : 1;
}
