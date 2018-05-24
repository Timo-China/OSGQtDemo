#include "OsgQtDemo.h"
#include <osgViewer/ViewerEventHandlers>
#include <osgDB/ReadFile>

OSGQtDemo::OSGQtDemo(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    m_pOSGWidget = new OSGViewWidget(this);
    ui.gridLayout->addWidget(m_pOSGWidget);
    InitOSGViewWidget();
}

OSGQtDemo::~OSGQtDemo()
{

}

void OSGQtDemo::InitOSGViewWidget()
{
    if (m_pOSGWidget)
    {
        m_pOSGWidget->addEventHandler( new osgViewer::StatsHandler );
        m_pOSGWidget->addEventHandler(new osgViewer::ScreenCaptureHandler);
        m_pOSGWidget->addEventHandler(new osgViewer::HelpHandler);
        osg::ref_ptr<osg::Group> root = new osg::Group;
        root->addChild(osgDB::readNodeFile("cow.osg"));
        m_pOSGWidget->setSceneData(root);
    }
}
