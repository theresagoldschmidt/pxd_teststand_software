/*
 * Copyright (c) 2011, fortiss GmbH.
 * All rights reserved.
 *
 * $Id$
 */

/**
 * \file
 *         Main Window
 *
 * \author
 *         Nadine Keddis <keddis@fortiss.org>
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wrapperHeader.h"

#include <QPainter>
#include <QColor>
#include <QPixmap>
#include <QList>
#include <QFileDialog>
#include <QDateTime>

#define RADIUS_GLOBAL 30
#define RADIUS_NODE 80

#define POS_X 20
#define POS_Y 5

#define NAME_COLUMN 0
#define IP_COLUMN 1
#define STATUS_COLUMN 2

#define OK 0
#define ERROR 1
#define NO_CONNECTION 2
#define EMERGENCY_STOP 3

#define START 0
#define STOP 1
#define CALIBRATE 2
#define BOOTLOAD 3
#define RECALIBRATE 4
#define COMMAND 5
#define EMERGENCY_SHUTDOWN 6

// File size (in Bytes) of the logging file. In case the logging data file exceeds 50MB a new file is opened.
//#define FILE_SIZE_LOG_FILE 52428800
#define FILE_SIZE_LOG_FILE 300000
#define FILE_SIZE_LOG_WINDOW 100000

typedef struct{
	uint8_t nodeNumber;
	uint8_t commandType;
}command_data_t;

//! Main Window constructor.

/// This function initializes the Main window and initializes Chromosome in order to start the connection with the PS Nodes.*/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFocusPolicy(Qt::StrongFocus);
    this->setWindowTitle("Belle II");
	this->setAttribute(Qt::WA_DeleteOnClose);

    ui->logView->setReadOnly(true);

    ui->statusTable->setColumnCount(3);
    ui->statusTable->setRowCount(200);
    ui->statusTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QStringList headerLabels;
	for (int i = 0; i < ui->statusTable->rowCount(); ++i)
	{
	  headerLabels << QString::number(i);
	}
	ui->statusTable->setVerticalHeaderLabels(headerLabels);

    QList<QString> headers;
    headers.append("Node Number");
    headers.append("MAC Address");
    headers.append("Status");
    ui->statusTable->setHorizontalHeaderLabels(headers);

	monitoring = new monitoringWindow;
	if (monitoring == NULL){ return; }
	monitoring->setWindowTitle("Monitoring");
	monitoring->initTable();

	window = new calibrationWindow;
	if (window == NULL){ return; }
	window->setWindowTitle("Calibration");

 	NetworkingWindow = new Form;  // class with socket to connect to daq server
 	if (NetworkingWindow == NULL){ return; }
 	NetworkingWindow->setWindowTitle("Networking");
    DaqProc=new ServerCmdProc(this);  // class to process commands
    if (DaqProc==NULL){ return; }
   
	init();
    connect(ui->bootloadButton, SIGNAL(clicked()), this, SLOT(bootloadNodes()));
    connect(ui->calibrateButton, SIGNAL(clicked()), this, SLOT(calibrateNodes()));
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(start()));
    connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stop()));
	connect(ui->emergencyButton, SIGNAL(clicked()), this, SLOT(emergencyShutdown()));
	connect(ui->statusTable, SIGNAL(cellClicked(int, int)), this, SLOT(showMonitoring(int, int)));
	connect(this, SIGNAL(newNodeChannel(int, int)), this, SLOT(writeNodeChannel(int, int)));
	connect(this, SIGNAL(newText(int, const QString &, int)), this, SLOT(writeLogdata(int, const QString &, int)));
	connect(window, SIGNAL(updateInfo(int , int, int, int)), this, SLOT(updateMonitoring(int, int, int, int)));

	connect(window,SIGNAL(SendInfo(const QString &)),ui->logView,SLOT(append(const QString &)));
	
	connect(this, SIGNAL(newTextLog(int, const QString &, int)), this, SLOT(writeLogdataInFile(int, const QString &, int)));

	
	connect(monitoring,SIGNAL(StartNode(int)),this,SLOT(StartNode(int)));
	connect(monitoring,SIGNAL(StopNode(int)),this,SLOT(StopNode(int)));
    connect(monitoring,SIGNAL(ShutdownNode(int)),this,SLOT(ShutdownNode(int)));
	
	// new slots for individual current voltage update
	connect(DaqProc,SIGNAL(updateVoltage(int,int,int)),monitoring, SLOT(updateVoltage(int, int, int))); // put numbers into gui
	connect(DaqProc,SIGNAL(updateCurrent(int,int,int)),monitoring, SLOT(updateCurrent(int, int, int))); // put numbers into gui


	//**********************************************************************
	// new stuff from LMU
	connect(ui->pushButton,SIGNAL(clicked()), this, SLOT(showNetworking())); // shows network interface
	connect(DaqProc,SIGNAL(GetVoltage(int,int)),monitoring,SLOT(monGetVoltage(int, int)));
	connect(DaqProc,SIGNAL(LogInfo(const char*)),NetworkingWindow,SLOT(GetInfo(const char*)));// moves logging string to networking window
	connect(monitoring,SIGNAL(monSendSignals(const QString&)),NetworkingWindow,SLOT(SendData(const QString&))); // send data from monitoring via server
	connect(NetworkingWindow,SIGNAL(ReceivedData(QString *)),DaqProc,SLOT(SetCMD(QString *)));
	connect(DaqProc,SIGNAL(updateInfo(int,int,int,int)),this, SLOT(updateMonitoring(int, int, int, int))); // put numbers into gui
	is_updated=false; // bool to sync data
	//**********************************************************************
	
	
	// Start Chromosome
    startupWrapper();
}

