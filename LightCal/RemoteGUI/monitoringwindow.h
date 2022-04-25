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

#ifndef MONITORINGWINDOW_H
#define MONITORINGWINDOW_H

#include <QWidget>
#include <stdint.h>
#include "calibration.h"

namespace Ui {
    class monitoringWindow;
}

#define REGULATOR_STATUS_LOW  0
#define REGULATOR_STATUS_HIGH 1

#define EMERGENCY_PINS_STATUS_LOW  0
#define EMERGENCY_PINS_STATUS_HIGH 1

#define NUMBER_OF_CALIBRATION_CHANNELS 24

typedef struct
{
	int32_t voltage_at_regulator;
	int32_t voltage_at_load;
	int32_t current;
	uint8_t regulator_status_bit;
}monitor_channel_t;

#define NODE_STATUS_OK 0
#define NODE_STATUS_NOT_OK 1

typedef struct
{
	uint8_t nodeNumber;
	uint8_t node_status;
	uint8_t ups_status;
	uint8_t thermal_status;
	uint8_t ovp_fast_status;
	monitor_channel_t monitor_node_info_all_channels[NUMBER_OF_CALIBRATION_CHANNELS];
}monitor_information_node_t;

typedef struct
{
	int32_t voltage;
	int32_t current;
}voltage_current_information_t;

typedef struct
{
	monitor_information_node_t monitoringInfo;
	voltage_current_information_t volt_current_info[NUMBER_OF_CALIBRATION_CHANNELS];
}monitoring_information_t;

class calibration;

class monitoringWindow : public QWidget
{
    Q_OBJECT

public:
    explicit monitoringWindow(QWidget *parent = 0);
    ~monitoringWindow();
	void initTable();
	void updateContent(int nodeNumber);
	void updateCurrentContent(int nodeNumber);
	void updateInformation(int nodeNumber, monitor_information_node_t information);
	void updateVoltageCurrent(int nodeNumber, int channelNumber, voltage_current_information_t information, bool voltageOrCurrent);
	int currentNodeNumber;
	calibration *calWind;


signals:
	void StopNode(int node);
	void StartNode(int node);
	void ShutdownNode(int node);
	void monSendSignals(const QString&);  // signal to send data 

public slots:
    void monGetVoltage(int node, int channel);  // slot to receive request
	void Shutdown();
	void Start();
	void Stop();
private slots:
    void start_calibration(int row, int column);
	void updateVoltage(int nodeNumber, int channelNumber, int voltage);
	void updateCurrent(int nodeNumber, int channelNumber, int current);

private:
    Ui::monitoringWindow *ui;
	monitoring_information_t monitoring[200];
	bool is_updated;
	//calibration* CalWindow; 
	//calibration *calwindow;
};

#endif // MONITORINGWINDOW_H
