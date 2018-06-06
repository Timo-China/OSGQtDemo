#include "OsgQtDemo.h"
#include <osgViewer/ViewerEventHandlers>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/TexGen>
#include <osg/MatrixTransform>
#include <osgText/Text>
#include <osg/LineWidth>
#include <osg/PositionAttitudeTransform>
#include <osgUtil/SmoothingVisitor>
#include <osg/Point>
#include <osgShadow/ShadowTexture>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgShadow/MinimalShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgParticle/ExplosionEffect>
#include <osgParticle/ExplosionDebrisEffect>
#include <osgParticle/SmokeEffect>
#include <osgParticle/FireEffect>
#include <osgParticle/ModularProgram>
#include <osgParticle/AccelOperator>
#include <osgParticle/FluidFrictionOperator>
#include <osgParticle/ParticleSystemUpdater>
#include <osgFX/AnisotropicLighting>
#include <osgFX/Cartoon>
#include "PickHandler.h"
#include "AutoRotateCallBack.h"
#include "TankOperator.h"
#include "TankNodeCallback.h"
#include "TankKeyHandlerEvent.h"

OSGQtDemo::OSGQtDemo(QWidget *parent)
    : QMainWindow(parent),
    m_pHudCamera(NULL),
    m_pTankNode(NULL),
    m_dTurrentAngle(0.0),
    m_bGoodTank(true)
{
    ui.setupUi(this);
    m_pOSGWidget = new OSGViewWidget(this);
    ui.gridLayout->addWidget(m_pOSGWidget);
    InitOSGViewWidget();
    CreateConnect();
}

OSGQtDemo::~OSGQtDemo()
{

}

void OSGQtDemo::InitOSGViewWidget()
{
    if (m_pOSGWidget)
    {
        // m_pOSGWidget->addEventHandler( new osgViewer::StatsHandler );
        m_pOSGWidget->addEventHandler(new osgViewer::ScreenCaptureHandler);
        m_pOSGWidget->addEventHandler(new osgViewer::HelpHandler);
        m_pOSGWidget->addEventHandler(new TM::PickHandler);
        m_SceneRoot = new osg::Group;
        m_SceneRoot->setName("root");
        m_pOSGWidget->setSceneData(m_SceneRoot);

        // 键盘处理
        TM::TankKeyHandlerEvent* tank_key_handler = new TM::TankKeyHandlerEvent();
        tank_key_handler->setCallbackWindow(this);
        tank_key_handler->addFunction('a', OSGQtDemo::ToggleAACallback);
        tank_key_handler->addFunction('r',TM::TankKeyHandlerEvent::KEY_DOWN,OSGQtDemo::RotateTurrentRightCallback);
        tank_key_handler->addFunction('r',TM::TankKeyHandlerEvent::KEY_UP,OSGQtDemo::StopTurrentCallback);
        tank_key_handler->addFunction('l',OSGQtDemo::RotateTurrentLeftCallback);
        tank_key_handler->addFunction('l',TM::TankKeyHandlerEvent::KEY_UP,OSGQtDemo::StopTurrentCallback);
        m_pOSGWidget->addEventHandler(tank_key_handler);
    }
}

osg::Camera* OSGQtDemo::CreateHUD(const std::string& hud_text, const unsigned int& width, const unsigned int& height)
{
    osg::ref_ptr<osg::Camera> hud_camera = new osg::Camera;
    hud_camera->setName("hud_camera");
    hud_camera->setProjectionMatrix(osg::Matrix::ortho2D(0,width, 0 , height));

    hud_camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    hud_camera->setViewMatrix(osg::Matrix::identity());

    hud_camera->setClearMask(GL_DEPTH_BUFFER_BIT);

    hud_camera->setRenderOrder(osg::Camera::POST_RENDER);
    hud_camera->setAllowEventFocus(false);

    {
        osg::ref_ptr<osg::Geode> geode = new osg::Geode;
        std::string font("fonts/arial.ttf");

        // turn lighting off for the text and disable depth test to ensure it's always ontop.
        osg::StateSet* stateset = geode->getOrCreateStateSet();
        stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

        osg::Vec3 position(15.0f, 5.0f, 0.0f);

        {
            osgText::Text* text = new  osgText::Text;
            text->setCharacterSizeMode(osgText::TextBase::SCREEN_COORDS);
            text->setName("hud_geode");
            geode->addDrawable( text );

            text->setColor(osg::Vec4(1.0, 0.5,0.5, 1.0));
            text->setFont(font);
            text->setPosition(position);
            text->setText(hud_text);
        }

        hud_camera->addChild(geode);
    }

    return hud_camera.release();
}

