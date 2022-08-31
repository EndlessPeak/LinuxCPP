#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
/* 本程序用于选择提供多版本文件共存时进行控制
   使用方法：
   eselect kernel list
   eselect kernel set X
*/
int findExcutable(char *path,char *argv[]);
int doExecutable(char *path,char *filename,char *argv[]);
int main(int argc,char *argv[]){
    char *path=(char*)malloc(11);
    strcpy(path,"./Modules/");
    int status=-1;
    if (argc !=3 && argc != 4)
        ;//可选：打印帮助信息后退出
    else{
        //检查目录下是否有对应组件
        status=findExcutable(path,argv);
    }
    return status;
}

int findExcutable(char *path,char *argv[]){
    //检查目录下是否有对应组件
    int status=-1;
    struct dirent *direntp;
    DIR *dirptr;
    char *filename=argv[1];
    if((dirptr = opendir(path))==NULL){
        fprintf(stderr, "eselect cannot open folder\n");
        return status;
    }
    while ((direntp = readdir(dirptr))!= NULL) {
        //printf("%s\n",direntp->d_name);
        if(!strcmp(direntp->d_name,filename)){
            //printf("doexcute\n");
            status=doExecutable(path,filename,argv);
            return status;
        }
    }
    return status;
}

int doExecutable(char* path,char *filename,char *argv[]){
    //执行对应组件
    char *executeParams[4];
    //pid_t pid;
    int status=-1;
    executeParams[0]=(char*)malloc(strlen(filename)+strlen(path));
    strcpy(executeParams[0], path); 
    strcat(executeParams[0], filename);
    //printf("%s\n",executeParams[0]);
    executeParams[1]=argv[2];
    if(argv[3]!=NULL){
        executeParams[2]=argv[3];
        executeParams[3]=NULL;
    }else {
        executeParams[2]=NULL;
    }
    // if((pid = fork()))
    //     // 指明令父进程等待
    //     waitpid(pid, &status, 0);
    // else if(pid < 0){
    //     printf("error in fork");
    // }
    // else {
    //     //子进程执行真实命令
    //     execv(executeParams[0], executeParams);
    // } 
    // printf("%s\n",executeParams[0]);
    // printf("%s\n",executeParams[1]);
    // printf("%s\n",executeParams[2]);
    //printf("%s\n",executeParams[3]);
    execv(executeParams[0], executeParams);
    return status;
}
