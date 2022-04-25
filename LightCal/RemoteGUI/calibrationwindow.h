/*
 * Copyright (c) 2011, fortiss GmbH.
 * All rights reserved.
 *
 * $Id$
 */

/**
 * \file
 *         Calibration Window
 *
 * \author
 *         Nadine Keddis <keddis@fortiss.org>
 */

#ifndef CALIBRATIONWINDOW_H
#define CALIBRATIONWINDOW_H

#include <QWidget>
#include <stdint.h>
#include "monitoringwindow.h"

namespace Ui {
    class calibrationWindow;
}

#define EEPROM_NUMBER_OF_CAL_UNITS_PER_CHANNEL 5
#define CALIBRATE_VALUE 0
#define NOT_CALIBRATE_VALUE 1

#define SET_CURRENT 1
#define SET_VOLTAGE 2

typedef struct{
	uint8_t nodeNumber;
	uint8_t channelNumber;
	uint8_t calibrate_bool;
	uint8_t voltage_or_current_bool;
	int32_t voltage;
	int32_t current;
}calibration_input_data_t;

typedef struct{
	int32_t gain;
	int32_t offset;
}calibration_channel_t;

typedef struct{
	uint8_t nodeNumber;
	uint8_t channelNumber;
	calibration_channel_t calibration_page[EEPROM_NUMBER_OF_CAL_UNITS_PER_CHANNEL];
}recalibration_data_t;

namespace Ui {
    class calibrationWindow;
}

class calibrationWindow : public QWidget
{
    Q_OBJECT

public:
    explicit calibrationWindow(QWidget *parent = 0);
    ~calibrationWindow();

signals:
	void updateInfoVoltage(int nodeNumber, int channelNumber, int voltage);
	void updateInfoCurrent(int nodeNumber, int channelNumber, int current);
	void SendInfo(QString);

private slots:
    void setValueVoltage();
	void setCalibratedValueVoltage();
	void setValueCurrent();
	void setCalibratedValueCurrent();
    void recalibrate();
	void ResetConstants();
	void LoadConstants();
	void LoadChannel();

private:
    Ui::calibrationWindow *ui;
	//QSettings *settings;

};

#endif // CALIBRATIONWINDOW_H