void OSGQtDemo::resizeEvent(QResizeEvent * event)
{
    QMainWindow::resizeEvent(event);

    // 更新hudCamera坐标
    if (m_pOSGWidget && m_pHudCamera)
    {
        osg::GraphicsContext* gc = m_pOSGWidget->getCamera()->getGraphicsContext();
        int w = gc->getTraits()->width;
        int h = gc->getTraits()->height;
        m_pHudCamera->setProjectionMatrix(osg::Matrix::ortho2D(0,w, 0 , h));
    }
}

void OSGQtDemo::CreateConnect()
{
    connect(ui.action_newproject, &QAction::triggered, this, &OSGQtDemo::OnActionNewProject);
    connect(ui.actionNewWarScene, &QAction::triggered, this, &OSGQtDemo::OnActionNewWarScene);
    connect(ui.actionLight_test, &QAction::triggered, this, &OSGQtDemo::OnActionLightTest);
    connect(ui.actionClearScene, &QAction::triggered, this, &OSGQtDemo::OnActionClearScene);
    connect(ui.actionSequence_test, &QAction::triggered, this, &OSGQtDemo::OnActionSequeceTest);
    connect(ui.actionBoomEffect, &QAction::triggered, this, &OSGQtDemo::OnActionBoomEffect);
    connect(ui.actionParticleEffect, &QAction::triggered, this, &OSGQtDemo::OnActionParticleEffect);
    connect(ui.actionAnisotropic_light, &QAction::triggered, this, &OSGQtDemo::OnActionAnisotropicLight);
    connect(ui.actionCartoon_render, &QAction::triggered, this, &OSGQtDemo::OnActionCartoonRender);
}

void OSGQtDemo::OnActionNewProject()
{
    CreateCow(m_SceneRoot.get());
    // 由于初始状态是未添加任何物体，所以未计算初始位置，所以添加新的物体后需要重新计算一下初始位置
    m_pOSGWidget->home();
}

void OSGQtDemo::OnActionNewWarScene()
{
    CreateWarScene(m_SceneRoot.get());
    m_SceneRoot->addChild(CreateCoordinateAxis(osg::Vec3(0.0f,0.0f,0.0f),osg::Vec3(5.0f,0.0f,0.0f),osg::Vec3(0.0f,5.0f,0.0f),osg::Vec3(0.0f,0.0f,5.0f)));

    // 由于初始状态是未添加任何物体，所以未计算初始位置，所以添加新的物体后需要重新计算一下初始位置
    m_pOSGWidget->home();

}

void OSGQtDemo::OnActionLightTest()
{
    // 光照测试
    osg::ref_ptr<osg::Node> glider_node = osgDB::readNodeFile("glider.osg");

    osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene = CreateShadow();

    CreateRoom(shadowedScene.get(), glider_node);

    m_SceneRoot->addChild(shadowedScene);

    m_pOSGWidget->home();


}

void OSGQtDemo::CreateTextureForNode(osg::Node* node, osg::Image* img)
{
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setDataVariance(osg::Object::DYNAMIC);
    texture->setImage(img);

    //设置自动生成纹理坐标
    osg::ref_ptr<osg::TexGen> texgen=new osg::TexGen();
    texgen->setMode(osg::TexGen::SPHERE_MAP);

    //设置纹理环境，模式为BLEND
    osg::ref_ptr<osg::TexEnv> texenv=new osg::TexEnv;
    texenv->setMode(osg::TexEnv::BLEND);
    texenv->setColor(osg::Vec4(0.3,0.3,0.3,0.0));

    //启动单元一自动生成纹理坐标，并使用纹理
    osg::ref_ptr<osg::StateSet> state=new osg::StateSet;
    state->setTextureAttributeAndModes(1,texture.get(),osg::StateAttribute::ON);
    state->setTextureAttributeAndModes(1,texgen.get(),osg::StateAttribute::ON);
    state->setTextureAttribute(1,texenv.get());

    node->setStateSet(state.get());

}

