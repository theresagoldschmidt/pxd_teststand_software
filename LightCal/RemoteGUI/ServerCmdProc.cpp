
#include <QStringList>
#include <QString>
 #include <QRegExp>
#include "logdataDefines.h"
#include "ServerCmdProc.h"

#include <stdint.h>
 #include "wrapperHeader.h"



//#include "calibration.h"


// Definition of topics for xme

#define START 0
#define STOP 1 
#define CALIBRATE 2
#define BOOTLOAD 3
#define RECALIBRATE 4
#define COMMAND 5
#define EMERGENCY_SHUTDOWN 6



#define EEPROM_NUMBER_OF_CAL_UNITS_PER_CHANNEL 5
#define CALIBRATE_VALUE 0
#define NOT_CALIBRATE_VALUE 1

#define SET_BOTH    0
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
	uint8_t nodeNumber;
	uint8_t channelNumber;
	calibration_channel_t calibration_page[EEPROM_NUMBER_OF_CAL_UNITS_PER_CHANNEL];
}recalibration_data_t;


typedef struct{
	uint8_t nodeNumber;
	uint8_t commandType;
}command_data_t;



ServerCmdProc::ServerCmdProc(QObject *parent,char *name): QObject(parent){

 CMD_STRING="CMD";
 PWR_STRING="POWER";
 PS_STRING= "PS:";
 SET_STRING="SET";
 SET_VOLT_STRING="SET_U";
 SET_CURRENT_STRING="SET_I";
 SETUI_STRING="SET_UI"; // set both u and i simultaneour expected string: CMD POWER _NODE_ SET_UI _Channel_ double(U) double(I)
 GET_STRING="GET";
 ID_STRING= "ID";
 DELIM_STRING=" ";
 START_STRING= "Powerup";
 STOP_STRING= "Shutdown";
 S3B_STRING="S3B";
 LOG_STRING="LOG";

emit LogInfo("TOKENIZER: Instance created");

}

ServerCmdProc::~ServerCmdProc(){ 
	
}


bool ServerCmdProc::Tokenize(){  // tokenizer not stable with respect to length of list!!!
	
	bool retval=false;
    bool is_ok=false;

	int numEnt;
	emit LogInfo("TOKENIZER: Received String");

	List=new QStringList(Command->split(QRegExp("\\s+")));
	numEnt=List->length();
	//List->remove(S3B);
	
	if(numEnt<=2){emit LogInfo("TOKENIZER: No CMD recognized"); return false;}
	
	if(List->at(0)==CMD_STRING && List->at(1)==PWR_STRING){
	// identified command
		    Node=List->at(2).toInt(&is_ok);
			
			if (is_ok==false){ emit LogInfo("TOKENIZER: Received wrong CMD format"); return false;}

			if(List->at(3)==START_STRING){
										emit LogInfo("TOKENIZER: Received Start");
										CmdStart();
										retval=true;}										
			if(List->at(3)==STOP_STRING){
										emit LogInfo("TOKENIZER: Received Stop");
										CmdStop();
										retval=true;}
			if(List->at(3)==LOG_STRING){
										emit LogInfo("TOKENIZER: Log Request");  
										SendLog();
										retval=true;}
			if(List->at(3)==SET_STRING){
										if(numEnt>=6){
											emit LogInfo("TOKENIZER: Set request");}
										else {
											emit LogInfo("TOKENIZER: # Parameter wrong");
											return false;
										}
										Channel=List->at(4).toInt(&is_ok);
										Voltage=List->at(5).toInt();
										
										SetVoltage();
										retval=true;}

			if(List->at(3)==SET_VOLT_STRING){
										if(numEnt>=6){
											emit LogInfo("TOKENIZER: Set Voltage request");}
										else {
											emit LogInfo("TOKENIZER: # Parameter wrong");
											return false;
										}
										Channel=List->at(4).toInt(&is_ok);
										Voltage=List->at(5).toInt();
										
										SetVoltage();
										retval=true;}

			if(List->at(3)==SET_CURRENT_STRING){
										if(numEnt>=6){
											emit LogInfo("TOKENIZER: Set Current request");}
										else {
											emit LogInfo("TOKENIZER: # Parameter wrong");
											return false;
										} 
										Channel=List->at(4).toInt(&is_ok);
										Current=List->at(5).toInt();
										
										SetCurrent();
										retval=true;}


			if(List->at(3)==SETUI_STRING){
										if(numEnt>=7){
											emit LogInfo("TOKENIZER: Set Voltage / Current request");}
										else {
											emit LogInfo("TOKENIZER: # Parameter wrong");
											return false;
										}
										Channel=List->at(4).toInt(&is_ok);
										Voltage=List->at(5).toInt();
										Current=List->at(6).toInt(); // sanity check with bool is ok!
										SetCalibratedValue();
										retval=true;
										}	

			if(List->at(3)==GET_STRING){
										emit LogInfo("TOKENIZER: Get Voltage request");
										
										Channel=List->at(4).toInt(&is_ok);
										
										emit GetVoltage(Node,Channel);
										retval=true;
										}
						

					 			//else { this.reset();} // give some error message
			}
	
	else {
		Reset();
			}
	
	return retval;

	}

