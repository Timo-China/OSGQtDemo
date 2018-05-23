#include "OsgQtDemo.h"
#include <QtWidgets/QApplication>
#include <osgQt/GraphicsWindowQt>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    osgQt::initQtWindowingSystem();

    OSGQtDemo w;
    w.show();
    return a.exec();
}