void OSGQtDemo::CreateCow(osg::Group* root)
{
    osg::ref_ptr<osg::Node> cow_node = osgDB::readNodeFile("cow.osg");
    cow_node->setName("cow_node");

    osg::ref_ptr<osg::Image> image=osgDB::readImageFile("Images\\blueFlowers.png");
    CreateTextureForNode(cow_node, image);

    // 创建Cow物体
    osg::ref_ptr<osg::MatrixTransform> mat_trans = new osg::MatrixTransform();
    osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
    trans->addChild(cow_node);
    osg::ref_ptr<osgText::Text> label = new osgText::Text;
    label->setCharacterSize(1);
    label->setFont("fonts/arial.ttf");
    label->setText("Cow");
    // 始终面向屏幕
    label->setAxisAlignment(osgText::Text::SCREEN);
    // 设置文字渲染时包括一个对齐点和包围矩形
    label->setDrawMode(osgText::Text::TEXT | osgText::Text::ALIGNMENT | osgText::Text::BOUNDINGBOX);
    label->setAlignment(osgText::Text::CENTER_TOP);
    label->setPosition( osg::Vec3(0,0,4) );
    label->setColor( osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) );

    trans->addChild(label);
    // mat_trans->addUpdateCallback(new TM::AutoRotateCallBack);
    mat_trans->addUpdateCallback(new osg::AnimationPathCallback(osg::Vec3d(5.0, 0.0,0.0), osg::Z_AXIS, osg::DegreesToRadians(30.0f)));
    mat_trans->addChild(trans);

    trans->setMatrix(osg::Matrix::translate(osg::Vec3d(10.0,0.0,0.0)));


    root->addChild(cow_node);
    root->addChild(mat_trans);
}

void OSGQtDemo::CreateWarScene(osg::Group* root)
{
    // 放置陆地
//     osg::ref_ptr<osg::Node> ground_node = osgDB::readNodeFile("JoeDirt/JoeDirt.flt");
//     root->addChild(ground_node);

    osg::ref_ptr<osg::Node> tank_Node = osgDB::readNodeFile("T72-tank/t72-tank_des.flt");
    if (!tank_Node.valid())
    {
        return;
    }
    osg::ref_ptr<TankOperator> tank_oper = new TankOperator(tank_Node);
    tank_Node->setUserData(tank_oper);
    tank_Node->addUpdateCallback(new TM::TankNodeCallback);
    osg::ref_ptr<osg::PositionAttitudeTransform> tank_pos_trans = new osg::PositionAttitudeTransform();
    tank_pos_trans->addChild(tank_Node);
    root->addChild(tank_pos_trans);
    tank_pos_trans->setPosition(osg::Vec3d(5.0, 0.0, 0.0));


    // 设置第二个坦克
    osg::ref_ptr<osg::PositionAttitudeTransform> second_trans = new osg::PositionAttitudeTransform();
    m_pTankNode = dynamic_cast<osg::Node*>(tank_Node->clone(osg::CopyOp::DEEP_COPY_ALL));
    osg::ref_ptr<TankOperator> second_tank_oper = new TankOperator(m_pTankNode);
    m_pTankNode->setUserData(second_tank_oper);
    second_tank_oper->ShowTank(false);
    second_trans->addChild(m_pTankNode);
    second_trans->setPosition(osg::Vec3d(-5.0, 0.0, 0.0));
    second_trans->setAttitude(osg::Quat(osg::PI/8.0, osg::Vec3(0,0,1))); // 按Z轴旋转

    root->addChild(second_trans);
}

void OSGQtDemo::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    // add hud
    osg::GraphicsContext* gc = m_pOSGWidget->getCamera()->getGraphicsContext();
    m_pHudCamera = CreateHUD("Demo",gc->getTraits()->width, gc->getTraits()->height);
    m_SceneRoot->addChild(m_pHudCamera);
}

