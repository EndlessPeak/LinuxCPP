CXX = g++
CXXFLAGS = -Wall -O2

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

emerge: emerge.o
	$(CXX) emerge.o -o emerge

clean:
	rm emerge.o emerge
