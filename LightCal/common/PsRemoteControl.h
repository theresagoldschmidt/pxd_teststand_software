#ifndef PSREMOTECONTROL_H
#define PSREMOTECONTROL_H

// ******************************************************************
// server class to communicate with power supply control programm
//
//
//*********************************************************************




#include  "ui_server_ui.h"
#include "StartStop.h"
#include <QMainWindow>
#include <QTextStream>
#include <QFile>
#include <QDateTime>
#include <QDialog>
#include <qtcpsocket>
#include <QtNetwork>
#include <QNetworkInterface>



namespace Ui {
    class PSMainWin;
}

class StartStop;


class PSMainWin : public QMainWindow
{
    Q_OBJECT

friend class StartStop;

public:

        PSMainWin(QWidget *parent = 0);
	   ~PSMainWin();
	 
	   StartStop *sequenzer;

public slots:

    void sessionOpened();
    void sendLine();
    void sendData(const QString &);
    void connectionAccepted();
    void readSocket();
	void StartNode();
	void StopNode();
	void test();

signals:
void receivedData(QString *string);
void sigStopNode(int node);
void sigStartNode(int node);

private:

    Ui::PSMainWin *ui;
    
	

    char* buff;
      
	QTcpSocket *clientConnection;
    QTcpServer *tcpServer;

    QNetworkSession *networkSession;
    
	quint16 blockSize;

};

#endif // PSREMOTECONTROL_H

