#pragma once
#ifndef ROOM_MANAGER_H
#define ROOM_MANAGER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QString>

class RoomManager
{
public:
    RoomManager(const QString &filePath);

    bool        validateRoomId(const QString &identifier);
    QString     createRoom(const QString &roomName, const QString &gmUsername, const QString &system);
    bool        addPlayerToRoom(const QString &identifier, const QString &username);
    QJsonObject getRoomInfo(const QString &identifier);
    QJsonArray  getRoomsForUser(const QString &username);

private:
    QString     m_filePath;
    QString     generateRoomId();
    QJsonArray  loadRooms();
    void        saveRooms(const QJsonArray &rooms);
};

#endif // ROOM_MANAGER_H