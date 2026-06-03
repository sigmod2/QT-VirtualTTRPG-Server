#include "room_manager.h"
#include <QDateTime>
#include <QRandomGenerator>

RoomManager::RoomManager(const QString &filePath)
    : m_filePath(filePath)
{
    // Jeśli plik nie istnieje, stwórz pusty
    QFile file(m_filePath);
    if (!file.exists()) {
        saveRooms(QJsonArray());
    }
}

// -------------------------------------------------------
// Prywatne: zapis i odczyt
// -------------------------------------------------------

QJsonArray RoomManager::loadRooms()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly))
        return QJsonArray();
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    return doc.object()["rooms"].toArray();
}

void RoomManager::saveRooms(const QJsonArray &rooms)
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly))
        return;
    QJsonObject root;
    root["rooms"] = rooms;
    file.write(QJsonDocument(root).toJson());
}

// -------------------------------------------------------
// Prywatne
// -------------------------------------------------------

QString RoomManager::generateRoomId()
{
    const QString chars = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
    QString id;
    for (int i = 0; i < 6; ++i) {
        id += chars[QRandomGenerator::global()->bounded(chars.size())];
    }

    if (validateRoomId(id)) {
        return generateRoomId();
    }
    return id;
}

// -------------------------------------------------------
// Publiczne
// -------------------------------------------------------

bool RoomManager::validateRoomId(const QString &identifier)
{
    for (const QJsonValue &val : loadRooms()) {
        if (val.toObject()["id"].toString() == identifier)
            return true;
    }
    return false;
}

QString RoomManager::createRoom(const QString &roomName,
                                const QString &gmUsername,
                                const QString &system)
{
    QJsonArray rooms = loadRooms();

    QString newId = generateRoomId();

    QJsonObject room;
    room["id"] = newId;
    room["name"] = roomName;
    room["gm"] = gmUsername;
    room["system"] = system;
    room["created_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    room["players"] = QJsonArray();
    room["characters"] = QJsonObject();
    rooms.append(room);
    saveRooms(rooms);

    return newId;
}

bool RoomManager::addPlayerToRoom(const QString &identifier, const QString &username)
{
    QJsonArray rooms = loadRooms();

    for (int i = 0; i < rooms.size(); ++i) {
        QJsonObject room = rooms[i].toObject();
        if (room["id"].toString() == identifier) {
            QJsonArray players = room["players"].toArray();

            for (const QJsonValue &p : players) {
                if (p.toString() == username)
                    return true;
            }

            players.append(username);
            room["players"] = players;
            rooms[i] = room;
            saveRooms(rooms);
            return true;
        }
    }
    return false;
}

QJsonObject RoomManager::getRoomInfo(const QString &identifier)
{
    for (const QJsonValue &val : loadRooms()) {
        QJsonObject room = val.toObject();
        if (room["id"].toString() == identifier)
            return room;
    }
    return QJsonObject();
}

QJsonArray RoomManager::getRoomsForUser(const QString &username)
{
    QJsonArray result;
    for (const QJsonValue &val : loadRooms()) {
        QJsonObject room = val.toObject();

        if (room["gm"].toString() == username) {
            result.append(room);
            continue;
        }

        QJsonArray players = room["players"].toArray();
        for (const QJsonValue &p : players) {
            if (p.toString() == username) {
                result.append(room);
                break;
            }
        }
    }
    return result;
}

void RoomManager::saveCharacter(const QString &roomId,
                                const QString &username,
                                const QJsonObject &character)
{
    QJsonArray rooms = loadRooms();
    for (int i = 0; i < rooms.size(); ++i) {
        QJsonObject room = rooms[i].toObject();
        if (room["id"].toString() == roomId) {
            QJsonObject characters = room["characters"].toObject();
            characters[username]   = character;
            room["characters"]     = characters;
            rooms[i]               = room;
            saveRooms(rooms);
            return;
        }
    }
}