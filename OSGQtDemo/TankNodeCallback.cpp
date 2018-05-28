#include "TankNodeCallback.h"
#include "TankOperator.h"

namespace TM
{
    TankNodeCallback::TankNodeCallback(void)
    {
    }


    TankNodeCallback::~TankNodeCallback(void)
    {
    }

    void TankNodeCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        osg::ref_ptr<TankOperator> tank_oper = dynamic_cast<TankOperator*>(node->getUserData());

        if (tank_oper)
        {
            tank_oper->UpdateGunElevation();
            tank_oper->UpdateTurretRotation();
        }
        traverse(node,nv);
    }
}


