#include "CalManager.h"
#include <QCoreApplication>
 #include <QDir>
 #include <QTest>

#include <QStringList>
#include <QString>
#include <QRegExp>
#include <QFileDialog>

CalManager::CalManager(QObject *parent,char *name) : QObject(parent) {


    errorMessage=new QErrorMessage();

	QCoreApplication::setOrganizationName("LMU");
     QCoreApplication::setOrganizationDomain("www.lmu.de");
     QCoreApplication::setApplicationName("Lightweight Calibration");
	
	 QString path="C:/BelleII/devenv/workspace/LightCal/target/Debug/";
	 QString iniFile="C:/BelleII/devenv/workspace/LightCal/target/Debug/PS_Calibration.ini";
	 settings = new QSettings(iniFile,QSettings::IniFormat); //QSettings::UserScope);
	 settings->setDefaultFormat(QSettings::IniFormat);
	 settings->setPath(QSettings::IniFormat,QSettings::UserScope,path);

/*
	  settings->setValue("global/num_voltages", 16); // number of voltages to callibrate
      settings->setValue("global/points", 10);       // number of points per callibration 
	  settings->setValue("global/node", 1);
/************************************************************/
// Order of measurement & Assignment to Voltage
/************************************************************/

	 /*
	settings->setValue("0", "AVDD");
	settings->setValue("1", "Refin");	
	settings->setValue("2", "AMP_LOW");	
	settings->setValue("3", "Source");	


	settings->setValue("4", "DCD_DVDD");
	settings->setValue("5", "DHP_IO");	
	settings->setValue("6", "DHP_CORE");	
	settings->setValue("7", "DVDD_SW");	

	settings->setValue("8", "BULK");
	settings->setValue("9", "GATE_ON");	
	settings->setValue("10", "GATE_OFF");	
	settings->setValue("11", "CCG");	
	
	settings->setValue("12", "Clear_ON");
	settings->setValue("13", "Clear_OFF");	
	settings->setValue("14", "HV");	
	settings->setValue("15", "Guard");	
	
/************************************************************/
// Definition of the measurement ranges per channel
/************************************************************/
/*	  settings->setValue("AVDD/ID", 0);			// relais ID
	  settings->setValue("AVDD/StartVolt", 0);	// start voltage
	  settings->setValue("AVDD/StopVolt", 5);
	  settings->setValue("AVDD/StartCurrent", 0);	
	  settings->setValue("AVDD/StopCurrent", .5);
	
	  settings->setValue("Refin/ID", 1);	
	  settings->setValue("Refin/StartVolt", 0);	
	  settings->setValue("Refin/StopVolt", 5);
	  settings->setValue("Refin/StartCurrent", 0);	
	  settings->setValue("Refin/StopCurrent", .5);	
	
	  settings->setValue("Source/ID", 2);	
	  settings->setValue("Source/StartVolt", 0);	
	  settings->setValue("Source/StopVolt",5);
	  settings->setValue("Source/StartCurrent", 0);	
	  settings->setValue("Source/StopCurrent", .5);	
	
	  settings->setValue("AMP_LOW/ID", 3);
	  settings->setValue("AMP_LOW/StartVolt", 0);	
	  settings->setValue("AMP_LOW/StopVolt", 5);
	  settings->setValue("AMP_LOW/StartCurrent",.1);	
	  settings->setValue("AMP_LOW/StopCurrent", -0.5);	
	  
	  */

	  settings->sync();

	  PsMonValues = new int[6];


	  parse=new PsReceiveParser();

	 wait=false;
	 stop=true;


	}


CalManager::~CalManager(){}

void CalManager::SetPath(char *str){

	path=QString::QString(str);

}

