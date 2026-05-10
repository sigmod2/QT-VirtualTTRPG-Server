/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QTextEdit *textEdit_Messages;
    QLineEdit *lineEdit_Message;
    QGridLayout *gridLayout;
    QPushButton *pushButton_Send_File;
    QPushButton *pushButton_Send_Text;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *comboBox_Transfer_Type;
    QLabel *label_2;
    QComboBox *comboBox_Client_List;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(838, 572);
        QFont font;
        font.setBold(true);
        MainWindow->setFont(font);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setObjectName("gridLayout_2");
        textEdit_Messages = new QTextEdit(centralwidget);
        textEdit_Messages->setObjectName("textEdit_Messages");
        QFont font1;
        font1.setPointSize(15);
        font1.setBold(true);
        textEdit_Messages->setFont(font1);

        gridLayout_2->addWidget(textEdit_Messages, 0, 0, 1, 1);

        lineEdit_Message = new QLineEdit(centralwidget);
        lineEdit_Message->setObjectName("lineEdit_Message");
        lineEdit_Message->setFont(font1);

        gridLayout_2->addWidget(lineEdit_Message, 1, 0, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        pushButton_Send_File = new QPushButton(centralwidget);
        pushButton_Send_File->setObjectName("pushButton_Send_File");
        pushButton_Send_File->setFont(font1);

        gridLayout->addWidget(pushButton_Send_File, 0, 0, 1, 1);

        pushButton_Send_Text = new QPushButton(centralwidget);
        pushButton_Send_Text->setObjectName("pushButton_Send_Text");
        pushButton_Send_Text->setFont(font1);

        gridLayout->addWidget(pushButton_Send_Text, 0, 1, 1, 1);


        gridLayout_2->addLayout(gridLayout, 2, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(centralwidget);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        comboBox_Transfer_Type = new QComboBox(centralwidget);
        comboBox_Transfer_Type->addItem(QString());
        comboBox_Transfer_Type->addItem(QString());
        comboBox_Transfer_Type->setObjectName("comboBox_Transfer_Type");
        QFont font2;
        font2.setPointSize(14);
        font2.setBold(true);
        comboBox_Transfer_Type->setFont(font2);

        horizontalLayout->addWidget(comboBox_Transfer_Type);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");
        label_2->setFont(font1);

        horizontalLayout->addWidget(label_2);

        comboBox_Client_List = new QComboBox(centralwidget);
        comboBox_Client_List->setObjectName("comboBox_Client_List");

        horizontalLayout->addWidget(comboBox_Client_List);


        gridLayout_2->addLayout(horizontalLayout, 3, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 838, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButton_Send_File->setText(QCoreApplication::translate("MainWindow", "Send File", nullptr));
        pushButton_Send_Text->setText(QCoreApplication::translate("MainWindow", "Send Text", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Transfer Type: ", nullptr));
        comboBox_Transfer_Type->setItemText(0, QCoreApplication::translate("MainWindow", "Broadcast", nullptr));
        comboBox_Transfer_Type->setItemText(1, QCoreApplication::translate("MainWindow", "receiver", nullptr));

        label_2->setText(QCoreApplication::translate("MainWindow", "Client List", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
