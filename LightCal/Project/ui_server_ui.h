/********************************************************************************
** Form generated from reading UI file 'server_ui.ui'
**
** Created: Tue 13. Feb 16:29:20 2018
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVER_UI_H
#define UI_SERVER_UI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTextBrowser>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PSMainWin
{
public:
    QWidget *centralwidget;
    QPushButton *SendButton;
    QPushButton *CloseButton;
    QTextBrowser *StatusOutput;
    QPushButton *ResetButton;
    QLineEdit *PortEdit;
    QLabel *label;
    QLineEdit *IpEdit;
    QLabel *label_2;
    QLineEdit *DataEdit;
    QLabel *label_3;
    QPushButton *StartUpButton;
    QPushButton *ShutdownButton;
    QLineEdit *NodeEdit;
    QLabel *NodeLabel;
    QTabWidget *tabWidget;
    QWidget *SmuTab;
    QPushButton *SmuInitPush;
    QPushButton *SmuResetPush;
    QTextBrowser *SmuStatusBrowser;
    QPushButton *SmuTestPush;
    QPushButton *SmuTest2Push;
    QPushButton *SetVoltPush;
    QPushButton *SetCurrPush;
    QLineEdit *VoltCurrentEdit;
    QPushButton *SmuCompUPush;
    QPushButton *SmuCompIPush;
    QPushButton *SmuEnablePush;
    QPushButton *SmuDisablePush;
    QPushButton *SmuSourceIPush;
    QPushButton *SmuSourceUPush;
    QWidget *RelaisTab;
    QSpinBox *ChannelSpin;
    QPushButton *RelaisInitPush;
    QPushButton *RelaisResetPush;
    QTextBrowser *RelaisStatusBrowers;
    QWidget *ServerTab;
    QWidget *tab;
    QPushButton *CalibStartPush;
    QPushButton *CalibStopPush;
    QTextBrowser *CalibStatusBrowser;
    QProgressBar *ChProgBar;
    QProgressBar *TotProgBar;
    QLineEdit *ChStatusLineEdit;
    QPushButton *CalWaitPush;
    QPushButton *ReloadSettingsPush;
    QWidget *tab_2;
    QPushButton *StartOvpTest_PushButton;
    QPushButton *StopOvpTest_PushButton;
    QTextBrowser *OvpStatusBrowser;
    QLineEdit *InputNode;
    QLabel *label_4;
    QPushButton *PrepareGates_PushButton;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *PSMainWin)
    {
        if (PSMainWin->objectName().isEmpty())
            PSMainWin->setObjectName(QString::fromUtf8("PSMainWin"));
        PSMainWin->resize(752, 758);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PSMainWin->sizePolicy().hasHeightForWidth());
        PSMainWin->setSizePolicy(sizePolicy);
        centralwidget = new QWidget(PSMainWin);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        SendButton = new QPushButton(centralwidget);
        SendButton->setObjectName(QString::fromUtf8("SendButton"));
        SendButton->setGeometry(QRect(560, 20, 91, 41));
        CloseButton = new QPushButton(centralwidget);
        CloseButton->setObjectName(QString::fromUtf8("CloseButton"));
        CloseButton->setGeometry(QRect(560, 120, 91, 41));
        StatusOutput = new QTextBrowser(centralwidget);
        StatusOutput->setObjectName(QString::fromUtf8("StatusOutput"));
        StatusOutput->setGeometry(QRect(30, 530, 631, 181));
        sizePolicy.setHeightForWidth(StatusOutput->sizePolicy().hasHeightForWidth());
        StatusOutput->setSizePolicy(sizePolicy);
        ResetButton = new QPushButton(centralwidget);
        ResetButton->setObjectName(QString::fromUtf8("ResetButton"));
        ResetButton->setGeometry(QRect(560, 70, 91, 41));
        PortEdit = new QLineEdit(centralwidget);
        PortEdit->setObjectName(QString::fromUtf8("PortEdit"));
        PortEdit->setGeometry(QRect(70, 50, 113, 20));
        PortEdit->setCursorPosition(0);
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 50, 46, 13));
        QFont font;
        font.setPointSize(11);
        label->setFont(font);
        IpEdit = new QLineEdit(centralwidget);
        IpEdit->setObjectName(QString::fromUtf8("IpEdit"));
        IpEdit->setGeometry(QRect(70, 20, 113, 20));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 20, 46, 13));
        label_2->setFont(font);
        DataEdit = new QLineEdit(centralwidget);
        DataEdit->setObjectName(QString::fromUtf8("DataEdit"));
        DataEdit->setGeometry(QRect(20, 130, 511, 20));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 110, 101, 16));
        label_3->setFont(font);
        StartUpButton = new QPushButton(centralwidget);
        StartUpButton->setObjectName(QString::fromUtf8("StartUpButton"));
        StartUpButton->setGeometry(QRect(430, 20, 91, 41));
        ShutdownButton = new QPushButton(centralwidget);
        ShutdownButton->setObjectName(QString::fromUtf8("ShutdownButton"));
        ShutdownButton->setGeometry(QRect(430, 70, 91, 41));
        NodeEdit = new QLineEdit(centralwidget);
        NodeEdit->setObjectName(QString::fromUtf8("NodeEdit"));
        NodeEdit->setGeometry(QRect(310, 20, 91, 20));
        NodeLabel = new QLabel(centralwidget);
        NodeLabel->setObjectName(QString::fromUtf8("NodeLabel"));
        NodeLabel->setGeometry(QRect(260, 20, 46, 13));
        NodeLabel->setFont(font);
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(30, 190, 591, 301));
        SmuTab = new QWidget();
        SmuTab->setObjectName(QString::fromUtf8("SmuTab"));
        SmuInitPush = new QPushButton(SmuTab);
        SmuInitPush->setObjectName(QString::fromUtf8("SmuInitPush"));
        SmuInitPush->setGeometry(QRect(10, 40, 75, 23));
        SmuResetPush = new QPushButton(SmuTab);
        SmuResetPush->setObjectName(QString::fromUtf8("SmuResetPush"));
        SmuResetPush->setGeometry(QRect(10, 70, 75, 23));
        SmuStatusBrowser = new QTextBrowser(SmuTab);
        SmuStatusBrowser->setObjectName(QString::fromUtf8("SmuStatusBrowser"));
        SmuStatusBrowser->setGeometry(QRect(40, 130, 511, 121));
        sizePolicy.setHeightForWidth(SmuStatusBrowser->sizePolicy().hasHeightForWidth());
        SmuStatusBrowser->setSizePolicy(sizePolicy);
        SmuTestPush = new QPushButton(SmuTab);
        SmuTestPush->setObjectName(QString::fromUtf8("SmuTestPush"));
        SmuTestPush->setGeometry(QRect(100, 40, 75, 23));
        SmuTest2Push = new QPushButton(SmuTab);
        SmuTest2Push->setObjectName(QString::fromUtf8("SmuTest2Push"));
        SmuTest2Push->setGeometry(QRect(100, 70, 75, 23));
        SetVoltPush = new QPushButton(SmuTab);
        SetVoltPush->setObjectName(QString::fromUtf8("SetVoltPush"));
        SetVoltPush->setGeometry(QRect(380, 40, 75, 23));
        SetCurrPush = new QPushButton(SmuTab);
        SetCurrPush->setObjectName(QString::fromUtf8("SetCurrPush"));
        SetCurrPush->setGeometry(QRect(470, 40, 75, 23));
        VoltCurrentEdit = new QLineEdit(SmuTab);
        VoltCurrentEdit->setObjectName(QString::fromUtf8("VoltCurrentEdit"));
        VoltCurrentEdit->setGeometry(QRect(380, 10, 161, 20));
        SmuCompUPush = new QPushButton(SmuTab);
        SmuCompUPush->setObjectName(QString::fromUtf8("SmuCompUPush"));
        SmuCompUPush->setGeometry(QRect(380, 70, 75, 23));
        SmuCompIPush = new QPushButton(SmuTab);
        SmuCompIPush->setObjectName(QString::fromUtf8("SmuCompIPush"));
        SmuCompIPush->setGeometry(QRect(470, 70, 75, 23));
        SmuEnablePush = new QPushButton(SmuTab);
        SmuEnablePush->setObjectName(QString::fromUtf8("SmuEnablePush"));
        SmuEnablePush->setGeometry(QRect(300, 40, 75, 23));
        SmuDisablePush = new QPushButton(SmuTab);
        SmuDisablePush->setObjectName(QString::fromUtf8("SmuDisablePush"));
        SmuDisablePush->setGeometry(QRect(300, 70, 75, 23));
        SmuSourceIPush = new QPushButton(SmuTab);
        SmuSourceIPush->setObjectName(QString::fromUtf8("SmuSourceIPush"));
        SmuSourceIPush->setGeometry(QRect(200, 40, 75, 23));
        SmuSourceUPush = new QPushButton(SmuTab);
        SmuSourceUPush->setObjectName(QString::fromUtf8("SmuSourceUPush"));
        SmuSourceUPush->setGeometry(QRect(200, 70, 75, 23));
        tabWidget->addTab(SmuTab, QString());
        RelaisTab = new QWidget();
        RelaisTab->setObjectName(QString::fromUtf8("RelaisTab"));
        ChannelSpin = new QSpinBox(RelaisTab);
        ChannelSpin->setObjectName(QString::fromUtf8("ChannelSpin"));
        ChannelSpin->setGeometry(QRect(190, 70, 42, 22));
        RelaisInitPush = new QPushButton(RelaisTab);
        RelaisInitPush->setObjectName(QString::fromUtf8("RelaisInitPush"));
        RelaisInitPush->setGeometry(QRect(30, 50, 75, 23));
        RelaisResetPush = new QPushButton(RelaisTab);
        RelaisResetPush->setObjectName(QString::fromUtf8("RelaisResetPush"));
        RelaisResetPush->setGeometry(QRect(30, 80, 75, 23));
        RelaisStatusBrowers = new QTextBrowser(RelaisTab);
        RelaisStatusBrowers->setObjectName(QString::fromUtf8("RelaisStatusBrowers"));
        RelaisStatusBrowers->setGeometry(QRect(20, 140, 421, 121));
        sizePolicy.setHeightForWidth(RelaisStatusBrowers->sizePolicy().hasHeightForWidth());
        RelaisStatusBrowers->setSizePolicy(sizePolicy);
        tabWidget->addTab(RelaisTab, QString());
        ServerTab = new QWidget();
        ServerTab->setObjectName(QString::fromUtf8("ServerTab"));
        tabWidget->addTab(ServerTab, QString());
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        CalibStartPush = new QPushButton(tab);
        CalibStartPush->setObjectName(QString::fromUtf8("CalibStartPush"));
        CalibStartPush->setGeometry(QRect(20, 20, 75, 23));
        CalibStopPush = new QPushButton(tab);
        CalibStopPush->setObjectName(QString::fromUtf8("CalibStopPush"));
        CalibStopPush->setGeometry(QRect(20, 80, 75, 23));
        CalibStatusBrowser = new QTextBrowser(tab);
        CalibStatusBrowser->setObjectName(QString::fromUtf8("CalibStatusBrowser"));
        CalibStatusBrowser->setGeometry(QRect(40, 140, 501, 121));
        sizePolicy.setHeightForWidth(CalibStatusBrowser->sizePolicy().hasHeightForWidth());
        CalibStatusBrowser->setSizePolicy(sizePolicy);
        ChProgBar = new QProgressBar(tab);
        ChProgBar->setObjectName(QString::fromUtf8("ChProgBar"));
        ChProgBar->setGeometry(QRect(300, 80, 251, 23));
        ChProgBar->setValue(1);
        TotProgBar = new QProgressBar(tab);
        TotProgBar->setObjectName(QString::fromUtf8("TotProgBar"));
        TotProgBar->setGeometry(QRect(300, 20, 251, 23));
        TotProgBar->setValue(1);
        ChStatusLineEdit = new QLineEdit(tab);
        ChStatusLineEdit->setObjectName(QString::fromUtf8("ChStatusLineEdit"));
        ChStatusLineEdit->setGeometry(QRect(300, 50, 113, 20));
        CalWaitPush = new QPushButton(tab);
        CalWaitPush->setObjectName(QString::fromUtf8("CalWaitPush"));
        CalWaitPush->setGeometry(QRect(20, 50, 75, 23));
        ReloadSettingsPush = new QPushButton(tab);
        ReloadSettingsPush->setObjectName(QString::fromUtf8("ReloadSettingsPush"));
        ReloadSettingsPush->setGeometry(QRect(130, 20, 91, 23));
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        StartOvpTest_PushButton = new QPushButton(tab_2);
        StartOvpTest_PushButton->setObjectName(QString::fromUtf8("StartOvpTest_PushButton"));
        StartOvpTest_PushButton->setGeometry(QRect(40, 20, 75, 23));
        StopOvpTest_PushButton = new QPushButton(tab_2);
        StopOvpTest_PushButton->setObjectName(QString::fromUtf8("StopOvpTest_PushButton"));
        StopOvpTest_PushButton->setGeometry(QRect(140, 20, 75, 23));
        OvpStatusBrowser = new QTextBrowser(tab_2);
        OvpStatusBrowser->setObjectName(QString::fromUtf8("OvpStatusBrowser"));
        OvpStatusBrowser->setGeometry(QRect(40, 120, 511, 131));
        InputNode = new QLineEdit(tab_2);
        InputNode->setObjectName(QString::fromUtf8("InputNode"));
        InputNode->setGeometry(QRect(320, 20, 71, 20));
        label_4 = new QLabel(tab_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(250, 20, 47, 21));
        PrepareGates_PushButton = new QPushButton(tab_2);
        PrepareGates_PushButton->setObjectName(QString::fromUtf8("PrepareGates_PushButton"));
        PrepareGates_PushButton->setGeometry(QRect(40, 70, 181, 23));
        tabWidget->addTab(tab_2, QString());
        PSMainWin->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(PSMainWin);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        PSMainWin->setStatusBar(statusbar);

        retranslateUi(PSMainWin);

        tabWidget->setCurrentIndex(4);


        QMetaObject::connectSlotsByName(PSMainWin);
    } // setupUi

    void retranslateUi(QMainWindow *PSMainWin)
    {
        PSMainWin->setWindowTitle(QApplication::translate("PSMainWin", "PS Remote Control Server", 0, QApplication::UnicodeUTF8));
        SendButton->setText(QApplication::translate("PSMainWin", "Send", 0, QApplication::UnicodeUTF8));
        CloseButton->setText(QApplication::translate("PSMainWin", "Close", 0, QApplication::UnicodeUTF8));
        ResetButton->setText(QApplication::translate("PSMainWin", "Reset", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("PSMainWin", "Port", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("PSMainWin", "IP", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("PSMainWin", "DataToSend", 0, QApplication::UnicodeUTF8));
        StartUpButton->setText(QApplication::translate("PSMainWin", "StartUp", 0, QApplication::UnicodeUTF8));
        ShutdownButton->setText(QApplication::translate("PSMainWin", "Shutdown", 0, QApplication::UnicodeUTF8));
        NodeEdit->setText(QString());
        NodeLabel->setText(QApplication::translate("PSMainWin", "Node", 0, QApplication::UnicodeUTF8));
        SmuInitPush->setText(QApplication::translate("PSMainWin", "Init", 0, QApplication::UnicodeUTF8));
        SmuResetPush->setText(QApplication::translate("PSMainWin", "Rest", 0, QApplication::UnicodeUTF8));
        SmuTestPush->setText(QApplication::translate("PSMainWin", "GetVoltage", 0, QApplication::UnicodeUTF8));
        SmuTest2Push->setText(QApplication::translate("PSMainWin", "GetCurrent", 0, QApplication::UnicodeUTF8));
        SetVoltPush->setText(QApplication::translate("PSMainWin", "SetVoltage", 0, QApplication::UnicodeUTF8));
        SetCurrPush->setText(QApplication::translate("PSMainWin", "SetCurrent", 0, QApplication::UnicodeUTF8));
        VoltCurrentEdit->setText(QApplication::translate("PSMainWin", "0.1", 0, QApplication::UnicodeUTF8));
        SmuCompUPush->setText(QApplication::translate("PSMainWin", "SetVoltComp", 0, QApplication::UnicodeUTF8));
        SmuCompIPush->setText(QApplication::translate("PSMainWin", "SetCurrComp", 0, QApplication::UnicodeUTF8));
        SmuEnablePush->setText(QApplication::translate("PSMainWin", "Enable", 0, QApplication::UnicodeUTF8));
        SmuDisablePush->setText(QApplication::translate("PSMainWin", "Disable", 0, QApplication::UnicodeUTF8));
        SmuSourceIPush->setText(QApplication::translate("PSMainWin", "SourceI", 0, QApplication::UnicodeUTF8));
        SmuSourceUPush->setText(QApplication::translate("PSMainWin", "SourceU", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(SmuTab), QApplication::translate("PSMainWin", "SMU", 0, QApplication::UnicodeUTF8));
        RelaisInitPush->setText(QApplication::translate("PSMainWin", "Init", 0, QApplication::UnicodeUTF8));
        RelaisResetPush->setText(QApplication::translate("PSMainWin", "Rest", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(RelaisTab), QApplication::translate("PSMainWin", "Relais", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(ServerTab), QApplication::translate("PSMainWin", "Server", 0, QApplication::UnicodeUTF8));
        CalibStartPush->setText(QApplication::translate("PSMainWin", "Start", 0, QApplication::UnicodeUTF8));
        CalibStopPush->setText(QApplication::translate("PSMainWin", "Stop", 0, QApplication::UnicodeUTF8));
        CalWaitPush->setText(QApplication::translate("PSMainWin", "Wait", 0, QApplication::UnicodeUTF8));
        ReloadSettingsPush->setText(QApplication::translate("PSMainWin", "ReloadSetting", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("PSMainWin", "Callib", 0, QApplication::UnicodeUTF8));
        StartOvpTest_PushButton->setText(QApplication::translate("PSMainWin", "Start Test", 0, QApplication::UnicodeUTF8));
        StopOvpTest_PushButton->setText(QApplication::translate("PSMainWin", "Stop Test", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("PSMainWin", "<html><head/><body><p><span style=\" font-size:12pt;\">Node</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        PrepareGates_PushButton->setText(QApplication::translate("PSMainWin", "Set SW_SUB and SW_Ref to -15 V", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("PSMainWin", "OVP Test", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PSMainWin: public Ui_PSMainWin {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVER_UI_H