void ServerCmdProc::SetCalibratedValue(){
	
    calibration_input_data_t data;
	data.nodeNumber = (uint8_t) Node; // ps node 0-39
    data.channelNumber =(uint8_t) Channel; // channel of ps 0-23
	data.calibrate_bool =(uint8_t) CALIBRATE_VALUE;
	data.voltage_or_current_bool = SET_BOTH ;
	data.voltage = (int32_t)Voltage; // need to set voltage in mV in Integer
    data.current = (int32_t)Current; // need to set voltage in mA in Integer
    
	emit LogInfo("TOKENIZER: sendDataWrapper SET Calibrated");
    
	sendDataWrapper(CALIBRATE, &data, sizeof(data)); // callibrate topic: 2
    
	emit updateInfo((int)data.nodeNumber, (int)data.channelNumber, (int)data.voltage,(int) data.current); // function of calibration window sets values in gui

	}

void ServerCmdProc::SendLog(){}

void ServerCmdProc::SetVoltage(){

  calibration_input_data_t data;
	data.nodeNumber =(uint8_t) Node;
    data.channelNumber = (uint8_t) Channel;
	data.calibrate_bool = CALIBRATE_VALUE;
	data.voltage_or_current_bool = SET_VOLTAGE;
    data.voltage = (int32_t)Voltage; // need to set voltage in mV in Integer
    data.current = (int32_t)Current; // need to set voltage in mA in Integer
   
	 emit LogInfo("TOKENIZER: sendDataWrapper SET Calibrated Voltage");

	sendDataWrapper(CALIBRATE, &data, sizeof(data));

	emit updateVoltage((int)data.nodeNumber, (int)data.channelNumber, (int)data.voltage);
}

void ServerCmdProc::SetCurrent(){
	  calibration_input_data_t data;
	data.nodeNumber =(uint8_t) Node;
    data.channelNumber = (uint8_t) Channel;
	data.calibrate_bool = CALIBRATE_VALUE;
	data.voltage_or_current_bool = SET_CURRENT;
    data.voltage = (int32_t)Voltage; // need to set voltage in mV in Integer
    data.current = (int32_t)Current; // need to set voltage in mA in Integer
    
	emit LogInfo("TOKENIZER: sendDataWrapper SET Current");

	sendDataWrapper(CALIBRATE, &data, sizeof(data));
	
	emit updateCurrent((int)data.nodeNumber, (int)data.channelNumber,(int) data.current);

}

void ServerCmdProc::Reset(){ 
		List->clear();
		Command->clear();
		Node=Current=Voltage=Channel=0;
	}
	
void ServerCmdProc::CmdStop(){

	command_data_t dataToSend;
	dataToSend.commandType = STOP;
	dataToSend.nodeNumber = Node;
	//displayLogdata(dataToSend.nodeNumber, "STOP Command sent for node number: ",  dataToSend.nodeNumber);
	emit LogInfo("TOKENIZER: sendDataWrapper STOP");
  	sendDataWrapper(COMMAND, &dataToSend, sizeof(dataToSend));
	
}

	
void ServerCmdProc::CmdStart(){
	command_data_t dataToSend;
	dataToSend.commandType = START;
	dataToSend.nodeNumber = Node;
	emit LogInfo("TOKENIZER: sendDataWrapper Start");
	
	// displayLogdata(dataToSend.nodeNumber, "START Command sent for node number: ", dataToSend.nodeNumber);		
	 sendDataWrapper(COMMAND, &dataToSend, sizeof(dataToSend));
		
	}


 
void ServerCmdProc::SetCMD(QString *cmd){

Command=new QString(*cmd);

Tokenize();

}
