#include <cstdio>
#include <exception>
#include <iostream>
#include <ostream>
#include <string>

#include <unistd.h>

#include "../include/serial.hpp"
#include "../include/method.hpp"
Serial PgSerial;

void pg_sleep(unsigned long milliseconds) {
    usleep(milliseconds*1000); // 100 ms
}

void enumerate_ports()
{
    std::vector<PortInfo> devices_found = list_ports();

    std::vector<PortInfo>::iterator iter = devices_found.begin();

	while( iter != devices_found.end() ){
		PortInfo device = *iter++;
		printf( "(%s, %s, %s)\n",
                device.port.c_str(),
                device.description.c_str(),
                device.hardware_id.c_str() );
	}
}

void print_usage()
{
    std::cerr << "Usage: test_serial {-e|<serial port address>} ";
    std::cerr << "<baudrate> [test string]" << std::endl;
}

int setupSerial(std::string port,unsigned long baud){
    // port, baudrate, timeout in milliseconds
    // PgSerial(port, baud, Timeout::simpleTimeout(1000));
    PgSerial.setPort(port);
    PgSerial.setBaudrate(baud);
    PgSerial.setBytesize(eightbits);
    PgSerial.setParity(parity_none);
    PgSerial.setStopbits(stopbits_one);
    PgSerial.setFlowcontrol(flowcontrol_none);
    Timeout timeout = Timeout();
    timeout.simpleTimeout(1000);
    //Timeout timeout=Timeout::simpleTimeout(1000);
    PgSerial.setTimeout(timeout);
    PgSerial.open();
    PgSerial.setRTS(false);
    PgSerial.setDTR(false);
    return 0;
}

int test(int argc, char *argv[])
{ 
    std::cout << "Serial port open:";
    if(PgSerial.isOpen())
        std::cout << " Yes." << std::endl;
    else
        std::cout << " None." << std::endl;
 
    bool flags=true;
    while(flags){
        if(PgSerial.available()){
            std::vector<std::string> result = PgSerial.readlines(65536,"\n");

            std::vector<std::string>::iterator it = result.begin();
            // vector<int>::const_iterator iter=v.begin();
            for(; it != result.end(); ++it)
            {
                std::cout << it->c_str() << "\n";
            }
            flags=false;
        }
        pg_sleep(20);
    }
    return 0;
}

// int main(int argc, char *argv[]) {
//     try {
//         return run(argc,argv);
//     } catch (std::exception &e) {
//         std::cerr<<"Unhandled Exception:"<<e.what()<<std::endl;
//     }

//     return 0;
// }

