#ifndef  FTDI_H
#define FTDI_H


#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#include "..\FTDI\ftd2xx.h" //..\FTDI\

#include <QObject>


class FTDI : public QObject {

	Q_OBJECT

public:

FTDI(QObject *parent=0,char *name=0);
~FTDI();

public:

	int Channel2Code(int i);
	int ReturnError(int i);


public slots:

void SET_Channel(int i);

void FTDI_OpenDevice();

void reset();
 
signals:

void SendInfo(const QString &String);


private:

	int assignment[25];

	FT_HANDLE  ftdi_handle;
	FT_STATUS res;
};



#endif 