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
    osg::Camera* CreateHUD(const std::string& hud_text, const unsigned int& width, const unsigned int& height);


    /*
     *@brief:   创建
     *@author:  Timo
     *@date: 2018/05/28
     */
    void CreateCow(osg::Group* root);

    /*
     *@brief:   创建信号槽
     *@author:  Timo
     *@date: 2018/05/24
     */
    void CreateConnect();

    /*
     *@brief:   创建战争场景
     *@author:  dgy
     *@date: 2018/05/28
     */
    void CreateWarScene(osg::Group* root);

    /*
     *@brief:   穿甲坐标轴方便查看
     *@author:  dgy
     *@date: 2018/05/28
     */
    osg::Geode* CreateCoordinateAxis(const osg::Vec3& corner,const osg::Vec3& xdir,const osg::Vec3& ydir,const osg::Vec3& zdir);

    /*
     *@brief:   创建房间
     *@author:  dgy
     *@date: 2018/06/05
     */
    void CreateRoom(osg::Group* root, const osg::ref_ptr<osg::Node>& load_model);


    /*
     *@brief:   创建墙壁
     *@author:  dgy
     *@date: 2018/06/05
     */
    osg::Geometry* CreateWall(const osg::Vec3& v1,const osg::Vec3& v2,const osg::Vec3& v3,osg::StateSet* stateset);

    osg::Node* CreateLights(osg::BoundingBox& bb,osg::StateSet* rootStateSet);

public:

    void ToggleAA();
    void RotateTurrentRight();
    void StopTurrentRight();
    void RotateTurrentLeft();
    void StopTurrentLeft();

    /*
     *@brief:   显示好坦克与坏坦克回调
     *@author:  Timo
     *@date: 2018/05/28
     */
    static void ToggleAACallback(void* handle_window);

    /*
     *@brief:   右旋转炮塔回调
     *@author:  Timo
     *@date: 2018/05/28
     */
    static void RotateTurrentRightCallback(void* handle_window);


    /*
     *@brief:   左旋转炮塔回调
     *@author:  Timo
     *@date: 2018/05/28
     */
    static void RotateTurrentLeftCallback(void* handle_window);

    /*
     *@brief:   
     *@author:  Timo
     *@date: 2018/05/28
     */
    static void StopTurrentCallback(void* handle_window);

public slots:
    void OnActionNewProject();
    void OnActionNewWarScene();
    void OnActionLightTest();
    void OnActionClearScene();

protected:
    virtual void resizeEvent(QResizeEvent * event);
    virtual void showEvent(QShowEvent *event);

private:
    Ui::OSGQtDemoClass ui;
    OSGViewWidget* m_pOSGWidget;
    osg::Camera* m_pHudCamera;
    osg::ref_ptr<osg::Group> m_SceneRoot;
    osg::Node* m_pTankNode; // 第二个坦克
    bool m_bGoodTank;   // 默认显示好坦克
    double m_dTurrentAngle;
};

#endif // OSGQTDEMO_H