/// Main Window destructor.
/// This function firstly shuts down Chromosome and in that way stops the connection between the GUI and the PS module and then closes the Main and Monitoring Window.
MainWindow::~MainWindow()
{
	shutdownWrapper();
	monitoring->calWind->close();
	monitoring->close();
	window->close();
	NetworkingWindow->close();
    delete ui;
}
///  This function initializes the Main window. (Draws status circles, initialzes the table with Node number and MAC Address etc.)
void MainWindow::init(){
    int i;

    //Draw a pixmap with a circle to show the global state of the system
    //The circle's color is set to red in the initialization
    QBrush brush = QBrush(Qt::red);
    QPixmap background(200,50);
    background.fill(Qt::transparent);
    QPainter painter(&background);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.setPen(Qt::black);
    painter.setBrush(brush);
    painter.drawEllipse(POS_X, POS_Y, RADIUS_GLOBAL, RADIUS_GLOBAL);
    painter.end();
    ui->globalStatus->setPixmap(background);

    //Prepare the pixmap for the detailed status of the system
    QPixmap node(100, 100);
    node.fill(Qt::white);
    QPainter defaultPainter(&node);
    defaultPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    defaultPainter.setPen(Qt::black);
    defaultPainter.setBrush(brush);
    defaultPainter.drawEllipse(10, 0, RADIUS_NODE, RADIUS_NODE);
    defaultPainter.end();

	//Create a file to store the logdata
	QString header = "Node Number;Timestamp;Log Message";
	header += "\r\n";
	filePath = "BelleII";
	logFileNumber = 0;
	logTextNumber = 1;
	timestamp_log_data  = QDateTime::currentDateTime();
	QDir home = QDir::home();
	if(!home.exists(filePath))
		home.mkdir(filePath);
	//filePath = QDir::homePath() + "/" + filePath + "/log_" + timestamp_log_data.toString("hh_mm_MMMM_d_yy") + "-" + QString::number((logFileNumber)) + ".csv";
	filePath = QDir::homePath() + "/" + filePath + "/log_" + QString::number((logFileNumber)) + ".csv";
	logFile.setFileName(filePath);
	if (!logFile.exists(filePath))
	{
	logFile.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream logFileStream(&logFile);
	logFileStream << header;
	}

    filePath_MAC = "BelleII";
	filePath_MAC = QDir::homePath() + "/" + filePath_MAC + "/MAC_Addresses.csv";
	filePath_MAC = QDir::homePath() +"/" + filePath_MAC +  "/MAC_Addresses.csv";
	filePath_MAC = QDir::currentPath()+ "/MAC_Addresses.csv";
   //filePath_MAC = "Z:/PS/devenv/workspace/MAC_Addresses.csv";

	QFile file(filePath_MAC);
	QStringList lines;

	if ( file.open( QIODevice::ReadOnly) ) {
    // file opened successfully
    QTextStream t( &file );// use a text stream
    // until end of file...
	QString line;
	QStringList fields;
	QStringList fields_MAC;
	// Skip the first line since it contains string with node name and MAC address.
	line = t.readLine();
	 //Initialize the table
    QString name;
    for(i = 0; i < 200; i++){
        name = "Node " + QString::number(i);
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setText(name);
        ui->statusTable->setItem(i, NAME_COLUMN, item);
        QTableWidgetItem *secondItem = new QTableWidgetItem();
		name =  t.readLine();
		fields = name.split(";");
		fields_MAC = fields[1].split("-");
		secondItem->setText(fields[1]);
        ui->statusTable->setItem(i, IP_COLUMN, secondItem);
        QTableWidgetItem *thirdItem = new QTableWidgetItem();
        thirdItem->setIcon(node);
        ui->statusTable->setItem(i, STATUS_COLUMN, thirdItem);
    }
	file.close();
	}


	//Initialize the status of each node
	for(i = 0; i < 200; i++){
		status[i] = ERROR;
	}

    //Initialize the color code explaining the different states a node can be in
    //Red circle indicates an Emergency Shutdown
    //Green circle indicates that the system is running
    QPixmap colorMap(100, 100);
    colorMap.fill (Qt::transparent);
    QPainter colorPainter(&colorMap);
    colorPainter.setPen(Qt::black);
    colorPainter.setBrush(brush);
    colorPainter.drawEllipse(50, 5, 15, 15);
    brush.setColor(Qt::green);
    colorPainter.setBrush(brush);
    colorPainter.drawEllipse(50, 40, 15, 15);
	brush.setColor(QColor::fromRgb(255, 165, 0));
    colorPainter.setBrush(brush);
    colorPainter.drawEllipse(50, 75, 15, 15);
    ui->colorCode->setPixmap(colorMap);
}

