#ifndef  SMU_H
#define SMU_H


// assumes Agilent B2901A SMU connected via USB


#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include "C:\Program Files (x86)\IVI Foundation\VISA\WinNT\include\visa.h"
#include <stdlib.h>
#include <stdio.h>


#include <QObject>



class SMU : public QObject {


	Q_OBJECT
public:

	SMU(QObject *parent=0,char *name=0);
	~SMU();




public slots:

void open_session();
void reset_session();

void SourceCurrent(double curr);
void SetVoltage(double curr); // needed for current measurements into short

void GetCurrent();
void GetVoltage();
double GetVoltageDouble();

void EnableOutput();
void DisableOutput();
void GetOutputStat();

void SetSourceCurrent();
void SetSourceVoltage();

void SetVoltageCompliance(double volt);
void SetCurrentCompliance(double current);

void SetMeasureRangeManual();
void SetMeasureRangeAuto();

void SetApperture();

void SetRemoteSenseOn();
void SetRemoteSenseOff(); // enables - disables the remote sense feature of the smu

void  SetMeasureRange(double volt);

signals:

void VoltageApplied();
void CurrentApplied();

void MeasuredVoltage(double volt);
void MeasuredCurrent(double current);
void SendInfo(const QString & string);
void OutputStatus(long mode);

private:

 ViSession defaultRM, b2901;
 ViAddr address;
  unsigned short id_reg, devtype_reg;


};








#endif 