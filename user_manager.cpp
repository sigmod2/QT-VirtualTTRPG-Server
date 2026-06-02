#include "user_manager.h"

UserManager::UserManager(const  QString &filePath) :  m_filePath(filePath) {

    // jeżeli nie istnieje to dodaj
    QFile file(m_filePath);
    if (!file.exists()) {
        registerUser("adminadmin", "lubieplacki");
    }
}

QJsonArray UserManager::loadUsers() {
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) return QJsonArray();
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    return doc.object()["users"].toArray();
}

void UserManager::saveUsers(const QJsonArray &users) {
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly)) return;
    QJsonObject root;
    root["users"] = users;
    file.write(QJsonDocument(root).toJson());
}

bool UserManager::validateUser(const QString &username, const QString &password) {
    for (const QJsonValue &val : loadUsers()) {
        QJsonObject user = val.toObject();
        if (user["username"].toString() == username &&
            user["password"].toString() == password) {
            return true;
        }
    }
    return false;
}

bool UserManager::registerUser(const QString &username,
                               const QString &password,
                               const QString &role)
{
    QJsonArray users = loadUsers();

    // Sprawdź czy użytkownik już istnieje
    for (const QJsonValue &val : users) {
        if (val.toObject()["username"].toString() == username)
            return false;
    }

    QJsonObject newUser;
    newUser["username"] = username;
    newUser["password"] = password;
    newUser["role"]     = role;
    users.append(newUser);
    saveUsers(users);
    return true;
}

QString UserManager::getUserRole(const QString &username)
{
    for (const QJsonValue &val : loadUsers()) {
        QJsonObject user = val.toObject();
        if (user["username"].toString() == username)
            return user["role"].toString("player");
    }
    return "player";
}