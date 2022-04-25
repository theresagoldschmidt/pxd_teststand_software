#include "SMU.h"


SMU::SMU(QObject *parent,char *name) : QObject(parent) {
	
	
	}


	SMU::~SMU(){}
	
	

void SMU::open_session(){

	QString str="Open Visa Session";
	
	emit SendInfo(str);

  /* Open session to VXI device at address 24 */
  viOpenDefaultRM (&defaultRM);
 
  viOpen(defaultRM, "USB0::0x0957::0x8B18::MY51140130::0::INSTR", VI_NULL, VI_NULL, &b2901);
 
  viPrintf(b2901, ":OUTPut1:ON:AUTO %@1d\n", 1);

   viPrintf(b2901, ":OUTPut:PROTection:STATe %@1d\n", 1);

}

void SMU::EnableOutput(){

	viPrintf(b2901, ":OUTPut1:STATe %@1d\n", 1);

}

 void SMU::DisableOutput(){
	
	 viPrintf(b2901, ":OUTPut1:STATe %@1d\n", 0);

}

 void SMU::GetOutputStat(){
	 long mode=0;
	 viQueryf(b2901, ":OUTPut:STATe?\n", "%d", &mode);
	 emit OutputStatus(mode);

 }


void SMU::SetVoltageCompliance(double volt){

	viPrintf(b2901, ":SENSe:VOLTage:DC:PROTection:LEVel %f\n", volt);

}

void SMU::SetCurrentCompliance(double current){

	viPrintf(b2901, ":SENSe:CURRent:DC:PROTection:LEVel %f\n", current);

}
	

void SMU::reset_session(){

	QString str="Reset Visa Session";
	
  /* Unmap memory space */
  viUnmapAddress (b2901);


  /* Close sessions */
  viClose (b2901);
  viClose (defaultRM);

  emit SendInfo(str);
}



void SMU::SourceCurrent(double curr){

		QString str="Set Current";
	
	QString buff=QString::number(curr,'f',6);

	str.append(buff);
	
	
	QString cmd=":SOURce1:CURRent:LEVel:IMMediate:AMPLitude "; 
	cmd.append(buff);
	cmd.append("\n");
	
	emit SendInfo(cmd);

	viPrintf(b2901,cmd.toAscii().data());

	//viPrintf(b2901, ":SOURce:CURRent:MODE %s\n", "FIX");

	
}

	

void SMU::SetVoltage(double volt){// needed for current measurements into short

	QString str="Set Voltage";
	
	QString buff=QString::number(volt,'f',6);

	str.append(buff);
	
	
	QString cmd=":SOURce1:VOLTage:LEVel:IMMediate:AMPLitude "; 
	cmd.append(buff);
	cmd.append("\n");
	
	emit SendInfo(cmd);

//	viPrintf(b2901,cmd.toAscii().data());

	viPrintf(b2901, ":SOURce1:VOLTage:LEVel:IMMediate:AMPLitude %f\n", volt);

} 
void SMU::GetCurrent(){
	
	QString str="Get Current: ";
	
	double current;

viQueryf(b2901, ":MEASure:CURRent:DC?\n", "%lf", &current);

str.append(QString::number(current));

emit SendInfo(str);

emit MeasuredCurrent(current);

} 

void SMU::GetVoltage(){
	
QString str="Get Voltage: ";
	
double volt;

viQueryf(b2901, ":MEASure:VOLTage:DC?\n", "%lf", &volt);

str.append(QString::number(volt));

emit SendInfo(str);

emit MeasuredVoltage(volt);

}

double SMU::GetVoltageDouble(){
	
QString str="Get Voltage: ";
	
double volt;

viQueryf(b2901, ":MEASure:VOLTage:DC?\n", "%lf", &volt);

return volt;
}


void SMU::SetSourceCurrent(){ viPrintf(b2901, ":SOURce:FUNCtion:MODE %s\n", "CURRent");   }

void SMU::SetSourceVoltage(){ 	viPrintf(b2901, ":SOURce:FUNCtion:MODE %s\n", "VOLTage"); }


void SMU::SetMeasureRangeManual(){ 	viPrintf(b2901, ":SENSe:VOLTage:DC:RANGe:AUTO 0\n"); }

void SMU::SetMeasureRangeAuto(){ 	viPrintf(b2901, ":SENSe:VOLTage:DC:RANGe:AUTO 1\n"); }

void SMU::SetMeasureRange(double volt){ 	viPrintf(b2901, ":SENSe:VOLTage:DC:RANGe:UPPer %f\n", volt); }

void SMU::SetRemoteSenseOn(){viPrintf(b2901, ":SENSe:REMote %@1d\n", 1);}

void SMU::SetRemoteSenseOff(){viPrintf(b2901, ":SENSe:REMote %@1d\n", 0);}

void SMU::SetApperture(){

	viPrintf(b2901, ":SENSe:VOLTage:DC:APERture %@3lf\n", 0.08);

	viPrintf(b2901, ":SENSe:CURRent:DC:APERture %@3lf\n", 0.08); 

}
