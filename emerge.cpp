#include <cstdio>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

/* 本程序用于接受gentoo portage前端emerge命令及安装参数，并传递给arch pacman程序以安装对应包。
 *
 * emerge用法如下：
 * 1.--ask -a 动作之前要求确认
 * 2.--verbose -v 要求输出详细内容
 * 3.--search -s 搜索软件包
 * 4.--usepkg -K 尽量使用二进制包（或本地包）
 * 5.--unmerge -C 忽略依赖，删除某个包
 * 6.--oneshot -1 仅作为依赖安装
 * 7.--sync 同步软件仓库
 * 8.--deselect 删除包
 * 9.--depclean 删除包及依赖
 * 10.--update 更新@world中的包
 * 11.--deep 更新，包括AUR更新
 *
 * pacman/yay映射如下：
 * 1.confirm
 * 2.verbose
 * 3.yay -Ss 包名
 * 4.pacman -U 包名
 * 5.pacman -Rdd 包名
 * 6.pacman -Sdd 包名
 * 7.pacman -Sy
 * 8.pacman -R 包名
 * 9.pacman -Rsn 包名
 * 10.pacman -Syu
 * 11.yay -Syu
 */

/* 程序开始 */
// 用于指明操作
enum actionParameter{
Search=0,
Install,
Sync,
Remove,
};//actions=Install;

struct {
    actionParameter action=Install;
    int modify=0;
}Action;

struct {
    int dependence=0;//0为删除依赖，1为不删除依赖，2为忽略依赖检查
    int modify=0;
}Dependence;

// 用于修饰操作细节
bool pacman=false;//直接让pacman接管参数解析
bool confirm=false;//属于长参数
bool verbose=false;//属于长参数
bool yay=false;//安装时是否考虑AUR
bool updateSystem=false;//是否更新系统（否则仅更新软件仓库,不更新软件）
bool useLocalPkg=false;//安装时是否从本地
int longArgsNum=1;//每一个长参数需要占用一个位置，默认时有--noconfirm
bool printAndExit=false;//打印信息后退出

// 函数声明
int usage();
int version();
int systemExec(int index,char *executeCommand[]);
int produce_command(int packages,char *command[],char *longArgs[],char *packageArgs[]);
int search_package(char *command[]);
int install_package(char *command[]);
int sync_package(char *command[]);
int remove_package(char *command[]);
int decode_args(int argc,char *argv[]);
int decode_longArgs(char **&longArgs);
int decode_operations(char *command[]);
int excute_command(char *mainProgram,char *excuteCommand[]);
int operate_packages(int &packages,char **&packageArgs,char *argv[],int position,int status);
int operate_args(int packages,int position,char *argv[]);
int checkOptionConflict();

/* 主函数执行过程
 * 1.解析命令行参数
 * 2.对参数进行操作 */
int main(int argc, char *argv[]) {
    int i=decode_args(argc, argv);
    int status=0;
    if(i < 0){
        return -1;
    }
    //对pacman略过解析步骤
    if(pacman)
        ;
    int packages=argc-i;//需要安装的包数量
    status=operate_args(packages,i,argv);
    return status;
}

/* 检查操作参数是否冲突
 * 默认操作参数为 安装 检查依赖
 * 若在修改操作参数前发现操作参数非默认，说明有冲突参数 */
int checkOptionConflict(){
    if(Action.modify > 1 || Dependence.modify >1 ){
        printf("Wrong arguments or conflict options received.");
        usage();
        return 1;
    }
    return 0;
}
/* 操作参数
 * 参数分为三部分：
 * 1.命令与短参数
 * 2.长参数
 * 3.操作软件包 */
int operate_args(int packages,int position,char *argv[]){
    if(printAndExit)
        return 0;
    char *command[2];//记录命令与参数
    char **longArgs;//=(char**)malloc(sizeof(char*)*longArgsNum);
    char **packageArgs;//=(char**)malloc(sizeof(char*)*(packages));
    /* 上述两个字符串数组申请的空间在子函数中
     * 需要带回到本函数，故需要使用引用传递 */
    //解析长参数返回长参数结束位置
    decode_longArgs(longArgs);
    int status=operate_packages(packages,packageArgs,argv, position,0);
    if(status)
        return status;
    status=produce_command(packages,command,longArgs,packageArgs);
    return status;
}
/* 解析命令行参数
 * 函数返回值为“需要操作的包数量”
 * 如有要求获取参数的选项，获取的参数（包名）不包括在函数返回值之内 */
