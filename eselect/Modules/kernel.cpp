#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include "kernel.h"
int checkIsNum(char *str);
int reflect(Base *p,char *argv[]);
//构造函数与析构函数
Kernel::Kernel() {
    numItems=0;
    selected=0;
    itemBytes=0;
    selectedItems=NULL;
    symbolFile=NULL;
    symbolFileFullName=NULL;
    //先赋10个空间，运行时空间不够，每次追加分配10个空间
    listItems=(char**)malloc(sizeof(char*)*10);
} 
Kernel::~Kernel(){
    for(int i=0;i<numItems;i++)
        if(listItems!=NULL)
            free(listItems[i]);
    free(listItems);
    if(selectedItems!=NULL)
        free(selectedItems);
    if(symbolFile!=NULL)
        free(symbolFile);
    if(symbolFileFullName!=NULL)
        free(symbolFileFullName);
}
//成员函数
int Kernel::print() { 
    cout << "Print Class Kernel" << endl;
    return 0;
}
//列出目录下所有内容
int Kernel::openList(char *pathname) {
    //printf("Enter openList Function\n");
    numItems=0;//每次统计前都将numItems清空
    int status=-1;
    struct dirent *direntp;
    char *filenamePrefix=(char*)malloc(6);
    DIR *dirptr; 
    if((dirptr = opendir(pathname))==NULL){
        fprintf(stderr, "eselect cannot open folder\n");
        return status;
    }
    while ((direntp = readdir(dirptr))!= NULL) {
        //跳过被链接的文件和本级、上级目录
        strncpy(filenamePrefix,direntp->d_name,5);//拷贝前5个字符
        if(strcmp(direntp->d_name, ".")//忽略指示当前目录的文件
           &&strcmp(direntp->d_name, "..")//忽略指示上级目录的文件
           &&strcmp(direntp->d_name, "linux")//忽略链接文件本身
           &&!strcmp(filenamePrefix,"linux")//判断文件名是否与内核有关
           &&direntp->d_type==DT_DIR//判断是否是目录
           ){
            listItems[numItems]=direntp->d_name;
            //strlen 使用siez_t
            if (itemBytes<(int)strlen(listItems[numItems])) {
                itemBytes=(int)strlen(listItems[numItems]); 
            }
            numItems++;
            if(numItems%10==0)
                //每次内存不够就重新分配
                listItems=(char**)realloc(listItems,sizeof(char*)*(numItems+10));
        }
    }
    free(filenamePrefix);
    itemBytes+=1;//字符串终结符号需要额外1个位置,它将在checkList中体现。
    //printf("%d\n",numItems);
    return status+1;
}

