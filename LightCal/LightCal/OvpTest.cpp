#include "OvpTest.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTest>
#include <QStringList>
#include <QString>
#include <QRegExp>
#include <QFileDialog>


OvpTester::OvpTester(QObject *parent, char *name) : QObject(parent) {
	
	//QCoreApplication::setOrganizationName("LMU");
    //QCoreApplication::setOrganizationDomain("www.lmu.de");
	//QCoreApplication::setApplicationName("OVP-Tester");

	//InputNode -> setText("0");
	path = "C:/LightCal";
	QString inifile = "C:/LightCal/MainProdMasterOVP_TESTING.ini";
	settings = new QSettings(inifile, QSettings::IniFormat);
	settings -> setDefaultFormat(QSettings::IniFormat);
	//settings -> setPath(QSettings::IniFormat, QSettings::UserScope, path);
	settings -> sync();
	
	QString notifier = "Init File: ";
	notifier.append(inifile);
	emit SendInfo(notifier);
	notifier.clear();

	//wait = false; 
	stop = true;
	ID_SW_SUB = 22;
	ID_SW_REFIN = 21;
	Relais_SW_SUB = 13;
	Relais_SW_REFIN = 14;
	Node = settings->value("global/node").toInt(); // Unique ID for each PS unit; // Not important anymore. s. StartOVPTest()

	for (int n = 0; n < 24; ++n) {
		passed [n] = true;
	}

	//InputNode -> SetText(QString("%1").arg(Node)
	

}

OvpTester::~OvpTester() {}

void OvpTester::ReceiveNodeGui(int i){

Node=i;

}



void OvpTester::retrive_channel_info(int i) {
	
	QString buff, buffID, buffCheckInt, buffStart, buffRelais, buffRange, buffChecktype;
	QString buffLimit_L, buffLimit_H, buffOVP_Start_L, buffOVP_Start_H, buffOVP_Stop_L, buffOVP_Stop_H;
	QString buffChannel_Name;
	buff.clear();
	buff.append(QVariant(i).toString());
	buffID = buff;
	buffID.append("/ID");
	buffCheckInt = buff;
	buffCheckInt.append("/Checkint");
	buffRelais = buff;
	buffRelais.append("/RELAIS");
	buffLimit_L = buff;
	buffLimit_L.append("/Limit_L");
	buffLimit_H = buff;
	buffLimit_H.append("/Limit_H");
	buffRange = buff;
	buffRange.append("/Range");
	buffOVP_Start_L = buff;
	buffOVP_Start_L.append("/OVP_Start_L");
	buffOVP_Start_H = buff;
	buffOVP_Start_H.append("/OVP_Start_H");
	buffOVP_Stop_L = buff;
	buffOVP_Stop_L.append("/OVP_Stop_L");
	buffOVP_Stop_H = buff;
	buffOVP_Stop_H.append("/OVP_Stop_H");
	buffChecktype = buff;
	buffChecktype.append("/Checktype");
	buffChannel_Name = buff;
	buffChannel_Name.append("/name");
	ID = settings -> value(buffID).toInt();
	CheckInt = settings -> value(buffCheckInt).toInt();
	Relais = settings -> value(buffRelais).toInt();
	Limit_L = settings -> value(buffLimit_L).toDouble();
	Limit_H = settings -> value(buffLimit_H).toDouble();
	Range = settings -> value(buffRange).toDouble();
	OVP_Start_L = settings -> value(buffOVP_Start_L).toDouble();
	OVP_Start_H = settings -> value(buffOVP_Start_H).toDouble();
	OVP_Stop_L = settings -> value(buffOVP_Stop_L).toDouble();
	OVP_Stop_H = settings -> value(buffOVP_Stop_H).toDouble();
	Checktype = settings -> value(buffChecktype).toString();
	Channel_Name = settings -> value(buffChannel_Name).toString();
}