osg::Geode* OSGQtDemo::CreateCoordinateAxis(const osg::Vec3& corner,
    const osg::Vec3& xdir,const osg::Vec3& ydir,const osg::Vec3& zdir)
{
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    osg::Vec3Array* coordinate = new osg::Vec3Array(6);
    (*coordinate)[0] = corner;
    (*coordinate)[1] = corner + xdir;
    (*coordinate)[2] = corner;
    (*coordinate)[3] = corner + ydir;
    (*coordinate)[4] = corner;
    (*coordinate)[5] = corner + zdir;

    geom->setVertexArray(coordinate);

    osg::Vec4 x_color(0.0f,1.0f,1.0f,1.0f);
    osg::Vec4 y_color(0.0f,1.0f,1.0f,1.0f);
    osg::Vec4 z_color(1.0f,0.0f,0.0f,1.0f);

    osg::Vec4Array* color = new osg::Vec4Array(6);
    (*color)[0] = x_color;
    (*color)[1] = x_color;
    (*color)[2] = y_color;
    (*color)[3] = y_color;
    (*color)[4] = z_color;
    (*color)[5] = z_color;

    geom->setColorArray(color);
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 6));

    osg::ref_ptr<osgText::Text> pTextXAuxis = new osgText::Text;
    pTextXAuxis->setText(L"X轴");
    pTextXAuxis->setFont("Fonts/simhei.ttf");
    pTextXAuxis->setAxisAlignment(osgText::Text::SCREEN);
    pTextXAuxis->setCharacterSize(0.1);
    pTextXAuxis->setPosition(xdir);

    osg::ref_ptr<osgText::Text> pTextYAuxis = new osgText::Text;
    pTextYAuxis->setText(L"Y轴");
    pTextYAuxis->setFont("Fonts/simhei.ttf");
    pTextYAuxis->setAxisAlignment(osgText::Text::SCREEN);
    pTextYAuxis->setCharacterSize(0.1);
    pTextYAuxis->setPosition(ydir);

    osg::ref_ptr<osgText::Text> pTextZAuxis = new osgText::Text;
    pTextZAuxis->setText(L"Z轴");
    pTextZAuxis->setFont("Fonts/simhei.ttf");
    pTextZAuxis->setAxisAlignment(osgText::Text::SCREEN);
    pTextZAuxis->setCharacterSize(0.1);
    pTextZAuxis->setPosition(zdir);


    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* line_width = new osg::LineWidth();
    line_width->setWidth(4.0f);
    stateset->setAttributeAndModes(line_width,osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    geom->setStateSet(stateset);

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;

    geode->addDrawable(geom.get());
    geode->addDrawable(pTextXAuxis.get());
    geode->addDrawable(pTextYAuxis.get());
    geode->addDrawable(pTextZAuxis.get());


    return geode.release();
}

void OSGQtDemo::ToggleAA()
{
    if (m_pTankNode)
    {
        osg::ref_ptr<TankOperator> tank_oper = dynamic_cast<TankOperator*>(m_pTankNode->getUserData());
        if (tank_oper)
        {
            m_bGoodTank = !m_bGoodTank;
            tank_oper->ShowTank(m_bGoodTank);
        }
    }
}

void OSGQtDemo::RotateTurrentRight()
{
    if (m_pTankNode)
    {
        osg::ref_ptr<TankOperator> tank_oper = dynamic_cast<TankOperator*>(m_pTankNode->getUserData());
        if (tank_oper)
        {
            m_dTurrentAngle += 0.01;
            tank_oper->RotateTurret(m_dTurrentAngle);
        }
    }
}

void OSGQtDemo::StopTurrentRight()
{

}

void OSGQtDemo::RotateTurrentLeft()
{
    if (m_pTankNode)
    {
        osg::ref_ptr<TankOperator> tank_oper = dynamic_cast<TankOperator*>(m_pTankNode->getUserData());
        if (tank_oper)
        {
            m_dTurrentAngle -= 0.01;
            tank_oper->RotateTurret(m_dTurrentAngle);
        }
    }
}

void OSGQtDemo::StopTurrentLeft()
{

}

void OSGQtDemo::ToggleAACallback(void* handle_window)
{
    OSGQtDemo* window = static_cast<OSGQtDemo*>(handle_window);
    if (window)
    {
        window->ToggleAA();
    }
}

void OSGQtDemo::RotateTurrentRightCallback(void* handle_window)
{
    OSGQtDemo* window = static_cast<OSGQtDemo*>(handle_window);
    if (window)
    {
        window->RotateTurrentRight();
    }
}

void OSGQtDemo::RotateTurrentLeftCallback(void* handle_window)
{
    OSGQtDemo* window = static_cast<OSGQtDemo*>(handle_window);
    if (window)
    {
        window->RotateTurrentLeft();
    }
}

void OSGQtDemo::StopTurrentCallback(void* handle_window)
{
    OSGQtDemo* window = static_cast<OSGQtDemo*>(handle_window);
    if (window)
    {
        window->StopTurrentLeft();
    }
}

