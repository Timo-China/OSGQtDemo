#include "PickHandler.h"
#include <QDebug>

namespace TM
{
    PickHandler::PickHandler(void)
    {
    }


    PickHandler::~PickHandler(void)
    {
    }

    bool PickHandler::handle(const osgGA::GUIEventAdapter& ea ,osgGA::GUIActionAdapter& aa)
    {
        switch (ea.getEventType())
        {
        case osgGA::GUIEventAdapter::PUSH:
            {
                // Êó±ê°´ÏÂ
                osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
                if (view)
                    Pick(view,ea);
            }
            return true;
        default:
            return false;
        }

    }

    void PickHandler::Pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea)
    {
        osgUtil::LineSegmentIntersector::Intersections intersections;

        float x = ea.getX();
        float y = ea.getY();
        if (view->computeIntersections(ea, intersections))
        {
            for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
                hitr != intersections.end();
                ++hitr)
            {
                if (!hitr->nodePath.empty() && !(hitr->nodePath.back()->getName().empty()))
                {
                    osg::Node* pick_node = hitr->nodePath.back();
                    std::string name = pick_node->getName();
                    qDebug() << "node name: " << name.c_str() << endl;
                }
            }
        }

    }
}


