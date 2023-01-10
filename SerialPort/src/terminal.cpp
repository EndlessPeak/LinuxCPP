#include <cstddef>
#include <sys/ioctl.h>
#include <locale.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <pthread.h>

#include <cstdio>
#include <exception>
#include <iostream>
#include <ostream>
#include <string>

#include "../include/window.hpp"

#include "../include/method.hpp"

bool quit=false;
std::string device;

int rxdata(){
    if(!PgSerial.available()){
        return 0;
    }

    std::vector<std::string> result = PgSerial.readlines(65536,"\n");
    std::vector<std::string>::iterator it = result.begin();

    for(; it != result.end(); ++it)
    {
        std::cout << it->c_str();
    }
    std::cout<<std::endl;
    return 0;
}

int txdata(std::string command){
    if(command=="quit"){
        quit=true;
        return 0;
    }
    command+='\n';
    PgSerial.write(command);
    //rxdata();
    return 0;
}

void* tx_run(void*){
    std::string command="";
    while(quit == false){
        std::cin>>command;
        txdata(command);
    }
}

void* rx_run(void*){
    while(quit == false){
        rxdata();
        pg_sleep(50);
    }
}

int thread_run(){
    int res=0;
    pthread_t thread_tx,thread_rx;
    void* thread_result;
    res = pthread_create(&thread_rx, NULL, rx_run, NULL);
    if (res != 0) {
        printf("Thread gui failed.\n");
        return -1;
    }
    res = pthread_create(&thread_tx, NULL, tx_run, NULL);
    if (res != 0) {
       printf("Thread rx data failed.\n");
       return -1;
    }

    // block the main thread until
    res = pthread_join(thread_rx, &thread_result);

    return res;
}

int decode_args(int argc,char *argv[]){
    if(argc<2){
        print_usage();
        return -1;
    }

    // Argument 1 is the serial port or enumerate flag
    std::string port(argv[1]);

    if( port == "-e" ) {
        enumerate_ports();
        return 1;
    }else if (argc < 3) {
        print_usage();
        return -1;
    }

    // Argument 2 is the baudrate
    unsigned long baud = 0;
    sscanf(argv[2], "%lu", &baud);

    setupSerial(port,baud);
    device=port;
    return 0;
}

int main(int argc, char *argv[]) {
    int res=0;
    res=decode_args(argc,argv);
    if(res==-1)
        return res;
    if(res==1)
        return 0;
    res=thread_run();
    return res;
}