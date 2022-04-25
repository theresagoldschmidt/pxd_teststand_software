/*
 * Copyright (c) 2011, fortiss GmbH.
 * All rights reserved.
 *
 * $Id$
 */

/**
 * \file
 *         File
 *
 * \author
 *         Nadine Keddis <keddis@fortiss.org>
 */
 
#include "file.h"

File::File(){
}

bool File::fileExists(QString filename){
	QFile file(filename);
	return file.exists();
}

void File::writeFile(QString filename, QString text){
	QFile file(filename);
	if(file.open(QFile::WriteOnly | QFile::Truncate)) {
		QTextStream myFile(&file);
		myFile << text << "\r\n";
	}
    file.close();
}

void File::append(QString filename, QString text)
{
    QFile file(filename);
	if(file.open(QFile::WriteOnly | QFile::Append)){
        QTextStream myFile(&file);
		myFile << text << "\r\n";
    }
	file.close();
}