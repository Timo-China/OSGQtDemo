#include "OsgQtDemo.h"
#include <osgViewer/ViewerEventHandlers>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/TexGen>
#include <osg/MatrixTransform>
#include <osgText/Text>
#include <osg/LineWidth>
#include <osg/PositionAttitudeTransform>
#include "PickHandler.h"
#include "AutoRotateCallBack.h"
#include "TankOperator.h"
#include "TankNodeCallback.h"

OSGQtDemo::OSGQtDemo(QWidget *parent)
    : QMainWindow(parent),
    m_pHudCamera(NULL)
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
        m_pOSGWidget->addEventHandler( new osgViewer::StatsHandler );
        m_pOSGWidget->addEventHandler(new osgViewer::ScreenCaptureHandler);
        m_pOSGWidget->addEventHandler(new osgViewer::HelpHandler);
        m_pOSGWidget->addEventHandler(new TM::PickHandler);
        m_SceneRoot = new osg::Group;
        m_SceneRoot->setName("root");
        m_pOSGWidget->setSceneData(m_SceneRoot);
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
    
}

void OSGQtDemo::OnActionNewProject()
{
    CreateCow(m_SceneRoot.get());
}

void OSGQtDemo::OnActionNewWarScene()
{
    CreateWarScene(m_SceneRoot.get());
    // m_SceneRoot->addChild(CreateCoordinateAxis(osg::Vec3(0.0f,0.0f,0.0f),osg::Vec3(1.0f,0.0f,0.0f),osg::Vec3(0.0f,1.0f,0.0f),osg::Vec3(0.0f,0.0f,1.0f)));

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
    osg::ref_ptr<TankOperator> tank_oper = new TankOperator(tank_Node);
    tank_Node->setUserData(tank_oper);
    tank_Node->addUpdateCallback(new TM::TankNodeCallback);
    osg::ref_ptr<osg::PositionAttitudeTransform> tank_pos_trans = new osg::PositionAttitudeTransform();
    tank_pos_trans->addChild(tank_Node);
    root->addChild(tank_pos_trans);
    tank_pos_trans->setPosition(osg::Vec3d(5.0, -10.0, 0.0));


    // 设置第二个坦克
    osg::ref_ptr<osg::PositionAttitudeTransform> second_trans = new osg::PositionAttitudeTransform();
    osg::ref_ptr<osg::Node> second_tank_node = dynamic_cast<osg::Node*>(tank_Node->clone(osg::CopyOp::DEEP_COPY_ALL));
    osg::ref_ptr<TankOperator> second_tank_oper = new TankOperator(second_tank_node);
    second_tank_node->setUserData(second_tank_oper);
    second_tank_oper->ShowTank(false);
    second_trans->addChild(second_tank_node);
    second_trans->setPosition(osg::Vec3d(10.0, -10.0, 0.0));
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