/// This function updates the Monitoring window with the voltage and current information.

void MainWindow::updateMonitoring(int nodeNumber, int channelNumber, int voltage, int current){
	voltage_current_information_t info;
	info.voltage = (int32_t) voltage;
	info.current = (int32_t) current;
	monitoring->updateVoltageCurrent(nodeNumber, channelNumber, info, true);
	monitoring->updateVoltageCurrent(nodeNumber, channelNumber, info, false);
}

/// This function is called when the Bootload Button is pressed. Either a Bootload command for a specific node or for all nodes (nodeNumber is 40) is sent.

void MainWindow::bootloadNodes(){
	command_data_t dataToSend;
	dataToSend.commandType = BOOTLOAD;
	if(ui->bootloadCheckBox->isChecked()){
		dataToSend.nodeNumber = 200;
		displayLogdata(dataToSend.nodeNumber, "BOOTLOAD Command sent for all nodes ", dataToSend.nodeNumber);
	}
	else{
		dataToSend.nodeNumber = ui->bootloadNodeNumber->value();
		displayLogdata(dataToSend.nodeNumber, "BOOTLOAD Command sent for node number: ", dataToSend.nodeNumber);
	}
	sendDataWrapper(COMMAND, &dataToSend, sizeof(dataToSend));
}


///  This function is called when the Calibrate Button is pressed. The Clibration Window is then shown.
void MainWindow::calibrateNodes(){
	window->show();
	window->raise();
}

void MainWindow::showNetworking(){

	NetworkingWindow->show();
	NetworkingWindow->raise();
}


/// This function is called when the Start Button is pressed. Either a Start command for a specific node or for all nodes (nodeNumber is 40) is sent.
void MainWindow::start(){
	command_data_t dataToSend;
	dataToSend.commandType = START;
	if(ui->startCheckBox->isChecked()){
		dataToSend.nodeNumber = 200;
		displayLogdata(dataToSend.nodeNumber, "START Command sent for all nodes ", dataToSend.nodeNumber);
	}
	else{
		dataToSend.nodeNumber = ui->startNodeNumber->value();
		displayLogdata(dataToSend.nodeNumber, "START Command sent for node number: ", dataToSend.nodeNumber);		
	}
	sendDataWrapper(COMMAND, &dataToSend, sizeof(dataToSend));
}