//进行快速排序（按字母序）
int Kernel::quicksort(char **listItems, int low, int high) {
    if(low>high)
        return 1;
    int first=low,last=high;
    char *key=listItems[first];
    while (first < last) {
        while (first < last && (strcmp(listItems[last],key)>=0))
            --last;
        listItems[first]=listItems[last];
        while (first < last && (strcmp(listItems[first],key)<=0))
            ++first;
        listItems[last]=listItems[first];
    }
    listItems[first]=key;
    return (quicksort(listItems, low, first-1) && quicksort(listItems, first+1, high));
}
//根据选中的目标和路径生成最终路径
char* Kernel::genFullname(char *pathname, char *filename) {
    int length=1;//1指代的是路径符号
    length+=strlen(pathname);
    length+=strlen(filename);
    char *fullname=(char*)malloc(sizeof(char)*length);
    strcpy(fullname, pathname);
    strcat(fullname, "/");
    strcat(fullname, filename);
    return fullname;
}
//根据路径查找当前被选中的项目
int Kernel::getSelectedList(char *pathname) {
    //查看某文件的链接位置，可以用readlink
    //printf("%d\n",itemBytes);
    //selectedItems=(char*)malloc(sizeof(char)*itemBytes);
    //printf("%d\n",(int)strlen(pathname));
    //char *symbolfile=(char*)malloc(sizeof(char)*(strlen(pathname)+10));
    //strcpy(symbolfile, pathname);
    //strcat(symbolfile, "/linux");

    //symbolFile是软链接文件名
    //symbolFileFullName是文件名带路径
    symbolFile=(char*)malloc(10);
    strcpy(symbolFile, "/linux");
    symbolFileFullName=genFullname(pathname,symbolFile);
    //当前选中的项目的完整文件名长度
    int selectedItemsFullLength=itemBytes+strlen(pathname)+1;//其中1表示一个路径符号
    //selectedItemsFullName=NULL;
    selectedItemsFullName=(char*)malloc(sizeof(char)*selectedItemsFullLength);
    if(readlink(symbolFileFullName, selectedItemsFullName, selectedItemsFullLength)<0){
        fprintf(stderr,"error when read symbolfile");
        //如果未能得到文件的链接位置，返回-1
        return -1;
    }
    //free(selectedItems);//防止内存泄漏
    selectedItems=selectedItemsFullName+(selectedItemsFullLength-itemBytes);
    //printf("selected items full name is %s\n",selectedItemsFullName);
    //printf("selected items is %s\n",selectedItems);
    return 0;
}
//显示目录下所有内容
int Kernel::printList(char *pathname) {
    if(numItems==0)
        printf("No available items can be listed!\n");
    //selected=0;
    //如果未得到链接文件，返回的是-1
    //接下来的if会短路，从而不会为任何可选项加*
    selected=getSelectedList(pathname);
    //如果有链接文件，为可选项加*
    for(int i=0;i<numItems;i++){ 
        printf("[%d] ",i+1);
        printf("%s",listItems[i]);
        if(selected==0 && selectedItems!=NULL && !strcmp(selectedItems, listItems[i])){
            printf(" *");
            selected=i;
        }
        printf("\n");
    }
    return 0;
}
//对结果进行排序
int Kernel::sortList() {
    if(numItems<=1)
        ;//不用排序
    else
        quicksort(listItems, 0, numItems-1);
    return 0;
}
//显示所有可选项
int Kernel::list(char *pathname) {
    //如无路径，则使用默认路径
    if (pathname==NULL) {
        pathname=(char*)malloc(sizeof(char)*10);
        strcpy(pathname,"/usr/src");
    }
    //printf("Enter List Function\n");
    //打开指定目录,统计个数,并将内容附给成员变量
    openList(pathname);
    sortList(); 
    return printList(pathname);
}
//更换当前被选中项
int Kernel::checkList(int option,char *pathname){
    if (option>numItems) {
        printf("You cannot select an unexist item!");
        return -1;
    }
    selectedItems=listItems[option-1];
    if(selectedItems==NULL){
        printf("Cannot obtain selection!");
        return -1;
    }
    //char *symbolfile=(char*)malloc(sizeof(char)*(strlen(pathname)+10));
    //strcpy(symbolfile, pathname);
    //strcat(symbolfile, "/linux");
    symbolFile=(char*)malloc(sizeof(char)*10);
    strcpy(symbolFile, "/linux");
    symbolFileFullName=genFullname(pathname, symbolFile);
    unlink(symbolFileFullName);
    //char *filename=(char*)malloc(sizeof(char)*(strlen(pathname)+strlen(selectedItems)+1));
    selectedItemsFullName=genFullname(pathname, selectedItems);
    //strcpy(filename,pathname);
    //strcat(filename, "/");
    //strcat(filename, selectedItems);
    //printf("%s\n",symbolfile);
    //printf("%s\n",filename);
    symlink(selectedItemsFullName, symbolFileFullName);
    return 0;
}
//实现选中功能
int Kernel::set(int options,char *pathname) {
    //printf("%s\n",argv[2]);
    //printf("%d\n",userSet);
    if(options<=0) 
        fprintf(stderr,"You must input a number greater than zero!");
    openList(pathname);
    sortList();
    return checkList(options,pathname);
}
//非成员函数从此开始
//检查输入是否是数字，若是，转换为int类型
int checkIsNum(char *str) {
    int sum=0;
    for(int i=strlen(str)-1;i>=0;i--){
        int tmp=(int)str[i];
        if(tmp>=48 && tmp<=57)
            sum+=i*10+tmp-48;
        else
            return -1;
    }
    return sum;
}
//通过参数执行对应的函数
int reflect(Base *p,char *argv[]) {
    char *pathname=(char*)malloc(sizeof(char)*50);
    strcpy(pathname,"/home/leesin/Develop/Test/src"); 
    int status=-1;
    if(!strcmp(argv[1], "list"))
        status=p->list(pathname);
    else if(!strcmp(argv[1], "set")){
        int userSet=0;
        if(argv[2]!=NULL)
            userSet=checkIsNum(argv[2]);
        else{
            printf("You should type a selection!");
            return -1;
        }
        status=p->set(userSet,pathname);
    }
    else {
        printf("wrong argument detected!");
        return -1;
    }
    return status;
}
int main(int argc, char *argv[]) {
    //printf("kernel test succeed\n");
    //printf("%s\n",argv[0]);
    //printf("%s\n",argv[1]);
    //printf("%s\n",argv[2]);
    Base *p = NULL;
    REGISTER_CLASS(Kernel);
    p = (Base *)ClassFactory::getInstance().getClassByName("Kernel");
    //CreateObjectByName("Kernel");
    //int (*memfun)();
    //printf("%s\n",pathname);
    //p->print();
    //p->list(pathname);
    return reflect(p, argv);
} 
