#ifndef OSGQTDEMO_H
#define OSGQTDEMO_H

#include <QtWidgets/QMainWindow>
#include <Osg/TextureCubeMap>
#include "ui_osgqtdemo.h"
#include "OSGViewWidget.h"

class OSGQtDemo : public QMainWindow
{
    Q_OBJECT

public:
    OSGQtDemo(QWidget *parent = 0);
    ~OSGQtDemo();

private:
    void InitOSGViewWidget();

    /*
     *@brief:   为node创建纹理贴图
     *@author:  Timo
     *@date: 2018/05/24
     */
    void CreateTextureForNode(osg::Node* node, osg::Image* img);

    /*
     *@brief:   创建HUD
     *@author:  Timo
     *@date: 2018/05/24
     */
    osg::Camera* CreateHUD(const unsigned int& width, const unsigned int& height);

    /*
     *@brief:   创建信号槽
     *@author:  Timo
     *@date: 2018/05/24
     */
    void CreateConnect();

public slots:
    void OnActionNewProject();

protected:
    virtual void resizeEvent(QResizeEvent * event);

private:
    Ui::OSGQtDemoClass ui;
    OSGViewWidget* m_pOSGWidget;
    osg::Camera* m_pHudCamera;
};

#endif // OSGQTDEMO_H
