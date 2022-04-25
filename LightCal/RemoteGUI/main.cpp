/*
 * Copyright (c) 2011, fortiss GmbH.
 * All rights reserved.
 *
 * $Id$
 */

/**
 * \file
 *         Main
 *
 * \author
 *         Nadine Keddis <keddis@fortiss.org>
 */

#include <QtGui/QApplication>
#include "mainwindow.h"

 /*! \brief This is the main function that starts (initializes and shows) the main window.
 *
 */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
