#pragma once

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

#ifndef USER_MANAGER_H
#define USER_MANAGER_H

class UserManager
{
public:
    UserManager(const QString &filePath);

    bool validateUser(const QString &username, const QString &password);
    bool registerUser(const QString &username,
                      const QString &password,
                      const QString &role = "player");
    QString getUserRole(const QString &username);

private:
    QString m_filePath;
    QJsonArray loadUsers();
    void saveUsers(const QJsonArray &users);
};

#endif // USER_MANAGER_H