void OvpTester::WorkAroundInitialization() {
	///* This loop is a workaround to bring the PS into a defined state. It exits as soon as setting a voltage works
	// or after ten failed attempts to set a voltage. 
	TestCounter = 0;
	int ID_Test;
	if (CheckInt == 2) {
		if (ID == 16 || ID == 17 || ID == 19) { // The Gates have to start positive as SW_SUB is still at 0
			// and they can't be lower than SW_SUB
			TestVolt1 = 1.0;
		}
		else {
			TestVolt1 = -1.0;
		}
		ID_Test = ID;
	}

	else if (CheckInt == 4) {
		TestVolt1 = -0.2;
		ID_Test = ID;
	}

	else if (CheckInt == 5) {
		TestVolt1 = -1.0;
		ID_Test = ID_SW_SUB;
	}

	else {
		TestVolt1 = 1.0;
		ID_Test = ID;
	}

	if(CheckInt==1000){
	TestVolt1=-1;
	ID_Test=ID;
	}

	TestVolt2 = 0;

			while (abs(TestVolt1 - TestVolt2) > 0.1) {
				DisablePS(Node);
				QTest::qWait(500);
				EnablePS(Node);
   				QTest::qWait(500);
				SetPsCurrent(Node, ID_Test, 500);
				QTest::qWait(500);
				SetPsVoltage(Node, ID_Test, (int)(1000*TestVolt1));
				QTest::qWait(500);
				TestVolt2 = SourceMeasurementUnit->GetVoltageDouble();
				TestCounter += 1;

				if (TestCounter > 10) {
					emit SendInfo("Error: Not able to initialize a sensible state!");
					data.append("\nError: Not able to initialize a sensible state!\n!");
					TestSuccessful = false;
					file->write((const char *)data.toAscii().data());
					data.clear();
					StillOn = false;
					PS_Working = false;
					break;

				}
			}
			//*/ End of initializing loop
}

void OvpTester::PrepareGateOnTest() { // Sets SW_SUB and SW_RefIn to -15 Volt
	emit GetNodeGui();
	emit SendInfo("Setting SW_SUB and SW_RefIn to -15 Volt");

	double TestVolt, SMU_SUB, SMU_REFIN;
	for (int i=1; i<=10; ++i) {
		TestVolt = -1.5 * i;
		SetPsVoltage(Node, ID_SW_SUB, (int) (1000*TestVolt));
		QTest::qWait(400);
		SetPsVoltage(Node, ID_SW_REFIN, (int) (1000*TestVolt));
		QTest::qWait(400);
			}

		RelaisCard->SET_Channel(Relais_SW_SUB);
		QTest::qWait(500);
		SMU_SUB = SourceMeasurementUnit -> GetVoltageDouble();
		RelaisCard->SET_Channel(Relais_SW_REFIN);
		QTest::qWait(500);
		SMU_REFIN = SourceMeasurementUnit -> GetVoltageDouble();

		if (abs(TestVolt - SMU_SUB) > 0.2 || abs(TestVolt - SMU_REFIN) > 0.2) {
			emit SendInfo("Failed to prepare SW_SUB and SW_REFIN");}
			

		data.append("\nVoltage at SW_SUB and SW_REFIN set to ");
		data.append(QVariant(TestVolt).toString());
		data.append(" Volt");
	
				
}


void OvpTester::TestUpperLimit() {
	QTest::qWait(2000);
	while (StillOn == true && Voltage <= OVP_Stop_H && stop == false) {
				
				SetPsVoltage(Node, ID, (int)(1000*Voltage)); //Setting the Voltage
				QTest::qWait(500);
				SMUVolt=SourceMeasurementUnit->GetVoltageDouble(); //Measure Voltage with SMU
				QTest::qWait(200);

				///* Check whether first voltage was set correctly
				if (abs(Voltage - SMUVolt)/abs(Voltage) > 0.05 && counter == 0) {
					emit SendInfo("Initialzing voltage did not work\n");
					emit SendInfo("Either 'OVP_Start_H' was set too high in the ini File, or something else did not work");
					data.append("\nInitializing voltage did not work\n");
					file->write((const char *)data.toAscii().data());
					data.clear();
					TestSuccessful = false;
					break;

				}
				//*/

				VoltageString.clear();
				VoltageString = QString("%1").arg(Voltage);
				data.append(VoltageString);
				data.append("\t");
				if (abs(SMUVolt) < 0.25) { // Test ob PS abgeschalten ist
					StillOn = false;
					ShutDown = true;
					data.append("\nUnit shut down at ");
					data.append(VoltageString);
					data.append(" Volt \n");

					status = "Unit Shutdown";
					emit SendInfo(status);

					if (Voltage <= Limit_H + 0.01){ // OVP works correctly
						data.append("passed \n");
						emit SendInfo("Test passed");
					}
					else { //OVP does not work correctly
						data.append("failed \n");
						emit SendInfo("\nPassed set to false in case 1\n");
						Passed = false;
						testpassed = false;
					}
					status.clear();
				}

				status = "Voltage set to: ";
				status.append(VoltageString);
				emit SendInfo(status);
				status.clear();

				Voltage += VoltageStep;
				counter +=1;
			}
}

