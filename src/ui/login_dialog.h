#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <memory>

class UserManager;

/**
 * @brief Login dialog for authenticating users
 */
class LoginDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit LoginDialog(std::shared_ptr<UserManager> userManager, QWidget* parent = nullptr);
    
    /**
     * @brief Get the username of the authenticated user
     */
    QString getAuthenticatedUser() const;
    
private slots:
    void onLoginClicked();
    void onCreateAdminClicked();
    
private:
    void setupUI();
    void showCreateAdminDialog();
    void showLoginUI();
    
    std::shared_ptr<UserManager> userManager;
    QString authenticatedUser;
    
    // UI Components
    QLineEdit* usernameInput;
    QLineEdit* passwordInput;
    QPushButton* loginButton;
    QPushButton* createAdminButton;
    QLabel* errorLabel;
    QLabel* titleLabel;
};

/**
 * @brief Dialog for creating the first superuser
 */
class CreateAdminDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit CreateAdminDialog(std::shared_ptr<UserManager> userManager, QWidget* parent = nullptr);
    
private slots:
    void onCreateClicked();
    
private:
    void setupUI();
    
    std::shared_ptr<UserManager> userManager;
    
    QLineEdit* usernameInput;
    QLineEdit* passwordInput;
    QLineEdit* confirmPasswordInput;
    QPushButton* createButton;
    QLabel* errorLabel;
    QLabel* infoLabel;
};
