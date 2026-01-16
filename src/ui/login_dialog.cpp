#include "login_dialog.h"
#include "../core/user_manager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

LoginDialog::LoginDialog(std::shared_ptr<UserManager> userManager, QWidget* parent)
    : QDialog(parent), userManager(userManager) {
    setWindowTitle("SimpleRDBMS - Login");
    setModal(true);
    setMinimumWidth(400);
    
    // Check if we need to create admin first
    if (!userManager->hasUsers()) {
        showCreateAdminDialog();
    } else {
        showLoginUI();
    }
}

void LoginDialog::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    
    // Title
    titleLabel = new QLabel("Login to SimpleRDBMS");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);
    
    // Username
    mainLayout->addWidget(new QLabel("Username:"));
    usernameInput = new QLineEdit();
    usernameInput->setPlaceholderText("Enter username");
    mainLayout->addWidget(usernameInput);
    
    // Password
    mainLayout->addWidget(new QLabel("Password:"));
    passwordInput = new QLineEdit();
    passwordInput->setPlaceholderText("Enter password");
    passwordInput->setEchoMode(QLineEdit::Password);
    mainLayout->addWidget(passwordInput);
    
    // Error label
    errorLabel = new QLabel();
    errorLabel->setStyleSheet("color: red;");
    errorLabel->setWordWrap(true);
    mainLayout->addWidget(errorLabel);
    
    // Buttons
    auto buttonLayout = new QHBoxLayout();
    
    loginButton = new QPushButton("Login");
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    buttonLayout->addWidget(loginButton);
    
    createAdminButton = new QPushButton("Create Admin");
    connect(createAdminButton, &QPushButton::clicked, this, &LoginDialog::onCreateAdminClicked);
    buttonLayout->addWidget(createAdminButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Allow Enter key to login
    connect(passwordInput, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
}

void LoginDialog::showLoginUI() {
    setupUI();
}

void LoginDialog::showCreateAdminDialog() {
    auto createAdminDialog = std::make_unique<CreateAdminDialog>(userManager, this);
    createAdminDialog->exec();
    
    if (userManager->hasUsers()) {
        setupUI();
    } else {
        // User cancelled, close the login dialog
        reject();
    }
}

void LoginDialog::onLoginClicked() {
    QString username = usernameInput->text().trimmed();
    QString password = passwordInput->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        errorLabel->setText("Username and password are required");
        return;
    }
    
    if (userManager->authenticate(username, password)) {
        authenticatedUser = username;
        accept();
    } else {
        errorLabel->setText("Invalid username or password");
        passwordInput->clear();
        usernameInput->setFocus();
    }
}

void LoginDialog::onCreateAdminClicked() {
    showCreateAdminDialog();
}

QString LoginDialog::getAuthenticatedUser() const {
    return authenticatedUser;
}

// CreateAdminDialog implementation

CreateAdminDialog::CreateAdminDialog(std::shared_ptr<UserManager> userManager, QWidget* parent)
    : QDialog(parent), userManager(userManager) {
    setWindowTitle("Create Administrator Account");
    setModal(true);
    setMinimumWidth(400);
    setupUI();
}

void CreateAdminDialog::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    
    // Title
    auto titleLabel = new QLabel("Create First Administrator Account");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);
    
    // Info
    infoLabel = new QLabel("This will be the superuser account with full database access.");
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("color: #666;");
    mainLayout->addWidget(infoLabel);
    
    // Username
    mainLayout->addWidget(new QLabel("Username:"));
    usernameInput = new QLineEdit();
    usernameInput->setPlaceholderText("e.g., admin");
    mainLayout->addWidget(usernameInput);
    
    // Password
    mainLayout->addWidget(new QLabel("Password:"));
    passwordInput = new QLineEdit();
    passwordInput->setPlaceholderText("Enter strong password");
    passwordInput->setEchoMode(QLineEdit::Password);
    mainLayout->addWidget(passwordInput);
    
    // Confirm Password
    mainLayout->addWidget(new QLabel("Confirm Password:"));
    confirmPasswordInput = new QLineEdit();
    confirmPasswordInput->setPlaceholderText("Confirm password");
    confirmPasswordInput->setEchoMode(QLineEdit::Password);
    mainLayout->addWidget(confirmPasswordInput);
    
    // Error label
    errorLabel = new QLabel();
    errorLabel->setStyleSheet("color: red;");
    errorLabel->setWordWrap(true);
    mainLayout->addWidget(errorLabel);
    
    // Create button
    createButton = new QPushButton("Create Administrator");
    connect(createButton, &QPushButton::clicked, this, &CreateAdminDialog::onCreateClicked);
    mainLayout->addWidget(createButton);
}

void CreateAdminDialog::onCreateClicked() {
    QString username = usernameInput->text().trimmed();
    QString password = passwordInput->text();
    QString confirmPassword = confirmPasswordInput->text();
    
    // Validation
    if (username.isEmpty()) {
        errorLabel->setText("Username cannot be empty");
        return;
    }
    
    if (username.length() < 3) {
        errorLabel->setText("Username must be at least 3 characters");
        return;
    }
    
    if (password.isEmpty()) {
        errorLabel->setText("Password cannot be empty");
        return;
    }
    
    if (password.length() < 6) {
        errorLabel->setText("Password must be at least 6 characters");
        return;
    }
    
    if (password != confirmPassword) {
        errorLabel->setText("Passwords do not match");
        confirmPasswordInput->clear();
        passwordInput->setFocus();
        return;
    }
    
    if (userManager->createUser(username, password, true)) {
        QMessageBox::information(this, "Success", 
            QString("Administrator account '%1' created successfully").arg(username));
        accept();
    } else {
        errorLabel->setText("Failed to create administrator account");
    }
}