void OvpTester::TestLowerLimit() {

	while(StillOn == true && Voltage >= OVP_Stop_L && stop == false) {
				
				SetPsVoltage(Node,ID,(int)(1000*Voltage)); //Powersupply works with mV instead of Volt
				QTest::qWait(500);
				SMUVolt=SourceMeasurementUnit->GetVoltageDouble(); //Measure Voltage with SMU
				QTest::qWait(200);

				///* Check whether first voltage was set correctly
				if (abs(Voltage - SMUVolt) > 0.1 && counter == 0) {
					emit SendInfo("Initialzing voltage did not work\n");
					emit SendInfo("Either 'OVP_Start_H' was set too high in the ini File, or something else did not work");
					data.append("\nInitializing voltage did not work\n");
					file->write((const char *)data.toAscii().data());
					data.clear();
					TestSuccessful = false;
					break;

				}
				//*/

				VoltageString.clear();
				VoltageString = QString("%1").arg(Voltage);
				data.append(VoltageString); //Write current Voltage to 'data'
				data.append("\t");

				if (abs(SMUVolt) < 0.3) { // Test ob PS abgeschalten ist

					StillOn = false;
					ShutDown = true;
					data.append("\nUnit shut down at "); 
					data.append(VoltageString);
					data.append(" Volt \n");
					status.clear();
					status = "Unit Shutdown";
					emit SendInfo(status);
					status.clear();

					if (Voltage >= Limit_L) { //Test passed
						data.append("passed \n");
						emit SendInfo("Test passed");
					}
					else {
						data.append("failed \n");
						emit SendInfo("\nPassed set to false in case 2\n");
						Passed = false;
						testpassed = false;
					}
				}
				status.clear();
				status = "Voltage set to: ";
				status.append(VoltageString);
				emit SendInfo(status); // Send Info about current voltage
				status.clear();

				Voltage -= VoltageStep; // increase voltage
				counter +=1;
			}
}


