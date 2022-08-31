#ifndef _REGISTER_ACTION_H_
#define _REGISTER_ACTION_H_
#include "ClassFactory.h"
class RegisterAction
{
public:
    RegisterAction(string className, pCreateObject ptrCreateFn)
    {
        ClassFactory::getInstance().registClass(className, ptrCreateFn);
    }
};

#define REGISTER_CLASS(className)                \
    RegisterAction g_creatorRegister##className( \
        #className, reinterpret_cast<pCreateObject>(createObj<className>))

#endif
