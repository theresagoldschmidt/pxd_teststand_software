/*
 * Copyright (c) 2011, fortiss GmbH.
 * All rights reserved.
 *
 * $Id$
 */

/**
 * \file
 *         Calibration Window.
 *
 * \author
 *         Nadine Keddis <keddis@fortiss.org>
 */

#include "calibration.h"
#include "ui_calibration.h"
#include "wrapperHeader.h"

#define CALIBRATE 2

calibration::calibration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::calibration)
{
    ui->setupUi(this);

    connect(ui->setValueVoltageButton, SIGNAL(clicked()), this, SLOT(setValueVoltage()));
	connect(ui->setCalibratedValueVoltageButton, SIGNAL(clicked()), this, SLOT(setCalibratedValueVoltage()));
	connect(ui->setValueCurrentButton, SIGNAL(clicked()), this, SLOT(setValueCurrent()));
	connect(ui->setCalibratedValueCurrentButton, SIGNAL(clicked()), this, SLOT(setCalibratedValueCurrent()));
}

calibration::~calibration()
{
    delete ui;
}

void calibration::updateWindow(int node, int channel){
	ui->calibrateNodeNumber->setValue(node);
	ui->calibrationChannel->setCurrentIndex(channel);
}

//This function is called when theSet Uncalibrated Voltage Value Button has been pressed
void calibration::setValueVoltage(){
    calibration_input_data_t data;
	data.nodeNumber = ui->calibrateNodeNumber->value();
    data.channelNumber = ui->calibrationChannel->currentIndex();
	data.calibrate_bool = NOT_CALIBRATE_VALUE;
	data.voltage_or_current_bool = SET_VOLTAGE;
    data.voltage = ui->voltage->value();
    data.current = ui->current->value();
    sendDataWrapper(CALIBRATE, &data, sizeof(data));
	updateInfoVoltage(data.nodeNumber, (int)data.channelNumber, (int)data.voltage);
}

//This function is called when the Set Calibrated Voltage Value Button has been pressed
void calibration::setCalibratedValueVoltage(){
    calibration_input_data_t data;
	data.nodeNumber = ui->calibrateNodeNumber->value();
    data.channelNumber = ui->calibrationChannel->currentIndex();
	data.calibrate_bool = CALIBRATE_VALUE;
	data.voltage_or_current_bool = SET_VOLTAGE;
    data.voltage = ui->voltage->value();
    data.current = ui->current->value();
    sendDataWrapper(CALIBRATE, &data, sizeof(data));
	updateInfoVoltage(data.nodeNumber, (int)data.channelNumber, (int)data.voltage);
}

//This function is called when the Set Uncalibrated Current Value Button has been pressed
void calibration::setValueCurrent(){
    calibration_input_data_t data;
	data.nodeNumber = ui->calibrateNodeNumber->value();
    data.channelNumber = ui->calibrationChannel->currentIndex();
	data.calibrate_bool = NOT_CALIBRATE_VALUE;
	data.voltage_or_current_bool = SET_CURRENT;
    data.voltage = ui->voltage->value();
    data.current = ui->current->value();
    sendDataWrapper(CALIBRATE, &data, sizeof(data));
	updateInfoCurrent(data.nodeNumber, (int)data.channelNumber, data.current);
}

//This function is called when the Set Calibrated Current Value Button has been pressed
void calibration::setCalibratedValueCurrent(){
    calibration_input_data_t data;
	data.nodeNumber = ui->calibrateNodeNumber->value();
    data.channelNumber = ui->calibrationChannel->currentIndex();
	data.calibrate_bool = CALIBRATE_VALUE;
	data.voltage_or_current_bool = SET_CURRENT;
    data.voltage = ui->voltage->value();
    data.current = ui->current->value();
    sendDataWrapper(CALIBRATE, &data, sizeof(data));
	updateInfoCurrent(data.nodeNumber, (int)data.channelNumber, data.current);
}
