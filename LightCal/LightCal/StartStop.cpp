
#include "StartStop.h"
 #include <QTest>


StartStop::StartStop(QObject *parent,char *name) : QObject(parent)
		  {
	   
buff=new QString();
help=new QString();

NUM_CHAN=25;
// order of switching
int help[]={0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 , 9 , 8 , 24 , 10 , 11 ,12 , 13 , 14 , 15 , 16, 17, 18, 19, 20, 21, 22, 23};

// names, channel numbers, nominal voltage and current limit
channel_descr help_channels[]= {
	{"DCD_DVDD",0,1800,800},
	{"SWDVDD",1,1800,60},
	{"DHP_CORE",2,1200,800},
	{"DHPCORE",3,1200,800},
	{"DCD_RERIN",4,1200,1000},
	{"SOURCE",5,7000,150},
	{"DCD_AVDD",6,1800,3000},
	{"AMPLOW",7,400,1000},	
	{"SwREF",21,-2000,50},
	{"SWSUB",22,-2000,50},	
	{"CGG1",8,-10000,25},
	{"CGG2",9,-10000,25},
	{"CGG2",10,-10000,25},
	{"Drift",11,-10000,25},
	{"PK",12,5000,50},
	{"HV",13,-60000,500},
	{"Guard",14,-3500,40},
	{"Bulk",15,15000,30},
	{"G1",16,-3500,50},
	{"G2",17,-3500,400},
	{"GO",18,3000,60},
	{"G3",19,-3500,100},
	{"ClOn",20,20000,50},
	{"ClearOFF",23,5000,100},
	{"SWSUB_2",22,-4000,50}

}; 

	for(int i=0;i<25;i++){
		order[i]=help[i];
		channels[i]=help_channels[i];
	}

		  } 


StartStop::~StartStop()
		  {  
		  
		  
		  
		  }


void StartStop::SeqReceiveCMD(){


}
	

void StartStop::SeqStartUp(int node){


int Node=node;
    buff->clear();
	buff->append("CMD POWER ");
	help->clear();
	buff->append(help->setNum(Node));
    buff->append(" Powerup S3B "); // enables regulators



	emit SeqSendCMD(*buff);

	QTest::qWait(500);

	int k;

for(int i=0;i<25;i++){ //(NUM_CHAN-1);i++){ // test for power
	   
	   buff->clear();

		buff->append("CMD POWER ");
		help->clear();
		buff->append(help->setNum(Node));
		buff->append(" SET_I ");
		
		k=order[i];
			
		buff->append(help->setNum(channels[k].id));
		buff->append(" ");
		buff->append(help->setNum(channels[k].I_mA));
        buff->append(" ");


		emit SeqSendCMD(*buff);
		
	QTest::qWait(300); 
	
		    buff->clear();
		buff->append("CMD POWER ");
		help->clear();
		buff->append(help->setNum(Node));
		buff->append(" SET_U ");
		
		k=order[i];
			
		buff->append(help->setNum(channels[k].id));
		buff->append(" ");
		buff->append(help->setNum(channels[k].U_mV));
        buff->append(" ");


		emit SeqSendCMD(*buff);
		
	QTest::qWait(300);


	}
	
	

}

void StartStop::SeqStopPS(int node){
int Node=node;
int k;

	for(int i=(NUM_CHAN-1);i>=0;i--){
	
		buff->clear();
		buff->append("CMD POWER ");
	   	help->clear();
		buff->append(help->setNum(Node));
		buff->append(" SET_U ");
		
		k=order[i];
	
		buff->append(help->setNum(channels[k].id));
		buff->append(" ");
		buff->append(help->setNum(0));
        buff->append(" ");
        
		emit SeqSendCMD(*buff);
		
	 QTest::qWait(500); // waits but QT event processing is still working! --> give the system time to process the stuff
	
			buff->clear();
		buff->append("CMD POWER ");
	   	help->clear();
		buff->append(help->setNum(Node));
		buff->append(" SET_I ");
		
		k=order[i];
	
		buff->append(help->setNum(channels[k].id));
		buff->append(" ");
		buff->append(help->setNum(0));
        buff->append(" ");
        
		emit SeqSendCMD(*buff);
		
	 QTest::qWait(500); // waits but QT event processing is still working! --> give the system time to process the stuff	





	}
	
	buff->clear();
	buff->append("CMD POWER ");
	help->clear();
	buff->append(help->setNum(Node));
    buff->append(" Shutdown S3B ");
	
	emit SeqSendCMD(*buff);
	 
	QTest::qWait(500);

}

