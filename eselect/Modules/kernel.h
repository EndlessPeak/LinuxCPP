#include "../core/Base.h"
using namespace std;
class Kernel : public Base
{
public:
    Kernel();
    ~Kernel();
    int print();
    int list(char *pathname);
    int set(int options,char *pathname);
    int openList(char *pathname);
    int sortList();
    int checkList(int options,char *pathname);
    int getSelectedList(char *pathname);
    int quicksort(char **listItems,int low,int high);
    int printList(char *pathname);
    char* genFullname(char *pathname,char *filename);
private:
    int numItems;
    int itemBytes;//最长的文件的文件名长度
    //int pathnameBytes;//路径长度
    int selected;//指示是否有选中的文件及其序号
    char *selectedItems;//指示选中的目标
    char *selectedItemsFullName;
    char *symbolFile;//软链接文件名
    char *symbolFileFullName;//软链接全文件名
    //char *pathname;
    char **listItems;//所有列出的选项
};
