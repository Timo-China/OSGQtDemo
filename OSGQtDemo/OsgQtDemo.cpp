#include "OsgQtDemo.h"

OSGQtDemo::OSGQtDemo(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    m_pOSGWidget = new OSGViewWidget(this);
    ui.gridLayout->addWidget(m_pOSGWidget);
}

OSGQtDemo::~OSGQtDemo()
{

}
