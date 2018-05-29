#include "TankKeyHandlerEvent.h"

namespace TM
{
    TankKeyHandlerEvent::TankKeyHandlerEvent(void)
        :m_pHandlerWindow(NULL)
    {
    }


    TankKeyHandlerEvent::~TankKeyHandlerEvent(void)
    {
    }

    bool TankKeyHandlerEvent::handle(const osgGA::GUIEventAdapter&ea, osgGA::GUIActionAdapter& aa)
    {
        bool newKeyDownEvent = false;
        bool newKeyUpEvent   = false;

        switch(ea.getEventType())
        {
        case(osgGA::GUIEventAdapter::KEYDOWN):
            {
                KeyFunction::iterator itr = m_KeyFuntionMap.find(ea.getKey());
                if (itr != m_KeyFuntionMap.end())
                {
                    if ( (*itr).second.keyState == KEY_UP )
                    {
                        (*itr).second.keyState = KEY_DOWN;
                        newKeyDownEvent = true;
                    }
                    if (newKeyDownEvent)
                    {
                        (*itr).second.keyFunction(m_pHandlerWindow);
                        newKeyDownEvent = false;
                    }
                    return true;
                }
                return false;
            }
        case(osgGA::GUIEventAdapter::KEYUP):
            {
                KeyFunction::iterator itr = m_KeyFuntionMap.find(ea.getKey());
                if (itr != m_KeyFuntionMap.end() )
                {
                    (*itr).second.keyState = KEY_UP;
                }
                itr = m_KeyUpFunction.find(ea.getKey());
                if (itr != m_KeyUpFunction.end())
                {
                    (*itr).second.keyFunction(m_pHandlerWindow);
                    return true;
                }
                return false; 
            }
        default:
            return false;
        }
    }

    bool TankKeyHandlerEvent::addFunction(int key, keyStatusType keyPressStatus, functionType fun)
    {
        if (keyPressStatus == KEY_DOWN)
        {
            return addFunction(key, fun);
        }
        else
        {
            if (m_KeyUpFunction.end() != m_KeyUpFunction.find(key))
            {
                return false;
            }
            else
            {
                m_KeyUpFunction[key].keyFunction = fun;
                return true;
            }
        }
    }

    bool TankKeyHandlerEvent::addFunction(int key, functionType fun)
    {
        if (m_KeyFuntionMap.end() != m_KeyFuntionMap.find(key))        
        {
            return false;
        }
        else
        {
            m_KeyFuntionMap[key].keyFunction = fun;
            return true;
        }
    }

}


