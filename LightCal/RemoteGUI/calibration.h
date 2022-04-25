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

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QWidget>
#include <QDateTime>

#include "calibrationwindow.h"

namespace Ui {
class calibration;
}


class calibration : public QWidget
{
    Q_OBJECT
    
public:
    explicit calibration(QWidget *parent = 0);
    ~calibration();
	void updateWindow(int node, int channel);

signals:
	void updateInfoVoltage(int nodeNumber, int channelNumber, int voltage);
	void updateInfoCurrent(int nodeNumber, int channelNumber, int current);

private slots:
    void setValueVoltage();
	void setCalibratedValueVoltage();
	void setValueCurrent();
	void setCalibratedValueCurrent();
    
private:
    Ui::calibration *ui;
};

#endif // CALIBRATION_H
