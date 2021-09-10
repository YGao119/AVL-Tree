CXX=g++
CXXFLAGS=-g -Wall -std=c++11

floorplan: floorplan.cpp
	$(CXX) $(CXXFLAGS) floorplan.cpp -o floorplan


clean:
	rm -f floorplan