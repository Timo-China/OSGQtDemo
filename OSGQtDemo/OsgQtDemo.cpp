#include "OsgQtDemo.h"
#include <osgViewer/ViewerEventHandlers>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/TexGen>
#include <osg/MatrixTransform>
#include "PickHandler.h"
#include "AutoRotateCallBack.h"

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
    }
}

osg::Camera* OSGQtDemo::CreateHUD(const unsigned int& width, const unsigned int& height)
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

        osg::Vec3 position(0.0f, 0.0f, 0.0f);

        {
            osgText::Text* text = new  osgText::Text;
            text->setName("hud_geode");
            geode->addDrawable( text );

            text->setColor(osg::Vec4(1.0, 0.5,0.5, 1.0));
            text->setFont(font);
            text->setPosition(position);
            text->setText("hud text");
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
}

void OSGQtDemo::OnActionNewProject()
{
    osg::ref_ptr<osg::Group> root = new osg::Group;
    root->setName("root");
    osg::ref_ptr<osg::Node> cow_node = osgDB::readNodeFile("cow.osg");
    cow_node->setName("cow_node");

    osg::ref_ptr<osg::Image> image=osgDB::readImageFile("Images\\blueFlowers.png");

    CreateTextureForNode(cow_node, image);

    osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
    trans->addChild(cow_node);

    trans->addUpdateCallback(new TM::AutoRotateCallBack);

    root->addChild(trans);

    m_pOSGWidget->setSceneData(root);

    // add hud
    osg::GraphicsContext* gc = m_pOSGWidget->getCamera()->getGraphicsContext();
    m_pHudCamera = CreateHUD(gc->getTraits()->width, gc->getTraits()->height);
    root->addChild(m_pHudCamera);
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