void CalManager::ReloadSettings(){

//		 QString path="C:/BelleII/devenv/workspace/LightCal/target/Debug/";
//	 QString iniFile="C:/BelleII/devenv/workspace/LightCal/target/Debug/PS_Calibration.ini";
//	 settings = new QSettings(iniFile,QSettings::IniFormat); //QSettings::UserScope);
//	 settings->setDefaultFormat(QSettings::IniFormat);
//	 settings->setPath(QSettings::IniFormat,QSettings::UserScope,path);


QString fileName = QFileDialog::getOpenFileName(NULL, tr("Open File"), "", tr("Files (*.ini)"));

settings = new QSettings(fileName,QSettings::IniFormat);

QString notifier="Init File: ";
		notifier.append(fileName);

emit SendInfo(notifier);

	  settings->sync();



}

void CalManager::start_cal_session(){
	
	errorMessage->showMessage("Start Cal Session");
	errorMessage->exec();
						 
stop=false; // rest stop variable to false

QString notifier="Start Cal Session";

emit SendInfo(notifier);


	QString file_path;
	QFile   *file;
	QString data_string;
	
    double step_size;
	double step;

	//if(QDir::exists(path)!)
		
	QDir  dir;
	
if(dir.exists(path)==false) dir.mkdir(path); // create folder for data
 
dir.cd(path);

	int NumCalibrations = settings->value("global/num_voltages").toInt();
    int NumPoints =settings->value("global/points").toInt();
	int Node	  =settings->value("global/node").toInt();

notifier="Number of channels: ";
notifier.append(QString::number(NumCalibrations));
notifier.append(" Node: ");
notifier.append(QString::number(Node));
notifier.append(" Points: ");
notifier.append(QString::number(NumPoints));

emit SendInfo(notifier);

 ////////////////////////////////////////////////////////////////////////////
//loop over all channel
 ////////////////////////////////////////////////////////////////////////////
	int sync_counter;
	int counter_tot=0;
	int counter_chan=0;
	double prog1,prog2;
	int total_prog=NumCalibrations*NumPoints*3;  // three different measurements
	int total_chan=NumPoints*3;
	double milliStep;
	relais_chan=0;

	//*************************************************************************
	// loop over all channels
	//**************************************************************************
	
	for(int i=0;i<NumCalibrations;i++){
	
     
		
		retrive_channel_info(i); // sets the configuration variables

		 ////////////////////////////////////////////////////////////////////////////
		// measurement dac voltage vs. output voltage
		 ////////////////////////////////////////////////////////////////////////////

		file_path="Channel_";
		file_path.append(QString::number(ps_channel_id)); // id is placed in filename --> no continous numbering.
		file_path.append("_U_vs_U.dat");
		file=new QFile(file_path);
		file->open(QIODevice::ReadWrite);
		file->remove();
		file=new QFile(file_path);
		file->open(QIODevice::ReadWrite);

  	SourceMeasurementUnit->SourceCurrent(0); // smu acts as voltmeter
	 
	SourceMeasurementUnit->SetSourceCurrent();
		
	RelaisCard->SET_Channel(relais_chan); 
	SourceMeasurementUnit->SetApperture();
	QTest::qWait(50);
	SourceMeasurementUnit->SetRemoteSenseOn();

	//if(BiasCurrent>0.1) BiasCurrent=0.0;
	SourceMeasurementUnit->SourceCurrent(BiasCurrent); // smu acts as voltmeter
	 
	SourceMeasurementUnit->SetSourceCurrent();//   current sourced -- high impedand intput
	 QTest::qWait(150);
	//SourceMeasurementUnit->SetMeasureRangeManual();
	SourceMeasurementUnit->SetMeasureRangeAuto();
	 QTest::qWait(150);
	SourceMeasurementUnit->SetMeasureRangeAuto();
	 //SourceMeasurementUnit->SetMeasureRange(SMUVoltageRange); // set manual range to avoid kinks in callibration curve (up to 10mV kinks when range is changed at 2V/20V
	QTest::qWait(150);
	SourceMeasurementUnit->EnableOutput();
	QTest::qWait(150);
	SourceMeasurementUnit->SetVoltageCompliance(Compliance); // must be set above maximum output voltage for this measurement
	QTest::qWait(150);
	SourceMeasurementUnit->GetVoltage();
	QTest::qWait(150);
	SetPsCurrent(Node,ps_channel_id,2600); // generously to midscale 
   QTest::qWait(400);
   
   RequestPsInfo(Node,ps_channel_id);
   QTest::qWait(220);
   
   SetPsCurrent(Node,ps_channel_id,2500);
    QTest::qWait(420);

	step_size=(StopVolt-StartVolt)/(double)NumPoints;
	counter_chan=0;
   for(int k=0;k<NumPoints;k++){ // voltage dac vs. output
			
	    if(stop==true) break;
	   
			counter_tot++;
			counter_chan++;
			prog1=100.0*counter_tot/total_prog;
			prog2=1.00001*counter_chan/total_chan*100.0;
			if(k%5==0) emit progress((int)prog1,(int)prog2);
			
			step=StartVolt+step_size*k;
			milliStep=step*1000;
			SetPsVoltage(Node,ps_channel_id,(int)milliStep);
			QTest::qWait(300); // wait to allow voltage set!
	    	//SetPsVoltage(Node,ps_channel_id,(int)milliStep);
			//QTest::qWait(200);
			QTest::qSleep(200);	

			// further waiting on ps monitoring loop - data is requested from GUI!
		    SourceMeasurementUnit->GetVoltage();
			
			updated_ps=false;

					

			QTest::qWait(266);	
			RequestPsInfo(Node,ps_channel_id); 
			QTest::qWait(266);	
						
			
			sync_counter=0;
			while(updated_ps==false){
				sync_counter++;
				QTest::qWait(100); // event loop is still executed in this time 
				if(sync_counter>=10){
						errorMessage->showMessage("Timeout PS Message");
						errorMessage->exec();
						break; 
				}
			}

			data_string.clear();
		   data_string="";
		   data_string.append(QString::number(milliStep));
		   data_string.append(" ");
		   data_string.append(QString::number(1000*SMU_Voltage,'g',6));//PsMonValues
		   data_string.append(" ");
		   data_string.append(QString::number((double)PsMonValues[2],'g',6));
		   data_string.append(" ");
		   data_string.append(QString::number((double)PsMonValues[3],'g',6));
		   data_string.append(" ");
		   data_string.append(QString::number((double)PsMonValues[4],'g',6));
		   data_string.append(" ");
		   data_string.append(QString::number((double)PsMonValues[5],'g',6));
		   data_string.append("\r\n");
		   file->write((const char *)data_string.toAscii().data());
		    
		
  

   }


	file->close();

	
	SetPsVoltage(Node,ps_channel_id,0);
	QTest::qWait(400);
	SetPsCurrent(Node,ps_channel_id,0);
	QTest::qWait(400);
	delete file;

		 ////////////////////////////////////////////////////////////////////////////
		// measurement monitoring current vs. current
		 ////////////////////////////////////////////////////////////////////////////
	
		file_path="Channel_";
		file_path.append(QString::number(ps_channel_id));
		file_path.append("_I_vs_I.dat");
		file=new QFile(file_path);
		file->open(QIODevice::ReadWrite);
		file->remove();
		file=new QFile(file_path);
		file->open(QIODevice::ReadWrite);

	
	QTest::qWait(50);
	SourceMeasurementUnit->SetSourceCurrent(); // smu acts as current source - amperemeter
	QTest::qWait(50);
	SourceMeasurementUnit->SetVoltageCompliance(Compliance); // compliance at 5V
	QTest::qWait(50);
	SourceMeasurementUnit->GetVoltage();
	QTest::qWait(50);
	//SourceMeasurementUnit->SetMeasureRangeAuto();
	QTest::qWait(50);
	 SetPsVoltage(Node,ps_channel_id,1000*MidScale);// set ps output to .5V,1V,2.5V - value must fit to compliance
	 QTest::qWait(400);
	 SetPsCurrent(Node,ps_channel_id,5000); // generously to midscale 
	 QTest::qWait(500);
	 SetPsVoltage(Node,ps_channel_id,1000*MidScale);
	  QTest::qWait(500);
	   RequestPsInfo(Node,ps_channel_id);
	     QTest::qWait(300);

	int trial=0;
   
   while(PsMonValues[5]==0){
			  trial++;
			  SetPsCurrent(Node,ps_channel_id,5000); // generously to midscale 
			 QTest::qWait(350);
   
		   RequestPsInfo(Node,ps_channel_id);
		   QTest::qWait(300);
   
		   if(trial>=10){
						break;
						errorMessage->showMessage("Timeout PS Message");
						errorMessage->exec();	  
   				}
      }


	step_size=(StopCurr-StartCurr)/(double)NumPoints;	
	
	

	for(int k=0;k<NumPoints;k++){

		 if(stop==true) break;

			counter_tot++;
			counter_chan++;
			prog1=1.000*(double)counter_tot/total_prog*100.0;
			prog2=1.000*(double)counter_chan/total_chan*100.0;

		if(k%5==0)  emit progress((int)prog1,(int)prog2);


			step=StartCurr+(double)k*step_size;
			milliStep=step*1000;
			SourceMeasurementUnit->SourceCurrent(-step); // SMU must sink current for a PS source
			
			QTest::qWait(200);
			QTest::qSleep(200);		

	     	SourceMeasurementUnit->GetCurrent();
		
			updated_ps=false;
			QTest::qWait(266);	
			RequestPsInfo(Node,ps_channel_id); 
			QTest::qWait(66);	

			SourceMeasurementUnit->GetVoltage();
			
			sync_counter=0;
			while(updated_ps==false){
				sync_counter++;
				QTest::qWait(100); // event loop is still executed in this time 
				if(sync_counter>=10){
						errorMessage->showMessage("Timeout PS Message");
						errorMessage->exec();
						break; 
				}  
			}
			
			data_string.clear();
			data_string="";
			data_string.append(QString::number(milliStep));  //PsMonValues
			data_string.append(" ");
			data_string.append(QString::number(1000*SMU_Current,'g',6));
			data_string.append(" ");
			data_string.append(QString::number((double)PsMonValues[4],'g',6));
			data_string.append(" ");
			data_string.append(QString::number((double)PsMonValues[3],'g',6));
			data_string.append(" ");
			data_string.append(QString::number((double)PsMonValues[5],'g',6));
			data_string.append(" ");
			data_string.append(QString::number(1000*SMU_Voltage,'g',6));//PsMonValues
			data_string.append("\r\n");

			file->write((const char *)data_string.toAscii().data());

		}
	// reset to reasonalbe state

	file->close();
	SourceMeasurementUnit->SourceCurrent(0);
	QTest::qWait(400);
	SetPsVoltage(Node,ps_channel_id,0);
	QTest::qWait(400);
	SetPsCurrent(Node,ps_channel_id,0);
	QTest::qWait(400);
	delete file;

 ////////////////////////////////////////////////////////////////////////////
// measurement current limit vs. act. current limit
////////////////////////////////////////////////////////////////////////////

		file_path="Channel_";
		file_path.append(QString::number(ps_channel_id));
		file_path.append("_Ilimit_vs_I.dat");
		file=new QFile(file_path);
		file->open(QIODevice::ReadWrite);
		file->remove();
		file=new QFile(file_path);
		file->open(QIODevice::ReadWrite);


	SourceMeasurementUnit->SetSourceVoltage(); // smu acts as short - 
	QTest::qWait(150);
	SourceMeasurementUnit->SetVoltage(SMUOutputVoltage); // set output voltage at 0, acts as sink
	QTest::qWait(150);
	SourceMeasurementUnit->SetCurrentCompliance(3); // compliance at .5A
	QTest::qWait(150);
	SourceMeasurementUnit->GetVoltage();

	 SetPsCurrent(Node,ps_channel_id,0); // to 0 at the beginning
	 QTest::qWait(400);
	 SetPsVoltage(Node,ps_channel_id,1000*MidScale);// set ps output to .75V,1.5V,3V - value must fit resistance of traces
	 QTest::qWait(400);
	 //SetPsVoltage(Node,ps_channel_id,1000*MidScale);
	  QTest::qWait(400);

	 
	step_size=(StopLimCurr-StartLimCurr)/(double)NumPoints;	
	
	
	for(int k=0;k<NumPoints;k++){

		 if(stop==true) break;

			counter_tot++;
			counter_chan++;
			prog1=1.000*(double)counter_tot/total_prog*100.0;
			prog2=1.000*(double)counter_chan/total_chan*100.0;

			 emit progress((int)prog1,(int)prog2);
			 

			step=StartLimCurr+(double)k*step_size;
			milliStep=step*1000;
			
			//SetPsCurrent(Node,ps_channel_id,(int)milliStep);
			
			 //QTest::qWait(200);
			//QTest::qSleep(100);

			 SetPsCurrent(Node,ps_channel_id,(int)milliStep);
			 QTest::qWait(200);
			QTest::qSleep(100);	

	     	SourceMeasurementUnit->GetCurrent();
		
			updated_ps=false;

			QTest::qWait(366);	
			RequestPsInfo(Node,ps_channel_id); 
			QTest::qWait(166);	

			sync_counter=0;
			while(updated_ps==false){
				sync_counter++;
				QTest::qWait(100); // event loop is still executed in this time 
				if(sync_counter>=10){
						errorMessage->showMessage("Timeout PS Message");
						errorMessage->exec();
						break; 
				}  
			}
			
			data_string.clear();
			data_string="";
			data_string.append(QString::number(milliStep));  //PsMonValues
			data_string.append(" ");
			data_string.append(QString::number(1000*SMU_Current,'g',6));
			data_string.append(" ");
			data_string.append(QString::number((double)PsMonValues[4],'g',6));
			data_string.append(" ");
			data_string.append(QString::number((double)PsMonValues[3],'g',6));
			data_string.append(" ");
			data_string.append(QString::number((double)PsMonValues[5],'g',6));
			data_string.append("\r\n");

			file->write((const char *)data_string.toAscii().data());

		}
	
	file->close();

	
	delete file;	
	counter_chan=0;
 //********************************************************************************************************************************
// rest channel to 0 after measurement	
//********************************************************************************************************************************   
	SetPsCurrent(Node,ps_channel_id,0);
	QTest::qWait(400);
	SetPsVoltage(Node,ps_channel_id,0);
	 QTest::qWait(200);
	 SourceMeasurementUnit->SourceCurrent(0);
	 QTest::qWait(50);
	 SourceMeasurementUnit->SetSourceCurrent(); // smu acts as current source - amperemeter
	QTest::qWait(50);
	SourceMeasurementUnit->SourceCurrent(0);
   SetPsVoltage(Node,ps_channel_id,0);
	 QTest::qWait(200);
    QTest::qSleep(400);
	 SetPsCurrent(Node,ps_channel_id,0);
	QTest::qWait(400);
	SetPsVoltage(Node,ps_channel_id,0);

	}


	SourceMeasurementUnit->DisableOutput();
}

