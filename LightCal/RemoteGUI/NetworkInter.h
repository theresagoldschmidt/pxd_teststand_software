
#ifndef NETWORKINTER_H
#define NETWORKINTER_H


#include <QWidget>
//#include "calibrationwindow.h"
#include <qtcpsocket> 
#include <QtNetwork>
#include <QNetworkInterface>

//#include "monitoringwindow.h"
 
// class QTcpSocket;
// class QNetworkSession;



namespace Ui {
class Form;
}


class Form : public QWidget
{
    Q_OBJECT
    
public:
    explicit  Form(QWidget *parent = 0);
    ~Form();
	void DoSomething(int node, int channel);
 
signals:
	void ReceivedData(QString *info); // emmited when data received

public slots:
  void GetInfo(const char* info); // slot to display infos in gui
  void SendData(const QString& Data);

private slots:
    void restartConnection();
    void OpenConnection();
	void CloseConnection();
    void SendString();
	void displayError(QAbstractSocket::SocketError socketError);
	void ReadString();
	void sessionOpened();
	void enableSendBut();
	void Connect2Server();
	void readSocket();
	
private:
    Ui::Form *ui;
	//Stuff
	int divider;
	QString *data;
	
	
	// NETWORKING STUFF
	 QTcpSocket *tcpSocket;
     quint16 blockSize;
	 QNetworkSession *networkSession;
     QString hostIP;
	 quint16 hostPort;
};

#endif // NETWORKINTER_H
