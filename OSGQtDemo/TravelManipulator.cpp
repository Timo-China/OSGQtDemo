#include "TravelManipulator.h"
#include <osg/LineSegment>

namespace TM
{
    TravelManipulator::TravelManipulator(osgViewer::Viewer* osg_viewer)
        :m_dSpeed(1.0),
        m_bMousePress(false),
        m_dMouseX(0),
        m_dMouseY(0),
        m_dScreenAngle(2.5),
        m_bImpact(true),
        m_pOSGViewer(osg_viewer)
    {
        m_vPosition = osg::Vec3(-22.0, -247.0, 100.0);
        m_vRotation = osg::Vec3(osg::PI_2, 0, 0);
    }


    TravelManipulator::~TravelManipulator(void)
    {
    }

    void TravelManipulator::setByMatrix(const osg::Matrixd& matrix)
    {
        // CameraManipulator::setByMatrix(matrix);
    }

    void TravelManipulator::setByInverseMatrix(const osg::Matrixd& matrix)
    {
        // CameraManipulator::setByInverseMatrix(matrix);
    }

    osg::Matrixd TravelManipulator::getMatrix() const
    {
        osg::Matrix matrix;
        matrix.identity();
        matrix.makeRotate(m_vRotation._v[0], osg::Vec3(1.0, 0.0, 0.0),
            m_vRotation._v[1], osg::Vec3(0.0, 1.0, 0.0),
            m_vRotation._v[2], osg::Vec3(0.0, 0.0, 1.0));

        return matrix * osg::Matrixd::translate(m_vPosition);

    }

    osg::Matrixd TravelManipulator::getInverseMatrix() const
    {
        osg::Matrix matrix;
        matrix.identity();
        matrix.makeRotate(m_vRotation._v[0], osg::Vec3(1.0, 0.0, 0.0),
            m_vRotation._v[1], osg::Vec3(0.0, 1.0, 0.0),
            m_vRotation._v[2], osg::Vec3(0.0, 0.0, 1.0));

        return osg::Matrixd::inverse(matrix * osg::Matrixd::translate(m_vPosition));
    }

    bool TravelManipulator::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
    {
        float mouse_X = ea.getX();
        float mouse_Y = ea.getY();

        switch(ea.getEventType())
        {
        case osgGA::GUIEventAdapter::KEYDOWN:
            {
                if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Space)
                {
                    us.requestRedraw();
                    us.requestContinuousUpdate(false);
                    return true;
                }
                if (ea.getKey() == 'w' || ea.getKey() == 'W')
                {
                    // 上移
                    ChangePosition(osg::Vec3 (0, 0, m_dSpeed)) ;
                    return true;
                }
                if (ea.getKey() == 's' || ea.getKey() == 'S')
                {
                    // 下移
                    ChangePosition(osg::Vec3 (0, 0, -m_dSpeed)) ;
                    return true;
                }
                if (ea.getKey() == 'a' || ea.getKey() == 'A')
                {
                    // 左移
                    return true;
                }
                if (ea.getKey() == 'd' || ea.getKey() == 'D')
                {
                    // 右移
                    return true;
                }
                if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Plus)
                {
                    m_dSpeed += 0.2;
                    return true;
                }
                if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Minus)
                {
                    m_dSpeed -= 0.2;
                    if (m_dSpeed < 1.0)
                    {
                        m_dSpeed = 1.0;
                    }
                    return true;
                }
                if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
                {
                    // 前进

                    ChangePosition(osg::Vec3 (0, m_dSpeed * sinf(osg::PI_2+m_vRotation._v[2]), 0)) ;

                    ChangePosition(osg::Vec3 (m_dSpeed * cosf(osg::PI_2+m_vRotation._v[2]), 0, 0)) ;
                    return true;
                }
                if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)
                {
                    // 后退
                    ChangePosition(osg::Vec3 (0, -m_dSpeed * sinf(osg::PI_2+m_vRotation._v[2]), 0)) ;

                    ChangePosition(osg::Vec3(-m_dSpeed * cosf(osg::PI_2+m_vRotation._v[2]), 0, 0)) ;
                    return true;
                }
                if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Left)
                {
                    // 向左
                    ChangePosition(osg::Vec3 (0, m_dSpeed * cosf(osg::PI_2+m_vRotation._v[2]), 0)) ;

                    ChangePosition(osg::Vec3 (-m_dSpeed * sinf(osg::PI_2+m_vRotation._v[2]), 0, 0)) ;
                    return true;
                }
                if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Right)
                {
                    // 向右
                    ChangePosition(osg::Vec3 (0,-m_dSpeed * cosf(osg::PI_2+m_vRotation._v[2]), 0)) ;

                    ChangePosition(osg::Vec3 (m_dSpeed * sinf(osg::PI_2+m_vRotation._v[2]), 0, 0)) ;
                    return true;
                }
            }
        case osgGA::GUIEventAdapter::PUSH:
            {
                if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
                {
                    // 左键
                    m_bMousePress = true;
                    m_dMouseX = ea.getX();
                    m_dMouseY = ea.getY();
                    return false;
                }
            }
        case osgGA::GUIEventAdapter::DRAG:
            {
                // 拖动
                if (m_bMousePress)
                {
                    m_vRotation._v[2] -= osg::DegreesToRadians(m_dScreenAngle * (mouse_X-m_dMouseX));

                    m_vRotation._v[0] += osg::DegreesToRadians(1.1*(mouse_Y-m_dMouseY)) ;

                    if (m_vRotation._v [0] >= osg::PI)
                    {
                        m_vRotation._v [0] = osg::PI ;
                    }
                    if (m_vRotation._v [0] <= 0)
                    {
                        m_vRotation._v [0] = 0 ;
                    }
                }
                return false;
            }
        case osgGA::GUIEventAdapter::RELEASE:
            {
                // 释放
                if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
                {
                    m_bMousePress = false;
                }
                return false;
            }
        default:
            return false;
        }

    }

    void TravelManipulator::ChangePosition(const osg::Vec3& offset)
    {
        if (m_bImpact)
        {
            osg::Vec3 new_pos =  m_vPosition + offset;

            osg::ref_ptr<osgUtil::LineSegmentIntersector> line_xy = new osgUtil::LineSegmentIntersector(new_pos, m_vPosition);
            osg::ref_ptr<osgUtil::LineSegmentIntersector> line_z = new osgUtil::LineSegmentIntersector(new_pos+osg::Vec3(0.0f,0.0f,10.0f), new_pos-osg::Vec3(0.0f,0.0f,-10.0f));
            osg::ref_ptr<osgUtil::IntersectorGroup> inter_group = new osgUtil::IntersectorGroup();
            inter_group->addIntersector(line_xy.get());
            inter_group->addIntersector(line_z.get());
            osgUtil::IntersectionVisitor inter_visitor(inter_group.get());

            if (m_pOSGViewer)
            {
                m_pOSGViewer->getSceneData()->accept(inter_visitor);
                if (!inter_group->containsIntersections())
                {
                    m_vPosition += offset;
                }
            }
        }
        else
        {
            m_vPosition += offset;
        }
    }

    TravelManipulator* TravelManipulator::CreateManipulator(osgViewer::Viewer* viewer)
    {
        if (viewer)
        {
            TM::TravelManipulator* manipulator = new TM::TravelManipulator(viewer);
            return manipulator;
        }
        return NULL;
    }

}