void CalManager::RetriveIni(){


settings->sync();



}

void CalManager::StopMeasurement(){

	stop=true;

}

void CalManager::SmuVoltageIs(double volt){

SMU_Voltage=volt;

}
void CalManager::SmuCurrentIs(double current){

SMU_Current=current;

}
void CalManager::PsVoltageIs(int regulator, int load){

PS_Regulator=(double)regulator/1000;
PS_Load		=(double)load/1000;

}
void CalManager::PsCurrentIs(double current){

PS_Current=current/1000;

}

void CalManager::retrive_channel_info(int i){


			QString buff=QString::number(i);

			channel=settings->value(buff.toAscii().data()).toString();

			QString relais="";
			relais.append(channel);
			relais.append("/ID");

			ps_channel_id=settings->value(relais.toAscii().data()).toInt();
			
			
		    relais="";
			relais.append(channel);
			relais.append("/RELAIS");
			relais_chan=settings->value(relais.toAscii().data()).toInt();


			relais="";
			relais.append(channel);
			relais.append("/StartVolt");

			StartVolt=settings->value(relais.toAscii().data()).toDouble();


			relais="";
			relais.append(channel);
			relais.append("/StopVolt");

			StopVolt=settings->value(relais.toAscii().data()).toDouble();

			relais="";
			relais.append(channel);
			relais.append("/StartCurrent");

			StartCurr=settings->value(relais.toAscii().data()).toDouble();

			relais="";
			relais.append(channel);
			relais.append("/Compliance");

			Compliance=settings->value(relais.toAscii().data()).toDouble();


			relais="";
			relais.append(channel);
			relais.append("/StopCurrent");

			StopCurr=settings->value(relais.toAscii().data()).toDouble();


			relais="";
			relais.append(channel);
			relais.append("/StartLimit");
			StartLimCurr=settings->value(relais.toAscii().data()).toDouble();

			relais="";
			relais.append(channel);
			relais.append("/StopLimit");

			StopLimCurr=settings->value(relais.toAscii().data()).toDouble();

			relais="";
			relais.append(channel);
			relais.append("/MidScale");
	
			MidScale=settings->value(relais.toAscii().data()).toDouble();


			relais="";
			relais.append(channel);
			relais.append("/BiasCurr");

			BiasCurrent=settings->value(relais.toAscii().data(),0).toDouble();; //BiasCurr

		

			relais="";
			relais.append(channel);
			relais.append("/SMUOutput");

			SMUOutputVoltage=settings->value(relais.toAscii().data(),0).toDouble(); //BiasCurr			


			relais="";
			relais.append(channel);
			relais.append("/Range");

			SMUVoltageRange=settings->value(relais.toAscii().data(),200).toDouble(); //Voltage Range of SMU Voltmeter --			

	

QString notifier="Callibrate: ";
notifier.append(channel);
notifier.append(" PS Channel: ");
notifier.append(QString::number(ps_channel_id));
notifier.append(" Relais: ");
notifier.append(QString::number(relais_chan));
notifier.append(" Start ");
notifier.append(QString::number(StartVolt));
notifier.append("V Stop: ");
notifier.append(QString::number(StopVolt));
notifier.append("V Compliance: ");
notifier.append(QString::number(Compliance));
notifier.append("V Start ");
notifier.append(QString::number(StartCurr));
notifier.append("A Stop: ");
notifier.append(QString::number(StopCurr));
notifier.append("A, Start I Limit: ");
notifier.append(QString::number(StartLimCurr));
notifier.append("A, Stop I Limit: ");
notifier.append(QString::number(StopLimCurr));
notifier.append("Voltage Range: ");
notifier.append(QString::number(SMUVoltageRange));


emit SendInfo(notifier);

}

