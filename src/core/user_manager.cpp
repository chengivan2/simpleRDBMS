#include "user_manager.h"
#include "../utils/logger.h"
#include <QCryptographicHash>
#include <QUuid>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <cstring>

// Simple bcrypt-like implementation (for production, use a proper bcrypt library)
// For now, we'll use PBKDF2 which is built-in to Qt

UserManager::UserManager() {
    loadUsers();  // Load users from persistent storage on startup
}

void UserManager::initializeSystemDatabase() {
    // This will be called during app startup
    // System creates default admin user if no users exist
    Logger::instance().info("Initializing system database for user management");
}

bool UserManager::hasUsers() const {
    return !users.isEmpty();
}

bool UserManager::createUser(const QString& username, const QString& password, bool isAdmin) {
    if (username.isEmpty() || password.isEmpty()) {
        Logger::instance().error("Cannot create user: username or password is empty");
        return false;
    }
    
    if (users.contains(username)) {
        Logger::instance().error(QString("User '%1' already exists").arg(username));
        return false;
    }
    
    User newUser;
    newUser.username = username;
    newUser.passwordHash = hashPassword(password);
    newUser.isAdmin = isAdmin;
    newUser.isActive = true;
    
    users[username] = newUser;
    saveUsers();
    
    Logger::instance().info(QString("User '%1' created successfully").arg(username));
    return true;
}

bool UserManager::authenticate(const QString& username, const QString& password) {
    if (!users.contains(username)) {
        Logger::instance().warning(QString("Authentication failed: user '%1' not found").arg(username));
        return false;
    }
    
    const User& user = users[username];
    if (!user.isActive) {
        Logger::instance().warning(QString("Authentication failed: user '%1' is inactive").arg(username));
        return false;
    }
    
    if (!verifyPassword(password, user.passwordHash)) {
        Logger::instance().warning(QString("Authentication failed: invalid password for user '%1'").arg(username));
        return false;
    }
    
    currentUser = username;
    Logger::instance().info(QString("User '%1' authenticated successfully").arg(username));
    return true;
}

QString UserManager::getCurrentUser() const {
    return currentUser;
}

bool UserManager::isCurrentUserAdmin() const {
    if (currentUser.isEmpty()) {
        return false;
    }
    return users[currentUser].isAdmin;
}

void UserManager::setCurrentUser(const QString& username) {
    currentUser = username;
}

void UserManager::logout() {
    if (!currentUser.isEmpty()) {
        Logger::instance().info(QString("User '%1' logged out").arg(currentUser));
        currentUser.clear();
    }
}

void UserManager::loadUsers() {
    // Load users from system database JSON file
    QString usersFilePath = "data/system_users.json";
    
    QFile file(usersFilePath);
    if (!file.exists()) {
        Logger::instance().debug("Users file does not exist, starting with no users");
        return;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        Logger::instance().error(QString("Cannot open users file: %1").arg(usersFilePath));
        return;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        Logger::instance().error("Invalid users JSON format");
        return;
    }
    
    QJsonObject obj = doc.object();
    QJsonArray usersArray = obj["users"].toArray();
    
    users.clear();
    for (const QJsonValue& userVal : usersArray) {
        QJsonObject userObj = userVal.toObject();
        User user;
        user.username = userObj["username"].toString();
        user.passwordHash = userObj["passwordHash"].toString();
        user.isAdmin = userObj["isAdmin"].toBool(false);
        user.isActive = userObj["isActive"].toBool(true);
        
        // Load table permissions
        QJsonArray permsArray = userObj["permissions"].toArray();
        for (const QJsonValue& permVal : permsArray) {
            QJsonObject permObj = permVal.toObject();
            QString tableName = permObj["table"].toString();
            QJsonArray permList = permObj["permissions"].toArray();
            for (const QJsonValue& p : permList) {
                user.tablePermissions[tableName].insert(p.toString());
            }
        }
        
        users[user.username] = user;
    }
    
    Logger::instance().info(QString("Loaded %1 users from system database").arg(users.size()));
}

