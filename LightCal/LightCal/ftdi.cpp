
#include "ftdi.h"



FTDI::FTDI(QObject *parent,char *name): QObject(parent){
		
int	help[]={240 , 36 , 37 , 38 , 33  ,34 , 35 , 45 , 44 , 32 , 41 , 42 , 43 , 23 , 40 ,39 , 20, 21, 22, 17, 18,19, 24, 25, 16}; //physical assignment of channels 

	for(int i=0;i<25;i++){ assignment[i]=help[i];}
		
}

void FTDI::FTDI_OpenDevice(){

	QString buffer("try to open device"),

	emit SendInfo(buffer);

FT_STATUS open_stat=FT_Open(0,&ftdi_handle);

ReturnError((int) open_stat);

FT_STATUS status=FT_SetBitMode(ftdi_handle,255,FT_BITMODE_ASYNC_BITBANG);

ReturnError((int) status);
		
}

FTDI::~FTDI(){


	int buff = Channel2Code(0);
	DWORD bytes_written;
	QString str=QString::number(buff);;
	emit SendInfo(str);

    FT_Write(ftdi_handle,(LPVOID)&buff,1, &bytes_written);

	FT_Close(ftdi_handle);

}

void FTDI::reset(){
	
	int buff = Channel2Code(0);
	DWORD bytes_written;
	QString str=QString::number(buff);;
	emit SendInfo(str);

 FT_Write(ftdi_handle,(LPVOID)&buff,1, &bytes_written);

FT_ResetDevice(ftdi_handle);
	
FT_SetBitMode(ftdi_handle,FT_BITMODE_ASYNC_BITBANG,255);

FT_Write(ftdi_handle,(LPVOID)&buff,1, &bytes_written);

}

int FTDI::Channel2Code(int i){

// depends on assignment on relaiscard.

if(i<0) i=0;
if(i>=24)i=24;

return assignment[i];

	}


void FTDI::SET_Channel(int i) {

	int buff = Channel2Code(i);
	DWORD bytes_written;

	

	QString str=QString::number(buff);
	
	

	emit SendInfo(str);

FT_STATUS status =FT_Write(ftdi_handle,(LPVOID)&buff,1, &bytes_written);
	
}


int FTDI::ReturnError(int i){
	QString ErrorStrg;
	
	switch( i )
	{
case '0':		; //FT_OK,
case '1':		emit SendInfo(QString("FT_INVALID_HANDLE"));
case '2':	ErrorStrg;	emit SendInfo(QString("FT_DEVICE_NOT_FOUND"));
case '3':	ErrorStrg;	emit SendInfo(QString("FT_DEVICE_NOT_OPENED"));
case '4':	ErrorStrg;	emit SendInfo(QString("FT_IO_ERROR"));
case '5':	ErrorStrg;	emit SendInfo(QString("FT_INSUFFICIENT_RESOURCES"));
case '6':	ErrorStrg;	emit SendInfo(QString("FT_INVALID_PARAMETER"));
case '7':	ErrorStrg;	emit SendInfo(QString("FT_INVALID_BAUD_RATE"));
case '8':	ErrorStrg;	emit SendInfo(QString("FT_DEVICE_NOT_OPENED_FOR_ERASE"));
case '9':		emit SendInfo(QString("FT_DEVICE_NOT_OPENED_FOR_WRITE"));
case '10':	emit SendInfo(QString("	FT_FAILED_TO_WRITE_DEVICE,"));
case '11':	emit SendInfo(QString("	FT_EEPROM_READ_FAILED,"));
case '12':	emit SendInfo(QString("	FT_EEPROM_WRITE_FAILED,"));
case '13':	emit SendInfo(QString("	FT_EEPROM_ERASE_FAILED,"));
case '14':	emit SendInfo(QString("	FT_EEPROM_NOT_PRESENT,"));
case '15':	emit SendInfo(QString("	FT_EEPROM_NOT_PROGRAMMED,"));
case '16':	emit SendInfo(QString("	FT_INVALID_ARGS,"));
case '17':	emit SendInfo(QString("	FT_NOT_SUPPORTED,"));
case '18':	emit SendInfo(QString("	FT_OTHER_ERROR,"));
case '19':	emit SendInfo(QString("	FT_DEVICE_LIST_NOT_READY,"));

}
	return 0;
}