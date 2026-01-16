#pragma once

#include <QString>
#include <QMap>
#include <QSet>
#include <memory>

/**
 * @brief Represents a database user
 */
struct User {
    QString username;
    QString passwordHash;  // bcrypt hash
    bool isAdmin = false;
    bool isActive = true;
    
    // Permissions per table
    QMap<QString, QSet<QString>> tablePermissions;  // tableName -> {SELECT, INSERT, UPDATE, DELETE}
};

/**
 * @brief Manages user authentication and authorization
 */
class UserManager {
public:
    UserManager();
    
    /**
     * @brief Create the system database for users
     */
    void initializeSystemDatabase();
    
    /**
     * @brief Check if any users exist
     */
    bool hasUsers() const;
    
    /**
     * @brief Create a new user (admin only)
     */
    bool createUser(const QString& username, const QString& password, bool isAdmin = false);
    
    /**
     * @brief Authenticate a user
     */
    bool authenticate(const QString& username, const QString& password);
    
    /**
     * @brief Get current logged-in user
     */
    QString getCurrentUser() const;
    
    /**
     * @brief Check if current user is admin
     */
    bool isCurrentUserAdmin() const;
    
    /**
     * @brief Set current user (after authentication)
     */
    void setCurrentUser(const QString& username);
    
    /**
     * @brief Logout current user
     */
    void logout();
    
    /**
     * @brief Load users from system database
     */
    void loadUsers();
    
    /**
     * @brief Save users to system database
     */
    void saveUsers();
    
    /**
     * @brief Grant permission to user on table
     */
    bool grantPermission(const QString& username, const QString& tableName, const QString& permission);
    
    /**
     * @brief Revoke permission from user on table
     */
    bool revokePermission(const QString& username, const QString& tableName, const QString& permission);
    
    /**
     * @brief Check if user has permission on table
     */
    bool hasPermission(const QString& username, const QString& tableName, const QString& permission);
    
    /**
     * @brief Get user by username
     */
    User* getUser(const QString& username);
    
    /**
     * @brief Delete a user
     */
    bool deleteUser(const QString& username);
    
private:
    QMap<QString, User> users;  // username -> User
    QString currentUser;
    
    /**
     * @brief Hash a password using bcrypt
     */
    static QString hashPassword(const QString& password);
    
    /**
     * @brief Verify a password against a hash
     */
    static bool verifyPassword(const QString& password, const QString& hash);
};