osg::Geometry* OSGQtDemo::CreateWall(const osg::Vec3& v1,const osg::Vec3& v2,
    const osg::Vec3& v3,osg::StateSet* stateset)
{
    osg::Geometry* geom = new osg::Geometry();

    geom->setStateSet(stateset);

    const unsigned int step_x = 100;
    const unsigned int step_y = 100;

    osg::Vec3Array* coordinate = new osg::Vec3Array();
    coordinate->reserve(step_x * step_y);

    // 墙壁模拟，将墙壁拆分为100*100个的小矩形，然后绘制100*100个矩形

    osg::Vec3 dx = (v2-v1)/((float)step_x-1.0f);
    osg::Vec3 dy = (v3-v1)/((float)step_y-1.0f);

    // 定义每个矩形的点
    osg::Vec3 row_start_point = v1;
    for (int row = 0; row < step_y; row++)
    {
        osg::Vec3 v = row_start_point;
        for (int col = 0; col < step_y; col++)
        {
            coordinate->push_back(v);
            v += dx;
        }
        row_start_point += dy;
    }

    geom->setVertexArray(coordinate);

    // 定义墙壁颜色
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

    geom->setColorArray(colors, osg::Array::BIND_OVERALL);

    for (int row = 0; row < step_y-1; row++)
    {
        osg::DrawElementsUShort* quad_strip = new osg::DrawElementsUShort(osg::PrimitiveSet::QUAD_STRIP);
        for (int col = 0; col < step_x; col++)
        {
            quad_strip->push_back((row+1)*step_x + col);
            quad_strip->push_back(row*step_x + col);
        }
        geom->addPrimitiveSet(quad_strip);
    }

    // create the normals.
    osgUtil::SmoothingVisitor::smooth(*geom);

    return geom;

}

void OSGQtDemo::CreateRoom(osgShadow::ShadowedScene* shadow_scene, const osg::ref_ptr<osg::Node>& load_model)
{
    osg::BoundingSphere bound_sphere;

    if (load_model)
    {
        const osg::BoundingSphere& bs = load_model->getBound();

        osg::PositionAttitudeTransform* pat = new osg::PositionAttitudeTransform();
        pat->setPivotPoint(bs.center());

        pat->addChild(load_model);
        bound_sphere = pat->getBound();

        shadow_scene->addChild(pat);
    }

    bound_sphere.radius() *= 2.f;
    osg::BoundingBox bb;
    bb.expandBy(bound_sphere);

    // create statesets.
    osg::StateSet* rootStateSet = new osg::StateSet;
    shadow_scene->setStateSet(rootStateSet);

    osg::StateSet* wall = new osg::StateSet;
    wall->setMode(GL_CULL_FACE,osg::StateAttribute::ON);

    osg::StateSet* floor = new osg::StateSet;
    floor->setMode(GL_CULL_FACE,osg::StateAttribute::ON);

    osg::StateSet* roof = new osg::StateSet;
    roof->setMode(GL_CULL_FACE,osg::StateAttribute::ON);

    osg::Geode* geode = new osg::Geode;

    // create front side.
    geode->addDrawable(CreateWall(bb.corner(0),
        bb.corner(4),
        bb.corner(1),
        wall));

    // right side
    geode->addDrawable(CreateWall(bb.corner(1),
        bb.corner(5),
        bb.corner(3),
        wall));

    // left side
    geode->addDrawable(CreateWall(bb.corner(2),
        bb.corner(6),
        bb.corner(0),
        wall));
    // back side
    geode->addDrawable(CreateWall(bb.corner(3),
        bb.corner(7),
        bb.corner(2),
        wall));

    // floor
    geode->addDrawable(CreateWall(bb.corner(0),
        bb.corner(1),
        bb.corner(2),
        floor));

    // roof
    geode->addDrawable(CreateWall(bb.corner(6),
        bb.corner(7),
        bb.corner(4),
        roof));

    shadow_scene->addChild(geode);

    shadow_scene->addChild(CreateLights(bb, rootStateSet));
}



/*
 光照的参数部分说明
 GL_AMBIENT （0，0，0，1） 光源泛光强度的RGBA值
 GL_DIFFUSE （1，1，1，1） 光源漫反射强度的RGBA值
 GL_SPECULAR （1，1，1，1） 光源镜面反射强度的RGBA值
 GL_POSITION （0，0，1，0） 光源的位置（x,y,z,w）
 GL_SPOT_DIRCTION （0，0，-1）聚光灯的方向（x,y,z）
 GL_SPOT_EXPONENT 0 聚光灯指数
 GL_SPOT_CUTOFF 180 指定光源的最大散布角
 GL_CONSTANT_ATTENUATION 1 衰减因子常量
 GL_LINEAR_ATTENUATION 0 线形衰减因子
 GL_QUADRIC_ATTENUATION 0 二次衰减因子
*/

