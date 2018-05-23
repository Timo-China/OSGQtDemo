#ifndef __OSG_VIEW_WIDGET_H__
#define __OSG_VIEW_WIDGET_H__

#include <osgQt/GraphicsWindowQt>
#include <osgViewer/Viewer>

class OSGViewWidget : public osgQt::GLWidget, public osgViewer::Viewer
{
    Q_OBJECT

public:
    OSGViewWidget(QWidget* parent = 0);
    ~OSGViewWidget(void);

protected:
    virtual void resizeEvent( QResizeEvent* event );
    virtual void paintEvent(QPaintEvent* event);

};

#endif // __OSG_VIEW_WIDGET_H__