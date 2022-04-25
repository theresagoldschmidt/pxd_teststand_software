

#include <QApplication>
#include <QtCore>

#include <stdlib.h>

#include "PsRemoteControl.h"


PSMainWin *ps = 0;

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);

   ps=new PSMainWin();


   ps->show();


    return app.exec();
}
