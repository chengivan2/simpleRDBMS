# SimpleRDBMS - Quick Reference Guide

**Version:** 0.1.0  
**Date:** January 15, 2026  
**Status:** Foundation Phase Complete ✅

---

## Quick Start

### For Users

**Want to use SimpleRDBMS?**

1. Read [README.md](README.md#user-guide) - User Guide section
2. Build it: Follow [BUILDING.md](BUILDING.md)
3. Run it: `./bin/SimpleRDBMS` or `SimpleRDBMS.exe`
4. Start executing SQL queries!

### For Developers

**Want to contribute or extend SimpleRDBMS?**

1. Read [README.md](README.md#developer-guide) - Developer Guide section
2. Read [CONTRIBUTING.md](CONTRIBUTING.md) - Development workflow
3. Read [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md) - Architecture
4. Follow [BUILDING.md](BUILDING.md) - Build setup
5. Check out [PROJECT_STATUS.md](PROJECT_STATUS.md) - What's done, what's next
6. Start coding!

---

## Project Files at a Glance

### Documentation (Start Here!)

| File | Purpose | Audience |
|------|---------|----------|
| [README.md](README.md) | Complete user & developer guide | Everyone |
| [BUILDING.md](BUILDING.md) | Build instructions for all platforms | Developers |
| [CONTRIBUTING.md](CONTRIBUTING.md) | How to contribute, code style | Contributors |
| [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md) | 18-phase development roadmap | Developers |
| [PROJECT_STATUS.md](PROJECT_STATUS.md) | Current status & milestones | Everyone |
| [QUICK_REFERENCE.md](QUICK_REFERENCE.md) | This file | Everyone |

### Source Code Structure

```
src/
├── main.cpp                    # Application entry point
├── parser/                     # SQL parsing
│   ├── lexer.h/cpp            # Tokenizes SQL
│   ├── parser.h/cpp           # Builds AST from tokens
│   ├── token.h                # Token definitions
│   └── ast_nodes.h            # AST node classes
├── core/                       # Database engine
│   ├── data_type.h/cpp        # SQL data types
│   ├── value.h/cpp            # Runtime values
│   ├── column.h               # Column definitions
│   ├── table_schema.h/cpp     # Table schemas
│   ├── constraint.h/cpp       # Constraints (PK, UNIQUE, etc)
│   ├── query_executor.h/cpp   # Query execution
│   ├── table_manager.h/cpp    # Table registry
│   ├── index.h/cpp            # Indexing
│   └── transaction_manager.h/cpp # Transactions
├── storage/                    # Persistence
│   └── storage_engine.h/cpp   # Disk I/O
├── ui/                         # User interface
│   ├── main_window.h/cpp      # Main window
│   ├── sql_editor_page.h/cpp  # SQL editor tab
│   └── table_manager_page.h/cpp # Table manager tab
└── utils/                      # Utilities
    └── logger.h/cpp           # Logging system
```

---

## Common Tasks

### I want to...

**...use the application:**
→ Read [README.md User Guide](README.md#user-guide)  
→ Build with [BUILDING.md](BUILDING.md)  
→ SQL examples in [README.md](README.md#supported-operations)

**...build the project:**
→ Read [BUILDING.md](BUILDING.md)  
→ Windows: `build.bat`  
→ Linux/macOS: `./build.sh`

**...understand the architecture:**
→ Read [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md#architecture-overview)  
→ Read [README.md Developer Guide](README.md#developer-guide)

**...see current status:**
→ Read [PROJECT_STATUS.md](PROJECT_STATUS.md)

**...contribute code:**
→ Read [CONTRIBUTING.md](CONTRIBUTING.md)  
→ Follow [Code Style](#code-style) section below

**...report a bug:**
→ Check [existing issues](https://github.com/simplerdbms/simplerdbms/issues)  
→ Use issue template in [CONTRIBUTING.md](CONTRIBUTING.md#reporting-issues)

**...add a feature:**
→ Open issue first (discuss)  
→ Create fork and branch  
→ Read [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md) phase details  
→ Submit PR with tests

**...understand the code:**
→ Each `.h` file has class comments  
→ Each method has `/// Doxygen comments`  
→ See class diagram in [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md#class-hierarchy)

---

## Phase Overview

| Phase | Component | Status | Effort |
|-------|-----------|--------|--------|
| 1 | Setup & Architecture | ✅ DONE | Complete |
| 2 | SQL Parser | ⏳ NEXT | 3-4 days |
| 3 | Data Types | ⏳ Future | 2-3 days |
| 4 | Schema & Constraints | ⏳ Future | 3-4 days |
| 5 | Storage Engine | ⏳ Future | 3-4 days |
| 6 | CRUD Execution | ⏳ Future | 3-4 days |
| 7 | DDL Execution | ⏳ Future | 2-3 days |
| 8 | Transactions | ⏳ Future | 2-3 days |
| 9 | JOINs | ⏳ Future | 2-3 days |
| 10 | Indexing | ⏳ Future | 2-3 days |
| 11 | SQL Editor UI | ⏳ Future | 2-3 days |
| 12 | Table Manager UI | ⏳ Future | 2-3 days |
| 13 | Logging UI | ⏳ Future | 1-2 days |
| 14 | UI Integration | ⏳ Future | 2-3 days |
| 15 | Error Messages | ⏳ Future | 1-2 days |
| 16 | Testing | ⏳ Future | 4-5 days |
| 17 | Documentation | ⏳ Future | 2-3 days |
| 18 | Optimization | ⏳ Future | 2-3 days |

**Current:** Phase 1 ✅  
**Next:** Phase 2 (SQL Parser enhancements)  
**MVP Target:** After Phase 6  
**Full Release:** After Phase 18  

---

## Code Style Quick Reference

### Naming
```cpp
class TableManager { };        // PascalCase
void executeQuery() { }        // camelCase
QString tableName;             // camelCase (no prefix)
const int MAX_SIZE = 100;      // UPPER_CASE
#define LOG_ERROR(m)           // UPPER_CASE
```

### Formatting
```cpp
// 4-space indentation
// Max 100 chars per line
// Opening brace on same line
if (condition) {
    doSomething();
}
```

### Comments
```cpp
/// Doxygen comment for public methods
void publicMethod();

// Regular comment for implementation
// Explain the "why", not the "what"

/* Multi-line comment
   for complex logic */
```

### Includes
```cpp
#include "logger.h"           // Project
#include <QString>            // Qt
#include <vector>             // Standard
```

See [CONTRIBUTING.md#code-style](CONTRIBUTING.md#code-style) for full details.

---

## Key Classes

### Parser
- `Lexer` - Tokenizes SQL input
- `Parser` - Builds AST from tokens
- `Token` - Individual token
- `ASTNode` - Base for all AST nodes

### Core
- `TableSchema` - Table definition
- `Column` - Column definition
- `Value` - Runtime value with type
- `QueryExecutor` - Query execution
- `TableManager` - Table registry

### Constraints
- `Constraint` - Base class
- `PrimaryKeyConstraint`
- `UniqueConstraint`
- `NotNullConstraint`
- `ForeignKeyConstraint`
- `CheckConstraint`

### UI
- `MainWindow` - Main app window
- `SQLEditorPage` - SQL editor tab
- `TableManagerPage` - Table manager tab

### Storage
- `StorageEngine` - Disk persistence

### Utils
- `Logger` - Centralized logging

---

## Build Commands Quick Reference

### Windows
```powershell
# Using script
.\build.bat

# Manual
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
.\bin\SimpleRDBMS.exe
```

### Linux
```bash
# Using script
./build.sh

# Manual
mkdir build && cd build
cmake ..
cmake --build . --config Release
./bin/SimpleRDBMS
```

### macOS
```bash
# Using script
./build.sh

# Manual (with Homebrew Qt6)
mkdir build && cd build
export CMAKE_PREFIX_PATH=/usr/local/opt/qt6:$CMAKE_PREFIX_PATH
cmake ..
cmake --build . --config Release
./bin/SimpleRDBMS
```

---

## SQL Features Supported

### Current Phase (1)
✅ Planned and interfaces created for:

### Planned in Phase 2-6
⏳ **DML (Data Modification):**
- SELECT with WHERE, ORDER BY, LIMIT
- INSERT
- UPDATE
- DELETE

⏳ **DDL (Data Definition):**
- CREATE TABLE
- ALTER TABLE
- DROP TABLE
- CREATE INDEX

⏳ **Transactions:**
- BEGIN
- COMMIT
- ROLLBACK

⏳ **JOINs:**
- INNER JOIN
- LEFT/RIGHT/FULL OUTER JOIN
- CROSS JOIN

### Data Types (24 total)
✅ Defined:
- INT, SMALLINT, BIGINT
- DECIMAL, NUMERIC, FLOAT, DOUBLE
- CHAR, VARCHAR, TEXT
- NCHAR, NVARCHAR
- TINYTEXT, MEDIUMTEXT, LONGTEXT
- ENUM, BOOL, JSON
- DATE, TIME, DATETIME, TIMESTAMP

### Constraints (5 types)
✅ Framework ready:
- PRIMARY KEY
- UNIQUE
- NOT NULL
- FOREIGN KEY
- CHECK

---

## Logging

### Using Logger
```cpp
// Include header
#include "logger.h"

// Use macros
LOG_DEBUG("Debug message");
LOG_INFO("Information");
LOG_WARNING("Warning message");
LOG_ERROR("Error occurred");
LOG_CRITICAL("Critical issue");
```

### Configure Logger
```cpp
// Set log level
Logger::instance().setLogLevel(LogLevel::INFO);

// Set log file
Logger::instance().setLogFile("./app.log");
```

### Log Output
- **Console:** Printed to stdout
- **File:** Written to specified file
- **Timestamp:** Included automatically
- **Level:** DEBUG, INFO, WARN, ERROR, CRIT

---

## Testing

### Run Tests
```bash
cd build
ctest --output-on-failure
```

### Write Tests
Tests go in `tests/` directory using Catch2 framework (Phase 16).

### Coverage
Target: 80% overall, 90% for core components.

---

## File Sizes

| Category | Count | Purpose |
|----------|-------|---------|
| Headers | 20 | Interface definitions |
| Sources | 18 | Implementations |
| Config | 2 | Build system |
| Docs | 5 | Documentation |
| Scripts | 2 | Build automation |

**Total:** 47+ files, ~15KB code (interfaces)

---

## Milestones

✅ **Phase 1:** Foundation complete  
⏳ **Phase 2-6:** Core functionality (3-4 weeks)  
⏳ **Phase 7-10:** Advanced features (2 weeks)  
⏳ **Phase 11-14:** UI integration (2 weeks)  
⏳ **Phase 15-18:** Polish & release (1-2 weeks)  

**Target:** Version 0.1.0 release in ~8 weeks

---

## Support & Help

### Documentation
- [README.md](README.md) - Complete guide
- [BUILDING.md](BUILDING.md) - Build help
- [CONTRIBUTING.md](CONTRIBUTING.md) - Dev guide
- [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md) - Architecture
- [PROJECT_STATUS.md](PROJECT_STATUS.md) - Status

### Common Issues

**Qt not found?**  
→ See [BUILDING.md#troubleshooting](BUILDING.md#troubleshooting)

**Build fails?**  
→ See [BUILDING.md#troubleshooting](BUILDING.md#troubleshooting)

**How to contribute?**  
→ See [CONTRIBUTING.md](CONTRIBUTING.md)

**What's the architecture?**  
→ See [IMPLEMENTATION_PLAN.md#architecture-overview](IMPLEMENTATION_PLAN.md#architecture-overview)

---

## Important Links

| Resource | Link |
|----------|------|
| User Guide | [README.md#user-guide](README.md#user-guide) |
| Developer Guide | [README.md#developer-guide](README.md#developer-guide) |
| Build Instructions | [BUILDING.md](BUILDING.md) |
| Contributing | [CONTRIBUTING.md](CONTRIBUTING.md) |
| Architecture | [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md) |
| Current Status | [PROJECT_STATUS.md](PROJECT_STATUS.md) |
| Source Code | `src/` |
| Tests | `tests/` |

---

## One-Page Summary

**SimpleRDBMS** is a lightweight RDBMS built in C++ with Qt.

**Current Status:** Phase 1 complete - Foundation ready ✅

**What Works:**
- ✅ Complete project structure
- ✅ Build system (CMake)
- ✅ All class interfaces defined
- ✅ Logging framework
- ✅ UI scaffolding

**What's Next:**
- ⏳ Phase 2: SQL Parser (in progress)
- ⏳ Phases 3-6: Core database features
- ⏳ Phases 7-10: Advanced features
- ⏳ Phases 11-18: UI integration & polish

**Timeline:** ~8 weeks to full release

**How to Get Started:**
1. Read [README.md](README.md)
2. Follow [BUILDING.md](BUILDING.md)
3. Explore `src/` code
4. See [PROJECT_STATUS.md](PROJECT_STATUS.md)

---

**Let's build something awesome! 🚀**

---

**Last Updated:** January 15, 2026  
**For latest updates, see:** [PROJECT_STATUS.md](PROJECT_STATUS.md)
