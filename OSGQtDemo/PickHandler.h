#ifndef _PICK_HANDLER_H__
#define _PICK_HANDLER_H__

#include <osgGA/GUIEventHandler>
#include <osgViewer/View>

namespace TM
{
    class PickHandler :
        public osgGA::GUIEventHandler
    {
    public:
        PickHandler(void);
        ~PickHandler(void);

    private:
        void Pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

    public:
        virtual bool handle(const osgGA::GUIEventAdapter& ea ,osgGA::GUIActionAdapter& aa);

    };
}

#endif // _PICK_HANDLER_H__