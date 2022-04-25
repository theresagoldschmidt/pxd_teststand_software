#ifndef STARTSTOP_H
#define STARTSTOP_H

#include <QObject>
#include <QStringList>
#include <QString>
#include <QChar>


typedef struct
{
	QString   name;
	int     id;
	int     U_mV;
	int		I_mA;
}
channel_descr;

//int order[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}; // order of switch on 
/*	
channel_descr channels[16]= {
	{"DCD_DVDD",0,1800,500},
	{"DHP_IO",1,1200,500},
	{"DHP_CORE",2,1800,500},
	{"SW_DVDD",3,3300,50},

	{"DCD_AVDD",4,1800,500},
	{"DCD_REFIN",5,1200,100},
	{"DCD_AMPLOW",6,350,300},
	{"Source",7,6000,50},
	
	{"GATE_OFF",8,10000,10},
	{"Gate_ON",9,1800,10},
	{"ClearON",10,25000,20},
	{"ClearOff",11,12000,20},

	{"Bulk",12,15000,5},
	{"Guard",13,8000,5},
	{"CCG",14,5000,4},
	{"HV",15,-15000,1}   }; */

class PSMainWin;

class StartStop : public QObject
    { // private Definitions,

	Q_OBJECT


public:
 
  
  StartStop(QObject *parent=0,char *name=0);
 ~StartStop();
  QString *buff;
  QString *help;
  

public slots:

	void SeqReceiveCMD();
    void SeqStartUp(int node); 
	void SeqStopPS(int node);

 signals:

void SeqSendCMD(const QString & string);
	


private:

   int NUM_CHAN;
   int order[24];
   channel_descr channels[24];
   
};



#endif //STARTSTOP_H
