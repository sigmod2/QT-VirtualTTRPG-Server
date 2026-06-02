#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    TCP_Server = new QTcpServer(this); //init server object

    connect(TCP_Server, &QTcpServer::newConnection, this, &MainWindow::newConnection);
    if (TCP_Server->listen(QHostAddress::AnyIPv4, 43000)) //Tcp server will listenon 43000 port number and my system Ip
    {
        ui->statusbar->showMessage("TCP Server Started");
    }
    else
    {
        QMessageBox::information(this, "Qt With Ketan", "Server Start Failed somehow");
    }

    m_userManager = new UserManager(QCoreApplication::applicationDirPath() + "/users.json"); //pointer na UserManagera
    m_roomManager = new RoomManager(QCoreApplication::applicationDirPath() + "/rooms.json");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::readSocket()
{
 //code for recived file from client
    QTcpSocket *socket = reinterpret_cast< QTcpSocket*>(sender());

    QByteArray DataBuffer;
    QDataStream socketstream(socket);
    socketstream.setVersion(QDataStream::Qt_5_15); //again remeber to set proper version later

    socketstream.startTransaction();
    socketstream >> DataBuffer; // Get Data From Socket Or From Client Socket


    if (socketstream.commitTransaction() == false)
    {
        return;
    }

    //Now getFile Header Like FileName And Size Also

    QString HeaderData = DataBuffer.mid(0, 128); //Extract Header Data

    if (HeaderData.startsWith("filename:"))
    {
        QString Filename = HeaderData.split(",")[0].split(":")[1]; // Get FullFile Name
        QString FileExt = Filename.split(".")[1]; //Get File Ext
        QString FileSize = HeaderData.split(",")[1].split(":")[1]; // Get File Size

        DataBuffer = DataBuffer.mid(128); // Now Get File Data Only

        QString SaveFilePath = QCoreApplication::applicationDirPath() + "/" + Filename;

        QFile File(SaveFilePath);

        if (File.open(QIODevice::WriteOnly))
        {
            File.write(DataBuffer);
            File.close();

        }
    }
    else if (HeaderData.startsWith("/login"))
    {
        QString login_attempt = QString::fromUtf8(DataBuffer);
        QStringList parts = login_attempt.trimmed().split(" ");
        if (parts.size() == 3) {
            QString username = parts[1];
            QString password = parts[2];
            bool ok = m_userManager->validateUser(username, password);

            if (ok) {
                m_socketToUsername[socket] = username;
                QString role = m_userManager->getUserRole(username);
                QJsonArray rooms = m_roomManager->getRoomsForUser(username);
                QString roomsStr = QString::fromUtf8(
                    QJsonDocument(rooms).toJson(QJsonDocument::Compact));
                sendToSocket(socket, "auth_ok|" + role + "|" + roomsStr);
            } else {
                sendToSocket(socket, "auth_fail");
            }
        }
    }
    else if (HeaderData.startsWith("/register"))
    {
        QString cmd = QString::fromUtf8(DataBuffer).trimmed();
        QStringList parts = cmd.split(" ");
        // Format: /register username password role
        if (parts.size() == 4) {
            bool ok = m_userManager->registerUser(parts[1], parts[2], parts[3]);
            QString response = ok ? "register_ok" : "register_fail";
            QDataStream stream(socket);
            stream.setVersion(QDataStream::Qt_5_15);
            stream << response.toUtf8();
        }
    }
    else if (HeaderData.startsWith("/create_room"))
    {
        QString cmd = QString::fromUtf8(DataBuffer).trimmed();
        // Format: /create_room NazwaPokoyu System
        // Nazwa może mieć spacje więc bierzemy od 2 tokena do przedostatniego
        QStringList parts = cmd.split(" ");
        if (parts.size() >= 3) {
            QString gmUsername = m_socketToUsername.value(socket, "");
            if (gmUsername.isEmpty()) {
                sendToSocket(socket, "error|Nie jesteś zalogowany");
                return;
            }
            QString system   = parts.last();             // ostatni token = system
            QStringList nameParts = parts.mid(1, parts.size() - 2);
            QString roomName = nameParts.join(" ");      // środek = nazwa

            QString newId = m_roomManager->createRoom(roomName, gmUsername, system);
            sendToSocket(socket, "room_created|" + newId + "|" + roomName);

            ui->textEdit_Messages->append("Room created: " + newId +
                                          " by " + gmUsername);
        }
    }
    else if (HeaderData.startsWith("/join_room"))
    {
        QStringList parts = QString::fromUtf8(DataBuffer).trimmed().split(" ");
        if (parts.size() == 2) {
            QString roomId   = parts[1];
            QString username = m_socketToUsername.value(socket, "");

            if (username.isEmpty()) {
                sendToSocket(socket, "error|Nie jesteś zalogowany");
                return;
            }
            if (!m_roomManager->validateRoomId(roomId)) {
                sendToSocket(socket, "join_fail|Nie znaleziono pokoju");
                return;
            }

            m_roomManager->addPlayerToRoom(roomId, username);
            QJsonObject info = m_roomManager->getRoomInfo(roomId);
            QString infoStr  = QString::fromUtf8(
                QJsonDocument(info).toJson(QJsonDocument::Compact));

            sendToSocket(socket, "join_ok|" + infoStr);
        }
    }

    else if (HeaderData.startsWith("/my_rooms"))
    {
        QString username = m_socketToUsername.value(socket, "");
        if (username.isEmpty()) {
            sendToSocket(socket, "error|Nie jesteś zalogowany");
            return;
        }
        QJsonArray rooms = m_roomManager->getRoomsForUser(username);
        QString roomsStr = QString::fromUtf8(
            QJsonDocument(rooms).toJson(QJsonDocument::Compact));
        sendToSocket(socket, "rooms_list|" + roomsStr);
    }
    else if (HeaderData.startsWith("/")) //prawidiwe komendy
    {
        QString command = QString::fromUtf8(DataBuffer);
        if (command.startsWith("/roll"))
        {
            QStringList segmenty = command.split(" ");
            if (segmenty.size() != 2 || !segmenty[1].contains("d")){
                ui->textEdit_Messages->append("Error /roll ");}
            QStringList rzuty = segmenty[1].split("d");
            int ile = rzuty[0].toInt();
            int jakich = rzuty[1].toInt();
            int wynik = 0;
            srand(time(NULL));
            for (int i=0; i<ile; i++){
                wynik+=(1 + (rand() % jakich));
            }

            QString Message_For_Client = "Wynik rzutu to: " + QString::number(wynik);
            foreach (QTcpSocket *sockettemp, Client_List)
            {
                QDataStream stream(sockettemp);
                stream.setVersion(QDataStream::Qt_5_15);
                stream << Message_For_Client.toUtf8();
            }
        ui->textEdit_Messages->append("Wykonano /roll ");

        }
        else if (command.startsWith("/stats"))
        {

        }
        else if (command.startsWith("/wounds"))
        {

        }
        else if (command.startsWith("/inv"))
        {

        }
        else if (command.startsWith("/add"))
        {

        }
        else if (command.startsWith("/remove"))
        {

        }
        else if (command.startsWith("/money"))
        {

        }
        else if (command.startsWith("/write")) //to opcjonalnie, bo będzie trudne
        {

        }
        else if (command.startsWith("/me"))
        {

        }
        else if (command.startsWith("/help"))
        {
            //test
        }
        else if (command.startsWith("/attack")) //jesli znajde czas
        {

        }
        else
        {

            QString Information = "Command not found - please use /help";

            foreach (QTcpSocket *sockettemp, Client_List)
            {
                QDataStream stream(sockettemp);
                stream.setVersion(QDataStream::Qt_5_15);
                stream << Information.toUtf8();
            }
        }
    }
    else
    {
        QString message = QString::fromUtf8(DataBuffer);
        ui->textEdit_Messages->append("Klient " + QString::number(socket->socketDescriptor()) + ": " + message);
    }
}

