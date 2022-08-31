# Make tools
MAKE = make
CXX = g++
CXXFLAGS = -Wall
#PATH
MODULES_PATH = Modules

all: eselect test

eselect: eselect.cpp
	$(CXX) $(CXXFLAGS) eselect.cpp -o eselect

test:
	$(MAKE)  -C $(MODULES_PATH)

clean:
	$(MAKE) clean -C $(MODULES_PATH)
	rm eselect
