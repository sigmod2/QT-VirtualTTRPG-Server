#include "room_manager.h"
#include <QRandomGenerator>
#include <QDateTime>

RoomManager::RoomManager(const QString &filePath) : m_filePath(filePath)
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
    if (!file.open(QIODevice::ReadOnly)) return QJsonArray();
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    return doc.object()["rooms"].toArray();
}

void RoomManager::saveRooms(const QJsonArray &rooms)
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly)) return;
    QJsonObject root;
    root["rooms"] = rooms;
    file.write(QJsonDocument(root).toJson());
}

// -------------------------------------------------------
// Prywatne: generowanie unikalnego ID
// -------------------------------------------------------

QString RoomManager::generateRoomId()
{
    const QString chars = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789"; // bez mylących 0/O/1/I
    QString id;
    for (int i = 0; i < 6; ++i) {
        id += chars[QRandomGenerator::global()->bounded(chars.size())];
    }

    // Upewnij się że ID jest unikalne
    if (validateRoomId(id)) {
        return generateRoomId(); // rekurencja, kolizja bardzo mało prawdopodobna
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

QString RoomManager::createRoom(const QString &roomName, const QString &gmUsername, const QString &system)
{
    QJsonArray rooms = loadRooms();

    QString newId = generateRoomId();

    QJsonObject room;
    room["id"]         = newId;
    room["name"]       = roomName;
    room["gm"]         = gmUsername;
    room["system"]     = system;
    room["created_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    room["players"]    = QJsonArray();   // na razie pusty
    room["characters"] = QJsonObject();  // klucz: username → dane postaci

    rooms.append(room);
    saveRooms(rooms);

    return newId; // serwer odeśle ten ID do GM-a
}

bool RoomManager::addPlayerToRoom(const QString &identifier, const QString &username)
{
    QJsonArray rooms = loadRooms();

    for (int i = 0; i < rooms.size(); ++i) {
        QJsonObject room = rooms[i].toObject();
        if (room["id"].toString() == identifier) {

            QJsonArray players = room["players"].toArray();

            // Sprawdź czy już jest w roomie
            for (const QJsonValue &p : players) {
                if (p.toString() == username) return true; // już jest, ok
            }

            players.append(username);
            room["players"] = players;
            rooms[i] = room;
            saveRooms(rooms);
            return true;
        }
    }
    return false; // nie znaleziono roomu
}

QJsonObject RoomManager::getRoomInfo(const QString &identifier)
{
    for (const QJsonValue &val : loadRooms()) {
        QJsonObject room = val.toObject();
        if (room["id"].toString() == identifier)
            return room;
    }
    return QJsonObject(); // pusty jeśli nie znaleziono
}

QJsonArray RoomManager::getRoomsForUser(const QString &username)
{
    QJsonArray result;
    for (const QJsonValue &val : loadRooms()) {
        QJsonObject room = val.toObject();

        // Dodaj jeśli jest GM-em
        if (room["gm"].toString() == username) {
            result.append(room);
            continue;
        }

        // Dodaj jeśli jest graczem
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