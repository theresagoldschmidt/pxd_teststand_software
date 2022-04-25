#ifndef SERVERCMDPROC_H
#define SERVERCMDPROC_H

#include <QObject>
#include <QStringList>
#include <QString>
#include <stdint.h>


class ServerCmdProc : public QObject
{

	Q_OBJECT


public:

ServerCmdProc(QObject *parent=0,char *name=0);
~ServerCmdProc();


bool Tokenize();

//interface functions to chromosome 

void CmdStop();
void CmdStart();
void SendData(int Channel,double voltage, double current);
void Reset();
void SetCalibratedValue();
void SendLog();
void SetVoltage();
void SetCurrent();

public slots:

void SetCMD(QString *cmd);

signals:

void receivedCMD();
void LogInfo(const char* info);
void GetVoltage(int node,int channel);
void updateInfo(int nodeNumber, int channelNumber, int voltage, int current);
void updateVoltage(int nodeNumber, int channelNumber, int voltage);
void updateCurrent(int nodeNumber, int channelNumber, int current);

private:
    int Node;
    int Channel; // channel of ps 0-23
	int Voltage;
    int Current;

	QString *Command;
	QStringList *List;

 // Strings used for parsing the input from daq   

QString CMD_STRING;
QString PWR_STRING;//="POWER";
QString PS_STRING;//= "PS:";
QString SET_STRING;//="SET";
QString GET_STRING;//="GET";
QString ID_STRING;//= "ID";
QString DELIM_STRING;//=" ";
QString START_STRING;//= "Powerup";
QString STOP_STRING;//= "Shutdown";
QString S3B_STRING;//="S3B";
QString LOG_STRING;//="LOG";
QString SETUI_STRING; // SET_UI
QString  SET_VOLT_STRING; //="SET_U";
QString  SET_CURRENT_STRING; //="SET_I";
	// pointer to the callibration window and functionaly set calibrated values, update nominal values in gui
//	calibration *calib;



};



#endif // SERVERCMDPROC_H