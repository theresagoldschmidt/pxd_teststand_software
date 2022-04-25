/*
 * Copyright (c) 2011, fortiss GmbH.
 * All rights reserved.
 *
 * $Id$
 */

/**
 * \file
 *         Monitoring Window
 *
 * \author
 *         Nadine Keddis <keddis@fortiss.org>
 */

#include "monitoringwindow.h"
#include "ui_monitoringwindow.h"
#include "calibration.h"
#include <QTest>
#include <QPainter>
#include <QColor>
#include <QPixmap>
#include <QList>
#include <QFileDialog>
#include <QDateTime>
#include <QChar>

#define SET_VOLTAGE_COLUMN 0
#define SET_CURRENT_COLUMN 1
#define VOLTAGE_REGULATOR_COLUMN 2
#define VOLTAGE_LOAD_COLUMN 3
#define CURRENT_COLUMN 4
#define REGULATOR_STATUS_COLUMN 5


monitoringWindow::monitoringWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::monitoringWindow)
{
	calWind = new calibration;
	if (calWind == NULL){ return; }
	calWind->setWindowTitle("Calibration");

    ui->setupUi(this);

	ui->monitoringTable->setColumnCount(6);
	ui->monitoringTable->setRowCount(24);
	ui->monitoringTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->monitoringTable->setColumnWidth(0, 150);
	ui->monitoringTable->setColumnWidth(1, 150);
	ui->monitoringTable->setColumnWidth(2, 150);
	ui->monitoringTable->setColumnWidth(3, 150);
	ui->monitoringTable->setColumnWidth(4, 150);
	ui->monitoringTable->setColumnWidth(5, 150);

	QList<QString> headers;
	headers.append("Set Voltage (mV)");
	headers.append("Set Current (mA)");
	headers.append("Voltage at Regulator (mV)");
	headers.append("Voltage at Load (mV)");
	headers.append("Current (mA)");
	headers.append("Regulator Status Bit");
	ui->monitoringTable->setHorizontalHeaderLabels(headers);

	QList<QString> verticalHeaders;
	verticalHeaders.append("DCD_DVDD");
	verticalHeaders.append("DHP_IO");
	verticalHeaders.append("DHP_CORE");
	verticalHeaders.append("SW_DVDD");
	verticalHeaders.append("DCD_AVDD");
	verticalHeaders.append("DCD_REFIN");
	verticalHeaders.append("DCD_AMPLOW");
	verticalHeaders.append("Source");
	verticalHeaders.append("GATE_OFF");
	verticalHeaders.append("GATE_ON1");
	verticalHeaders.append("GATE_ON2");
	verticalHeaders.append("GATE_ON3");
	verticalHeaders.append("Clear_ON");
	verticalHeaders.append("Clear_OFF");
	verticalHeaders.append("Bulk");
	verticalHeaders.append("Guard");
	verticalHeaders.append("NOT_USED_1");
	verticalHeaders.append("SW_SUB");
	verticalHeaders.append("SW_REF");
	verticalHeaders.append("HV");
	verticalHeaders.append("CCG1");
	verticalHeaders.append("CCG2");
	verticalHeaders.append("CCG3");
	verticalHeaders.append("NOT_USED_2");
	ui->monitoringTable->setVerticalHeaderLabels(verticalHeaders);

	for(int i = 0; i < 200; ++i){
		monitoring[i].monitoringInfo.nodeNumber = i;
		monitoring[i].monitoringInfo.node_status = NODE_STATUS_NOT_OK;
		for(int j = 0; j < NUMBER_OF_CALIBRATION_CHANNELS; ++j){
			monitoring[i].monitoringInfo.monitor_node_info_all_channels[j].voltage_at_regulator = 0;
			monitoring[i].monitoringInfo.monitor_node_info_all_channels[j].voltage_at_load = 0;
			monitoring[i].monitoringInfo.monitor_node_info_all_channels[j].current = 0;
			monitoring[i].monitoringInfo.monitor_node_info_all_channels[j].regulator_status_bit = REGULATOR_STATUS_LOW;
			monitoring[i].volt_current_info[j].voltage = 0;
			monitoring[i].volt_current_info[j].current = 0;
		}
	}

	//Initialize the color code explaining the statuses of the OVP, THERMAL and UPS pin
    //Red circle indicates that an Emegency Shutdown state
    //Green circle indicates that the system is ok
	QBrush brush = QBrush(Qt::red);
    QPixmap emergency_status(60,90);
    emergency_status.fill(Qt::transparent);
    QPainter painter_status(&emergency_status);
    painter_status.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter_status.setPen(Qt::black);
    painter_status.setBrush(brush);
    painter_status.drawEllipse(30, 2, 15, 15);
    painter_status.setBrush(brush);
    painter_status.drawEllipse(30, 37, 15, 15);
	painter_status.setBrush(brush);
    painter_status.drawEllipse(30, 72, 15, 15);
    painter_status.end();
    ui->colorCode->setPixmap(emergency_status);

	currentNodeNumber = 0;
	is_updated=false;
	connect(ui->monitoringTable, SIGNAL(cellClicked(int, int)), this, SLOT(start_calibration(int,int)));
	connect(calWind, SIGNAL(updateInfoVoltage(int , int, int)), this, SLOT(updateVoltage(int, int, int)));
	connect(calWind, SIGNAL(updateInfoCurrent(int , int, int)), this, SLOT(updateCurrent(int, int, int)));
	
	connect(ui->StartPush, SIGNAL(clicked()),this, SLOT(Start()));
	connect(ui->StopPush, SIGNAL(clicked()),this, SLOT(Stop()));
	connect(ui->ShutdownPush, SIGNAL(clicked()),this, SLOT(Shutdown()));

}

