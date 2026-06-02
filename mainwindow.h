#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QList>
#include <QByteArray>
#include <QString>
#include <QMessageBox>
#include <QtGui>
#include <QtCore>
#include <QtWidgets>
#include "user_manager.h"
#include "room_manager.h"



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void readSocket();
    void discardSocket();

    void newConnection();
    void AddToSocketList(QTcpSocket *socket);

    void on_pushButton_Send_File_clicked();

    void on_pushButton_Send_Text_clicked();

private:
    void Send_File(QTcpSocket *socket, QString filename);
    void sendToSocket(QTcpSocket *socket, const QString &message);

    Ui::MainWindow *ui;
    QTcpServer *TCP_Server;
    QList<QTcpSocket*> Client_List;

    UserManager *m_userManager;
    RoomManager             *m_roomManager;                        // NOWE
    QMap<QTcpSocket*, QString> m_socketToUsername;
};
#endif // MAINWINDOW_H
