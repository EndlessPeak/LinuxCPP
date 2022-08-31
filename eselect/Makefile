# Make tools
MAKE = make
CXX = g++
CXXFLAGS = -Wall
#PATH
MODULES_PATH = Modules

all: eselect modules

eselect: eselect.cpp
	$(CXX) $(CXXFLAGS) eselect.cpp -o eselect

modules:
	$(MAKE)  -C $(MODULES_PATH)

clean:
	$(MAKE) clean -C $(MODULES_PATH)
	rm eselect
