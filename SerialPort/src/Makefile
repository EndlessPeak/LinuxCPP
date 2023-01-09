# Make tools
MAKE = make
CXX = g++
CXXFLAGS = -Wall -std=c++11

all: list_ports.o serialImpl.o serial.o

serialImpl.o: serialImpl.cpp
	$(CXX) $(CXXFLAGS) -c serialImpl.cpp -o serialImpl.o

list_ports.o: list_ports.cpp
	$(CXX) $(CXXFLAGS) -c list_ports.cpp -o list_ports.o

serial.o: serial.cpp
	$(CXX) $(CXXFLAGS) -c serial.cpp -o serial.o

clean:
	rm serialImpl.o list_ports.o serial.o
