# Make tools
MAKE = make
CXX = g++
CXXFLAGS = -Wall -std=c++11

all: list_ports.o serialImpl.o serial.o method.o  terminal #window

serialImpl.o: serialImpl.cpp
	$(CXX) $(CXXFLAGS) -c serialImpl.cpp -o serialImpl.o

list_ports.o: list_ports.cpp
	$(CXX) $(CXXFLAGS) -c list_ports.cpp -o list_ports.o

serial.o: serial.cpp
	$(CXX) $(CXXFLAGS) -c serial.cpp -o serial.o

method.o: method.cpp 
	$(CXX) $(CXXFLAGS) -c method.cpp -o method.o

#window: window.cpp method.o list_ports.o serialImpl.o serial.o
#	$(CXX) $(CXXFLAGS) {method.o,list_ports.o,serialImpl.o,serial.o,window.cpp} -lncurses -o window

terminal: terminal.cpp method.o list_ports.o serialImpl.o serial.o
	$(CXX) $(CXXFLAGS) {method.o,list_ports.o,serialImpl.o,serial.o,terminal.cpp} -o terminal

clean:
	rm *.o
