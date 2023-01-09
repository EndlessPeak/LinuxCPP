#include <cstdio>
#include <exception>
#include <iostream>
#include <ostream>
#include <string>

#include <unistd.h>

#include "include/serial.hpp"

void my_sleep(unsigned long milliseconds) {
    usleep(milliseconds*1000); // 100 ms
}

void enumerate_ports()
{
    std::vector<PortInfo> devices_found = list_ports();

    std::vector<PortInfo>::iterator iter = devices_found.begin();

	while( iter != devices_found.end() ){
		PortInfo device = *iter++;
		printf( "(%s, %s, %s)\n", device.port.c_str(), device.description.c_str(),
     device.hardware_id.c_str() );
	}
}

void print_usage()
{
    std::cerr << "Usage: test_serial {-e|<serial port address>} ";
    std::cerr << "<baudrate> [test string]" << std::endl;
}

int run(int argc, char **argv)
{
    if(argc < 2) {
        print_usage();
        return 0;
    }

    // Argument 1 is the serial port or enumerate flag
    std::string port(argv[1]);

    if( port == "-e" ) {
        enumerate_ports();
        return 0;
    }
    else if( argc < 3 ) {
        print_usage();
        return 1;
    }

    // Argument 2 is the baudrate
    unsigned long baud = 0;
    sscanf(argv[2], "%lu", &baud);

    // port, baudrate, timeout in milliseconds
    Serial my_serial(port, baud, Timeout::simpleTimeout(1000));
    my_serial.setRTS(false);
    my_serial.setDTR(false);
    std::cout << "Is the serial port open?";
    if(my_serial.isOpen())
        std::cout << " Yes." << std::endl;
    else
        std::cout << " No." << std::endl;

    // // Get the Test string
    // int count = 0;
    // std::string test_string;
    // if (argc == 4) {
    //     test_string = argv[3];
    // } else {
    //     test_string = "Testing.";
    // }


    std::cout << "Timeout == 1000ms, asking for 1 more byte than written." << std::endl;
    bool flags=true;
    while(flags){
        if(my_serial.available()){
            std::vector<std::string> result = my_serial.readlines(65536,"\n");

            std::vector<std::string>::iterator it = result.begin();
            // vector<int>::const_iterator iter=v.begin();
            for(; it != result.end(); ++it)
            {
                std::cout << it->c_str() << "\n";
            }
            flags=false;
        }
        my_sleep(20);
    }
    // Test the timeout, there should be 1 second between prints
    
    // while (count < 10) {
    //size_t bytes_wrote = my_serial.write(test_string);
    //while (count < 3){
    

    // std::cout << "Iteration: " << count << ", Bytes written: ";
    // std::cout << bytes_wrote << ", Bytes read: ";
    // std::cout << "Result:" << result.length() << ", String read: " << result << std::endl;

    // count += 1;
    //}

    // // Test the timeout at 250ms
    // my_serial.setTimeout(Timeout::max(), 250, 0, 250, 0);
    // count = 0;
    // std::cout << "Timeout == 250ms, asking for 1 more byte than written." << std::endl;
    // while (count < 10) {
    //     size_t bytes_wrote = my_serial.write(test_string);

    //     std::string result = my_serial.read(test_string.length()+1);

    //     std::cout << "Iteration: " << count << ", Bytes written: ";
    //     std::cout << bytes_wrote << ", Bytes read: ";
    //     std::cout << result.length() << ", String read: " << result << std::endl;

    //     count += 1;
    // }

    // // Test the timeout at 250ms, but asking exactly for what was written
    // count = 0;
    // std::cout << "Timeout == 250ms, asking for exactly what was written." << std::endl;
    // while (count < 10) {
    //     size_t bytes_wrote = my_serial.write(test_string);

    //     std::string result = my_serial.read(test_string.length());

    //     std::cout << "Iteration: " << count << ", Bytes written: ";
    //     std::cout << bytes_wrote << ", Bytes read: ";
    //     std::cout << result.length() << ", String read: " << result << std::endl;

    //     count += 1;
    // }

    // // Test the timeout at 250ms, but asking for 1 less than what was written
    // count = 0;
    // std::cout << "Timeout == 250ms, asking for 1 less than was written." << std::endl;
    // while (count < 10) {
    //     size_t bytes_wrote = my_serial.write(test_string);

    //     std::string result = my_serial.read(test_string.length()-1);

    //     std::cout << "Iteration: " << count << ", Bytes written: ";
    //     std::cout << bytes_wrote << ", Bytes read: ";
    //     std::cout << result.length() << ", String read: " << result << std::endl;

    //     count += 1;
    // }

    return 0;
}

int main(int argc, char *argv[]) {
    try {
        return run(argc,argv);
    } catch (std::exception &e) {
        std::cerr<<"Unhandled Exception:"<<e.what()<<std::endl;
    }

    return 0;
}