int decode_args(int argc,char *argv[]){
    //int lopt;
    struct option long_options[] = {
        {"ask" , no_argument , NULL , 'a'},
        {"search" , no_argument , NULL , 's'},
        {"verbose" , no_argument , NULL , 'v'},
        {"usepkg" , no_argument , NULL , 'K'},
        {"unmerge" , no_argument , NULL , 'C'},
        {"oneshot" , no_argument , NULL , '1'},
        {"deselect" , no_argument , NULL , 1},
        {"depclean" , no_argument , NULL , 2},
        {"update" , no_argument , NULL , 3},
        {"version", no_argument, NULL , 4},
        {"sync" , no_argument , NULL , 5},
        {"deep" , no_argument , NULL , 'D'},
        {"pacman" , no_argument , NULL , 'p'},
        {0,         0,                 0,  0 },
        //{"search" , required_argument , &lopt , 's'},
    };
    int index = -1;

    /* 若有要求参数的选项，可用下列代码调试。
     * 再次注意由此获取的参数个数不包括在本函数返回值之内。 */
    // int opt;
    // 替换long_options中的{"XXX", required_argument,&lopt,num}
    // printf("option %s",long_options[index].name);
    // if(optarg)
    //     printf(" with package %s\n",optarg);
    while (true) {
        int c = getopt_long(argc, argv, "asvC1Kdp", long_options, &index);
        if(c == -1)
            break;
        if(checkOptionConflict())
            return -1;
        switch (c) {
            case 'a':
                //printf("option %s\n",long_options[index].name);
                confirm=true;
                //argDetails+=1;//默认有--noconfirm,替换为--confirm
                break;
            case 's':
                Action.action=Search;
                Action.modify+=1;
                break;
            case 'v':
                //actions=Sync;
                verbose=true;
                longArgsNum+=1;
                //yay=true;
                break;
            case 'K':
                useLocalPkg=true;
                break;
            case 'C':
                Action.action=Remove;
                Action.modify+=1;
                Dependence.dependence=2;
                Dependence.modify+=1;
                break;
            case '1':
                Action.action=Sync;
                Action.modify+=1;
                Dependence.dependence=2;
                Dependence.modify+=1;
                break;
            case 5:
                Action.action=Sync;
                Action.modify+=1;
                break;
            case 1:
                Action.action=Remove;
                Action.modify+=1;
                Dependence.dependence=1;
                Dependence.modify+=1;
                break;
            case 2:
                Action.action=Remove;
                Action.modify+=1;
                Dependence.dependence=0;
                Dependence.modify+=1;
                break;
            case 3:
                Action.action=Sync;
                Action.modify+=1;
                break;
            case 4:
                version();
                break;
            case 'D':
                yay=true;
                break;
            case 'p':
                pacman=true;
                break;
            case '?':
                //printf("unrecognized option %s",argv[optopt]);
                usage();
                return -1;
            default:
                usage();
        }
    }
    return optind;
}

/* 解析包名
 * packages 是需要操作的包的数量
 * argv 是命令行参数
 * position 是当前操作包的位置，是实际下标，对应数组下标时需减1
 * packageArgs 是转换的操作包的数组，packageArgs[0] 表示操作选项，因此不需减1 */
int operate_packages(int& packages,char **&packageArgs,char *argv[],int position,int status){
    if(packages==0 && Action.action!=Sync){
        printf("You must specify at least one package!");
        return -1;
    }
    //本句话应输出绿色
    if(Action.action!=Search){
        printf("\033[01;32mThese are the packages that would be merged,in order: \033[0m");
        printf("\n");
    }
    //printf("Total package is %d operate now\n",packages);
    // packageArgs=(char**)malloc(sizeof(char*)*(packages+3+longArgsNum));
    int i=0;
    for (i=0; i < packages; i++) {
        //检查是否有@world,若有则说明是更新系统，跳出循环不再记录需要更新的软件包
        if(!strcmp(argv[position+i],"@world")){
            //printf("update system!");
            updateSystem=true;
            packages=0;//更新整个系统，不再记录需要更新的软件包
            return 0;
        }
    }
    //申请对应包数量的空间
    packageArgs=(char**)malloc(sizeof(char*)*(packages));
    //packageArgs=(char**)malloc(sizeof(char*)*(packages));
    for(i=0; i < packages; i++){
        //申请字符数+1
        packageArgs[i]=(char*)malloc(sizeof(char)*(strlen(argv[position+i])+1));
        //packageArgs[i]=argv[position+i-1];
        strcpy(packageArgs[i], argv[position+i]);
        //printf("argv is %s\n",argv[position+i]);
        //printf("packageArgs is %s\n",packageArgs[i]);

    }
    return 0;
}

/* 对操作进行解析 */
int decode_operations(char *command[]){
    int status=0;
    switch (Action.action) {
        case Install:
            //printf("Install packages!\n");
            status=install_package(command);
            break;
        case Sync:
            //printf("Sync packages!\n");
            status=sync_package(command);
            break;
        case Search:
            //printf("Search packages!\n");
            status=search_package(command);
            break;
        case Remove:
            //printf("Remove packages!\n");
            status=remove_package(command);
            break;
        default:
            printf("Emerge don't know what to do.");
            status=-1;
            break;
    }
    return status;
}

/* 解析长参数
 * 返回参数结束的位置
 * 长参数需要longArgsNum个位置,目前最多2个位置 */