osg::Node* OSGQtDemo::CreateLights(osg::BoundingBox& bb,osg::StateSet* rootStateSet)
{
    osg::Group* lightGroup = new osg::Group;

    float modelSize = bb.radius();

    // 固定光源
    osg::Light* myLight1 = new osg::Light;
    myLight1->setLightNum(0);
    myLight1->setPosition(osg::Vec4(bb.corner(4),1.0f));
    myLight1->setAmbient(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
    myLight1->setDiffuse(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
    myLight1->setSpotCutoff(20.0f);         //.光照的散步角
    myLight1->setSpotExponent(50.0f);
    myLight1->setDirection(osg::Vec3(1.0f,1.0f,-1.0f));

    osg::LightSource* lightS1 = new osg::LightSource;
    lightS1->setLight(myLight1);
    lightS1->setLocalStateSetModes(osg::StateAttribute::ON);

    lightS1->setStateSetModes(*rootStateSet,osg::StateAttribute::ON);
    lightGroup->addChild(lightS1);


    // 移动光源
    osg::Light* myLight2 = new osg::Light;
    myLight2->setLightNum(1);
    myLight2->setPosition(osg::Vec4(0.0,0.0,0.0,1.0f));
    myLight2->setAmbient(osg::Vec4(0.0f,1.0f,1.0f,1.0f));
    myLight2->setDiffuse(osg::Vec4(0.0f,1.0f,1.0f,1.0f));
    myLight2->setConstantAttenuation(1.0f);
    myLight2->setLinearAttenuation(2.0f/modelSize);
    myLight2->setQuadraticAttenuation(2.0f/osg::square(modelSize));

    osg::LightSource* lightS2 = new osg::LightSource;
    lightS2->setLight(myLight2);
    lightS2->setLocalStateSetModes(osg::StateAttribute::ON);

    lightS2->setStateSetModes(*rootStateSet,osg::StateAttribute::ON);

    osg::MatrixTransform* mt = new osg::MatrixTransform();
    {
        // set up the animation path
        osg::AnimationPath* animationPath = new osg::AnimationPath;
        animationPath->insert(0.0,osg::AnimationPath::ControlPoint(bb.corner(0)));
        animationPath->insert(1.0,osg::AnimationPath::ControlPoint(bb.corner(1)));
        animationPath->insert(2.0,osg::AnimationPath::ControlPoint(bb.corner(2)));
        animationPath->insert(3.0,osg::AnimationPath::ControlPoint(bb.corner(3)));
        animationPath->insert(4.0,osg::AnimationPath::ControlPoint(bb.corner(4)));
        animationPath->insert(5.0,osg::AnimationPath::ControlPoint(bb.corner(5)));
        animationPath->insert(6.0,osg::AnimationPath::ControlPoint(bb.corner(6)));
        animationPath->insert(7.0,osg::AnimationPath::ControlPoint(bb.corner(7)));
        animationPath->insert(8.0,osg::AnimationPath::ControlPoint(bb.corner(0)));
        animationPath->setLoopMode(osg::AnimationPath::SWING);

        mt->setUpdateCallback(new osg::AnimationPathCallback(animationPath));
    }

    // create marker for point light.
    osg::Geometry* marker = new osg::Geometry;
    osg::Vec3Array* vertices = new osg::Vec3Array;
    vertices->push_back(osg::Vec3(0.0,0.0,0.0));
    marker->setVertexArray(vertices);
    marker->addPrimitiveSet(new osg::DrawArrays(GL_POINTS,0,1));

    osg::StateSet* stateset = new osg::StateSet;
    osg::Point* point = new osg::Point;
    point->setSize(4.0f);
    stateset->setAttribute(point);

    marker->setStateSet(stateset);

    osg::Geode* markerGeode = new osg::Geode;
    markerGeode->addDrawable(marker);

    mt->addChild(lightS2);
    mt->addChild(markerGeode);

    lightGroup->addChild(mt);

    return lightGroup;
}

void OSGQtDemo::OnActionClearScene()
{
    if (m_SceneRoot && m_SceneRoot->getNumChildren() > 0)
    {
        int children_count = m_SceneRoot->getNumChildren();
        for (int i = 0; i< children_count; i++)
        {
            osg::Node* node = m_SceneRoot->getChild(i);
            if (node->getName() == "hud_camera")
            {
                continue;
            }
            m_SceneRoot->removeChild(node);
            i--;
            children_count--;
        }
        m_pOSGWidget->update();
    }
}

osg::ref_ptr<osgShadow::ShadowedScene> OSGQtDemo::CreateShadow()
{
    //标识阴影接收对象  
    const int ReceivesShadowTraversalMask=0x1;
    //标识阴影投影对象  
    const int CastsShadowTraversalMask=0x2;

    //创建一个阴影节点，并标识接收对象和投影对象
    osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene = new osgShadow::ShadowedScene();
    shadowedScene->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);
    shadowedScene->setCastsShadowTraversalMask(CastsShadowTraversalMask);

    //创建阴影纹理，使用的是shadowTexture技法
    osg::ref_ptr<osgShadow::ShadowTexture> st = new osgShadow::ShadowTexture;
    osg::ref_ptr<osgShadow::ParallelSplitShadowMap> pss = new osgShadow::ParallelSplitShadowMap;
    //osg::ref_ptr<osgShadow::ShadowVolume> sv = new osgShadow::ShadowVolume;
    osg::ref_ptr<osgShadow::MinimalShadowMap> ms = new osgShadow::MinimalShadowMap;
    osg::ref_ptr<osgShadow::StandardShadowMap> ss = new osgShadow::StandardShadowMap;
    osg::ref_ptr<osgShadow::SoftShadowMap> softS = new osgShadow::SoftShadowMap;
    osg::ref_ptr<osgShadow::ViewDependentShadowTechnique> vds = new osgShadow::ViewDependentShadowTechnique;
    //关联阴影纹理  
    shadowedScene->setShadowTechnique(softS);

    return shadowedScene;
}

void OSGQtDemo::OnActionSequeceTest()
{
    osg::ref_ptr<osg::Sequence> seq = CreateSequence();

    m_SceneRoot->addChild(seq.get());

    m_pOSGWidget->home();
}

osg::ref_ptr<osg::Sequence> OSGQtDemo::CreateSequence()
{
    osg::ref_ptr<osg::Sequence> seq = new osg::Sequence;

    typedef std::vector<std::string> FileNames;

    FileNames file_names;

    file_names.push_back("cow.osg");
    file_names.push_back("spaceship.osg");
    file_names.push_back("cessna.osg");
    file_names.push_back("glider.osg");

    for (FileNames::iterator iter = file_names.begin(); iter != file_names.end(); iter++)
    {
        osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(*iter);

        if (node)
        {
            seq->addChild(ScaleNode(node.get(), 100.f));
            seq->setTime(seq->getNumChildren() - 1, 1.0f);
        }
    }

    seq->setInterval(osg::Sequence::LOOP, 0, -1);

    seq->setDuration(1.0f);

    seq->setMode(osg::Sequence::START);

    return seq;
}

osg::ref_ptr<osg::Node> OSGQtDemo::ScaleNode(osg::Node* node, float target_scale)
{
    const osg::BoundingSphere& bound_sphere = node->getBound();

    float scale = target_scale / bound_sphere.radius();

    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();

    pat->setPosition(osg::Vec3(0.0, 0.0, 0.0));

    pat->setScale(osg::Vec3(scale, scale, scale));

    pat->setDataVariance(osg::Object::DYNAMIC);
    pat->addChild(node);

    pat->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

    return pat;

}

void OSGQtDemo::OnActionBoomEffect()
{
    m_SceneRoot->addChild(CreateBoomEffect().get());

    m_pOSGWidget->home();

}

osg::ref_ptr<osg::Node> OSGQtDemo::CreateBoomEffect()
{
    osg::ref_ptr<osg::Group> root = new osg::Group();

    osg::Vec3 wind_direction(1.0f, 0.f, 0.f);

    osg::Vec3 boom_position(0.f, 0.f, -10.f);

    // 爆炸模拟
    osg::ref_ptr<osgParticle::ExplosionEffect> explosion = new osgParticle::ExplosionEffect(boom_position, 10.f);

    // 爆炸碎片模拟
    osg::ref_ptr<osgParticle::ExplosionDebrisEffect> explosion_debris = new osgParticle::ExplosionDebrisEffect(boom_position, 10.0f);

    // 烟雾效果
    osg::ref_ptr<osgParticle::SmokeEffect> smoke = new osgParticle::SmokeEffect(boom_position, 10.0f);

    // 火焰效果
    osg::ref_ptr<osgParticle::FireEffect> fire = new osgParticle::FireEffect(boom_position, 10.f);

    explosion->setWind(wind_direction);
    explosion_debris->setWind(wind_direction);
    smoke->setWind(wind_direction);
    fire->setWind(wind_direction);

    root->addChild(explosion);
    root->addChild(explosion_debris);
    root->addChild(smoke);
    root->addChild(fire);

    return root;

}

void OSGQtDemo::OnActionParticleEffect()
{
    m_SceneRoot->addChild(CreateParticleEffect().get());

    m_pOSGWidget->home();
}

osg::ref_ptr<osg::Node> OSGQtDemo::CreateParticleEffect()
{
    osg::ref_ptr<osg::Group> root = new osg::Group;

    // 单个粒子系统模版
    osgParticle::Particle particle_template;

    // 粒子生命周期，2秒钟
    particle_template.setLifeTime(2);

    // 粒子大小的范围
    particle_template.setSizeRange(osgParticle::rangef(0.5f, 3.0f));

    // 粒子的alpha范围
    particle_template.setAlphaRange(osgParticle::rangef(0.5, 1));

    // 粒子的颜色变化范围
    particle_template.setColorRange(osgParticle::rangev4(osg::Vec4(1.0f, 0.5f, 0.6f, 1.0f),  osg::Vec4(0.0f, 1.0f, 0.6f, 0.0f)));

    // 设置粒子的半径
    particle_template.setRadius(0.05f);

    // 设置粒子重量
    particle_template.setMass(0.05f);


    // 创建粒子系统
    osg::ref_ptr<osgParticle::ParticleSystem> particle_sys = new osgParticle::ParticleSystem();

    // 设置材质
    particle_sys->setDefaultAttributes("Images/smoke.rgb", false, false);

    particle_sys->setDefaultParticleTemplate(particle_template);


    // 创建粒子的放射器
    osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter;

    // 将粒子系统与粒子发射器关联
    emitter->setParticleSystem(particle_sys.get());

    // 创建粒子计数器
    osg::ref_ptr<osgParticle::RandomRateCounter> counter = new osgParticle::RandomRateCounter();
    // 设置每秒添加的例子数
    counter->setRateRange(100.f, 150.f);

    emitter->setCounter(counter.get());

    // 设置点放置器
    osg::ref_ptr<osgParticle::PointPlacer> placer = new osgParticle::PointPlacer;
    placer->setCenter(osg::Vec3(0.0f, 0.0f, 0.0f));
    emitter->setPlacer(placer.get());


    // 创建弧度发射器
    osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter;
    shooter->setInitialSpeedRange(50,100);
    // 弧度值，与Z轴夹角
    shooter->setThetaRange(0.0, osg::PI_2);
    emitter->setShooter(shooter.get());

    root->addChild(emitter.get());

    // 创建标准编程器对象， 控制粒子在生命周期中的更新
    osg::ref_ptr<osgParticle::ModularProgram> program = new osgParticle::ModularProgram;
    program->setParticleSystem(particle_sys.get());

    // 创建重力模拟对象
    osg::ref_ptr<osgParticle::AccelOperator> ap = new osgParticle::AccelOperator();
    // 设置重力加速度，默认为9.8
    ap->setToGravity(-1.0);
    program->addOperator(ap.get());

    // 创建空气阻力模拟
    osg::ref_ptr<osgParticle::FluidFrictionOperator> fluid_friction = new osgParticle::FluidFrictionOperator();
    // 设置空气属性， FluidViscosity 1.8e-5f, FluidDensity 1.2929f
    fluid_friction->setFluidToAir();
    program->addOperator(fluid_friction.get());

    // 添加场景
    root->addChild(program.get());

    // 添加更新器，实现每个粒子的管理
    osg::ref_ptr<osgParticle::ParticleSystemUpdater> particle_sys_updater = new osgParticle::ParticleSystemUpdater;
    particle_sys_updater->addParticleSystem(particle_sys.get());


    // 加入到场景中
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    geode->addDrawable(particle_sys.get());

    root->addChild(geode.get());
    root->addChild(particle_sys_updater.get());

    return root;

}

void OSGQtDemo::OnActionAnisotropicLight()
{
    osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cessna.osg");

    osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/osg256.png");

    osg::ref_ptr<osgFX::AnisotropicLighting> atl = new osgFX::AnisotropicLighting;
    atl->setLightingMap(image.get());

    atl->setLightNumber(7);

    atl->addChild(node.get());

    m_SceneRoot->addChild(atl.get());

    m_pOSGWidget->home();

}

void OSGQtDemo::OnActionCartoonRender()
{
    osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cessna.osg");

    osg::ref_ptr<osgFX::Cartoon> cartoon = new osgFX::Cartoon();

    // 设置轮廓线的颜色
    cartoon->setOutlineColor(osg::Vec4(1.0f, 1.0f,1.0f,1.0f));

    cartoon->setOutlineLineWidth(2.0f);

    cartoon->setLightNumber(0);

    cartoon->addChild(node.get());

    m_SceneRoot->addChild(cartoon.get());

    m_pOSGWidget->home();
}


