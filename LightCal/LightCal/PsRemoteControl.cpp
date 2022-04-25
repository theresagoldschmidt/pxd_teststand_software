
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
 // FTDI Stuff- Relaiscard


RelaisFtdi = new FTDI(this);

if(RelaisFtdi==NULL){ui->StatusOutput->append("relais interface not created"); return;}

connect(ui->RelaisInitPush,SIGNAL(clicked()),RelaisFtdi,SLOT(FTDI_OpenDevice()));
connect(ui->RelaisResetPush,SIGNAL(clicked()),RelaisFtdi,SLOT(reset()));
connect(ui->ChannelSpin,SIGNAL(valueChanged ( int  )),RelaisFtdi,SLOT(SET_Channel(int)));
connect(RelaisFtdi,SIGNAL(SendInfo(const QString &)),ui->RelaisStatusBrowers,SLOT(append( const QString &  )));

 //*****************************************************************
 // SMU
 //*****************************************************************

SourceMeasure = new SMU(this);
if( SourceMeasure == NULL) return;

connect(ui->SmuInitPush,SIGNAL(clicked()),SourceMeasure,SLOT(open_session()));
connect(ui->SmuResetPush,SIGNAL(clicked()),SourceMeasure,SLOT(reset_session()));

connect(SourceMeasure,SIGNAL(SendInfo(const QString &)),ui->SmuStatusBrowser,SLOT(append( const QString &  )));

connect(ui->SmuTestPush,SIGNAL(clicked()),SourceMeasure,SLOT(GetVoltage()));
connect(ui->SmuTest2Push,SIGNAL(clicked()),SourceMeasure,SLOT(GetCurrent()));
			
connect(ui->SmuSourceIPush,SIGNAL(clicked()),SourceMeasure,SLOT(SetSourceCurrent()));
connect(ui->SmuSourceUPush,SIGNAL(clicked()),SourceMeasure,SLOT(SetSourceVoltage()));

connect(ui->SetVoltPush,SIGNAL(clicked()),this,SLOT(SetSMUVoltage()));
connect(ui->SetCurrPush,SIGNAL(clicked()),this,SLOT(SetSMUCurrent()));

connect(ui->SetVoltPush,SIGNAL(clicked()),this,SLOT(SetSMUVoltage()));
connect(ui->SetCurrPush,SIGNAL(clicked()),this,SLOT(SetSMUCurrent()));

connect(ui->SmuCompIPush,SIGNAL(clicked()),this,SLOT(SetSmuCompCurrent()));
connect(ui->SmuCompUPush,SIGNAL(clicked()),this,SLOT(SetSmuCompVoltage()));

connect(ui->SmuEnablePush,SIGNAL(clicked()),SourceMeasure,SLOT(EnableOutput()));
connect(ui->SmuDisablePush,SIGNAL(clicked()),SourceMeasure,SLOT(DisableOutput()));


 //*****************************************************************
 // Callibration Manager
 //*****************************************************************


 CalibrationManager = new CalManager(this);
 
 connect(CalibrationManager,SIGNAL(SendInfo(const QString &)),ui->CalibStatusBrowser,SLOT(append( const QString &  )));  // status infos into TextBrowser
 connect(CalibrationManager,SIGNAL(progress(int,int)),this,SLOT(UpdateBar(int,int))); // callibration progress for bars
 connect(ui->CalibStopPush,SIGNAL(clicked()),CalibrationManager,SLOT(StopMeasurement())); // stop callibration 
  connect(ui->CalibStartPush,SIGNAL(clicked()),CalibrationManager,SLOT(start_cal_session())); // start callibration 

 connect(ui->ReloadSettingsPush,SIGNAL(clicked()),CalibrationManager,SLOT(ReloadSettings()));

connect(CalibrationManager, SIGNAL(SendCMD(const QString&)),this, SLOT(sendData(const QString&)));  // send command to ps via server
connect(this, SIGNAL( receivedData(const QString &)),CalibrationManager, SLOT(TakeData(const QString &))); //write data to manager
  
connect(SourceMeasure,SIGNAL(MeasuredVoltage(double)),CalibrationManager,SLOT(SmuVoltageIs(double)));
connect(SourceMeasure,SIGNAL(MeasuredCurrent(double)),CalibrationManager,SLOT(SmuCurrentIs(double)));

 CalibrationManager->SetSMU(SourceMeasure);
 CalibrationManager->SetRelaisCard(RelaisFtdi);


//*****************************************************************
// OVP Tester
//*****************************************************************

OVPTest = new OvpTester(this);

connect(ui->StartOvpTest_PushButton, SIGNAL(clicked()), OVPTest, SLOT(StartOvpTest()));
connect(ui->StopOvpTest_PushButton, SIGNAL(clicked()), OVPTest, SLOT(StopOvpTest()));
connect(ui->PrepareGates_PushButton, SIGNAL(clicked()), OVPTest, SLOT(PrepareGateOnTest()));
connect(OVPTest, SIGNAL(SendCMD(const QString&)),this, SLOT(sendData(const QString&)));  // send command to ps via server
connect(OVPTest,SIGNAL(SendInfo(const QString &)),ui->OvpStatusBrowser,SLOT(append( const QString &  )));

connect(OVPTest,SIGNAL(GetNodeGui()),this,SLOT(ReadNodeOVP()));
connect(this,SIGNAL(SendNodeGui(int)),OVPTest,SLOT(ReceiveNodeGui(int)));


OVPTest->SetSMU(SourceMeasure);
OVPTest->SetRelaisCard(RelaisFtdi); // gives the pointer to the class


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


void PSMainWin::SetChannel(){

int i= ui->ChannelSpin->value();

RelaisFtdi->SET_Channel(i);


}


void PSMainWin::ReadNodeOVP(){

	int node = (ui -> InputNode -> text()).toInt();

emit SendOVPNode(node);
emit SendNodeGui(node);

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
   emit receivedData(string); // emit signal with data attached

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
	 if (!tcpServer->listen(QHostAddress::Any,6666)) {  //QHostAddress::LocalHost,54332
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


void PSMainWin::UpdateBar(int total,int channel){


	ui->ChProgBar->setValue(channel);
	ui->TotProgBar->setValue(total);
	ui->ChProgBar->update();
	ui->TotProgBar->update();

}

void PSMainWin::SetSMUVoltage(){

double val =ui->VoltCurrentEdit->text().toDouble();

SourceMeasure->SetVoltage(val);

}

void PSMainWin::SetSMUCurrent(){
	
	double val =	ui->VoltCurrentEdit->text().toDouble();

	SourceMeasure->SourceCurrent(val);

	}

void PSMainWin::SetSmuCompCurrent(){
	
	double val =	ui->VoltCurrentEdit->text().toDouble();

	SourceMeasure->SetCurrentCompliance(val);

	}

void PSMainWin::SetSmuCompVoltage(){
	
	double val =	ui->VoltCurrentEdit->text().toDouble();

	SourceMeasure->SetVoltageCompliance(val);

	}


void  PSMainWin::SmuEnable(){ SourceMeasure->EnableOutput(); }

void  PSMainWin::SmuDisable(){ SourceMeasure->DisableOutput(); }


void PSMainWin::SmuSourceI(){}
void PSMainWin::SmuSourceU(){}