monitoringWindow::~monitoringWindow()
{
	delete ui;
}

void monitoringWindow::initTable(){
	QString value = QString::number(0);
	for(int i = 0; i < 24; ++i){
		QTableWidgetItem * item = new QTableWidgetItem();
		item->setTextAlignment(Qt::AlignHCenter);
		item->setText(value);
		ui->monitoringTable->setItem(i, SET_VOLTAGE_COLUMN, item);
		QTableWidgetItem * secondItem = new QTableWidgetItem();
		secondItem->setTextAlignment(Qt::AlignHCenter);
		secondItem->setText(value);
		ui->monitoringTable->setItem(i, SET_CURRENT_COLUMN, secondItem);
		QTableWidgetItem * thirdItem = new QTableWidgetItem();
		thirdItem->setTextAlignment(Qt::AlignHCenter);
		thirdItem->setText(value);
		ui->monitoringTable->setItem(i, VOLTAGE_REGULATOR_COLUMN, thirdItem);
		QTableWidgetItem * fourthItem = new QTableWidgetItem();
		fourthItem->setTextAlignment(Qt::AlignHCenter);
		fourthItem->setText(value);
		ui->monitoringTable->setItem(i, VOLTAGE_LOAD_COLUMN, fourthItem);
		QTableWidgetItem * fifthItem = new QTableWidgetItem();
		fifthItem->setTextAlignment(Qt::AlignHCenter);
		fifthItem->setText(value);
		ui->monitoringTable->setItem(i, CURRENT_COLUMN, fifthItem);
		QTableWidgetItem * sixthItem = new QTableWidgetItem();
		sixthItem->setTextColor(Qt::red);
		sixthItem->setTextAlignment(Qt::AlignHCenter);
		sixthItem->setText("LOW");
		ui->monitoringTable->setItem(i, REGULATOR_STATUS_COLUMN, sixthItem);
	}
}

void monitoringWindow::updateInformation(int nodeNumber, monitor_information_node_t information){
	monitoring[nodeNumber].monitoringInfo = information;
	updateCurrentContent(nodeNumber);
	is_updated=true;
}

void monitoringWindow::updateVoltageCurrent(int nodeNumber, int channelNumber, voltage_current_information_t information, bool voltageOrCurrent){
	if(voltageOrCurrent){
		monitoring[nodeNumber].volt_current_info[channelNumber].voltage = information.voltage;
	}
	else{
		monitoring[nodeNumber].volt_current_info[channelNumber].current = information.current;
	}
	updateCurrentContent(nodeNumber);
}

