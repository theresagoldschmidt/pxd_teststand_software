#ifndef OVPTEST_H
#define OVPTEST_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QFile>
#include <QErrorMessage>
#include "SMU.h"
#include "ftdi.h"
//#include "PsRemoteControl.h"

class OvpTester : public QObject {
	
	Q_OBJECT
	
	public:
		void SetSMU(SMU* ptr2SMU);
		void SetRelaisCard(FTDI* ptr2RelaisCard);
		OvpTester(QObject *parent=0, char *name=0);
		~OvpTester();
		void RetrieveIni();
		void RequestPsInfo(int node,int channel);
		void SetPsVoltage(int node, int channel,int volt);// creates string and emit send request
		void SetPsCurrent(int node, int channel,int current); // creates string and emit send request
		void EnablePS(int unit);
		void DisablePS(int unit);
		void WorkAroundInitialization();
		void TestUpperLimit();
		void TestLowerLimit();
		
		

	  public slots:

		void SetPath(char *str);
		void ReloadSettings();
		void StopMeasurement();
		void SmuVoltageIs(double volt);
		void SmuCurrentIs(double current);
		void PsVoltageIs(int regulator, int load);
		void PsCurrentIs(double current);
		void retrive_channel_info(int i);
		void StartOvpTest();
		void StopOvpTest();
		void ReceiveNodeGui(int i); //void SetNode(int i);
		void PrepareGateOnTest();

		

	signals: // Will be defined in moc_OvpTest.cpp (At least I hope so...)

		void SetRelaisChannel(int i);
		void GetSmuVoltage();
		void GetSmuCurrent();
		void GetPsVoltage(int channel);
		void GetPsCurrent(int channel);
		void SendCMD(const QString & string); 
		void SetSmuVoltage(double volt);
		void SetSmuCurrent(double volt);
		void progress(int total,int channel);
		void SendInfo(const QString & string);
		void GetNodeGui();
	
		

	private: 

		bool StillOn;	// Variable to determine, whether unit is still on, or has shut down (initialized as true)
		bool ShutDown;	// True if Unit shuts down during the test (initialized as false)
		bool Passed;		// initialized as true. if one Channel fails, variable gets set to false
		bool testpassed;	// initialized as true for every test. Used to feed the array passed[]
		bool passed [24];	// same as Passed, but as an array to store where the test failed
		bool TestSuccessful;	// Will be true if every initial voltage could be set. 
								// Does NOT mean that the unit passed the test
		bool PS_Working;
		QString path;
		QSettings *settings;
		SMU *SourceMeasurementUnit; 
		FTDI *RelaisCard;
	
		//PsRemoteControl *PsRemote;

		int ID, CheckInt, Relais, Range;
		double Limit_L, Limit_H, OVP_Start_L, OVP_Start_H, OVP_Stop_L, OVP_Stop_H; //Start und Endpunkte unseres Tests
		double SMU_Voltage, SMU_Current;
		int PS_Node, PS_Channel, PS_Regulator, PS_Load, PS_Current, Bit;
		QString Checktype;
		
		QFile *file;
		/*bool wait, stop;*/
		bool stop; // Test runs only as long stop==false; Function StopOVPTest() sets stop to true and terminates test

		QString VoltageString, NString, NodeString, ChannelString;
		QString data;
 
	

		double Voltage;
		double Voltage_SW_RefIn, Voltage_Gate; //For test case 4 and 5
		int ID_Gate, Relais_Gate; // For test cases 4 and 5
		int counter; //counts how often the voltage was increased/decreased
		double SMUVolt;
		double TestVolt1;
		double TestVolt2;
		int TestCounter;
		int Node;
		double VoltageStep;
		QString status;
		QString Channel_Name;
		int ID_SW_SUB;
		int ID_SW_REFIN;
		int Relais_SW_SUB;
		int Relais_SW_REFIN;
};

#endif