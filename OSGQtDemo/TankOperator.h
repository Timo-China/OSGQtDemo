#ifndef _TANK_OPERATOR_H__
#define _TANK_OPERATOR_H__

#include <osg/Node>
#include <osgSim/DOFTransform>
#include <osgSim/MultiSwitch>

class TankOperator : public osg::Referenced
{
public:
    TankOperator(osg::Node* node);
    ~TankOperator(void);

public:
    /*
     *@brief:   更新炮塔旋转角度
     *@author:  Timo
     *@date: 2018/05/28
     */
    void UpdateTurretRotation();
    
    /*
     *@brief:   更新枪管角度
     *@author:  Timo
     *@date: 2018/05/28
     */
    void UpdateGunElevation();

    /*
     *@brief:   显示坏坦克或好坦克
     *@author:  Timo
     *@date: 2018/05/28
     */
    void ShowTank(bool is_good_tank);

private:
    double m_dTurretRotation;
    double m_dGunElevation;

    osgSim::DOFTransform* m_pTankTurretNode;
    osgSim::DOFTransform* m_pTankGunNode;
    osgSim::MultiSwitch* m_pTankMutiSwitch;
};


#endif // _TANK_OPERATOR_H__