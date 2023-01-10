#ifndef METHOD_H
#define METHOD_H

#include "serial.hpp"

void pg_sleep(unsigned long milliseconds);
void enumerate_ports();
void print_usage();
int setupSerial(std::string port,unsigned long baud);
extern Serial PgSerial;

#endif
