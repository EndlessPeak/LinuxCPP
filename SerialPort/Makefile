# Make tools
MAKE = make
CXX = g++
CXXFLAGS = -Wall -std=c++11
# PATH
MODULES_PATH = src

all: modules main

modules:
	$(MAKE) -C $(MODULES_PATH)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

main: main.o $(MODULES_PATH)/list_ports.o $(MODULES_PATH)/serialImpl.o $(MODULES_PATH)/serial.o
	$(CXX) $(CXXFLAGS) main.o $(MODULES_PATH)/{list_ports.o,serial.o,serialImpl.o} -o main

clean:
	$(MAKE) clean -C $(MODULES_PATH)
	rm main.o main
