#include <qpe/qpeapplication.h>
#include <qmainwindow.h>

#include "clover.h"

int main(int argc, char **argv) {

  QPEApplication a(argc, argv);

  CloverPaint cp;
  a.showMainWidget(&cp);

  return a.exec();
}