void UserManager::saveUsers() {
    // Save users to system database JSON file
    // Ensure data directory exists
    QDir dir;
    if (!dir.exists("data")) {
        dir.mkdir("data");
    }
    
    QString usersFilePath = "data/system_users.json";
    
    QJsonArray usersArray;
    for (const User& user : users) {
        QJsonObject userObj;
        userObj["username"] = user.username;
        userObj["passwordHash"] = user.passwordHash;
        userObj["isAdmin"] = user.isAdmin;
        userObj["isActive"] = user.isActive;
        
        // Save table permissions
        QJsonArray permsArray;
        for (auto it = user.tablePermissions.begin(); it != user.tablePermissions.end(); ++it) {
            QJsonObject permObj;
            permObj["table"] = it.key();
            QJsonArray permList;
            for (const QString& perm : it.value()) {
                permList.append(perm);
            }
            permObj["permissions"] = permList;
            permsArray.append(permObj);
        }
        userObj["permissions"] = permsArray;
        
        usersArray.append(userObj);
    }
    
    QJsonObject rootObj;
    rootObj["users"] = usersArray;
    
    QJsonDocument doc(rootObj);
    
    QFile file(usersFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        Logger::instance().error(QString("Cannot write to users file: %1").arg(usersFilePath));
        return;
    }
    
    file.write(doc.toJson());
    file.close();
    
    Logger::instance().debug(QString("Saved %1 users to system database").arg(users.size()));
}

bool UserManager::grantPermission(const QString& username, const QString& tableName, const QString& permission) {
    if (!users.contains(username)) {
        Logger::instance().error(QString("Cannot grant permission: user '%1' not found").arg(username));
        return false;
    }
    
    users[username].tablePermissions[tableName].insert(permission);
    saveUsers();
    
    Logger::instance().info(QString("Granted %1 permission on table '%2' to user '%3'")
                           .arg(permission, tableName, username));
    return true;
}

bool UserManager::revokePermission(const QString& username, const QString& tableName, const QString& permission) {
    if (!users.contains(username)) {
        Logger::instance().error(QString("Cannot revoke permission: user '%1' not found").arg(username));
        return false;
    }
    
    users[username].tablePermissions[tableName].remove(permission);
    saveUsers();
    
    Logger::instance().info(QString("Revoked %1 permission on table '%2' from user '%3'")
                           .arg(permission, tableName, username));
    return true;
}

bool UserManager::hasPermission(const QString& username, const QString& tableName, const QString& permission) {
    // Admin users have all permissions
    if (users.contains(username) && users[username].isAdmin) {
        return true;
    }
    
    if (!users.contains(username)) {
        return false;
    }
    
    const auto& tablePerms = users[username].tablePermissions;
    if (!tablePerms.contains(tableName)) {
        return false;
    }
    
    return tablePerms[tableName].contains(permission);
}

User* UserManager::getUser(const QString& username) {
    if (users.contains(username)) {
        return &users[username];
    }
    return nullptr;
}

bool UserManager::deleteUser(const QString& username) {
    if (!users.contains(username)) {
        Logger::instance().error(QString("Cannot delete user: user '%1' not found").arg(username));
        return false;
    }
    
    // Prevent deleting the last admin
    int adminCount = 0;
    for (const auto& user : users) {
        if (user.isAdmin) adminCount++;
    }
    
    if (users[username].isAdmin && adminCount <= 1) {
        Logger::instance().error("Cannot delete the last admin user");
        return false;
    }
    
    users.remove(username);
    saveUsers();
    
    Logger::instance().info(QString("User '%1' deleted successfully").arg(username));
    return true;
}

QString UserManager::hashPassword(const QString& password) {
    // Using SHA-256 for now; in production use bcrypt library
    // Generate a random salt
    QString salt = QUuid::createUuid().toString();
    
    // Combine password and salt
    QString combined = password + salt;
    
    // Hash using SHA-256
    QByteArray hash = QCryptographicHash::hash(
        combined.toUtf8(),
        QCryptographicHash::Sha256
    );
    
    // Return hash with salt prefix for verification
    return salt + ":" + hash.toHex();
}

bool UserManager::verifyPassword(const QString& password, const QString& hash) {
    // Extract salt from stored hash
    QStringList parts = hash.split(":");
    if (parts.size() != 2) {
        Logger::instance().error("Invalid password hash format");
        return false;
    }
    
    QString salt = parts[0];
    QString storedHash = parts[1];
    
    // Hash the provided password with the same salt
    QString combined = password + salt;
    QByteArray computedHash = QCryptographicHash::hash(
        combined.toUtf8(),
        QCryptographicHash::Sha256
    );
    
    // Compare hashes
    return computedHash.toHex() == storedHash;
}
