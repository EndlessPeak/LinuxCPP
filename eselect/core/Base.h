#include "RegisterAction.h"
class Base
{
public:
    virtual int print() = 0;//测试函数
    virtual int list(char *pathname) = 0;
    virtual int set(int options,char *pathname) = 0;
// private:
//     int numItems;
//     char **listItems;
};
