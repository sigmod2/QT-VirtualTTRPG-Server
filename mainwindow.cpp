#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    else
    {
        QString message = QString::fromUtf8(DataBuffer);
        ui->textEdit_Messages->append("Klient " + QString::number(socket->socketDescriptor()) + ": " + message);
    }
}

void MainWindow::discardSocket()
{
    //removing client from listwhen client is disconnected
    QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(sender());

    int idx = Client_List.indexOf(socket);
    if (idx > -1)
    {
        Client_List.removeAt(idx);
    }

    //I think it's better to also print out that the connection is out.
    ui->textEdit_Messages->append("Client Disconnected : Socket ID : " + QString::number(socket->socketDescriptor()));

    //Refresch Client List ComboBox
    ui->comboBox_Client_List->clear();
    foreach (QTcpSocket *sockettemp, Client_List)
    {
        ui->comboBox_Client_List->addItem(QString::number(sockettemp->socketDescriptor()));
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

/*
// ReAd Daata From Client
void MainWindow::Read_Data_From_Socket()
{
    QTcpSocket *socket = reinterpret_cast<QTcpSocket*>(sender());

    QByteArray Message_From_Server = socket->readAll();

    QString Message = "Client : " + QString::number(socket->socketDescriptor()) + " :: " + QString::fromStdString(Message_From_Server.toStdString());

    ui->textEdit_Client_Messages->append(Message);
}*/




/*
void MainWindow::on_pushButton_Send_clicked()
{
    QString Message_For_Client = ui->lineEdit_Message_For_Client->text();
    QString Receiver = ui->comboBox_Client_List->currentText();
    if (ui->comboBox_Send_Message_Type->currentText() == "All") // Send Messge To All Connectd Clients
    {
         foreach(QTcpSocket *socket, Client_Connection_List)
        {
             socket->write(Message_For_Client.toStdString().c_str());
        }

    }
    else // Send Message to Only Selected Client
    {
        foreach(QTcpSocket *socket, Client_Connection_List)
        {
            if (socket->socketDescriptor() == Receiver.toLongLong())
            {
                socket->write(Message_For_Client.toStdString().c_str());
            }
        }
    }
}
*/


void MainWindow::on_pushButton_Send_Text_clicked()
{
    QString Message_For_Client = ui->lineEdit_Message->text();
    QString ReceiveType = ui->comboBox_Transfer_Type->currentText();

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
}

