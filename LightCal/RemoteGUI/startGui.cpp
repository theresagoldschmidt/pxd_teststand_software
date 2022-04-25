/*
 * Copyright (c) 2011, fortiss GmbH.
 * All rights reserved.
 *
 * $Id$
 */

/**
 * \file
 *         GUI Header
 *
 * \author
 *         Nadine Keddis <keddis@fortiss.org>
 */

#include <QtGui/QApplication>
#include "mainwindow.h"
#include "wrapperHeader.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "belle_II_GUI_XME_Component.h"
#ifdef __cplusplus
}
#endif

MainWindow *w = 0;

extern "C" int startGui(int argc, char *argv[])
{
	QApplication a(argc, argv);
	w = new MainWindow();
    w->show();
    return a.exec();
}

extern "C" void displayNodeChannel(int node, int channel){
	w->emitSignal(node, channel);
}


extern "C" void displayLogdata(int node, char *data, int number){
	w->emitSignalData(node, data, number);
}

void sendDataWrapper(int topic, void *data, unsigned short size){
	sendData(topic, data, size);
}

void shutdownWrapper(){
	shutdown();
}

void startupWrapper(){
	startup();
}

extern "C" void updateStatus(int node, int status){
	w->changeStatusDisplay(node, status);
}

extern "C" void updateMonitor(int node, monitor_information_node_t info){
	w->updateMonitorInformation(node, info);
}

extern "C" void updateLogFile(int node, char *data, int number){
	w->emitSignalLogData(node, data, number);
}