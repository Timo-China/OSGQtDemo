
/************************************************************************/
/*                             自定义漫游                                */
/************************************************************************/
#ifndef _TRAVEL_MANIPULATOR_H_
#define _TRAVEL_MANIPULATOR_H_

#include <osgGA/CameraManipulator>
#include <osgViewer/Viewer>

namespace TM
{
    class TravelManipulator :
        public osgGA::CameraManipulator
    {
    public:
        TravelManipulator(osgViewer::Viewer* osg_viewer);
        ~TravelManipulator(void);

    public:
        static TravelManipulator* CreateManipulator(osgViewer::Viewer* viewer);

    public:
        virtual void setByMatrix(const osg::Matrixd& matrix);
        virtual void setByInverseMatrix(const osg::Matrixd& matrix);
        virtual osg::Matrixd getMatrix() const;
        virtual osg::Matrixd getInverseMatrix() const;
        virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

    public:
        void SetSpeed(const double& speed) {m_dSpeed = speed;}
        double GetSpeed() {return m_dSpeed;}

        void SetPosition(const osg::Vec3& pos) {m_vPosition = pos;}
        osg::Vec3 GetPosition() {return m_vPosition;}

    private:
        void ChangePosition(const osg::Vec3& offset);

    private:
        double m_dMouseX;
        double m_dMouseY;
        bool m_bMousePress;

        osg::Vec3 m_vPosition;
        osg::Vec3 m_vRotation;
        double m_dSpeed; // 移动速度

        double m_dScreenAngle;
        bool m_bImpact; // 是否碰撞检测

        osgViewer::Viewer* m_pOSGViewer;

    };
}

#endif // _TRAVEL_MANIPULATOR_H_