void OvpTester::StartOvpTest() {


	// get node from gui
	emit GetNodeGui();

	stop = false;
	Passed = true;
	TestSuccessful = true;

	status = "OVP Test has started";
	emit SendInfo(status);
	status.clear();

	int NumCalibrations = settings->value("global/num_voltages").toInt();
    int NumPoints =settings->value("global/points").toInt();
	

	VoltageStep = 0.05;
	int N;
	QDir dir(path);	// QFile is now defined in the header
	if (!dir.exists()) {
		dir.mkdir(path); 
		dir.cd(path);
	}

	// configure SMU to voltage measurement:
	SourceMeasurementUnit->SetMeasureRangeAuto();
	QTest::qWait(150);
	SourceMeasurementUnit->SetRemoteSenseOn();
	QTest::qWait(150);
	SourceMeasurementUnit->SourceCurrent(0);
	QTest::qWait(150);
	//SourceMeasurementUnit->SetVoltageCompliance
	SourceMeasurementUnit->SetSourceCurrent();
	QTest::qWait(150);
	SourceMeasurementUnit->EnableOutput();

	status = "SMU initialized";
	emit SendInfo(status);
	status.clear();

	NodeString  = QString("%1").arg(Node);
	QString file_path = path;
	file_path.append("/results.");
	file_path.append(NodeString);
	file_path.append(".dat");
	file = new QFile(file_path);
	file->open(QIODevice::ReadWrite);

	status = "Output file set to: ";
	status.append(file_path);
	emit SendInfo(status);
	status.clear();
	

	for (N = 0; N < 24; N +=1) { //Testing every Channel
		
		if (stop == true) {
			data.append("\nTest terminated\n");
			file->write((const char *)data.toAscii().data());
			break;
		}

		emit SendInfo("Testing " + Channel_Name);
		VoltageString.clear();
		retrive_channel_info(N);
		data.clear();
		data.append("Test: ");
		NString  = QString("%1").arg(N);
		data.append(NString);
		data.append("\t");
		data.append(Channel_Name);
		data.append("\t");
		data.append(Checktype);
		data.append("\n");

		testpassed = true;
		StillOn = true; 
		ShutDown = false;
		PS_Working = true; // Set to false if unable to set voltage on PS
		counter = 0;

		switch (CheckInt) {

		case 1: //We test an upper limit
			

			emit SendInfo("Case 1: An upper limit is tested");

			RelaisCard->SET_Channel(Relais);
			QTest::qWait(100);
			Voltage = OVP_Start_H;
			DisablePS(Node);
			QTest::qSleep(500);
			DisablePS(Node);
			QTest::qWait(500);
			EnablePS(Node); //Switch on Powersupply
			QTest::qWait(500);
			EnablePS(Node); //Switch on Powersupply
			//QTest::qSleep(2000);
			QTest::qWait(500);
			SetPsCurrent(Node,ID,500); // current limit
			QTest::qWait(500);

			if (ID == 7) {  //AMP_LOW needs some special treatment
			SourceMeasurementUnit->SetVoltageCompliance(5); // must be set above maximum output voltage for this measurement
			QTest::qWait(150);
			SourceMeasurementUnit->SourceCurrent(0.001); // 0.001 = 1mA
			}

			WorkAroundInitialization(); //Trying to set a voltage, sets PS_Working to false if unsuccessful.
			
			

			// Break if initializing did not work
			if (!PS_Working) {
				break;
			}
			

			TestUpperLimit();

			if (ID==7) {  // After testing AMP_LOW we reset to normal settings
			SourceMeasurementUnit->SourceCurrent(0);
			SourceMeasurementUnit->SetVoltageCompliance(30);
			}

			if (ShutDown == false) { 
				Passed = false;
				testpassed = false;
				data.append("\nUnit did not shut down \nfailed\n");
				emit SendInfo("\nPassed set to false at the end of case 1\n");
			}

			data.append("\n");
			data.append("--------------------------------------------------\n");
			file->write((const char *)data.toAscii().data()); //Write data to file
			//emit SendInfo(data); //Write data to screen
			data.clear();
			break;

		case 2: //We test a lower limit
			emit SendInfo("Case 2: A lower limit is tested");

			RelaisCard->SET_Channel(Relais);
			QTest::qWait(100);
			Voltage = OVP_Start_L;
			DisablePS(Node);
			QTest::qWait(500);
			EnablePS(Node);
			QTest::qWait(500);
			SetPsCurrent(Node,ID,500); // current limit
			QTest::qWait(200);

			WorkAroundInitialization();

			if (ID == 16 || ID == 17 || ID == 19) { //Set SW_SUB and SW_REFIN to -15 Volt to test the Gates 1,2 and 3
				PrepareGateOnTest();
			}

			RelaisCard->SET_Channel(Relais);

			// Break if initializing did not work
			if (!PS_Working) {
				break;
			}

			TestLowerLimit();

			if (ShutDown == false) { 
				Passed = false; 
				testpassed = false;
				data.append("\nUnit did not shut down \nfailed\n");
				emit SendInfo("\nPassed set to false at the end of case 2\n");
			}

			data.append("\n");
			data.append("--------------------------------------------------\n");
			file->write((const char *)data.toAscii().data()); //Write data to file
			//emit SendInfo(data); //Write data to screen
			data.clear();
			break; 

		case 3: //We test a certain range 

			emit SendInfo("Case 3: Voltage has to stay in certain range");

			RelaisCard->SET_Channel(Relais);
			QTest::qWait(100);
			Voltage = OVP_Start_H; //We start with testing the upper limit
			status.clear();
			DisablePS(Node);
			QTest::qWait(500);
			EnablePS(Node);
			QTest::qWait(500);
			SetPsCurrent(Node, ID, 500); // current limit
			QTest::qWait(200);

			WorkAroundInitialization();

			// Break if initializing did not work
			if (!PS_Working) {
				break;
			}

			TestUpperLimit();

			if (ShutDown == false) { 
				Passed = false; 
				testpassed = false;
				data.append("\nUnit did not shut down \nfailed\n");
				emit SendInfo("\nPassed set to false at the end of case 3a\n");
			}

			file->write((const char *)data.toAscii().data()); //Write data to file
			//emit SendInfo(data); //Write data to screen

			// We now test the lower limit

			data.clear();
			data.append("Voltage: \t");
			Voltage = OVP_Start_L;
			status.clear();
			StillOn = true; 
			ShutDown = false;
			counter = 0;
			DisablePS(Node);
			QTest::qWait(500);
			EnablePS(Node);
			QTest::qWait(500);
			SetPsCurrent(Node,ID,500); // current limit
			QTest::qWait(200);

			WorkAroundInitialization();

			// Break if initializing did not work
			if (!PS_Working) {
				break;
			}

			TestLowerLimit();

			if (ShutDown == false) { 
				Passed = false; 
				testpassed = false;
				data.append("\nUnit did not shut down \nfailed\n");
				emit SendInfo("\nPassed set to false at the end of case 3b\n");
			}

			data.append("\n");
			data.append("--------------------------------------------------\n");
			file->write((const char *)data.toAscii().data()); //Write data to file
			//emit SendInfo(data); //Write data to screen
			data.clear();
			break;

		case 4: //Voltage - Voltage(SW_SUB) must be smaller than 2.3 V
			emit SendInfo("Case 4: Voltage must not be more than 2.3 V above Voltage(SW_SUB) but less than 0.5 V below");
			
			RelaisCard->SET_Channel(Relais_SW_REFIN);
			WorkAroundInitialization();
			// Initialize PS - Set voltage for SW_SUB
			
			RelaisCard->SET_Channel(Relais_SW_SUB); //Going to Relais of SW_SUB
			QTest::qWait(100);
			SetPsCurrent(Node, ID_SW_SUB, 500);
			QTest::qWait(200);
			SetPsVoltage(Node, ID_SW_SUB, (int)(-1500)); // SW_SUB has to have the lowest voltage
			// Set voltage RefIn 
			QTest::qWait(400);
			RelaisCard->SET_Channel(Relais); //Going to Relais of SW_RefIn
			QTest::qWait(100);
			
			

			// We still are at the SW_RefIn Relais and try to initialize working state

			Voltage_SW_RefIn = -1.0; // This voltage will stay constant, we will change only the voltage of SW_SUB
			SetPsCurrent(Node,ID,500); // current limit
			QTest::qWait(200);
			SetPsVoltage(Node,ID,(int)(1000*Voltage_SW_RefIn)); // Set voltage on SW_RefIn
			QTest::qWait(500);
			SMUVolt=SourceMeasurementUnit->GetVoltageDouble();
			QTest::qWait(200);

			if (abs(Voltage_SW_RefIn - SMUVolt) > 0.1) { 
				//Abort if setting the voltage to SW_RefIn does not work
				data.append("\nFailed to set Voltage at SW_RefIn \n\n");
				file->write((const char *)data.toAscii().data());
				data.clear();
				emit SendInfo("Failed to set Voltage at SW_RefIn \n");
				TestSuccessful = false;
				break;
			}

			data.append("\nSW_RefIn set to ");
			VoltageString.clear();
			VoltageString = QString("%1").arg(Voltage_SW_RefIn);
			data.append(VoltageString);
			data.append(" Volt\n");

			// So far: Voltage at SW_RefIn set to 1 Volt

			// Test SW_RefIn - SW_SUB < 2.3 Volt. 
			// We start with SW_RefIn - SW_SUB = OVP_Start_H 
			// => SW_SUB = SW_RefIn - OVP_Start_H ( = -1 - 1.8 = -2.8)
			// We then decrease SW_SUB to get closer to SW_RefIn - SW_SUB = 2.3
			Voltage = Voltage_SW_RefIn - OVP_Start_H; //Voltage is Voltage_SW_SUB
			RelaisCard->SET_Channel(Relais_SW_SUB); //Going to Relais of SW_SUB
			QTest::qWait(100);

			while (StillOn == true && (Voltage_SW_RefIn - Voltage) < OVP_Stop_H && stop == false) {


				SetPsVoltage(Node, ID_SW_SUB,(int)(1000*Voltage)); 
				QTest::qWait(500);
				SMUVolt=SourceMeasurementUnit->GetVoltageDouble();

				data.append("SW_SUB set to ");
				VoltageString = QString("%1").arg(Voltage);
				data.append(VoltageString);
				data.append(" Volt\n");

				if (abs(SMUVolt) < 0.25) {

					StillOn = false;
					ShutDown = true;
					data.append("\nUnit shut down at ");
					data.append(VoltageString);
					data.append(" Volt \n");
					status = "Unit Shutdown";
					emit SendInfo(status);
					status.clear();

					if (Voltage_SW_RefIn - Voltage <= Limit_H) {
						data.append("passed \n");
						emit SendInfo("Test passed");
					}
					else {
						data.append("failed \n");
						emit SendInfo("Test failed");
						Passed = false;
						testpassed = false;
					}
				}

				status = "Voltage_SW_SUB set to: ";
				status.append(VoltageString);
				emit SendInfo(status);
				status.clear();

				Voltage -= VoltageStep;
			} // End While loop

			if (ShutDown == false) { 
				Passed = false; 
				testpassed = false;
				data.append("\nUnit did not shut down \nfailed\n");
				emit SendInfo("failed");
			}

			// Finished testing the upper limit. Now we start testing the lower limit:
			// The next few lines are copied from above as we again need to initialize the PS unit. 

			// Some variables have to be reinitialized:
			StillOn = true;
			ShutDown = false;

			RelaisCard->SET_Channel(Relais_SW_REFIN);
			WorkAroundInitialization(); 
			// Initialize PS - Set voltage for SW_SUB
			
			RelaisCard->SET_Channel(Relais_SW_SUB); //Going to Relais of SW_SUB
			QTest::qWait(100);
			SetPsCurrent(Node, ID_SW_SUB, 500);
			QTest::qWait(200);
			SetPsVoltage(Node, ID_SW_SUB, (int)(-1500)); // SW_SUB has to have the lowest voltage
			// Set voltage RefIn 
			QTest::qWait(400);
			RelaisCard->SET_Channel(Relais); //Going to Relais of SW_RefIn
			QTest::qWait(100);
			
			

			// We still are at the SW_RefIn Relais and try to initialize working state

			Voltage_SW_RefIn = -1.0; // This voltage will stay constant, we will change only the voltage of SW_SUB
			SetPsCurrent(Node,ID,500); // current limit
			QTest::qWait(200);
			SetPsVoltage(Node,ID,(int)(1000*Voltage_SW_RefIn)); // Set voltage on SW_RefIn
			QTest::qWait(500);
			SMUVolt=SourceMeasurementUnit->GetVoltageDouble();
			QTest::qWait(200);

			if (abs(Voltage_SW_RefIn - SMUVolt) > 0.1) { 
				//Abort if setting the voltage to SW_RefIn does not work
				data.append("\nFailed to set Voltage at SW_RefIn \n\n");
				file->write((const char *)data.toAscii().data());
				data.clear();
				emit SendInfo("Failed to set Voltage at SW_RefIn \n");
				TestSuccessful = false;
				break;
			}

			data.append("\nSW_RefIn set to ");
			VoltageString.clear();
			VoltageString = QString("%1").arg(Voltage_SW_RefIn);
			data.append(VoltageString);
			data.append(" Volt\n");

			// So far: Voltage at SW_RefIn set to 1 Volt. Now we can test the lower limit. 

			// We now test SW_RefIn - SW_SUB > -0.5
			// We start with SW_RefIn - SW_SUB = OVP_Start_L = 0.2
			// Therefore we set SW_SUB = SW_RefIn - OVP_Start_L = 0.8 and then increase it. 
			Voltage = Voltage_SW_RefIn - OVP_Start_L; //Voltage is Voltage_SW_SUB
			RelaisCard->SET_Channel(Relais_SW_SUB); //Going to Relais of SW_SUB
			QTest::qWait(100);

			while (StillOn == true && (Voltage_SW_RefIn - Voltage) > OVP_Stop_L && stop == false) {

				SetPsVoltage(Node, ID_SW_SUB,(int)(1000*Voltage)); 
				QTest::qWait(500);
				SMUVolt=SourceMeasurementUnit->GetVoltageDouble();

				data.append("SW_SUB set to ");
				VoltageString = QString("%1").arg(Voltage);
				data.append(VoltageString);
				data.append(" Volt\n");

				if (abs(SMUVolt) < 0.25) {

					StillOn = false;
					ShutDown = true;
					data.append("\nUnit shut down at ");
					data.append(VoltageString);
					data.append(" Volt \n");
					status = "Unit Shutdown";
					emit SendInfo(status);
					status.clear();

					if (Voltage_SW_RefIn - Voltage <= Limit_H) {
						data.append("passed \n");
						emit SendInfo("Test passed");
					}
					else {
						data.append("failed \n");
						emit SendInfo("Test failed");
						Passed = false;
						testpassed = false;
					}
				} // End if clause

				status = "Voltage_SW_SUB set to: ";
				status.append(VoltageString);
				emit SendInfo(status);
				status.clear();

				Voltage += VoltageStep;

			} // End While loop

			if (ShutDown == false) { 
				Passed = false; 
				testpassed = false;
				data.append("\nUnit did not shut down \nfailed\n");
				emit SendInfo("failed");
			}

			data.append("\n");
			data.append("--------------------------------------------------\n");
			file->write((const char *)data.toAscii().data()); //Write data to file
			//emit SendInfo(data); //Write data to screen
			data.clear();
			
			break;

		case 5: //Voltage(Gate_ON_[1,2,3]) - Voltage(SW_SUB) must be larger than -0.6
			// str: Voltage (SW_REF)- VOltage(SW_SUB) must be lager than -0.6V
			emit SendInfo("Case 5: Gate_On_n (with n = 1,2,3) must not be larger than SW_SUB - 0.6 Volt");
			if (ID == 12) {
				ID_Gate = 19; //Gate_ON_3
				Relais_Gate = 4;
			}
			else if (ID == 13) {
				ID_Gate = 17; //Gate_ON_2
				Relais_Gate = 2;
			}
			else if (ID == 22) {
				ID_Gate = 16; //Gate_ON_1
				Relais_Gate = 3;
			}
			else if (ID==21){
			
			ID_Gate = 21; //SW_REF
			Relais_Gate = 14;
			
			}

			else {
				emit SendInfo("Something went wrong in case 5: Couldn't match correct ID");
				data.clear();
				data.append("!Something went wrong in case 5: Couldn't match correct ID");
				file->write((const char *)data.toAscii().data());
				break;
			}

			Voltage = -1.5;
			RelaisCard->SET_Channel(Relais_SW_SUB); //Going to Relais of SW_SUB
			QTest::qWait(100);
			DisablePS(Node);
			QTest::qWait(500);
			EnablePS(Node);
			QTest::qWait(500);
			SetPsCurrent(Node, ID_SW_SUB, 500); // current limit
			QTest::qWait(200);

			WorkAroundInitialization();

			SetPsVoltage(Node, ID_SW_SUB, (int)(1000*Voltage)); // Set voltage on Gate_ON_x
			QTest::qWait(500);
			SMUVolt=SourceMeasurementUnit->GetVoltageDouble();

			// Check whether setting the SW_SUB voltage worked
			if (abs(SMUVolt - Voltage) < 0.2) { //Voltage was set to -1 Volt
				emit SendInfo("Initializing SW_SUB voltage successful!");
				data.append("\nInitializing SW_SUB voltage successful!\n");
			}
			else {
				emit SendInfo("Initializing SW_SUB voltage not successful!");
				data.append("\nInitializing SW_SUB voltage not successful!\n");
				TestSuccessful = false;
				break;
			}

			RelaisCard->SET_Channel(Relais_Gate); //Going to Relais of Gate_ON_x
			QTest::qWait(100);
			Voltage_Gate = -1.0; // This voltage will stay constant, we will change only the voltage of SW_SUB
			SetPsCurrent(Node, ID_Gate, 500); // current limit
			QTest::qWait(200);
			SetPsVoltage(Node, ID_Gate, (int)(1000*Voltage_Gate)); // Set voltage on Gate_ON_x
			QTest::qWait(500);
			SMUVolt=SourceMeasurementUnit->GetVoltageDouble();
			QTest::qWait(200);

			data.append("Voltage at Gate set to ");
			VoltageString.clear();
			VoltageString = QString("%1").arg(Voltage_Gate);
			data.append(VoltageString);
			data.append(" Volt\n");

			if (abs(Voltage_Gate - SMUVolt) > 0.1) { 
				//Abort if setting the voltage to Gate_ON_x does not work
				data.append("\nFailed to set Voltage at Gate_ON \n\n");
				file->write((const char *)data.toAscii().data());
				data.clear();
				emit SendInfo("Failed to set Voltage at Gate_ON \n");
				TestSuccessful = false;
				break;
			}

			Voltage = Voltage_Gate + OVP_Start_L; //Voltage is Voltage_SW_SUB
			ID_SW_SUB = 22; // ID to set Voltage on SW_SUB
			RelaisCard->SET_Channel(13); //Going to Relais of SW_SUB
			QTest::qWait(100);

			while (StillOn == true && (Voltage_Gate - Voltage) > OVP_Stop_L && stop == false) {

			
				SetPsVoltage(Node, ID_SW_SUB,(int)(1000*Voltage)); 
				QTest::qWait(500);
				SMUVolt=SourceMeasurementUnit->GetVoltageDouble();
				QTest::qWait(200);

				data.append("SW_SUB set to ");
				VoltageString = QString("%1").arg(Voltage);
				data.append(VoltageString);
				data.append(" Volt\n");

				if (abs(SMUVolt) < 0.25) {

					StillOn = false;
					ShutDown = true;
					data.append("\nUnit shut down at ");
					data.append(VoltageString);
					data.append(" Volt \n");
					status = "Unit Shutdown";
					emit SendInfo(status);
					status.clear();

					if (Voltage_Gate - Voltage >= Limit_L) {
						data.append("passed \n");
						emit SendInfo("Test passed");
					}
					else {
						data.append("failed \n");
						emit SendInfo("Test failed");
						Passed = false;
						testpassed = false;
					}
				}

				status = "Voltage_SW_SUB set to: ";
				status.append(VoltageString);
				emit SendInfo(status);
				status.clear();

				Voltage += VoltageStep;

			}

			if (ShutDown == false) { 
				Passed = false; 
				testpassed = false;
				data.append("\nUnit did not shut down \nfailed\n");
				emit SendInfo("failed");

			}

			data.append("\n");
			data.append("--------------------------------------------------\n");
			file->write((const char *)data.toAscii().data()); //Write data to file
			//emit SendInfo(data); //Write data to screen
			data.clear();

			break;

		default: 
			emit SendInfo("Something went wrong! Please check your ini File and/or my code. One of use screwed up");
		}	// End of Switch

		if (testpassed == false) {  // Passed (capital P) is initialized for each of the 24 tests, 
								// passed (small p) is an array that stores which test worked. 
			passed [N] = false;
		}

	}	// End of for Loop

	if (TestSuccessful == true) {
		data.clear();
		data.append("\nUnit has been tested successfully\n");
		emit SendInfo("Test successful!");
	}
	else {
		data.clear();
		data.append("\n Test has not been successful\n");
		emit SendInfo("Test not successful, failed to set initial voltage in one or more tests.");
	}
	data.append("\nSUMMARY\n");
	for (int n = 0; n < 24; ++n) {	// Zusammenfassung der Tests
		if (passed [n] == true) {
			data.append("\nTest " + QString("%1").arg(n) + " passed");
		}
		else {
			data.append("\nTest " + QString("%1").arg(n) + " failed");
		}
	}


	file->write((const char *)data.toAscii().data());
	file->close();
}