/// This function is called when the Stop Button is pressed. Either a Stop command for a specific node or for all nodes (nodeNumber is 40) is sent.
void MainWindow::stop(){
	command_data_t dataToSend;
	dataToSend.commandType = STOP;
	if(ui->stopCheckBox->isChecked()){
		dataToSend.nodeNumber = 200;
		displayLogdata(dataToSend.nodeNumber, "STOP Command sent for all nodes ", dataToSend.nodeNumber);
	}
	else{
		dataToSend.nodeNumber = ui->stopNodeNumber->value();
		displayLogdata(dataToSend.nodeNumber, "STOP Command sent for node number: ",  dataToSend.nodeNumber);
	}
	sendDataWrapper(COMMAND, &dataToSend, sizeof(dataToSend));
}

/// This function is called when the Emergency Shutdown Button is pressed. Either a Emergency Shutdown command for a specific node or for all nodes (nodeNumber is 40) is sent.
void MainWindow::emergencyShutdown(){
	command_data_t dataToSend;
	dataToSend.commandType = EMERGENCY_SHUTDOWN;
	if(ui->emergencyCheckBox->isChecked()){
		dataToSend.nodeNumber = 200;
		displayLogdata(dataToSend.nodeNumber, "EMERGENCY SHUTDOWN Command sent for all nodes ", dataToSend.nodeNumber);
	}
	else{
		dataToSend.nodeNumber = ui->emergencyNodeNumber->value();
		displayLogdata(dataToSend.nodeNumber, "EMERGENCY SHUTDOWN Command sent for node number: ",  dataToSend.nodeNumber);
	}
	sendDataWrapper(COMMAND, &dataToSend, sizeof(dataToSend));
	
}

void MainWindow::showMonitoring(int row, int column){
	QString monitor_string("Monitoring information of node ");
	monitor_string = monitor_string + QString::number((row));;
	monitoring->currentNodeNumber = row;
	monitoring->setWindowTitle(monitor_string);
	monitoring->show();
	monitoring->raise();
	monitoring->updateContent(row);
}

/// This function changes the color of the status display for a specific node number
/// The color is changed depending on the given error code
/// When the system is running the color is green
/// When there is an error the color is yellow
/// When there is no connection to the node the color is orange
/// When the node has shutdown because of an emergency the color is red
///
void MainWindow::changeStatusDisplay(int nodeNumber, int errorCode){
    QTableWidgetItem *item;
    QPixmap map(100, 100);
    map.fill(Qt::white);
    QPainter painter(&map);
    QBrush brush = QBrush(Qt::red);
    switch(errorCode){
      case OK:
        brush.setColor(Qt::green);
		status[nodeNumber] = OK;
        break;
      case ERROR:
        brush.setColor(Qt::red);
		status[nodeNumber] = ERROR;
        break;
      case NO_CONNECTION:
        brush.setColor(QColor::fromRgb(255, 165, 0));
		status[nodeNumber] = NO_CONNECTION;
        break;
      case EMERGENCY_STOP:
        status[nodeNumber] = ERROR;
        break;
    }
    painter.setPen(Qt::black);
    painter.setBrush(brush);
    painter.drawEllipse(10, 0, RADIUS_NODE, RADIUS_NODE);
    painter.end();
    item = ui->statusTable->item(nodeNumber, STATUS_COLUMN);
    item->setIcon(map);
	updateStatus();
}

/// This function is called when the system status changes
void MainWindow::updateStatus(){
    //Check for the status of all nodes
    //If all nodes are ok, then draw a green Circle
    //Otherwise draw a red circle
	int globalStatus = OK;
	QBrush brush = QBrush(Qt::red);
	for(int i = 0; i < 200; i++){
		if(status[i] == ERROR)
			globalStatus = ERROR;
	}
	if(globalStatus == OK)
		brush = QBrush(Qt::green);
    QPixmap statusPixmap(200, 50);
    statusPixmap.fill(Qt::transparent);
    QPainter statusPainter(&statusPixmap);
    statusPainter.setPen(Qt::black);
    statusPainter.setBrush(brush);
    statusPainter.drawEllipse(POS_X, POS_Y, RADIUS_GLOBAL, RADIUS_GLOBAL);
    statusPainter.end();
    ui->globalStatus->setPixmap(statusPixmap);
}

