#include "TankOperator.h"
#include "FindChildNodeVisitor.h"

TankOperator::TankOperator(osg::Node* node)
    :m_dTurretRotation(0),
    m_dGunElevation(0),
    m_pTankGunNode(NULL),
    m_pTankTurretNode(NULL)
{

    FindChildNodeVisitor find_turret("turret");
    node->accept(find_turret);
    m_pTankTurretNode = dynamic_cast<osgSim::DOFTransform*>(find_turret.GetFirstNode());

    FindChildNodeVisitor find_gun("gun");
    node->accept(find_gun);
    m_pTankGunNode = dynamic_cast<osgSim::DOFTransform*>(find_gun.GetFirstNode());

    FindChildNodeVisitor switch_tank("sw1");
    node->accept(switch_tank);
    m_pTankMutiSwitch = dynamic_cast<osgSim::MultiSwitch*>(switch_tank.GetFirstNode());

}


TankOperator::~TankOperator(void)
{
}

void TankOperator::UpdateTurretRotation()
{
    m_dTurretRotation += 0.02;
    m_pTankTurretNode->setCurrentHPR( osg::Vec3(m_dTurretRotation,0,0));
}

void TankOperator::UpdateGunElevation()
{
    m_dGunElevation += 0.002;
    m_pTankGunNode->setCurrentHPR( osg::Vec3(0,m_dGunElevation,0) );
    if (m_dGunElevation > 0.5)
    {
        m_dGunElevation = 0.0;
    }
}

void TankOperator::ShowTank(bool is_good_tank)
{
    unsigned int status = 0;
    if (!is_good_tank)
    {
        status = 1;
    }
    m_pTankMutiSwitch->setSingleChildOn(0, status);
}

void TankOperator::RotateTurret(double rotate_angle)
{
    m_dTurretRotation = rotate_angle;
    m_pTankTurretNode->setCurrentHPR( osg::Vec3(m_dTurretRotation,0,0));
}