void OvpTester::StopOvpTest(){

	stop = true;
}

void OvpTester::RequestPsInfo(int node, int channel){

	QString *str=new QString("CMD POWER ");
	str->append(QString::number(node));
	str->append(" GET ");
	str->append(QString::number(channel));
	str->append(" ");
	emit SendCMD(*str);

//	delete str;
}


 void OvpTester::SetPsVoltage(int node, int channel,int volt){

	QString *str=new QString("CMD POWER ");
	str->append(QString::number(node));
	str->append(" SET_U ");
	str->append(QString::number(channel));
	str->append(" ");
	str->append(QString::number(volt));
	str->append(" ");
	emit SendCMD(*str); // signal is emitted ... 

//	delete str;
}

 void OvpTester::EnablePS(int node){

	QString buff,help;
    buff.clear();
	buff.append("CMD POWER ");
	help.clear();
	buff.append(help.setNum(node));
    buff.append(" Powerup S3B "); // enables regulators
	emit SendCMD(buff);
	QTest::qWait(300);

 }

  void OvpTester::DisablePS(int node){

	QString buff,help;
	buff.clear();
	buff.append("CMD POWER ");
	help.clear();
	buff.append(help.setNum(node));
    buff.append(" Shutdown S3B ");
	emit SendCMD(buff);
	QTest::qWait(300);

 }

   void OvpTester::SetPsCurrent(int node, int channel,int current){

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


  void OvpTester::SetSMU(SMU* ptr2SMU){SourceMeasurementUnit=ptr2SMU;}		

  void OvpTester::SetRelaisCard(FTDI* ptr2RelaisCard){RelaisCard=ptr2RelaisCard;}

  void OvpTester::SetPath(char *str) {

	path=QString::QString(str);
}

void OvpTester::RetrieveIni() {
	
	settings -> sync();
}

void OvpTester::StopMeasurement() {

	stop = true;
}

void OvpTester::ReloadSettings() {

	QString fileName = QFileDialog::getOpenFileName(NULL, tr("Open File"), "", tr("Files (*.ini)"));
	settings = new QSettings(fileName,QSettings::IniFormat);
	QString notifier = "Init File: ";
	notifier.append(fileName);
	emit SendInfo(notifier);

	settings->sync();
}

void OvpTester::SmuVoltageIs(double volt) {

	SMU_Voltage = volt;
}

void OvpTester::SmuCurrentIs(double current) {

	SMU_Current = current;
}

void OvpTester::PsVoltageIs(int regulator, int load) {

	PS_Regulator=(double)regulator/1000;
	PS_Load		=(double)load/1000;
}

void OvpTester::PsCurrentIs(double current) {

	PS_Current=current/1000;
}

