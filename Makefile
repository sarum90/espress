CXX=g++-7

FILES= \
  src/json.cpp \
  src/main.cpp

CXXFLAGS=-std=c++17 -O2 -g

all:
	$(CXX) $(CXXFLAGS) $(FILES) -o main
	./main


