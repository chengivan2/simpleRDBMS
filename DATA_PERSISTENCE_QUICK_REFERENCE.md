# Data Persistence - Quick Reference Guide

## Overview
The Simple RDBMS now automatically persists all table schemas and data to disk using JSON format. Tables are automatically loaded when the application starts.

## How It Works

### Automatic Persistence
When you use the TableManager:
```cpp
TableManager manager("./data");  // Data directory

// Create a table
auto schema = std::make_shared<TableSchema>("users");
// ... add columns ...
manager.addTable(schema);

// Insert data
QVector<QString> row;
row << "1" << "Alice" << "alice@example.com";
manager.insertRow("users", row);

// Save to disk (automatic with saveAllTables())
manager.saveAllTables();
```

### Automatic Loading
```cpp
TableManager manager("./data");  // Automatically loads existing tables
// All tables from previous sessions are now available
auto schema = manager.getTable("users");  // Returns previously saved table
```

## Data Files

Each table creates two JSON files in the data directory:

### 1. Schema File: `<tableName>_schema.json`
Contains:
- Column definitions (name, type, constraints)
- Primary key constraints
- Unique constraints
- Foreign key constraints
- Check constraints
- Table metadata

**Example**:
```json
{
    "name": "users",
    "columns": [
        {
            "name": "id",
            "type": "INT",
            "primaryKey": true,
            "nullable": false
        },
        {
            "name": "name",
            "type": "VARCHAR",
            "nullable": true
        }
    ],
    "constraints": {
        "primaryKey": ["id"]
    }
}
```

### 2. Data File: `<tableName>.json`
Contains:
- All row data as JSON objects with named columns
- Row count metadata
- Table name reference

**Example**:
```json
{
    "tableName": "users",
    "rowCount": 2,
    "rows": [
        {"id": "1", "name": "Alice"},
        {"id": "2", "name": "Bob"}
    ]
}
```

## File Organization

```
your_data_directory/
├── users_schema.json        # Schema for users table
├── users.json               # Data for users table
├── products_schema.json     # Schema for products table
├── products.json            # Data for products table
└── ...                       # Other tables
```

## API Usage

### Creating and Persisting Tables

```cpp
// Create table
auto schema = std::make_shared<TableSchema>("customers");
Column idCol("id", DataType::INT);
schema->addColumn(idCol);

Column nameCol("name", DataType::VARCHAR);
nameCol.setMaxLength(100);
schema->addColumn(nameCol);

// Add constraints
schema->addPrimaryKey(QStringList() << "id");

// Add to manager (saves schema)
manager.addTable(schema);

// Insert data (will be persisted)
QVector<QString> row;
row << "1" << "John";
manager.insertRow("customers", row);

// Save to disk
manager.saveAllTables();
```

### Loading Persisted Tables

```cpp
// New TableManager instance automatically loads tables
TableManager manager("./data");

// Access previously saved table
auto schema = manager.getTable("customers");

// Retrieve all data
auto allRows = manager.selectAll("customers");
```

### Important Methods

| Method | Purpose |
|--------|---------|
| `loadAllTables()` | Load all tables from disk |
| `saveAllTables()` | Save all tables to disk |
| `addTable(schema)` | Add table (auto-saves schema) |
| `insertRow(table, row)` | Insert row (auto-saves) |
| `updateRow(table, id, row)` | Update row (auto-saves) |
| `deleteRow(table, id)` | Delete row (auto-saves) |
| `selectAll(table)` | Get all rows from table |

## Key Features

✓ **Automatic**: Changes are saved automatically
✓ **Transparent**: No manual save/load code needed
✓ **Reliable**: Full constraint validation before save
✓ **Human-Readable**: JSON format is easy to inspect
✓ **Portable**: Works across different systems

## Best Practices

1. **Specify Data Directory**:
   ```cpp
   TableManager manager("./my_data");  // Use consistent path
   ```

2. **Save After Operations**:
   ```cpp
   manager.insertRow("users", row);
   manager.saveAllTables();  // Ensure persistence
   ```

3. **Check for Errors**:
   ```cpp
   OperationResult result = manager.insertRow("users", row);
   if (!result.success) {
       Logger::instance().error(result.errorMessage);
   }
   ```

4. **Verify Schema Constraints**:
   ```cpp
   QString error;
   if (!manager.validateRow("users", row, error)) {
       Logger::instance().error("Validation failed: " + error);
   }
   ```

## Testing

Run the included tests to verify persistence:

```bash
# Test basic storage engine
./build/bin/test_storage.exe

# Test integration across sessions
./build/bin/test_persistence_integration.exe

# Test all constraints with persistence
./build/bin/test_constraints.exe
```

## Troubleshooting

### Tables not loading
- Check data directory path is correct
- Verify JSON files exist in the directory
- Check file permissions

### Data not saving
- Call `manager.saveAllTables()` explicitly
- Check data directory exists and is writable
- Check logs for specific error messages

### JSON format issues
- Manual editing of JSON files is possible
- Must maintain column order in schema
- Keep table name consistent between schema and data files

## Example: Complete Workflow

```cpp
#include "table_manager.h"

int main() {
    // Initialize manager with data directory
    TableManager manager("./my_data");
    
    // Create a new table (if not already persisted)
    if (!manager.tableExists("employees")) {
        auto schema = std::make_shared<TableSchema>("employees");
        
        Column empId("emp_id", DataType::INT);
        schema->addColumn(empId);
        
        Column name("name", DataType::VARCHAR);
        name.setMaxLength(100);
        schema->addColumn(name);
        
        Column salary("salary", DataType::DECIMAL);
        schema->addColumn(salary);
        
        schema->addPrimaryKey(QStringList() << "emp_id");
        
        manager.addTable(schema);
    }
    
    // Insert employees
    QVector<QString> emp1;
    emp1 << "1" << "Alice" << "75000";
    manager.insertRow("employees", emp1);
    
    QVector<QString> emp2;
    emp2 << "2" << "Bob" << "65000";
    manager.insertRow("employees", emp2);
    
    // Save to disk
    manager.saveAllTables();
    
    // Query data
    auto employees = manager.selectAll("employees");
    
    return 0;
}
```

## Additional Resources

- See `PHASE_5_DATA_PERSISTENCE_REPORT.md` for detailed implementation notes
- Check `src/storage/storage_engine.h` for StorageEngine API
- Review `src/core/table_manager.h` for TableManager API