int decode_longArgs(char **&longArgs){
    //若无长参数，直接返回
    if (!longArgsNum) {
        return 0;
    }
    longArgs=(char**)malloc(sizeof(char*)*longArgsNum);
    int index=0;
    //index 用于动态添加长参数
    //第一个长参数为confirm或noconfirm
    if(!confirm && Action.action==Install){
        longArgs[index]=(char*)malloc(sizeof(char)*12);
        strcpy(longArgs[index],"--noconfirm");
        index+=1;
    }else {
        longArgs[index]=(char*)malloc(sizeof(char)*10);
        strcpy(longArgs[index],"--confirm");
        index+=1;
    }
    if(verbose){
        longArgs[index]=(char*)malloc(sizeof(char)*10);
        strcpy(longArgs[index],"--verbose");
        index+=1;
    }
    //status=decode_operations(packages,packageArgs,status);
    return 0;
}
/* 测试操作-通过system系统调用
 * packageArgs 是需要安装的包参数
 * 由于system潜在的安全问题，本函数暂时仅用于测试 */
int systemExec(int index,char *executeCommand[]){
    int status=0;
    char *output=(char*)malloc(60*index);
    //sprintf(output,"/usr/bin/pacman");
    //sprintf(output,packageArgs[0]);
    strcpy(output,executeCommand[0]);
    for(int i=1; i <= index; i++){
        strcat(output, " ");
        if(executeCommand[i]==NULL)
            continue;
        strcat(output, executeCommand[i]);
        //printf("%s\n",output);
    }
    //printf("Test the result!\n");
    printf("%s",output);
    //system(output);
    return status;
}

/* 搜索操作 */
int search_package(char *command[]){
    //printf("Search packages!");
    strcpy(command[1], "-Ss");
    return 0;
}

/* 同步操作
 * dependence为0时安装依赖，为1时不安装依赖，为2时忽略依赖 */
int sync_package(char *command[]){
    strcpy(command[1], "-Sy");
    if(updateSystem)
        strcat(command[1], "u");
    return 0;
}
/* 安装操作
 * dependence为0时安装依赖，为1时不安装依赖，为2时忽略依赖 */
int install_package(char *command[]){
    if(useLocalPkg)
        strcpy(command[1], "-U");
    else
        strcpy(command[1], "-S");
    if(Dependence.dependence==0)
        ;
    else if(Dependence.dependence==1)
        strcat(command[1], "-dd");
    else
        strcat(command[1], "-dd");
    return 0;
}
/* 卸载操作
 * dependence为0时删除依赖，为1时不删除依赖，为2时不考虑依赖 */
int remove_package(char *command[]){
    if(Dependence.dependence==0)
        strcpy(command[1], "-Rsn");
    else if(Dependence.dependence==1)
        strcpy(command[1], "-R");
    else
        strcpy(command[1], "-Rdd");
    return 0;
}

/* 统一操作接口-通过execv系统调用
 * 在此函数内拼装参数,最后生成execv接受的命令
 * command 是命令与短参数，第0个元素是程序名，第1个元素是操作选项
 * longArgs 是长参数
 * packageArgs 是需要安装的包参数
 * packages 记录需要安装的包数量
 * 特别地，execv系统调用定义最后一个元素必须是NULL，用于标记结尾
 */
int produce_command(int packages,char *command[],char *longArgs[],char *packageArgs[]){
    //命令为/usr/bin/pacman 或 /usr/bin/yay
    command[0]=(char*)malloc(sizeof(char)*16);
    //参数长度由程序指定，不允许超过10，不会溢出
    command[1]=(char*)malloc(sizeof(char)*10);
    if(!yay){//无deep选项
        strcpy(command[0], "/usr/bin/pacman");
    }
    else//deep选项
        strcpy(command[0], "/usr/bin/yay");
    int status=decode_operations(command);
    //生成最终的命令
    //printf("%s\n",command[1]);
    char **excuteCommand=(char**)malloc(sizeof(char*)*(3+longArgsNum+packages));
    excuteCommand[0]=command[0];
    excuteCommand[1]=command[1];
    int i=0,index=2;
    for(i=0; i < longArgsNum ; i++,index++)
        excuteCommand[index]=longArgs[i];
    for(i=0; i < packages ; i++,index++)
        excuteCommand[index]=packageArgs[i];
    excuteCommand[index]=NULL;
    excute_command(command[0],excuteCommand);
    //systemExec(index,excuteCommand);
    return status;
}

/* 本函数用于执行最终的真实命令
 * fork 函数返回三种值
 * 1.小于0,fork失败
 * 2.等于0,为子进程
 * 3.大于0,为父进程 */
int excute_command(char *mainProgram,char *executeCommand[]){
    pid_t pid;
    int status=-1;
    //所有出口统一复用代码 return status
    if(strlen(mainProgram) <= 0)
        ;
    else if((pid = fork()))
        // 指明令父进程等待
        waitpid(pid, &status, 0);
    else if(pid < 0){
        printf("error in fork");
    }
    else {
        //子进程执行真实命令
        execv(mainProgram, executeCommand);
    }
    return status;
}

int usage(){
    printAndExit=true;
    printf("Please view emerge's help for more usage information.\n");
    return 0;
}

int version(){
    char versionStr[10];
    strcpy(versionStr, "1.0.0");
    printf("emerge version %s",versionStr);
    printAndExit=true;
    return 0;
}
