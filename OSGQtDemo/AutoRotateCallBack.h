#ifndef _AUTO_ROTATE_CALLBACK_H__
#define _AUTO_ROTATE_CALLBACK_H__

#include <osg/NodeCallback>

namespace TM
{
    class AutoRotateCallBack :
        public osg::NodeCallback
    {
    public:
        AutoRotateCallBack(void);
        ~AutoRotateCallBack(void);

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

    private:
        double m_dRotateAngle;

    };

}


#endif //_AUTO_ROTATE_CALLBACK_H__