void MainWindow::discardSocket()
{
    QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(sender());

    m_socketToUsername.remove(socket);

    int idx = Client_List.indexOf(socket);
    if (idx > -1) Client_List.removeAt(idx);

    ui->textEdit_Messages->append("Client Disconnected: " + QString::number(socket->socketDescriptor()));
    ui->comboBox_Client_List->clear();
    foreach (QTcpSocket *s, Client_List)
    {
        ui->comboBox_Client_List->addItem(QString::number(s->socketDescriptor()));
    }

    socket->deleteLater();
}



// Constantly checking for incomming connections, until that accours
void MainWindow::newConnection()
{
    while (TCP_Server->hasPendingConnections())
    {
        // Adding new TCP Client connection
        AddToSocketList(TCP_Server->nextPendingConnection());
    }
}




//This Function Is uded for storing all new client connections and assigning it's signal and slot
void MainWindow::AddToSocketList(QTcpSocket *socket)
{
    Client_List.append(socket);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readSocket);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::discardSocket);

    ui->textEdit_Messages->append("Client Is connected with Server : Socket ID : " + QString::number(socket->socketDescriptor()));

    ui->comboBox_Client_List->addItem(QString::number(socket->socketDescriptor()));

    //QString Client = "Client : " + QString::number(socket->socketDescriptor()) = " Connected With The Server.";
    //ui->textEdit_Client_Messages->append(Client);
}


