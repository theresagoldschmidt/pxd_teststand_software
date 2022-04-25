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

#ifndef FILE_H
#define FILE_H

#include <QFile>
#include <QTextStream>
#include <QStringList>

class File
{
  public:
	File();
	void writeFile(QString filename, QString text);
	void append(QString filename, QString text);
	bool fileExists(QString filename);
	
};

#endif