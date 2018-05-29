#ifndef _TANK_KEY_HANDLER_EVENT_H_
#define _TANK_KEY_HANDLER_EVENT_H_

#include <osgGA/GUIEventHandler>


namespace TM
{
    class TankKeyHandlerEvent : public osgGA::GUIEventHandler
    {
    public:
        TankKeyHandlerEvent(void);
        ~TankKeyHandlerEvent(void);

        typedef void (*functionType)(void*);
        enum keyStatusType
        {
            KEY_UP, KEY_DOWN 
        };

        struct functionStatusType
        {
            functionStatusType() {keyState = KEY_UP; keyFunction = NULL;}
            functionType keyFunction;
            keyStatusType keyState;
        };

        virtual bool handle(const osgGA::GUIEventAdapter&, osgGA::GUIActionAdapter&);

        bool addFunction(int key, keyStatusType keyPressStatus, functionType fun);

        bool addFunction(int key, functionType fun);
        
        void setCallbackWindow(void* window) {m_pHandlerWindow = window;}

    protected:
        typedef std::map<int, functionStatusType> KeyFunction;
        KeyFunction m_KeyFuntionMap;
        KeyFunction m_KeyUpFunction;
        void* m_pHandlerWindow;
    };

}

#endif // _TANK_KEY_HANDLER_EVENT_H_