void monitoringWindow::updateContent(int nodeNumber){
	QTableWidgetItem *item;
	for(int i = 0; i < 24; ++i){
		item = ui->monitoringTable->item(i, SET_VOLTAGE_COLUMN);
		item->setText(QString::number(monitoring[nodeNumber].volt_current_info[i].voltage));
		item = ui->monitoringTable->item(i, SET_CURRENT_COLUMN);
		item->setText(QString::number(monitoring[nodeNumber].volt_current_info[i].current));
		item = ui->monitoringTable->item(i, VOLTAGE_REGULATOR_COLUMN);
		item->setText(QString::number(monitoring[nodeNumber].monitoringInfo.monitor_node_info_all_channels[i].voltage_at_regulator));
		item = ui->monitoringTable->item(i, VOLTAGE_LOAD_COLUMN);
		item->setText(QString::number(monitoring[nodeNumber].monitoringInfo.monitor_node_info_all_channels[i].voltage_at_load));
		item = ui->monitoringTable->item(i, CURRENT_COLUMN);
		item->setText(QString::number(monitoring[nodeNumber].monitoringInfo.monitor_node_info_all_channels[i].current));
		item = ui->monitoringTable->item(i, REGULATOR_STATUS_COLUMN);
		if(monitoring[nodeNumber].monitoringInfo.monitor_node_info_all_channels[i].regulator_status_bit == REGULATOR_STATUS_LOW){
			item->setTextColor(Qt::red);
			item->setTextAlignment(Qt::AlignHCenter);
			item->setText("LOW");
		}
		else{
			item->setTextColor(Qt::green);
			item->setTextAlignment(Qt::AlignHCenter);
			item->setText("HIGH");
		}
	}

	QPixmap emergency_status(60,90);
	emergency_status.fill(Qt::transparent);
	QPainter painter_status(&emergency_status);
	painter_status.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter_status.setPen(Qt::black);

	if(monitoring[nodeNumber].monitoringInfo.ups_status == EMERGENCY_PINS_STATUS_HIGH){
			QBrush brush = QBrush(Qt::green);
			painter_status.setBrush(brush);
			painter_status.drawEllipse(30, 2, 15, 15);
	}
	else{
			QBrush brush = QBrush(Qt::red);
			painter_status.setBrush(brush);
			painter_status.drawEllipse(30, 2, 15, 15);
	}

	if(monitoring[nodeNumber].monitoringInfo.thermal_status == EMERGENCY_PINS_STATUS_HIGH){
			QBrush brush = QBrush(Qt::green);
			painter_status.setBrush(brush);
			painter_status.drawEllipse(30, 37, 15, 15);
	}
	else{
			QBrush brush = QBrush(Qt::red);
			painter_status.setBrush(brush);
			painter_status.drawEllipse(30, 37, 15, 15);
	}

	if(monitoring[nodeNumber].monitoringInfo.ovp_fast_status== EMERGENCY_PINS_STATUS_HIGH){
			QBrush brush = QBrush(Qt::green);
			painter_status.setBrush(brush);
			painter_status.drawEllipse(30, 72, 15, 15);
	}
	else{
			QBrush brush = QBrush(Qt::red);
			painter_status.setBrush(brush);
			painter_status.drawEllipse(30, 72, 15, 15);
	}

	painter_status.end();
	ui->colorCode->setPixmap(emergency_status);
}