void MainWindow::Send_File(QTcpSocket *socket, QString filename)
{
    if (socket)
    {
        if (socket->isOpen())
        {
            QFile filedata(filename);
            if (filedata.open(QIODevice::ReadOnly))
            {
                QFileInfo fileinfo(filedata);
                QString FileNameWithExt(fileinfo.fileName());

                QDataStream socketstream(socket);
                socketstream.setVersion(QDataStream::Qt_5_15); //insert proper version later

                //This code use file name also with file data
                QByteArray header;
                header.prepend("filename:" + FileNameWithExt.toUtf8() + ",filesize:" + QString::number(filedata.size()).toUtf8());
                header.resize(128);

                //Now Let's Add FileData
                QByteArray ByteFileData = filedata.readAll();
                ByteFileData.prepend(header);

                //Write in Socket
                socketstream << ByteFileData;

                //I strictly need to further study this code
            }
            else
            {
                qDebug() << "File Not Open";
            }
        }
        else
        {
            qDebug() << "Client Socket Not Open";
        }
    }
    else
    {
        qDebug() << "Client Socket is invalid";
    }
}


void MainWindow::on_pushButton_Send_File_clicked()
{
    //Let's Send File To The Client


    //Browse File
    QString FilePath= QFileDialog::getOpenFileName(this, "Select File ", QCoreApplication::applicationDirPath(), "File (*.jpg *.txt *.png *.bmp)");


    // Send File to All Connected Clients
    if(ui->comboBox_Transfer_Type->currentText() == "Broadcast")
    {
        foreach (QTcpSocket *sockettemp, Client_List)
        {
            Send_File(sockettemp, FilePath);
        }
    }
    else if(ui->comboBox_Transfer_Type->currentText() == "receiver")
    {
        // Send file only to the selected client
        QString receiverid = ui->comboBox_Client_List->currentText();
        foreach (QTcpSocket *sockettemp, Client_List)
        {
            if (sockettemp->socketDescriptor() == receiverid.toLongLong())
            {
                Send_File(sockettemp, FilePath);
            }
        }
    }
}

void MainWindow::on_pushButton_Send_Text_clicked()
{
    QString Message_For_Client = ui->lineEdit_Message->text();
    QString ReceiveType = ui->comboBox_Transfer_Type->currentText();


    //sending to all the clients
    if(ReceiveType == "Broadcast")
    {
        foreach (QTcpSocket *sockettemp, Client_List)
        {
            QDataStream stream(sockettemp);
            stream.setVersion(QDataStream::Qt_5_15);
            stream << Message_For_Client.toUtf8();
        }
    }
    else if(ReceiveType == "receiver")
    {
        // Send  only to the selected client
        QString receiverid = ui->comboBox_Client_List->currentText();

        foreach (QTcpSocket *sockettemp, Client_List)
        {
            if (sockettemp->socketDescriptor() == receiverid.toLongLong())
            {
                QDataStream stream(sockettemp);
                stream.setVersion(QDataStream::Qt_5_15);
                stream << Message_For_Client.toUtf8();
            }
        }
    }
    ui->lineEdit_Message->clear();
}

void MainWindow::sendToSocket(QTcpSocket *socket, const QString &message)
{
    QDataStream stream(socket);
    stream.setVersion(QDataStream::Qt_5_15);
    stream << message.toUtf8();
}
