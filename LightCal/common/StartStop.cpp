
#include "StartStop.h"
 #include <QTest>


StartStop::StartStop(QObject *parent,char *name) : QObject(parent)
		  {
	   
buff=new QString();
help=new QString();

NUM_CHAN=24;
// order of switching
int help[]={0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 , 10 , 11 ,12 , 13 , 14 , 15 , 16, 17, 18, 19, 20, 21, 22, 23};

// names, channel numbers, nominal voltage and current limit
channel_descr help_channels[]= {
	{"DCD_DVDD",0,1800,500},
	{"DHP_IO",1,1200,500},
	{"DHP_CORE",2,1800,500},
	{"SW_DVDD",3,3300,50},

	{"DCD_AVDD",4,1800,500},
	{"DCD_REFIN",5,1200,100},
	{"DCD_AMPLOW",6,350,300},
	{"Source",7,4000,500},
	
	{"GATE_OFF",8,1000,100},
	{"Gate_ON",9,1800,100},
	{"ClearON",10,2500,200},
	{"ClearOff",11,1200,200},

	{"Bulk",12,1500,50},
	{"Guard",13,800,50},
	{"CCG",14,500,40},
	{"HV",15, 1500,10},

	{"Bulk",16,1500,50},
	{"Guard",17,800,50},
	{"CCG",18,500,40},
	{"HV",19, 1500,100},

	{"Bulk",20,1500,50},
	{"Guard",21,800,50},
	{"CCG",22,500,40},
	{"HV",23,1500,100}	

}; 

	for(int i=0;i<NUM_CHAN;i++){
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

for(int i=0;i<=(NUM_CHAN-1);i++){
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
		
	QTest::qWait(500);
	
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
		
	QTest::qWait(500);


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

