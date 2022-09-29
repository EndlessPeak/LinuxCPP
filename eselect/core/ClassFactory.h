#ifndef _CLASS_FACTORY_H_
#define _CLASS_FACTORY_H_
#include <iostream>
#include <string>
#include <map>
using namespace std;
/*
 * C++ 反射实现机制
 * 1. 为每个需要反射的类定义一个创建该类对象的回调函数
 * 2. 创建工厂类，使用std::map 保存类名和创建实例的回调函数;
 * 3. 通过类工厂动态创建类对象
 * 4. 程序开始运行时，回调函数存入std::map中，类名作为map的key值
 */
//定义一个函数指针类型，用于指向创建类实例的回调函数
typedef void *(*pCreateObject)(void);
//定义和实现一个工厂类，保存类名和创建类实例的回调函数
class ClassFactory
{
private:
    map<string, pCreateObject> m_classMap;
    ClassFactory(){};//构造函数私有化

public:
    //通过类名称字符串获取类的实例
    void *getClassByName(string className){
        map<string, pCreateObject>::const_iterator iter;
        iter = m_classMap.find(className);
        if (iter == m_classMap.end())
            return NULL;
        else
            return iter->second();
        //first是key值，second是value值
        //此处即为返回函数指针
    }
    //将给定的类名称字符串和对应的创建类对象的函数保存到map中
    void *registClass(string name, pCreateObject method){
        m_classMap.insert(pair<string, pCreateObject>(name, method));
        return NULL;
    }
    //获取工厂类的单个实例对象
    static ClassFactory& getInstance(){
        static ClassFactory cf;
        return cf;
    }

    //void *getClassByName(string className);
    //void *registClass(string name, pCreateObject method);
    //static ClassFactory &getInstance();
};

//将定义的类注册到工厂类中
//1.定义一个创建类实例的回调函数
//2.将类名称字符串和定义的回调函数保存到map中
template <typename T>
T *createObj(void)
{
    return new T;
}

#endif