void monitoringWindow::updateCurrentContent(int nodeNumber){
	if (currentNodeNumber == nodeNumber) 
	{
	QTableWidgetItem *item;
	for(int i = 0; i < 24; ++i){
		item = ui->monitoringTable->item(i, SET_VOLTAGE_COLUMN);
		item->setText(QString::number(monitoring[nodeNumber].volt_current_info[i].voltage));
		item = ui->monitoringTable->item(i, SET_CURRENT_COLUMN);
		item->setText(QString::number(monitoring[nodeNumber].volt_current_info[i].current));
		item = ui->monitoringTable->item(i, VOLTAGE_REGULATOR_COLUMN);
		item->setText(QString::number(monitoring[nodeNumber].monitoringInfo.monitor_node_info_all_channels[i].voltage_at_regulator));
		item = ui->monitoringTable->item(i, VOLTAGE_LOAD_COLUMN);
		item->setText(QString::number(monitoring[nodeNumber].monitoringInfo.monitor_node_info_all_channels[i].voltage_at_load));
		item = ui->monitoringTable->item(i, CURRENT_COLUMN);
		item->setText(QString::number(monitoring[nodeNumber].monitoringInfo.monitor_node_info_all_channels[i].current));
		item = ui->monitoringTable->item(i, REGULATOR_STATUS_COLUMN);
		if(monitoring[nodeNumber].monitoringInfo.monitor_node_info_all_channels[i].regulator_status_bit == REGULATOR_STATUS_LOW){
			item->setTextColor(Qt::red);
			item->setTextAlignment(Qt::AlignHCenter);
			item->setText("LOW");
		}
		else{
			item->setTextColor(Qt::green);
			item->setTextAlignment(Qt::AlignHCenter);
			item->setText("HIGH");
		}
	}

	QPixmap emergency_status(60,90);
	emergency_status.fill(Qt::transparent);
	QPainter painter_status(&emergency_status);
	painter_status.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter_status.setPen(Qt::black);

	if(monitoring[nodeNumber].monitoringInfo.ups_status == EMERGENCY_PINS_STATUS_HIGH){
			QBrush brush = QBrush(Qt::green);
			painter_status.setBrush(brush);
			painter_status.drawEllipse(30, 2, 15, 15);
	}
	else{
			QBrush brush = QBrush(Qt::red);
			painter_status.setBrush(brush);
			painter_status.drawEllipse(30, 2, 15, 15);
	}

	if(monitoring[nodeNumber].monitoringInfo.thermal_status == EMERGENCY_PINS_STATUS_HIGH){
			QBrush brush = QBrush(Qt::green);
			painter_status.setBrush(brush);
			painter_status.drawEllipse(30, 37, 15, 15);
	}
	else{
			QBrush brush = QBrush(Qt::red);
			painter_status.setBrush(brush);
			painter_status.drawEllipse(30, 37, 15, 15);
	}

	if(monitoring[nodeNumber].monitoringInfo.ovp_fast_status== EMERGENCY_PINS_STATUS_HIGH){
			QBrush brush = QBrush(Qt::green);
			painter_status.setBrush(brush);
			painter_status.drawEllipse(30, 72, 15, 15);
	}
	else{
			QBrush brush = QBrush(Qt::red);
			painter_status.setBrush(brush);
			painter_status.drawEllipse(30, 72, 15, 15);
	}

	painter_status.end();
	ui->colorCode->setPixmap(emergency_status);
	}
}
void monitoringWindow::start_calibration(int row, int column){
	QString monitor_string("Calibration window of node ");
	monitor_string = monitor_string + QString::number((currentNodeNumber));
	/*calibration *window =  new calibration;
	window->setWindowTitle(monitor_string);
	window->setAttribute(Qt::WA_DeleteOnClose);
	window->show();
	window->updateWindow(currentNodeNumber, row);
	connect(window, SIGNAL(updateInfoVoltage(int , int, int)), this, SLOT(updateVoltage(int, int, int)));
	connect(window, SIGNAL(updateInfoCurrent(int , int, int)), this, SLOT(updateCurrent(int, int, int)));*/
	calWind->setWindowTitle(monitor_string);
	calWind->show();
	calWind->raise();
	calWind->updateWindow(currentNodeNumber, row);
}

void monitoringWindow::updateVoltage(int nodeNumber, int channelNumber, int voltage){
	voltage_current_information_t info;
	info.voltage = (int32_t) voltage;
	updateVoltageCurrent(nodeNumber, channelNumber, info, true);
}
void monitoringWindow::updateCurrent(int nodeNumber, int channelNumber, int current){
	voltage_current_information_t info;
	info.current = (int32_t) current;
	updateVoltageCurrent(nodeNumber, channelNumber, info, false);
}

void monitoringWindow::monGetVoltage(int node, int channel){

int U_LOAD	=	monitoring[node].monitoringInfo.monitor_node_info_all_channels[channel].voltage_at_load;
int U_REG   =	monitoring[node].monitoringInfo.monitor_node_info_all_channels[channel].voltage_at_regulator;
int I_REG	=	monitoring[node].monitoringInfo.monitor_node_info_all_channels[channel].current;
int IsLimit = 	monitoring[node].monitoringInfo.monitor_node_info_all_channels[channel].regulator_status_bit;


	QString str="LMU PS Demonstrator ";
    QString buff="";
	
	str.append(buff.setNum(node));
	str.append(" Channel: "); 
	str.append(buff.setNum(channel));
	str.append(" U_Load: ");
	str.append(buff.setNum(U_LOAD));
	str.append(" U_Reg: ");
	str.append(buff.setNum(U_REG));
	str.append(" I_Load: ");
    str.append(buff.setNum(I_REG));
	str.append(" Bit: ");
    str.append(buff.setNum(IsLimit));

    str.append(" ");
 
	emit monSendSignals(str);

}



void monitoringWindow::Shutdown(){
	
	emit StopNode(currentNodeNumber);
}

void monitoringWindow::Start(){
	
	emit StartNode(currentNodeNumber);
}

void monitoringWindow::Stop(){

	emit ShutdownNode(currentNodeNumber);

}