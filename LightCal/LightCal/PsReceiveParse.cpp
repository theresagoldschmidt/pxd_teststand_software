






#include "PsReceiveParse.h"
#include <QStringList>
#include <QString>
#include <QRegExp>

PsReceiveParser::PsReceiveParser(){
	max_val=1e6; // to identify trash =1e6;
	 LMU_token="LMU";
	 PS_token="PS";
	 DEMO_token="Demonstrator";
	 CHAN_token= "Channel:";
	 U_token="U_Load:";
	 I_token="I_Load:";
	 UREG_token="U_Reg:";
	 BIT_token="Bit:";
}

 
PsReceiveParser::~PsReceiveParser(){}



bool PsReceiveParser::ParseString(const QString & string,int *array_6){

	bool retval=false;
    bool is_ok=false;
	int  NODE;
	int CHANNEL;
	int U_LOAD,U_REG,I_REG,BIT;
	int numEnt;
	NODE=CHANNEL=-1;
	U_LOAD=U_REG=I_REG=BIT=-1;

		// LMU PS Demonstrator 1 Channel: 20 U_Reg: 200 U_Load: 194 I_Load: -3 Bit: 0 

QStringList	*List=new QStringList(string.split(QRegExp("\\s+")));
	
   numEnt =List->length();
   // find start
   int k=0;
   while(k<numEnt){
   
	   if(List->at(k)==LMU_token){ // found token
	   break;
	   }
   k++;
   }
   if(k>=numEnt) return false;

	if(List->at(k)==LMU_token && List->at(k+1)==PS_token && List->at(k+2)==DEMO_token){
	// identified command
		    NODE=List->at(k+3).toInt(&is_ok);
			
			if (is_ok==false){ return false ;}

			if(List->at(k+4)==CHAN_token){ CHANNEL = List->at(k+5).toInt();
				                    if(List->at(k+6)==U_token){  U_LOAD = List->at(k+7).toInt(); 
										 if(List->at(k+8)==UREG_token){ U_REG = List->at(k+9).toInt(); 
												if(List->at(k+10)==I_token){ I_REG = List->at(k+11).toInt(); 
														if(List->at(k+12)==BIT_token){ BIT = List->at(k+13).toInt(); 
														retval=true;
														}
												}
										 }
									}
								}
										   
			}	
							
     array_6[0]=NODE;
	array_6[1]=CHANNEL;
	array_6[2]=U_REG;
	array_6[3]=U_LOAD;
	array_6[4]=I_REG;
	array_6[5]=BIT;

	return retval;

	}







