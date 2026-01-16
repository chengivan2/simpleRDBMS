# SimpleRDBMS

A simple yet feature-rich Relational Database Management System (RDBMS) built entirely in C++ with Qt. Execute SQL queries, manage tables with a modern UI, and persist data to disk.

---

## Table of Contents

1. [User Guide](#user-guide)
2. [Developer Guide](#developer-guide)
3. [Features](#features)
4. [Requirements](#requirements)
5. [License](#license)

---

## User Guide

### Getting Started

#### Installation

1. Download the latest SimpleRDBMS executable from the releases page.
2. Extract the archive to your desired location.
3. Run `SimpleRDBMS.exe` (Windows), `SimpleRDBMS` (Linux), or `SimpleRDBMS.app` (macOS).

#### First Steps

When you launch SimpleRDBMS, you'll see the main window with two tabs: **SQL Editor** and **Table Manager**.

### SQL Editor Page

The SQL Editor allows you to write and execute SQL queries.

**Features:**

- Write SQL queries in the text editor at the top
- Click "Execute" to run your query
- View results in the table below
- Query history dropdown for quick access to previous queries
- Execution time and row count display

**Supported Operations:**

#### CREATE TABLE

```sql
CREATE TABLE users (
    id INT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    email VARCHAR(100) UNIQUE,
    age SMALLINT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);
```

#### INSERT

```sql
INSERT INTO users (id, name, email, age)
VALUES (1, 'John Doe', 'john@example.com', 30);
```

#### SELECT

```sql
SELECT * FROM users;
SELECT id, name FROM users WHERE age > 25 LIMIT 10;
SELECT name FROM users ORDER BY age DESC;
```

#### UPDATE

```sql
UPDATE users SET age = 31 WHERE id = 1;
```

#### DELETE

```sql
DELETE FROM users WHERE age < 18;
```

#### ALTER TABLE

```sql
ALTER TABLE users ADD COLUMN phone VARCHAR(20);
ALTER TABLE users MODIFY age BIGINT;
ALTER TABLE users DROP COLUMN phone;
```

#### DROP TABLE

```sql
DROP TABLE users;
```

#### JOIN Operations

```sql
SELECT u.name, o.order_id
FROM users u
INNER JOIN orders o ON u.id = o.user_id;
```

#### Transactions

```sql
BEGIN;
INSERT INTO users (id, name) VALUES (2, 'Jane Doe');
COMMIT;
```

### Table Manager Page

The Table Manager allows you to browse and inspect your tables.

**Features:**

- **Left Sidebar:** List of all created tables
- **Right Panel:** Detailed view of the selected table
  - Column names and types
  - Table schema and constraints
  - Data browsing with pagination
  - Search/filter functionality
- **Create Table Button:** Opens dialog to create new tables
- **Delete Table Button:** Delete selected table (with confirmation)

### Data Types Supported

**Numeric Types:**

- TINYINT, SMALLINT, INT/INTEGER, BIGINT
- DECIMAL(p, s), NUMERIC, FLOAT, DOUBLE

**String Types:**

- CHAR(n), VARCHAR(n), TEXT
- NCHAR, NVARCHAR
- TINYTEXT, MEDIUMTEXT, LONGTEXT
- ENUM

**Other Types:**

- BOOL (TRUE/FALSE)
- JSON
- DATE, TIME, DATETIME, TIMESTAMP

### Constraints

**Supported Constraints:**

- PRIMARY KEY - Ensures unique, non-null values
- UNIQUE - Ensures unique values (can be NULL)
- NOT NULL - Prevents NULL values
- FOREIGN KEY - Enforces referential integrity
- CHECK - Custom validation conditions
- DEFAULT - Default values for columns

### Logs

SimpleRDBMS maintains detailed logs of all operations:

- **Location:** `simplerdbms.log` in the application directory
- **Content:** Timestamps, query execution, errors, warnings
- **UI Access:** View logs directly in the Logging panel at the bottom of the window
- **Log Levels:** DEBUG, INFO, WARNING, ERROR, CRITICAL

### Error Messages

SimpleRDBMS provides clear, helpful error messages:

- **Example:** "Column 'age' not found in table 'users'. Available columns: id, name, email"
- **Debugging:** Error messages include suggestions for resolution
- **Full Details:** Technical information is logged for developers

### Tips & Best Practices

1. **Primary Keys:** Always define a PRIMARY KEY for efficient queries
2. **Indices:** Use CREATE INDEX for frequently searched columns
3. **Transactions:** Use BEGIN/COMMIT/ROLLBACK for multi-step operations
4. **Disk Space:** Ensure sufficient disk space before large imports
5. **Backups:** Regularly export your database for backup purposes

---

## Developer Guide

### Architecture Overview

SimpleRDBMS is organized into modular components:

```
src/
├── core/           # Query execution, schema management
├── parser/         # SQL lexer and parser
├── storage/        # Disk I/O and persistence
├── ui/             # Qt-based graphical interface
└── utils/          # Logging and utilities
```

### Building from Source

#### Prerequisites

- C++ 17 compiler (MSVC, GCC, Clang)
- CMake 3.20 or later
- Qt 6.x (Core, Gui, Widgets, Sql modules)

#### Build Steps

```bash
# Clone or extract the repository
cd simplerdbms

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build
cmake --build . --config Release

# Run
./bin/SimpleRDBMS  # or .\bin\SimpleRDBMS.exe on Windows
```

#### On Windows with MSVC:

```bash
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

#### On Linux/macOS:

```bash
cmake .. -G "Unix Makefiles"
cmake --build .
```

### Project Structure

**Core Modules:**

1. **Parser (`src/parser/`)**
   - `lexer.h/cpp` - Tokenizes SQL input
   - `parser.h/cpp` - Parses tokens into AST
   - `token.h` - Token definitions
   - `ast_nodes.h` - Abstract Syntax Tree node classes

2. **Core (`src/core/`)**
   - `data_type.h/cpp` - SQL data type system
   - `value.h/cpp` - Runtime value representation
   - `column.h` - Column definition
   - `table_schema.h/cpp` - Table schema management
   - `constraint.h/cpp` - Constraint validation
   - `query_executor.h/cpp` - Query execution engine
   - `table_manager.h/cpp` - Table registry
   - `index.h/cpp` - Indexing structures
   - `transaction_manager.h/cpp` - Transaction handling

3. **Storage (`src/storage/`)**
   - `storage_engine.h/cpp` - Disk I/O and persistence

4. **UI (`src/ui/`)**
   - `main_window.h/cpp` - Main application window
   - `sql_editor_page.h/cpp` - SQL editor tab
   - `table_manager_page.h/cpp` - Table manager tab

5. **Utils (`src/utils/`)**
   - `logger.h/cpp` - Centralized logging system

### Class Hierarchy

```
ASTNode (base)
├── SelectStatement
├── InsertStatement
├── UpdateStatement
├── DeleteStatement
├── CreateTableStatement
├── AlterTableStatement
├── DropTableStatement
├── CreateIndexStatement
├── BeginStatement
├── CommitStatement
└── RollbackStatement

Constraint (base)
├── PrimaryKeyConstraint
├── UniqueConstraint
├── NotNullConstraint
├── ForeignKeyConstraint
└── CheckConstraint
```

### Key Components

#### Lexer

Converts SQL strings into tokens:

```cpp
Lexer lexer("SELECT * FROM users WHERE age > 25");
QVector<Token> tokens = lexer.tokenize();
```

#### Parser

Converts tokens into AST:

```cpp
Parser parser(tokens);
std::unique_ptr<ASTNode> ast = parser.parse();
```

#### QueryExecutor

Executes parsed queries:

```cpp
QueryExecutor executor;
auto result = executor.execute(ast);
```

#### StorageEngine

Handles persistence:

```cpp
StorageEngine engine("./data");
engine.saveTable("users", rows);
auto data = engine.loadTable("users");
```

#### Logger

Centralized logging:

```cpp
Logger::instance().info("Query executed successfully");
Logger::instance().error("Table not found");
```

### Extending SimpleRDBMS

#### Adding a New Data Type

1. Add to `DataType` enum in `data_type.h`
2. Update `typeToString()` and `stringToType()` in `data_type.cpp`
3. Update parser to recognize the type
4. Test with `Value` class

#### Adding a New SQL Statement

1. Create AST node class in `ast_nodes.h`
2. Add parser method in `parser.h/cpp`
3. Add executor method in `query_executor.h/cpp`
4. Add corresponding UI controls if needed

#### Adding Constraints Validation

1. Extend `Constraint` base class in `constraint.h`
2. Implement `validate()` method
3. Hook into `QueryExecutor` for INSERT/UPDATE operations

### Testing

Run unit tests:

```bash
cd build
ctest --output-on-failure
```

Create new tests in `tests/` directory following the same naming convention.

### Performance Considerations

- **Indices:** Use for large tables (> 10K rows)
- **Query Optimization:** Parser caches frequently used queries
- **Memory:** Tables stored entirely in memory; use transactions for atomic operations
- **Disk I/O:** Batch updates before committing

### Code Style

- **Naming:** camelCase for functions/variables, PascalCase for classes
- **Comments:** Use `//` for single-line, `/* */` for multi-line
- **Documentation:** All public methods should have Doxygen-style comments
- **Includes:** Use relative paths, group by category

### Debugging

Enable verbose logging:

```cpp
Logger::instance().setLogLevel(LogLevel::DEBUG);
```

Check `simplerdbms.log` for detailed execution information.

### Common Issues

**Issue:** Qt not found

- **Solution:** Ensure Qt is installed and in PATH; use `-DCMAKE_PREFIX_PATH=/path/to/Qt`

**Issue:** Compilation errors

- **Solution:** Ensure C++17 support; check CMakeLists.txt configuration

**Issue:** Database files not loading

- **Solution:** Check file permissions and disk space; review logs

### API Reference

#### Lexer

- `Lexer(const QString& input)`
- `QVector<Token> tokenize()`
- `Token nextToken()`

#### Parser

- `Parser(const QVector<Token>& tokens)`
- `std::unique_ptr<ASTNode> parse()`

#### QueryExecutor

- `std::unique_ptr<QueryResult> execute(const std::unique_ptr<ASTNode>& statement)`

#### StorageEngine

- `StorageEngine(const QString& dataPath)`
- `bool saveTable(const QString& tableName, const QVector<QStringList>& rows)`
- `QVector<QStringList> loadTable(const QString& tableName)`
- `bool deleteTableFile(const QString& tableName)`

#### Logger

- `Logger& instance()` - Singleton access
- `void setLogLevel(LogLevel level)`
- `void setLogFile(const QString& filePath)`
- `void info/debug/warning/error/critical(const QString& message)`

#### TableManager

- `void addTable(const std::shared_ptr<TableSchema>& schema)`
- `std::shared_ptr<TableSchema> getTable(const QString& tableName)`
- `bool tableExists(const QString& tableName)`
- `void removeTable(const QString& tableName)`

---

## Features

✓ **SQL Support**

- SELECT with WHERE, ORDER BY, LIMIT
- INSERT, UPDATE, DELETE
- CREATE/ALTER/DROP TABLE
- CREATE INDEX
- **Core RDBMS**:
  - SQL Support: `CREATE TABLE`, `INSERT`, `SELECT`, `UPDATE`, `DELETE`.
  - Data Persistence: JSON-based storage engine.
  - Basic constraints: `PRIMARY KEY`, `UNIQUE`, `NOT NULL`, `DEFAULT`.
  - Thread-safe storage with primitive locking.
- **Server Mode**:
  - Built-in HTTP Server (listening on port `8081`).
  - REST API endpoint: `POST /query`.
  - CORS support for web clients.

- **Desktop UI**:
  - Built with Qt6 Widgets.
  - SQL Editor with syntax highlighting.
  - Table Manager for schema visualization.
  - **Login**: Default credentials `admin` / `admin`.

- **Demo Application**:
  - React-based "Simple Todo" app demonstrating full CRUD operations against the C++ backend.

## 🚀 How to Run

### Prerequisites

- **C++ Backend**: Qt 6.x (MinGW 64-bit), CMake, Ninja/Make.
- **Frontend**: Node.js & npm.

### 1. Build and Run the C++ Server

The backend acts as the database engine.

1.  **Build**:
    ```powershell
    Example:
    ./build.bat
    ```
2.  **Run Server Mode**:
    ```powershell
    ./build/bin/SimpleRDBMS_new.exe --server
    ```
    _You should see: "Database Server is running" on port `8081`._

**⚠️ IMPORTANT LIMITATION**:  
The `SimpleRDBMS` executable uses a file-based lock on the data directory. You **cannot** run the Desktop GUI and the `--server` mode simultaneously.

- To use the **React App**, run with `--server`.
- To use the **Management UI**, stop the server and run without the flag.

### 2. Run the React Todo App

1.  Navigate to the todo app directory:
    ```powershell
    cd simpletodo
    ```
2.  Install dependencies (first time only):
    ```powershell
    npm install
    ```
3.  Start the development server:
    ```powershell
    npm run dev
    ```
4.  Open your browser at `http://localhost:5173`.

## Architecture

- Tab-based interface
- Real-time query results
- Table browsing with pagination
- Integrated logging panel
- **Modern Dark Theme**

✓ **Persistence**

- CSV-like on-disk storage
- Metadata persistence
- Transaction logs
- Automatic backups

✓ **Developer Friendly**

- Modular C++ design
- Comprehensive logging
- Clear error messages
- Extensible architecture

---

## Requirements

**Runtime:**

- Windows 7+, macOS 10.12+, Linux (glibc 2.17+)
- ~50 MB disk space for application
- Additional space for database files

**Development:**

- C++17 compatible compiler
- CMake 3.20+
- Qt 6.0+ (Core, Gui, Widgets, Sql)
- 500 MB free disk space for build

---

## License

SimpleRDBMS is released under the MIT License. See LICENSE file for details.

---

## Support & Contributing

For bug reports, feature requests, or contributions:

1. Check existing issues on GitHub
2. Create a detailed issue with reproduction steps
3. Submit pull requests with clear descriptions

---

**Version:** 0.1.0  
**Last Updated:** January 15, 2026  
**Maintainer:** SimpleRDBMS Development Team
