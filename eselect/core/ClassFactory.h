#ifndef _CLASS_FACTORY_H_
#define _CLASS_FACTORY_H_
#include <iostream>
#include <string>
#include <map>
using namespace std;
typedef void *(*pCreateObject)(void);
class ClassFactory
{
private:
    map<string, pCreateObject> m_classMap;
    ClassFactory(){};

public:
    void *CreateObjectByName(string className)
    {
        map<string, pCreateObject>::const_iterator iter;
        iter = m_classMap.find(className);
        if (iter == m_classMap.end())
            return NULL;
        else
            return iter->second(); //函数指针的调用
    }
    void registClass(string name, pCreateObject method)
    {
        m_classMap.insert(pair<string, pCreateObject>(name, method));
    }
    static ClassFactory &getInstance()
    {
        {
            static ClassFactory cf;
            return cf;
        }
    }
};

template <typename T>
T *createObj(void)
{
    return new T;
}

#endif
