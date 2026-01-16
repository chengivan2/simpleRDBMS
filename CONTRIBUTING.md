# Contributing to SimpleRDBMS

Thank you for your interest in contributing to SimpleRDBMS! This document outlines guidelines and processes for contributing.

## Table of Contents
- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Code Style](#code-style)
- [Commit Messages](#commit-messages)
- [Pull Requests](#pull-requests)
- [Testing](#testing)
- [Documentation](#documentation)
- [Reporting Issues](#reporting-issues)

---

## Code of Conduct

SimpleRDBMS is committed to providing a welcoming and inclusive environment. All contributors are expected to:
- Be respectful and constructive
- Welcome feedback and different perspectives
- Focus on what's best for the community
- Show empathy and patience

---

## Getting Started

### Prerequisites
- C++ 17 knowledge
- Familiarity with Qt6
- Git and GitHub
- CMake basics

### Setup Development Environment

1. **Fork the Repository**
   ```bash
   # On GitHub, click "Fork" button
   ```

2. **Clone Your Fork**
   ```bash
   git clone https://github.com/your-username/simplerdbms.git
   cd simplerdbms
   ```

3. **Add Upstream Remote**
   ```bash
   git remote add upstream https://github.com/simplerdbms/simplerdbms.git
   ```

4. **Build the Project**
   ```bash
   # Follow BUILDING.md for your platform
   ./build.sh  # Linux/macOS
   .\build.bat # Windows
   ```

5. **Verify Build**
   ```bash
   ./bin/SimpleRDBMS
   ```

---

## Development Workflow

### Creating a Feature Branch

```bash
# Update from upstream
git fetch upstream
git checkout main
git rebase upstream/main

# Create feature branch
git checkout -b feature/your-feature-name
```

### Branch Naming Conventions

- `feature/description` - New features
- `fix/description` - Bug fixes
- `docs/description` - Documentation updates
- `test/description` - Test improvements
- `refactor/description` - Code refactoring
- `perf/description` - Performance improvements

### Making Changes

1. **Keep commits focused** - One logical change per commit
2. **Update related files** - Code, tests, and docs together
3. **Run tests locally** - Ensure all tests pass
4. **Build successfully** - No compilation errors/warnings

### Testing During Development

```bash
# Build with debug symbols
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .

# Run tests
ctest --output-on-failure

# Check code coverage
cmake --build . --target coverage  # if available
```

---

## Code Style

### Naming Conventions

**Classes:**
```cpp
class TableManager {
    // PascalCase for class names
};
```

**Methods/Functions:**
```cpp
void executeQuery();           // camelCase
void setTableName();           // setters
bool isValid();                // boolean methods
int getColumnCount();          // getters
```

**Member Variables:**
```cpp
private:
    QString tableName;         // camelCase, no prefix
    int rowCount;
    bool isActive;
```

**Constants:**
```cpp
const int MAX_TABLE_SIZE = 1000;
const QString DEFAULT_PATH = "./data";
```

**Macros:**
```cpp
#define LOG_ERROR(msg) Logger::instance().error(msg)
#define MAX_COLUMNS 256
```

### Code Formatting

**Indentation:**
- Use 4 spaces (no tabs)

**Braces:**
```cpp
// Class definition
class MyClass {
public:
    void method();
};

// Functions
void MyClass::method() {
    if (condition) {
        doSomething();
    }
}

// Control structures
for (int i = 0; i < count; ++i) {
    process(i);
}
```

**Line Length:**
- Target: 80-100 characters
- Hard limit: 120 characters

**Spacing:**
```cpp
// Operators
int result = a + b;
x = (condition) ? value1 : value2;

// Function calls
method(arg1, arg2, arg3);

// Templates
QVector<QString> items;
std::unique_ptr<TableSchema> schema;
```

### Comments

**Doxygen-style (public methods):**
```cpp
/**
 * @brief Executes a SQL query
 * @param query The SQL query to execute
 * @return QueryResult containing results or error
 * 
 * @throws std::invalid_argument if query is empty
 * @note This method is thread-safe
 */
std::unique_ptr<QueryResult> execute(const QString& query);
```

**Inline comments:**
```cpp
// Use // for single-line comments
// Explain the "why", not the "what"

/* Use /* */ only for multi-line
   explanations or commented code */
```

### Include Order

```cpp
// 1. Project headers
#include "logger.h"
#include "query_executor.h"

// 2. Qt headers
#include <QString>
#include <QVector>

// 3. Standard library
#include <vector>
#include <map>

// 4. System headers (rarely needed)
#include <windows.h>
```

### Error Handling

```cpp
// Throw descriptive exceptions
if (tableName.isEmpty()) {
    throw std::invalid_argument("Table name cannot be empty");
}

// Use logging
try {
    result = execute(query);
} catch (const std::exception& e) {
    LOG_ERROR(QString::fromStdString(e.what()));
    return nullptr;
}

// Check return values
if (!storage.saveTable(tableName, rows)) {
    LOG_ERROR("Failed to save table: " + tableName);
}
```

---

## Commit Messages

### Format

```
<type>(<scope>): <subject>

<body>

<footer>
```

### Example

```
feat(parser): add support for JOIN operations

Implement INNER, LEFT, RIGHT, and FULL OUTER JOINs
with proper condition evaluation.

- Add JoinExecutor class
- Implement ON clause parsing
- Add tests for all JOIN types

Fixes #123
```

### Guidelines

- **Type:** feat, fix, docs, style, refactor, perf, test, chore
- **Scope:** parser, core, storage, ui, utils, etc.
- **Subject:** 
  - Imperative mood ("add", not "adds" or "added")
  - Don't capitalize first letter
  - No period at end
  - Max 50 characters
- **Body:**
  - Wrapped at 72 characters
  - Explain what and why
  - Reference issues: "Fixes #123"
- **Footer:**
  - Breaking changes: "BREAKING CHANGE: ..."
  - Issue references: "Fixes #123, Relates to #456"

### Examples

```
fix(lexer): handle escaped quotes in strings
docs(readme): add Windows build instructions
refactor(executor): simplify WHERE clause evaluation
perf(index): optimize B-tree search algorithm
test(core): add constraint validation tests
```

---

## Pull Requests

### Before Submitting

1. **Update with upstream:**
   ```bash
   git fetch upstream
   git rebase upstream/main
   ```

2. **Run all tests:**
   ```bash
   cd build
   ctest --output-on-failure
   ```

3. **Build successfully:**
   ```bash
   cmake --build . --config Release
   ```

4. **Check code style:**
   - Review formatting
   - Check naming conventions
   - Verify comments

### Creating a PR

1. **Push to your fork:**
   ```bash
   git push origin feature/your-feature-name
   ```

2. **Open Pull Request on GitHub**
   - Clear title and description
   - Reference related issues
   - Explain changes and motivation

3. **PR Template** (include this):
   ```markdown
   ## Description
   Brief description of changes
   
   ## Type of Change
   - [ ] Bug fix
   - [ ] New feature
   - [ ] Breaking change
   - [ ] Documentation
   
   ## Related Issues
   Fixes #123
   
   ## Testing
   How to test these changes
   
   ## Checklist
   - [ ] Code follows style guidelines
   - [ ] Tests added/updated
   - [ ] Documentation updated
   - [ ] No new warnings
   ```

### PR Review Process

1. **Automated Checks**
   - CI/CD pipeline runs
   - Code analysis checks
   - Build verification

2. **Code Review**
   - Maintainers review code
   - Provide feedback
   - Request changes if needed

3. **Approval & Merge**
   - Requires 2 approvals
   - All checks passing
   - Maintainer merges

---

## Testing

### Running Tests

```bash
cd build
ctest --output-on-failure
```

### Writing Tests

**Location:** `tests/` directory

**Format:**
```cpp
#include <catch2/catch.hpp>
#include "parser.h"

TEST_CASE("Lexer tokenizes SELECT statement", "[lexer]") {
    Lexer lexer("SELECT * FROM users");
    QVector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() > 0);
    REQUIRE(tokens[0].type == Token::SELECT);
}

TEST_CASE("Parser builds valid SELECT AST", "[parser]") {
    Lexer lexer("SELECT id, name FROM users WHERE age > 25");
    QVector<Token> tokens = lexer.tokenize();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    REQUIRE(ast != nullptr);
    REQUIRE(dynamic_cast<SelectStatement*>(ast.get()) != nullptr);
}
```

### Test Naming

- File: `<component>_tests.cpp`
- Test case: Describe what is being tested
- Assertion: Use descriptive messages

### Code Coverage

```bash
# Build with coverage
cmake -DENABLE_COVERAGE=ON ..
cmake --build . --target coverage

# View report
open coverage/index.html  # macOS
xdg-open coverage/index.html  # Linux
```

**Target:** 80% overall coverage, 90% for core components

---

## Documentation

### Code Documentation

**All public methods should have:**
```cpp
/**
 * @brief One-line summary
 * 
 * Detailed explanation of what the method does,
 * how it works, and when to use it.
 * 
 * @param param1 Description of param1
 * @param param2 Description of param2
 * @return Description of return value
 * 
 * @throws std::exception Description of exception
 * @note Any important notes
 * @see Related methods
 */
ReturnType methodName(Type param1, Type param2);
```

### File Header

```cpp
/**
 * @file query_executor.h
 * @brief SQL query execution engine
 * 
 * Handles execution of SELECT, INSERT, UPDATE, DELETE,
 * and DDL statements.
 */

#pragma once
```

### Updating Documentation

When making changes:
1. Update inline code comments
2. Update README.md if user-facing
3. Update IMPLEMENTATION_PLAN.md if architectural
4. Add entry to CHANGELOG.md

---

## Reporting Issues

### Before Reporting

1. Check [existing issues](https://github.com/simplerdbms/simplerdbms/issues)
2. Check [documentation](README.md)
3. Try reproducing with latest code

### Issue Template

```markdown
## Description
Brief description of the issue

## Steps to Reproduce
1. ...
2. ...
3. ...

## Expected Behavior
What should happen

## Actual Behavior
What actually happened

## Environment
- OS: Windows/Linux/macOS
- Qt Version: 6.x.x
- Compiler: MSVC/GCC/Clang

## Logs
```
[paste relevant logs]
```

## Screenshots
[if applicable]
```

### Issue Labels

- `bug` - Something isn't working
- `feature` - New functionality
- `enhancement` - Improvement to existing feature
- `documentation` - Documentation improvements
- `good first issue` - Good for newcomers
- `help wanted` - Extra attention needed
- `question` - Question about usage

---

## Development Phases

See [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md) for current development phase.

**Contributing to specific phases:**
- Review phase objectives
- Check related tasks
- Ensure changes align with phase goals
- Test comprehensively

---

## Communication

- **Issues:** Use GitHub issues for bugs/features
- **Discussions:** Use GitHub Discussions for questions
- **PRs:** Main review mechanism
- **Email:** simplerdbms@example.com (future)

---

## Recognition

Contributors will be recognized in:
- [CONTRIBUTORS.md](CONTRIBUTORS.md)
- Release notes
- GitHub contributors graph

---

## License

By contributing to SimpleRDBMS, you agree that your contributions are licensed under the MIT License.

---

## Questions?

- Check [README.md](README.md)
- Check [BUILDING.md](BUILDING.md)
- Review [existing issues](https://github.com/simplerdbms/simplerdbms/issues)
- Create a [discussion](https://github.com/simplerdbms/simplerdbms/discussions)

---

**Thank you for contributing to SimpleRDBMS!**

---

**Last Updated:** January 15, 2026
