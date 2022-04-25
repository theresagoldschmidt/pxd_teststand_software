#ifndef  CALMANAGER_H
#define CALMANAGER_H


// assumes Agilent B2901A SMU connected via USB


#include <QObject>
#include <QSettings>
#include <QString>
#include <QFile>
#include "SMU.h"
#include "ftdi.h"
#include "PsReceiveParse.h" 
#include <QErrorMessage>





class CalManager : public QObject {


Q_OBJECT

public:

	CalManager(QObject *parent=0,char *name=0);
	~CalManager();

	  
	  void RetriveIni();
	  bool PrepFolder(char *path);
	  void retrive_channel_info(int i);

	  void SetSMU(SMU *SourceMeasure);
	  void SetRelaisCard(FTDI *Relais);
	  void RequestPsInfo(int node,int channel);
	  void SetPsVoltage(int node, int channel,int volt);// creates string and emit send request
      void SetPsCurrent(int node, int channel,int current); // creates string and emit send request

public slots:

	void  StopMeasurement();

	void start_cal_session();
	void SetPath(char *str);

	void SmuVoltageIs(double volt);
	void SmuCurrentIs(double current);
	void PsVoltageIs(int regulator, int load);
	void PsCurrentIs(double current);
	void ReloadSettings();
	void TakeData(const QString & string);
	 
signals:

void SetRelaisChannel(int i);

void GetSmuVoltage();
void GetSmuCurrent();
void GetPsVoltage(int channel);
void GetPsCurrent(int channel);

void SendCMD(const QString & string); // connected with server


void SetSmuVoltage(double volt);
void SetSmuCurrent(double volt);


void progress(int total,int channel); // 100 100 full scale

void SendInfo(const QString & string);

private:

QErrorMessage *errorMessage;

QFile *dat_file;
QFile *log_file;

PsReceiveParser *parse;

QString path;

QSettings *settings;

SMU *SourceMeasurementUnit; 
FTDI *RelaisCard;

double SMU_Voltage,SMU_Current;

/// status infos from PS
bool updated_ps;
int PS_Node,PS_Channel,PS_Regulator,PS_Load,PS_Current,Bit;

int *PsMonValues;

int relais_chan,num_points,ps_channel_id; // config data for measurement and setup of string
double StartVolt,StopVolt,Compliance,StartCurr,StopCurr;
double StartLimCurr,StopLimCurr;
double MidScale; // dac value for current measurement
double BiasCurrent; //BiasCurr while voltage measurement ( to assure that the sink is properly regulating)
double SMUOutputVoltage; // set the voltage of the smu while limit measurement ( sink needs higher voltage than output)
double SMUVoltageRange;
QString channel;

//state variables wait, stop....

bool wait;
bool stop;

};








#endif 


