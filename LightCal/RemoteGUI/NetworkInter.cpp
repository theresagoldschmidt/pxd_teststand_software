

#include <QtGui>
#include "NetworkInter.h"
#include "ui_NetworkInterface.h"
//#include "wrapperHeader.h"

//#include <QTest>

 
Form::Form(QWidget *parent) :
    QWidget(parent), ui(new Ui::Form) ,networkSession(0)
	{
    ui->setupUi(this);
	 
	data=new QString();

	 ui->HostLabel->setText("&Server name:");
     ui->PortLabel->setText("S&erver port:");
	
	//Networking setup
	// find out which IP to connect to
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
     if (ipAddress.isEmpty()){
		 ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
	 }
	 
	  ui->HostLineEdit->setText(ipAddress);
      ui->PortLineEdit->setValidator(new QIntValidator(1, 65535, this));
      QString DfPort="6666";  
	  ui->PortLineEdit->setText(DfPort);
      ui->HostLabel->setBuddy(ui->HostLineEdit);
      ui->PortLabel->setBuddy(ui->PortLineEdit);

	   ui->StatusOutput->append("One has to run a server to");
   
	//   ui->SendBut->setDefault(true);
       ui->SendBut->setEnabled(true);

	   
   tcpSocket = new QTcpSocket(this);

//	 connect(ui->HostLineEdit, SIGNAL(textChanged(QString)),  this, SLOT(enableSendBut()));
  //   connect(ui->PortLineEdit, SIGNAL(textChanged(QString)),  this, SLOT(enableSendBut()));
    
   connect(ui->InitBut, SIGNAL(clicked()), this, SLOT(Connect2Server()));  // opens connection to server
   connect(ui->CloseBut, SIGNAL(clicked()), this, SLOT(CloseConnection()));  // closes connection

    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readSocket())); // if data recieved read socket called
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
   connect(ui->SendBut,SIGNAL(clicked()), this, SLOT(SendString())); // sends data from the line edit
	
   connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(restartConnection())); // allows to restart connection after connection loss
	 	 
     QNetworkConfigurationManager manager;

	 ui->StatusOutput->append("Manager is invoced");

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
		
         ui->StatusOutput->append("Opening network session.");
         networkSession->open();
     }
 
	//Networking setup end
	
//connect(ui->InitBut, SIGNAL(clicked()), this, SLOT(OpenConnection()));


}

 Form::~Form()
{
    delete ui;
}

void  Form::DoSomething(int node, int channel){
//	ui->calibrateNodeNumber->setValue(node);
	//ui->calibrationChannel->setCurrentIndex(channel);
}



void Form::CloseConnection(){


}

//This function is called when the Set Calibrated Value Button has been pressed
void Form::OpenConnection(){
	ui->StatusOutput->append("open connection");

	}

void Form::SendString(){


    QString string = ui->Data2Send->text();
    QByteArray array;
    array.append(string);
   tcpSocket->write(array);
		
	ui->StatusOutput->append("send something");

}

void Form::SendData(const QString& Data){

    QString string(Data);
    QByteArray array;
    array.append(Data); //string);
   tcpSocket->write(array);
	
	QString buff="Send: ";
	buff.append(Data);

	ui->StatusOutput->append((const char*)buff.data());

}

 void Form::displayError(QAbstractSocket::SocketError socketError)
 {
     switch (socketError) {
     case QAbstractSocket::RemoteHostClosedError:
         break;
     case QAbstractSocket::HostNotFoundError:
         QMessageBox::information(this, tr("Networking"),
                                  tr("The host was not found. Please check the "
                                     "host name and port settings."));
         break;
     case QAbstractSocket::ConnectionRefusedError:
         QMessageBox::information(this, tr("Networking"),
                                  tr("The connection was refused by the peer. "
                                     "Make sure the fortune server is running, "
                                     "and check that the host name and port "
                                     "settings are correct."));
         break;
     default:
         QMessageBox::information(this, tr("Networking"),
                                  tr("The following error occurred: %1.")
                                  .arg(tcpSocket->errorString()));
     }
	 
 
 }


  void Form::ReadString()
 {
     
	  ui->StatusOutput->append("Read String");
	 QDataStream in(tcpSocket);
     in.setVersion(QDataStream::Qt_4_0);

     if (blockSize == 0) {
         if (tcpSocket->bytesAvailable() < (int)sizeof(quint16))
             return;

         in >> blockSize;
     }

     if (tcpSocket->bytesAvailable() < blockSize)
         return;

     QString string;
     in >> string; 
   
	 ui->StatusOutput->append((const QString) string);

    
 }

  void Form::readSocket(){

   tcpSocket->readBufferSize();
   QByteArray data = tcpSocket->readAll();
   ui->StatusOutput->append((const char *) data);
   QString buff((const char *) data);
   
   emit ReceivedData(&buff);

}


   void Form::sessionOpened()
 {
     // Save the used configuration
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

   ui->StatusOutput->append("Need server as well"); 
 //  enableSendBut();
   
 }


    void Form::enableSendBut()
 {
  //  ui->SendBut->setEnabled((!networkSession || networkSession->isOpen()) &&
    //                             !ui->HostLineEdit->text().isEmpty() &&
      //                        !ui->PortLineEdit->text().isEmpty());

 }

void Form::Connect2Server(){

 ui->SendBut->setEnabled(true);
     blockSize = 0;
     tcpSocket->abort();
     hostIP=ui->HostLineEdit->text();
	 hostPort=ui->PortLineEdit->text().toInt();
     ui->StatusOutput->append("opend awerer"); 
     
	 
	 tcpSocket->connectToHost(ui->HostLineEdit->text(),ui->PortLineEdit->text().toInt()); //Write);
	ui->StatusOutput->append("opend awerer"); 
	
 
	if(tcpSocket->waitForConnected()) 
    {
        QString string = "LMU PS DEMONSTRATOR";
        QByteArray array;
        array.append(string);
        tcpSocket->write(array);
		ui->StatusOutput->append("opend connection"); 
    }
    else
    {
        ui->StatusOutput->append("couldend connect"); 
    } 
 
}

void Form::restartConnection(){
//	  QTest::qWait(4000);
	  ui->StatusOutput->append("try to restart connection"); 
	//  Connect2Server();
	  
}
 
void Form::GetInfo(const char* info){

ui->StatusOutput->append(info);


}



