#include "OSGViewWidget.h"
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>


OSGViewWidget::OSGViewWidget(QWidget* parent /*= 0*/)
    :osgQt::GLWidget(parent)
{
    this->_gw = new osgQt::GraphicsWindowQt(this);
//     osg::ref_ptr<osg::Camera> camera = new osg::Camera;
//     camera->setGraphicsContext(_gw);
//     camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
//     camera->setClearDepth(1.0);
//     camera->setViewport( new osg::Viewport(0, 0, _gw->getTraits()->width, _gw->getTraits()->height) );
//     setCamera(camera);

    this->getCamera()->setGraphicsContext(_gw);
    this->getCamera()->setViewport( new osg::Viewport(0, 0, _gw->getTraits()->width, _gw->getTraits()->height));

    addEventHandler( new osgViewer::StatsHandler );
    this->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);

    osgGA::TrackballManipulator* trackBallMpl = new osgGA::TrackballManipulator();
    setCameraManipulator(trackBallMpl);

    // default scene
    osg::ref_ptr<osg::Group> root = new osg::Group;
    root->addChild(osgDB::readNodeFile("cow.osg"));
    this->setSceneData(root);
}


OSGViewWidget::~OSGViewWidget(void)
{
}

void OSGViewWidget::resizeEvent(QResizeEvent* event)
{
    GLWidget::resizeEvent(event);

    if (_gw->getTraits()->height > 0)
    {
        getCamera()->setViewport(0 ,0, _gw->getTraits()->width, _gw->getTraits()->height);
        getCamera()->setProjectionMatrixAsPerspective(
            30.0f, static_cast<double>(_gw->getTraits()->width)/static_cast<double>(_gw->getTraits()->height), 1.0f, 10000.0f );

    }

    frame();
}

void OSGViewWidget::paintEvent(QPaintEvent* event)
{
    frame();

    update();
}