void CalManager::TakeData(const QString & string){ // slot is called from server if data is received
		
//	PsMonValues = new int[6];
// LMU PS Demonstrator 1 Channel: 20 U_Reg: 200 U_Load: 194 I_Load: -3 Bit: 0 

parse->ParseString(string,PsMonValues);
	
updated_ps=true; 


}


void  CalManager::SetSMU(SMU *SourceMeasure){ SourceMeasurementUnit=SourceMeasure;}

void CalManager::SetRelaisCard(FTDI *Relais){RelaisCard=Relais;}



void CalManager::RequestPsInfo(int node, int channel){

	QString *str=new QString("CMD POWER ");
	str->append(QString::number(node));
	str->append(" GET ");
	str->append(QString::number(channel));
	str->append(" ");
	emit SendCMD(*str);

//	delete str;
}


  void CalManager::SetPsVoltage(int node, int channel,int volt){

	QString *str=new QString("CMD POWER ");
	str->append(QString::number(node));
	str->append(" SET_U ");
	str->append(QString::number(channel));
	str->append(" ");
	str->append(QString::number(volt));
	str->append(" ");
	emit SendCMD(*str);

//	delete str;
}

  void CalManager::SetPsCurrent(int node, int channel,int current){

	QString *str=new QString("CMD POWER ");
	str->append(QString::number(node));
	str->append(" SET_I ");
	str->append(QString::number(channel));
	str->append(" ");
	str->append(QString::number(current));
	str->append(" ");
	emit SendCMD(*str);

//	delete str;
}

