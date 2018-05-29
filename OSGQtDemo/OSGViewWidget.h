#ifndef __OSG_VIEW_WIDGET_H__
#define __OSG_VIEW_WIDGET_H__

#include <osgQt/GraphicsWindowQt>
#include <osgViewer/Viewer>
#include "TravelManipulator.h"

class OSGViewWidget : public osgQt::GLWidget, public osgViewer::Viewer
{
    Q_OBJECT

public:
    OSGViewWidget(QWidget* parent = 0);
    ~OSGViewWidget(void);

public:
    osg::ref_ptr<TM::TravelManipulator> GetCurrentManipulator() {return m_pTravelManipulator;}

protected:
    virtual void resizeEvent( QResizeEvent* event );
    virtual void paintEvent(QPaintEvent* event);

private:
    osg::ref_ptr<TM::TravelManipulator> m_pTravelManipulator;


};

#endif // __OSG_VIEW_WIDGET_H__