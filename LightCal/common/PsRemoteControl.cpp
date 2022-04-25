
#include <QMainWindow>
#include <QTextStream>
#include <QFile>
#include <QDateTime>
#include <QtGui>
#include <QtNetwork>
#include <QChar>
#include <stdlib.h>
#include  "ui_server_ui.h"

#include "PsRemoteControl.h"
#include "StartStop.h"

PSMainWin::PSMainWin(QWidget *parent): QMainWindow(parent),  ui(new Ui::PSMainWin), tcpServer(0), networkSession(0)
{ 

    ui->setupUi(this);

 

    QNetworkConfigurationManager manager;

    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

        ui->StatusOutput->setText(tr("Opening network session."));
        networkSession->open();
    }
	else {
        sessionOpened();
    }

 
        connect(ui->CloseButton, SIGNAL(clicked()), this, SLOT(close()));
		connect(ui->SendButton, SIGNAL(clicked()), this, SLOT(sendLine()));

 //*****************************************************************
 //  sequenzer responsible for start stop sequences,
 // connection sequenzer - server  with via signal slot communication

sequenzer=new StartStop();

if(sequenzer==NULL){ui->StatusOutput->append("sequenzer not created"); return;}


connect(sequenzer, SIGNAL(SeqSendCMD(const QString&)),this, SLOT(sendData(const QString&)));
connect(this, SIGNAL(receivedData()),sequenzer, SLOT(SeqReceiveCMD()));

connect(ui->StartUpButton, SIGNAL(clicked()), this, SLOT(StartNode())); //sequenzer, SLOT(SeqStartUp(int)));
connect(ui->ShutdownButton,SIGNAL(clicked()), this, SLOT(StopNode()));  //sequenzer, SLOT(SeqStopPS(int)));

connect(this, SIGNAL(sigStartNode(int)), sequenzer, SLOT(SeqStartUp(int)));
connect(this,SIGNAL(sigStopNode(int)), sequenzer, SLOT(SeqStopPS(int)));

 //*****************************************************************



}

PSMainWin::~PSMainWin()
{
	delete sequenzer;
    delete ui;
}

void PSMainWin::test(){

QString string="test close";

sendData(string);


}


void PSMainWin::connectionAccepted(){

  ui->StatusOutput->append("connection accepted");

   clientConnection = tcpServer->nextPendingConnection();
 
    connect(clientConnection, SIGNAL(readyRead()), this, SLOT(readSocket()));

}

void PSMainWin::readSocket(){

   clientConnection->readBufferSize();
   QByteArray data = clientConnection->readAll();
   ui->StatusOutput->append((const char *) data);
   QString string((const char *) data);  
    emit receivedData(&string); // emit signal with data attached

}



void PSMainWin::sessionOpened()
{
    // Save the used configuration
    if (networkSession) {
        QNetworkConfiguration config = networkSession->configuration();
        QString id;
        if (config.type() == QNetworkConfiguration::UserChoice)
            id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
        else
            id = config.identifier();

        QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
        settings.endGroup();
    }

//! [0] //! [1]
    tcpServer = new QTcpServer(this);

     connect(tcpServer, SIGNAL(newConnection()), this, SLOT(connectionAccepted()));

   
	 
	 //QHostAddress::Any
	 if (!tcpServer->listen()) {  //QHostAddress::LocalHost,54332
        QMessageBox::critical(this, tr("PS Control Server"),
                              tr("Unable to start the server: %1.")
                              .arg(tcpServer->errorString()));
        close();
        return;
    }
//! [0]
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty()) ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
   
	ui->StatusOutput->append(tr("The server is running on\n\nIP: %1\nport: %2\n\n"
                            "Please run the PS GUI now.")
                         .arg(ipAddress).arg(tcpServer->serverPort()));
	
	QString port;
	port.setNum( tcpServer->serverPort());
	ui->IpEdit->setText(ipAddress);
   ui->PortEdit->setText(port);

}

void PSMainWin::StartNode(){
QString str=ui->NodeEdit->text();

int node=str.toInt();

emit sigStartNode(node);

}

void PSMainWin::StopNode(){

int node=ui->NodeEdit->text().toInt();

emit sigStopNode(node);

}

void PSMainWin::sendLine()
{

   
	QByteArray block;
	
	QString string=ui->DataEdit->text();
    
	block.append(string);

	if(!clientConnection->isOpen()){
		clientConnection = tcpServer->nextPendingConnection();}

    clientConnection->write((const char*)block); 
 
}


void PSMainWin::sendData(const QString & string){

ui->StatusOutput->append("sendData()");
   
QByteArray block;

//QString *string=sequenzer->buff;

//QChar *buff=string->data();

block.append(string); // *string //((const char *)string->data()));

	if(!clientConnection->isOpen()){
		clientConnection = tcpServer->nextPendingConnection();}

	ui->StatusOutput->append(string);
    

clientConnection->write((const char*)block); 


}