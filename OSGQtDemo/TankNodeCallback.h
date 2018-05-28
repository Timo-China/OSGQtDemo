#ifndef _TANK_NODE_CALL_BACK_H__
#define _TANK_NODE_CALL_BACK_H__

#include <osg/NodeCallback>

namespace TM
{
    class TankNodeCallback : public osg::NodeCallback
    {
    public:
        TankNodeCallback(void);
        ~TankNodeCallback(void);

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
    };
}

#endif // _TANK_NODE_CALL_BACK_H__