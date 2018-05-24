#include "AutoRotateCallBack.h"
#include <osg/MatrixTransform>

namespace TM
{

    AutoRotateCallBack::AutoRotateCallBack(void)
        :m_dRotateAngle(0)
    {

    }

    AutoRotateCallBack::~AutoRotateCallBack(void)
    {
    }

    void AutoRotateCallBack::operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        //创建矩阵转换节点
        osg::ref_ptr<osg::MatrixTransform> mt=dynamic_cast<osg::MatrixTransform*>(node);
        //创建矩阵
        osg::Matrix mx;
        //绕着Z轴旋转
        mx.makeRotate(m_dRotateAngle,osg::Vec3(0.0,0.0,1.0));
        //设置矩阵
        mt->setMatrix(mx);

        m_dRotateAngle+=0.01;

        //继续遍历
        traverse(node,nv);
    }
}