/// This function is called when new logdata is received
void MainWindow::emitSignal(int node, int channel){
    emit newNodeChannel(node, channel);
}

/// This function is called when new logdata is received
void MainWindow::emitSignalLogData(int node, char* data, int number){
	QString log(data);
    emit newTextLog(node, log, number);
}

/// This function is called when new logdata is received
void MainWindow::emitSignalData(int node, char* data, int number){
	QString log(data);
    emit newText(node, log, number);
}



/// This function writes the logdata on the text browser
void MainWindow::writeNodeChannel(int node, int channel){
//	ui->logView->append("");
//	QString text = "Node " + QString::number(node) + ": Channel " + QString::number(channel) + ":";
//    ui->logView->append(text);
	QDateTime timestamp = QDateTime::currentDateTime();
}

/// This function writes the logdata on the text browser
void MainWindow::writeLogdata(int node, const QString &data, int number){
	QString text = data + ": " + QString::number(number);
    //ui->logView->append(text);
	QDateTime timestamp = QDateTime::currentDateTime();
	QString line = QString::number(node) + ";" + timestamp.toString() + ";" + text;
	// Clear the text field that receives the log data i order the GUI not to use a lot of memory.
	if (logFile.size()>= (FILE_SIZE_LOG_WINDOW * logTextNumber))
	{
		//ui->logView->clear();
		logTextNumber = logTextNumber +1;
	}
	// Writes the logdata in the log file
	writeLogdataInFile(node, data, number);
}

/// This function writes the logdata in the log file
void MainWindow::writeLogdataInFile(int node, const QString &data, int number){
	QString text = data + ": " + QString::number(number);
	QDateTime timestamp = QDateTime::currentDateTime();
	QString line = QString::number(node) + ";" + timestamp.toString() + ";" + text;
	// If file exceeds 50MB create a new file.
	while (logFile.size()>= FILE_SIZE_LOG_FILE) 
	{
		//Create a file to store the logdata
		QString header = "Node Number;Timestamp;Log Message";
		header += "\r\n";
		filePath = "BelleII";
		logFileNumber = logFileNumber + 1;
		QDir home = QDir::home();
		if(!home.exists(filePath))
			home.mkdir(filePath);
		//filePath = QDir::homePath() + "/" + filePath + "/log_" + timestamp_log_data.toString("hh_mm_MMMM_d_yy") + "-" + QString::number((logFileNumber)) + ".csv";
		filePath = QDir::homePath() + "/" + filePath + "/log_" + QString::number((logFileNumber)) + ".csv";
		logFile.setFileName(filePath);
		if (!logFile.exists(filePath))
		{
			logFile.open(QIODevice::WriteOnly | QIODevice::Append);
			QTextStream logFileStream(&logFile);
			logFileStream << header;
			logTextNumber = 1;
			break;
		}
	}
	logFile.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream logFileStream(&logFile);
	logFileStream << line << endl;
}
/// This function updates the monitoring information.
void MainWindow::updateMonitorInformation(int node, monitor_information_node_t info){
	
	monitoring->updateInformation(node, info);
	

}


void MainWindow::StartNode(int node){
	command_data_t dataToSend;
		dataToSend.commandType = START;
 		dataToSend.nodeNumber = node;
		displayLogdata(dataToSend.nodeNumber, " START Command sent for node number: ",  dataToSend.nodeNumber);
		sendDataWrapper(COMMAND, &dataToSend, sizeof(dataToSend));

}

void MainWindow::StopNode(int node){
		command_data_t dataToSend;
		dataToSend.commandType = STOP;
 		dataToSend.nodeNumber = node;
		displayLogdata(dataToSend.nodeNumber, "STOP Command sent for node number: ",  dataToSend.nodeNumber);
		sendDataWrapper(COMMAND, &dataToSend, sizeof(dataToSend));
	}

void MainWindow::ShutdownNode(int node){
	
	command_data_t dataToSend;
	dataToSend.commandType = EMERGENCY_SHUTDOWN;
	dataToSend.nodeNumber = node;
	displayLogdata(dataToSend.nodeNumber, "EMERGENCY SHUTDOWN Command sent for node number: ",  dataToSend.nodeNumber);
	sendDataWrapper(COMMAND, &dataToSend, sizeof(dataToSend));
	
	}